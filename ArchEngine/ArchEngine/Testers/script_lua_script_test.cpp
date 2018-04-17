/*===========================================================================*
 * Arch Engine - "Testers/script_lua_script_test.cpp"                        *
 *                                                                           *
 * This is a tester file. It's purpose is to test some functionalities, thus *
 * should be removed from releases.                                          *
 * Expect some hard coded, bizarre stuff. You've been warned ;)              *
 *                                                                           *
 * Marcelo de Matos Menezes - marcelodmmenezes@gmail.com                     *
 * Created: 15/04/2018                                                       *
 * Last Modified: 16/04/2018                                                 *
 *===========================================================================*/


#include "../Core/engineMacros.hpp"
 
 // Check if this tester is active
#if defined(ARCH_ENGINE_SCRIPT_LUA_SCRIPT_TEST)

#include "../Script/luaScript.hpp"
#include "../Utils/serviceLocator.hpp"

#include <sstream>
#include <string>


using namespace Script;
using namespace Utils;


void test1(); // Tests LuaScript basic functionalities
void test2(); // Tests sharing lua_State between scripts
void startLoggingService();


int main(int argc, char* argv[]) {
	try {
		startLoggingService();

		ServiceLocator::getFileLogger()->log<LOG_INFO>(
			"Starting tests");

		ServiceLocator::getFileLogger()->log<LOG_INFO>(
			"Testing LuaScript basic functionalities");
		test1();
		ServiceLocator::getFileLogger()->log<LOG_INFO>(
			"Finished first test\n\
----------------------------------------------------------------------------");

		ServiceLocator::getFileLogger()->log<LOG_INFO>(
			"Testing shared lua_States");
		test2();
		ServiceLocator::getFileLogger()->log<LOG_INFO>(
			"Finished first test\n\
----------------------------------------------------------------------------");
	}
	catch (std::runtime_error) {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


void test1() {
	lua_State* lua = luaL_newstate();
	luaL_openlibs(lua);

	LuaScript script;
	script.initialize(lua,
		"../../ArchEngine/Testers/script_lua_script_test1.lua");

	float x = script.get<float>(lua, "player.pos.x");
	float y = script.get<float>(lua, "player.pos.y");

	std::string file_name = script.get<std::string>(lua, "player.file_name");

	int hp = script.get<int>(lua, "player.health_points");

	std::stringstream ss;
	ss << "\n\nPos: (" << x << ", " << y << ")\n";
	ss << "File name: " << file_name << "\n";
	ss << "Health points: " << hp << std::endl;

	ServiceLocator::getFileLogger()->log<LOG_INFO>(ss);

	script.destroy();

	lua_close(lua);
}

void test2() {
	lua_State* lua = luaL_newstate();
	luaL_openlibs(lua);

	LuaScript player_script, enemy_script;
	player_script.initialize(lua,
		"../../ArchEngine/Testers/script_lua_script_test1.lua");
	enemy_script.initialize(lua,
		"../../ArchEngine/Testers/script_lua_script_test2.lua");

	float p_x = player_script.get<float>(lua, "player.pos.x");
	float p_y = player_script.get<float>(lua, "player.pos.y");

	float e_x = enemy_script.get<float>(lua, "enemy.pos.x");
	float e_y = enemy_script.get<float>(lua, "enemy.pos.y");

	std::string p_file_name = player_script.get<std::string>(lua,
		"player.file_name");

	std::string e_file_name = enemy_script.get<std::string>(lua,
		"enemy.file_name");

	int p_hp = player_script.get<int>(lua, "player.health_points");

	int e_hp = enemy_script.get<int>(lua, "enemy.health_points");

	std::stringstream ss;
	ss << "\n\nPlayer position: (" << p_x << ", " << p_y << ")\n";
	ss << "Player file name: " << p_file_name << "\n";
	ss << "Player health points: " << p_hp << "\n\n";
	ss << "\n\nEnemy position: (" << e_x << ", " << e_y << ")\n";
	ss << "Enemy file name: " << e_file_name << "\n";
	ss << "Enemy health points: " << e_hp << "\n\n";

	ServiceLocator::getFileLogger()->log<LOG_INFO>(ss);

	player_script.destroy();
	enemy_script.destroy();

	lua_close(lua);
}

void startLoggingService() {
	std::shared_ptr<Logger<ConsoleLogPolicy>>
		console_logger(new Logger<ConsoleLogPolicy>(""));

	std::shared_ptr<Logger<FileLogPolicy>>
		file_logger(new Logger<FileLogPolicy>("log.txt"));

	console_logger->setThreadName("mainThread");
	file_logger->setThreadName("mainThread");

	ServiceLocator::provideConsoleLogger(console_logger);
	ServiceLocator::provideFileLogger(file_logger);

	ServiceLocator::getConsoleLogger()->log<LOG_INFO>(
		"Logging Systems online...");
	ServiceLocator::getFileLogger()->log<LOG_INFO>(
		"Logging Systems online...");
}


#endif	// ARCH_ENGINE_SCRIPT_LUA_SCRIPT_TEST