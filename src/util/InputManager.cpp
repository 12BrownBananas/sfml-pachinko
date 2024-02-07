#include "src/util/InputManager.hpp"
#include <Window.hpp>
#include <algorithm>
#include <vector>
#include <memory>

InputManager::InputManager() : 
    _cursorPos(0.0, 0.0),
    _deviceIndex(0)
    {}

InputManager::InputManager(int deviceIndex) :
    _cursorPos(0.0, 0.0),
    _deviceIndex(deviceIndex)
    {}
InputManager::InputManager(int deviceIndex, const sf::Window* window) :
    _cursorPos(0.0, 0.0),
    _deviceIndex(deviceIndex),
    _myWindow(window)
    {}
float InputManager::getCursorX() const {
    return _cursorPos.x;
}

float InputManager::getCursorY() const {
    return _cursorPos.y;
}

void InputManager::processInput() {
    //process buttons and keys
    for (auto it = _inputWrapperMap.begin(); it != _inputWrapperMap.end(); ++it) {
        auto semanticInput = it->first;
        auto vec = it->second;
        auto lastHighest = InputState::resting;
        for (auto i = vec.begin(); i != vec.end(); ++i) {
            InputWrapper* wrapper = i->get();
            wrapper->processInput();
            lastHighest = takeHigherInputStatePrecedence(lastHighest, wrapper->getInputState());
        }
        _inputStateMap[semanticInput] = lastHighest;
    }
    //TODO: process analog input (for joysticks)
    _setCursorPosition();
}

void InputManager::setWindow(const sf::Window* window) {
    _myWindow = window;
}

const sf::Window* InputManager::getWindow() const {
    return _myWindow;
}

void InputManager::_setCursorPosition() {
    sf::Vector2i pos;
    if (getWindow()) 
        pos = sf::Mouse::getPosition(*getWindow());
    else
        pos = sf::Mouse::getPosition();
    _cursorPos.x = pos.x;
    _cursorPos.y = pos.y;
}

InputState InputManager::getInputState(InputSemantics semanticInput) {
    return _inputStateMap[semanticInput];
}
static InputState takeHigherInputStatePrecedence(InputState firstState, InputState secondState) {
    return std::max(firstState, secondState);
}

bool InputManager::_checkInputRegistered(int inputCode, InputType inputType) {
    switch (inputType) {
        case InputType::keyboard:
            return std::find(_registeredKeyboardInputs.begin(), _registeredKeyboardInputs.end(), inputCode) != _registeredKeyboardInputs.end();
        case InputType::mouse:
            return std::find(_registeredMouseInputs.begin(), _registeredMouseInputs.end(), inputCode) != _registeredMouseInputs.end();
        case InputType::joystick:
            return std::find(_registeredJoystickInputs.begin(), _registeredJoystickInputs.end(), inputCode) != _registeredJoystickInputs.end();
        default:
            return false;
    }
}

bool InputManager::registerInput(int inputCode, InputSemantics semanticInput, InputType type) {
    if (_checkInputRegistered(inputCode, type))
        return false;
    std::vector<std::shared_ptr<InputWrapper>> vec;
    auto it = _inputWrapperMap.find(semanticInput);
    if (it != _inputWrapperMap.end())
        vec = it->second;
    else
        _inputStateMap[semanticInput] = InputState::resting; //new input, so initialize the corresponding state
    std::shared_ptr<InputWrapper> wrapper;
    switch (type) {
        case InputType::keyboard:
            wrapper = std::shared_ptr<InputWrapper>{new KeyboardInputWrapper()};
            break;
        case InputType::joystick:
            wrapper = std::shared_ptr<InputWrapper>{new JoystickInputWrapper{_deviceIndex}};
            break;
        case InputType::mouse:
            wrapper = std::shared_ptr<InputWrapper>{new MouseInputWrapper()};
            break;
        default:
            return false;
    }
    wrapper.get()->setInput(inputCode);
    vec.push_back(wrapper);
    _inputWrapperMap[semanticInput] = vec;
    return true;
}
InputState InputWrapper::getInputState() const {
    return _inputState;
}
void InputWrapper::setInput(int inputCode) {
    _myInput = inputCode;
}
void InputWrapper::_interpretInputState(bool inputPressed) {
    switch (_inputState) {
        case InputState::resting:
            if (inputPressed)
                return _processNewState(InputState::pressed);                
            break;
        case InputState::pressed:
            if (!inputPressed)
                return _processNewState(InputState::released);
            else
                return _processNewState(InputState::held);
        case InputState::held:
            if (!inputPressed)
                return _processNewState(InputState::released);
            break;
        case InputState::released:
            if (inputPressed)
                return _processNewState(InputState::pressed);
            else
                return _processNewState(InputState::resting);
        default:
            return _processNewState(InputState::resting);
    }
}
void InputWrapper::_processNewState(InputState newState) {
    _inputState = newState;
}
void KeyboardInputWrapper::setInput(sf::Keyboard::Key keyCode) {
    _myInput = keyCode;
}
void KeyboardInputWrapper::processInput() {
    _interpretInputState(sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(_myInput)));
}
void MouseInputWrapper::setInput(sf::Mouse::Button buttonCode) {
    _myInput = buttonCode;
}
void MouseInputWrapper::processInput() {
    _interpretInputState(sf::Mouse::isButtonPressed(static_cast<sf::Mouse::Button>(_myInput)));
}
JoystickInputWrapper::JoystickInputWrapper(): _deviceId(0) {}
JoystickInputWrapper::JoystickInputWrapper(int deviceId) :
    _deviceId(deviceId)
{}
void JoystickInputWrapper::processInput() {
    _interpretInputState(sf::Joystick::isButtonPressed(_deviceId, _myInput));
}

