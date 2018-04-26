/*===========================================================================*
 * Arch Engine - "Core/inputManager.hpp"                                     *
 *                                                                           *
 * Responsible for creating and managing all input associated stuff.         *
 * There's already too much layers of abstraction in the input system, due   *
 * to all the key mapping functionalities, so I opted for using a singleton  *
 * approach instead of using the ServiceLocator.                             *
 *                                                                           *
 * Based in:                                                                 *
 * - (https://www.gamedev.net/articles/programming/                          *
 *    general-and-gameplay-programming/                                      *
 *    designing-a-robust-input-handling-system-for-games-r2975)              *
 *                                                                           *
 * Marcelo de Matos Menezes - marcelodmmenezes@gmail.com                     *
 * Created: 23/04/2018                                                       *
 * Last Modified: 25/04/2018                                                 *
 *===========================================================================*/


#ifndef CORE_INPUT_MANAGER_HPP
#define CORE_INPUT_MANAGER_HPP


#include "inputContext.hpp"
#include "../Script/luaScript.hpp"
#include "../Utils/serviceLocator.hpp"

#if defined(__unix__)
#include <SDL2/SDL.h>
#elif defined(_MSC_VER)
#include <SDL.h>
#endif

#include <fstream>
#include <map>
#include <set>
#include <string>
#include <vector>


namespace Core {
	struct CurrentInput {
		// Sets for O(log(n)) access
		std::set<InputAction> m_actions;
		std::set<InputState> m_states;
		std::set<RangeInfo> m_ranges;

		void removeAction(InputAction action);
		void removeState(InputState state);
		void removeRange(RangeInfo range);
	};

	class InputManager {
	public:
		~InputManager();

		InputManager(const InputManager&) = delete;
		void operator=(const InputManager&) = delete;

		static InputManager& getInstance();

		bool initialize(const std::string& path);

		// Gathers and maps the events from the operating system
		void update(bool& running);

		// Adds or removes an active input context
		void contextOn(const std::string& context);
		void contextOff(const std::string& context);

		// Input gathering and clearing
		void setKeyState(SDL_Keycode key, bool pressed, bool prev_pressed);
		void setModState(SDL_Keymod mod, bool pressed, bool prev_pressed);
		void setMBState(MouseButton mb, bool pressed, bool prev_pressed);
		void setAxisValue(ControllerAxis axis, double value);
		void clearInput();

		// Sends the CurrentInput configuration to the engine
		void dispatch();

	private:
		InputManager();

		// Actions and states triggers
		bool triggerKeyAction(SDL_Keycode key, InputAction& action);
		bool triggerModAction(SDL_Keymod mod, InputAction& action);
		bool triggerMBAction(MouseButton mb, InputAction& action);

		bool triggerKeyState(SDL_Keycode key, InputState& state);
		bool triggerModState(SDL_Keymod mod, InputState& state);
		bool triggerMBState(MouseButton mb, InputState& state);

		void consumeKey(SDL_Keycode key);
		void consumeMod(SDL_Keymod mod);
		void consumeMB(MouseButton mb);

		// Force actions, states or ranges to happen
		void forceAction(InputAction action);
		void forceState(InputState state);
		void forceRangeInfo(RangeInfo range);

		// Mouse control variables
		// Auxiliary to make right calc when mouse first enters the screen
		bool m_mouse_first; 
		int m_mouse_last_x;
		int m_mouse_last_y;

		// List of sdl key modifiers for iteration,
		// for it's not a sequential enumeration.
		SDL_Keymod m_sdl_modifiers[12] = {
			KMOD_LSHIFT, KMOD_RSHIFT, KMOD_LCTRL, KMOD_RCTRL,
			KMOD_LALT, KMOD_RALT, KMOD_LGUI, KMOD_RGUI,
			KMOD_NUM, KMOD_CAPS, KMOD_MODE, KMOD_RESERVED
		};

		// Stores the previous key states (true = pressed, false = released)
		std::map<SDL_Keycode, bool> m_key_prev_state;
		// Stores the previous modifier states
		std::map<SDL_Keymod, bool> m_mod_prev_state;
		// Stores the previous mouse button states
		std::map<MouseButton, bool> m_mb_prev_state;

		// Input contexts
		std::vector<InputContext> m_contexts;
		// Maps the context name to its index in m_contexts
		std::map<std::string, unsigned> m_mapped_contexts;
		// The indices of the active contexts
		std::vector<unsigned> m_active_contexts;
		// Stores the actions, states and ranges that happen at each frame
		CurrentInput m_current_input;
	};
}


#endif	// CORE_INPUT_MANAGER_HPP