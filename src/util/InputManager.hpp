#include <unordered_map>
#include <Keyboard.hpp>
#include <Mouse.hpp>
#include <Joystick.hpp>
#include <Window.hpp>
#include <stdio.h>
#include <vector>
#include <memory>

enum class InputSemantics {
    accept,
    back,
    up,
    down,
    left,
    right
};

enum class InputState {
    resting,
    released,
    pressed,
    held
};

enum class InputType {
    keyboard,
    mouse,
    joystick
};

class InputWrapper {
    public:
        InputState getInputState() const;
        virtual void processInput() = 0;
        virtual void setInput(int inputCode);
    protected:
        void _interpretInputState(bool inputPressed);
        void _processNewState(InputState newState);
        InputState _inputState;
        int _myInput;
};
class KeyboardInputWrapper : public InputWrapper {
    public:
        void setInput(sf::Keyboard::Key keyCode);
        void processInput() override;
};

class MouseInputWrapper: public InputWrapper {
    public:
        void setInput(sf::Mouse::Button);
        void processInput() override;
};
class JoystickInputWrapper: public InputWrapper {
    public:
        JoystickInputWrapper();
        JoystickInputWrapper(int deviceId);
        void processInput() override;
    private:
        int _deviceId;
};

static InputState takeHigherInputStatePrecedence(InputState firstState, InputState secondState);

class InputManager {
    public:
        InputManager();
        InputManager(int deviceIndex);
        InputManager(int deviceIndex, const sf::Window* window);
        InputManager(const InputManager&) = delete;
        InputManager& operator=(const InputManager&) = delete;
        ~InputManager() = default;
        bool registerInput(int inputCode, InputSemantics semanticInput, InputType type);
        float getCursorX() const;
        float getCursorY() const;
        void processInput();
        void setWindow(const sf::Window* window);
        const sf::Window* getWindow() const;
        InputState getInputState(InputSemantics semanticInput);
    protected:
        sf::Vector2f _cursorPos;
        int _deviceIndex;
        virtual void _setCursorPosition();
    private:
        const sf::Window* _myWindow;
        bool _checkInputRegistered(int inputCode, InputType inputType);
        std::unordered_map<InputSemantics, std::vector<std::shared_ptr<InputWrapper>>> _inputWrapperMap;
        std::unordered_map<InputSemantics, InputState> _inputStateMap;
        std::vector<int> _registeredJoystickInputs;
        std::vector<sf::Mouse::Button> _registeredMouseInputs;
        std::vector<sf::Keyboard::Key> _registeredKeyboardInputs;
};