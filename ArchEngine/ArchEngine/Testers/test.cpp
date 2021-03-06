/*===========================================================================*
* Arch Engine - "Testers/test.cpp"                                          *
*                                                                           *
* This is a tester file. It's purpose is to test some functionalities, thus *
* should be removed from releases.                                          *
* Expect some hard coded, bizarre stuff. You've been warned ;)              *
*                                                                           *
* Marcelo de Matos Menezes - marcelodmmenezes@gmail.com                     *
* Created: 30/04/2018                                                       *
* Last Modified: 21/06/2018                                                 *
*===========================================================================*/


#define TEST
#if defined (TEST)

#include "../Config/engineMacros.hpp"


#include "../Core/engine.hpp"
#include "../Graphics/assimpLoader.hpp"
#include "../Graphics/graphicsManager.hpp"
#include "../Graphics/terrainGenerator.hpp"
#include "../Graphics/thirdPersonCamera.hpp"
#include "../GUI/guiManager.hpp"
#include "../GUI/renderableComponent.hpp"
#include "../GUI/pushButton.hpp"
#include "../OS/inputManager.hpp"
#include "../Physics/physicsManager.hpp"
#include "../Utils/serviceLocator.hpp"

#include <cstdlib>
#include <sstream>
#include <string>


using namespace Core;
using namespace Graphics;
using namespace GUI;
using namespace OS;
using namespace Physics;
using namespace Utils;


enum GameInputActions {
	CHANGE_PLAYER = 0,
	CHANGE_CAMERA_ACTION = 1,
	QUIT_ACTION = 2
};

enum GameInputStates {
	TEST_STATE,
	MOUSE_CLICK_STATE = 6
};


void loadData();
void onContextEvent(EventPtr e);
void onInputActionEvent(EventPtr e);
void onInputStateEvent(EventPtr e);
void onInputRangeEvent(EventPtr e);
void onInputMouseMoved(EventPtr e);
void onCollisionEvent(EventPtr e);
void onClosestRayTestEvent(EventPtr e);
void onAllRayTestEvent(EventPtr e);
void onButtonStateEvent(EventPtr e);
void onWindowResizeEvent(EventPtr e);
void onLoopFinishedEvent(EventPtr e);


ThirdPersonCamera tpcamera(15.0f);

DebugCamera debug_camera(
	glm::vec3(0.0f, 15.0f, 15.0f),
	glm::vec3(0.0f, 0.0f, -1.0f)
);

DebugDrawer* dd;


int main(int argc, char* argv[]) {
	srand(time(nullptr));

	Engine::startLoggingServices();

	if (Engine::getInstance().initialize("../../ArchEngine/Testers/"
		"core_engine_test_engine_config.lua")) {
		EventListener listener;

		loadData();

		listener.bind<&onContextEvent>();
		EventManager::getInstance().addListener(
			listener, EVENT_INPUT_CONTEXT);

		listener.bind<&onInputActionEvent>();
		EventManager::getInstance().addListener(
			listener, EVENT_INPUT_ACTION);

		listener.bind<&onInputStateEvent>();
		EventManager::getInstance().addListener(
			listener, EVENT_INPUT_STATE);

		listener.bind<&onInputRangeEvent>();
		EventManager::getInstance().addListener(
			listener, EVENT_INPUT_RANGE);

		listener.bind<&onInputMouseMoved>();
		EventManager::getInstance().addListener(
			listener, EVENT_MOUSE_MOVED);

		listener.bind<&onCollisionEvent>();
		EventManager::getInstance().addListener(
			listener, EVENT_COLLISION);

		listener.bind<&onClosestRayTestEvent>();
		EventManager::getInstance().addListener(
			listener, EVENT_RAY_TEST_CLOSEST);

		listener.bind<&onAllRayTestEvent>();
		EventManager::getInstance().addListener(
			listener, EVENT_RAY_TEST_ALL);

		listener.bind<&onButtonStateEvent>();
		EventManager::getInstance().addListener(
			listener, EVENT_BUTTON_STATE);

		listener.bind<&onWindowResizeEvent>();
		EventManager::getInstance().addListener(
			listener, EVENT_WINDOW_RESIZE);

		listener.bind<&onLoopFinishedEvent>();
		EventManager::getInstance().addListener(
			listener, EVENT_LOOP_FINISHED);

		InputManager::getInstance().pushContext("writing_context");
		InputManager::getInstance().pushContext("test1");
		InputManager::getInstance().prioritize("test1", true);

		Engine::getInstance().run();
	}
#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_ERROR
	else {
		ServiceLocator::getFileLogger()->log<LOG_ERROR>(
			"Failed to initialize ArchEngine");
	}
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_ERROR

	Engine::getInstance().exit();

	delete dd;
}

void loadData() {
	//PhysicsManager::getInstance().setGravity(glm::vec3(0.0f, 0.0f, 0.0f));

	GraphicsManager::getInstance().setProjectionMatrix(
		glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 500.0f));

	unsigned tpcam = GraphicsManager::getInstance().addCamera(&tpcamera);
	unsigned dbcam = GraphicsManager::getInstance().addCamera(&debug_camera);
	GraphicsManager::getInstance().setActiveCamera(tpcam);

	unsigned line_shader = GraphicsManager::getInstance().addShader(
		"../../ArchEngine/Shaders/linevs.glsl",
		"../../ArchEngine/Shaders/linefs.glsl"
	);

	unsigned dd_shader = GraphicsManager::getInstance().addShader(
		"../../ArchEngine/Shaders/debugdrawvs.glsl",
		"../../ArchEngine/Shaders/debugdrawfs.glsl"
	);

	dd = new DebugDrawer(dd_shader);
	//dd->setDebugMode(btIDebugDraw::DBG_DrawAabb);
	//dd->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	dd->setDebugMode(btIDebugDraw::DBG_DrawAabb | btIDebugDraw::DBG_DrawWireframe);
	PhysicsManager::getInstance().setDebugDrawer(dd);

	unsigned obj_shader = GraphicsManager::getInstance().addShader(
		"../../ArchEngine/Shaders/shadowvs.glsl",
		"../../ArchEngine/Shaders/shadowfs.glsl"
	);

	unsigned normal_shader = GraphicsManager::getInstance().addShader(
		"../../ArchEngine/Shaders/nmshadowvs.glsl",
		"../../ArchEngine/Shaders/nmshadowfs.glsl"
	);

	unsigned dir_depth_shader = GraphicsManager::getInstance().addShader(
		"../../ArchEngine/Shaders/depthmapvs.glsl",
		"../../ArchEngine/Shaders/depthmapfs.glsl"
	);

	unsigned point_depth_shader = GraphicsManager::getInstance().addShader(
		"../../ArchEngine/Shaders/cubedepthmapvs.glsl",
		"../../ArchEngine/Shaders/cubedepthmapgs.glsl",
		"../../ArchEngine/Shaders/cubedepthmapfs.glsl"
	);
	/*
	unsigned normal_debug_shader = GraphicsManager::getInstance().addShader(
		"../../ArchEngine/Shaders/normaldebugvs.glsl",
		"../../ArchEngine/Shaders/normaldebuggs.glsl",
		"../../ArchEngine/Shaders/normaldebugfs.glsl"
	);
	*/
	unsigned color_shader = GraphicsManager::getInstance().addShader(
		"../../ArchEngine/Shaders/colorvs.glsl",
		"../../ArchEngine/Shaders/colorfs.glsl"
	);

	GraphicsManager::getInstance().getShader(color_shader)
		->setVec3("u_color", glm::vec3(1.0f, 1.0f, 1.0f));

	glm::vec3 plight_pos(0.0f, 50.0f, 0.0f);
	glm::mat4 plight_proj =
		glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 1000.0f);
	/*
	PointLight plight = {
	plight_pos,
	64.0f,
	1.0f,
	//0.0007,
	//0.00014,
	0.027,
	0.0028,
	glm::vec3(0.1f, 0.1f, 0.1f),
	glm::vec3(1.0f, 1.0f, 1.0f),
	glm::vec3(1.0f, 1.0f, 1.0f),
	true,
	plight_proj,
	glm::lookAt(plight_pos, plight_pos +
	glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
	glm::lookAt(plight_pos, plight_pos +
	glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
	glm::lookAt(plight_pos, plight_pos +
	glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
	glm::lookAt(plight_pos, plight_pos +
	glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
	glm::lookAt(plight_pos, plight_pos +
	glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
	glm::lookAt(plight_pos, plight_pos +
	glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
	point_depth_shader,
	2000,
	2000,
	1000.0f
	};

	GraphicsManager::getInstance().addPointLight(plight);
	*/
	
	DirectionalLight dlight = {
		glm::vec3(-0.6f, -1.0f, -0.5f),
		32.0f,
		glm::vec3(0.1f, 0.1f, 0.05f),
		glm::vec3(0.8f, 0.8f, 0.4f),
		glm::vec3(0.8f, 0.8f, 0.4f),
		true,
		//glm::ortho(-150.0f, 150.0f, -150.0f, 150.0f, 0.1f, 1000.0f),
		glm::ortho(-1000.0f, 1000.0f, -1000.0f, 1000.0f, 0.1f, 1000.0f),
		//glm::lookAt(glm::vec3(60.0, 100.0, 50.0f), glm::vec3(-0.6f, -1.0f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f)),
		glm::lookAt(glm::vec3(300.0, 500.0, 250.0f), glm::vec3(-0.6f, -1.0f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f)),
		dir_depth_shader,
		12000,
		12000
	};

	GraphicsManager::getInstance().addDirectionalLight(dlight);
	

	std::vector<unsigned> loaded_meshes_ids;
	AssimpLoader loader;

	TerrainGenerator tg;
	auto terrain = tg.genHeightMapTerrain(128, 128, 4.0f, 4.0f, 100.0f, 16,
		"../../../../GameEngineLearning/assets/miscTextures/heightMaps/height_map3.png");

	loader.importScene(
		"../../../../GameEngineLearning/assets/plane/plane_terrain.obj",
		aiPostProcessSteps(
			aiProcess_GenSmoothNormals |
			//aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType |
			aiProcess_FlipUVs
		),
		loaded_meshes_ids
	);

	loaded_meshes_ids.clear();
	loaded_meshes_ids.push_back(terrain.first);

	g_entities.push_back(
		{
			obj_shader,
			loaded_meshes_ids,
			std::vector<unsigned>(),
			std::vector<glm::mat4>(1, glm::mat4(1.0f)),
			true
		}
	);

	int physics_id = 0;
	std::vector<unsigned> bodies;

	/*
	loaded_meshes_ids.clear();
	loader.importScene(
	"../../../../GameEngineLearning/assets/sponza/mergedSponza.obj",
	aiPostProcessSteps(
	aiProcess_GenSmoothNormals |
	aiProcess_CalcTangentSpace |
	aiProcess_Triangulate |
	aiProcess_JoinIdenticalVertices |
	aiProcess_SortByPType |
	aiProcess_FlipUVs
	),
	loaded_meshes_ids
	);

	g_entities.push_back(
	{
	normal_shader,
	loaded_meshes_ids,
	bodies,
	std::vector<glm::mat4>(loaded_meshes_ids.size(), glm::scale(glm::mat4(1.0f), glm::vec3(0.1f, 0.1f, 0.1f))),
	true
	}
	);
	*/
	
	loaded_meshes_ids.clear();
	loader.importScene(
		"../../../../GameEngineLearning/assets/cube/cube_wooden.obj",
		aiPostProcessSteps(
			aiProcess_GenSmoothNormals |
			//aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType |
			aiProcess_FlipUVs
		),
		loaded_meshes_ids
	);

	bodies.clear();
	bodies.resize(loaded_meshes_ids.size());

	for (unsigned i = 1; i < 5; i++) {
		g_entities.push_back(
			{
				obj_shader,
				loaded_meshes_ids,
				bodies,
				std::vector<glm::mat4>(loaded_meshes_ids.size(),
				glm::mat4(1.0f)),
				true
			}
		);

		for (auto& it : g_entities[g_entities.size() - 1].physics_objects)
			it = PhysicsManager::getInstance().addCube(
				physics_id++,
				glm::vec3(1.0f, 1.0f, 1.0f),
				glm::vec3((rand() % 100 - 50) * 1.0f, i * 100.0f,
				(rand() % 100 - 50) * 1.0f),
				1.0f);
	}

	loaded_meshes_ids.clear();
	loader.importScene(
		"../../../../GameEngineLearning/assets/sphere/sphere_brick.obj",
		aiPostProcessSteps(
			aiProcess_GenSmoothNormals |
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType |
			aiProcess_FlipUVs
		),
		loaded_meshes_ids
	);

	bodies.clear();
	bodies.resize(loaded_meshes_ids.size());

	for (unsigned i = 1; i < 5; i++) {
		g_entities.push_back(
			{
				normal_shader,
				loaded_meshes_ids,
				bodies,
				std::vector<glm::mat4>(loaded_meshes_ids.size(),
				glm::mat4(1.0f)),
				true
			}
		);

		for (auto& it : g_entities[g_entities.size() - 1].physics_objects)
			it = PhysicsManager::getInstance().addSphere(
				physics_id++,
				1.0f,
				glm::vec3((rand() % 100 - 50) * 1.0f, i * 100.0f,
				(rand() % 100 - 50) * 1.0f),
				1.0f);
	}

	//Engine::getInstance().releaseMouse();
	//PhysicsManager::getInstance().setGravity(glm::vec3(0.0f, 0.0f, 0.0f));

	unsigned quad_shader = GraphicsManager::getInstance().addShader(
		"../../ArchEngine/Shaders/quadvs.glsl",
		"../../ArchEngine/Shaders/quadfs.glsl"
	);

	unsigned button_shader = GraphicsManager::getInstance().addShader(
		"../../ArchEngine/Shaders/buttonvs.glsl",
		"../../ArchEngine/Shaders/buttonfs.glsl"
	);

	Engine::getInstance().releaseMouse();
}

void onContextEvent(EventPtr e) {
	auto evnt = std::static_pointer_cast<InputContextEvent>(e);

	if (evnt->getState()) {
		// Context pushed
		std::cout << "\nCONTEXT PUSHED: " << evnt->getName() <<
			"\n" << std::endl;
	}
	else {
		// Context popped
		std::cout << "\nCONTEXT POPPED: " << evnt->getName() <<
			"\n" << std::endl;
	}
}

int act_cm = 0;
int player = 1;
int context_priority = 0;

void onInputActionEvent(EventPtr e) {
	auto evnt = std::static_pointer_cast<InputActionEvent>(e);

	switch (evnt->getValue()) {
	case GameInputActions::CHANGE_PLAYER:
		if (context_priority == 0) {
			context_priority = 1;
			InputManager::getInstance().prioritize("writing_context", true);
			Engine::getInstance().releaseMouse();
		}
		else {
			context_priority = 0;
			InputManager::getInstance().prioritize("test1", true);
			Engine::getInstance().captureMouse();
		}

		if (player == 1)
			player = 5;
		else
			player = 1;

		break;
	case GameInputActions::CHANGE_CAMERA_ACTION:
		GraphicsManager::getInstance().setActiveCamera(act_cm);

		if (act_cm == 0)
			act_cm = 1;
		else
			act_cm = 0;
		break;
	case GameInputActions::QUIT_ACTION:
		EventPtr evnt = std::make_shared<CoreQuitEvent>(CoreQuitEvent());
		EventManager::getInstance().sendEvent(evnt);
		break;
	}

	if (context_priority == 1) {
		if (evnt->getValue() == '1')
			dynamic_cast<PushButton*>(
				GUIManager::getInstance().getControl("xablau"))
					->justifyLeft();
		else if (evnt->getValue() == '2')
			dynamic_cast<PushButton*>(
				GUIManager::getInstance().getControl("xablau"))
					->centralize();
		else if (evnt->getValue() == '3')
			dynamic_cast<PushButton*>(
				GUIManager::getInstance().getControl("xablau"))
					->justifyRight();
	}
}

bool mouse_left_released = false;
bool mouse_left_clicked = false;
bool mouse_right_clicked = false;

void onInputStateEvent(EventPtr e) {
	auto evnt = std::static_pointer_cast<InputStateEvent>(e);

	auto camera = GraphicsManager::getInstance().getActiveCamera();

	auto trn = g_entities[player].transforms[0];
	auto dir = glm::vec3(trn[3][0], trn[3][1], trn[3][2]) - camera->getPosition();
	auto up = glm::vec3(0.0f, 1.0f, 0.0f);
	auto left = glm::cross(up, glm::vec3(dir.x, 0.0f, dir.z));

	mouse_left_released = false;

	switch (evnt->getValue()) {
	case 0:
		if (auto cm = dynamic_cast<DebugCamera*>(camera))
			cm->move(CAMERA_FORWARD);
		else if (auto cm = dynamic_cast<ThirdPersonCamera*>(camera))
			PhysicsManager::getInstance().applyCentralForce(
				player - 1, glm::normalize(dir), 20.0f);
		break;
	case 1:
		if (auto cm = dynamic_cast<DebugCamera*>(camera))
			cm->move(CAMERA_BACKWARD);
		else if (auto cm = dynamic_cast<ThirdPersonCamera*>(camera))
			PhysicsManager::getInstance().applyCentralForce(
				player - 1, glm::normalize(-dir), 20.0f);
		break;
	case 2:
		if (auto cm = dynamic_cast<DebugCamera*>(camera))
			cm->move(CAMERA_LEFT);
		else if (auto cm = dynamic_cast<ThirdPersonCamera*>(camera))
			PhysicsManager::getInstance().applyCentralForce(
				player - 1, glm::normalize(left), 20.0f);
		break;
	case 3:
		if (auto cm = dynamic_cast<DebugCamera*>(camera))
			cm->move(CAMERA_RIGHT);
		else if (auto cm = dynamic_cast<ThirdPersonCamera*>(camera))
			PhysicsManager::getInstance().applyCentralForce(
				player - 1, glm::normalize(-left), 20.0f);
		break;
	case 4:
		if (auto cm = dynamic_cast<DebugCamera*>(camera))
			cm->move(CAMERA_UP);
		else if (auto cm = dynamic_cast<ThirdPersonCamera*>(camera))
			PhysicsManager::getInstance().applyCentralForce(
				player - 1, glm::normalize(up), 20.0f);
		break;
	case 5:
		if (auto cm = dynamic_cast<DebugCamera*>(camera))
			cm->move(CAMERA_DOWN);
		else if (auto cm = dynamic_cast<ThirdPersonCamera*>(camera))
			PhysicsManager::getInstance().applyCentralForce(
				player - 1, glm::normalize(-up), 20.0f);
		break;
	case 6:
		if (auto cm = dynamic_cast<DebugCamera*>(camera)) {
			if (evnt->isOver())
				cm->setSpeed(10.0f);
			else
				cm->setSpeed(25.0f);
		}
		break;
	case 7:
		if (evnt->isOver()) {
			mouse_left_released = true;
			mouse_left_clicked = false;
		}
		else
			mouse_left_clicked = true;
		break;
	case 8:
		if (evnt->isOver())
			mouse_right_clicked = false;
		else
			mouse_right_clicked = true;
		break;
	}
}

void onInputRangeEvent(EventPtr e) {
	auto evnt = std::static_pointer_cast<InputRangeEvent>(e);

	auto camera = dynamic_cast<DebugCamera*>(
		GraphicsManager::getInstance().getActiveCamera());

	if (camera) {
		Engine::getInstance().releaseMouse();

		switch (evnt->getValue().m_range) {
		case 0:
			if (mouse_right_clicked)
				camera->look((float)evnt->getValue().m_value, 0.0f);
			break;
		case 1:
			if (mouse_right_clicked)
				camera->look(0.0f, (float)evnt->getValue().m_value);
			break;
		}
	}
	else {
		Engine::getInstance().captureMouse();

		auto tpcamera = dynamic_cast<ThirdPersonCamera*>(
			GraphicsManager::getInstance().getActiveCamera());

		switch (evnt->getValue().m_range) {
		case 0:
			//if (mouse_right_clicked)
			tpcamera->calcYaw(evnt->getValue().m_value);
			break;
		case 1:
			//if (mouse_right_clicked)
			tpcamera->calcPitch(evnt->getValue().m_value);
			break;
		case 2:
			tpcamera->increaseDistanceFromTarget(
				30 * evnt->getValue().m_value);
		}
	}
}

bool picking_constraint = false;

int g_x, g_y;

void onInputMouseMoved(EventPtr e) {
	auto evnt = std::static_pointer_cast<InputMouseMoved>(e);

	evnt->getValues(g_x, g_y);

	if (mouse_left_clicked && !picking_constraint) {
		PhysicsManager::getInstance().createPickingConstraint(
			g_x, g_y, 1000.0f, true);
		picking_constraint = true;
	}
	else if (!mouse_left_clicked && picking_constraint) {
		PhysicsManager::getInstance().removePickingConstraint();
		picking_constraint = false;
	}
}

void onCollisionEvent(EventPtr e) {
	auto evnt = std::static_pointer_cast<CollisionEvent>(e);

	long id1, id2;
	evnt->getObjectIds(id1, id2);

	//std::cout << "Collision: " << id1 << " " << id2 << std::endl;
}

void onClosestRayTestEvent(EventPtr e) {
	auto evnt = std::static_pointer_cast<ClosestRayTestEvent>(e);
	std::cout << "ClosestRayTest: " << evnt->getObjectId() << std::endl;
}

void onAllRayTestEvent(EventPtr e) {
	auto evnt = std::static_pointer_cast<AllRayTestEvent>(e);

	std::cout << "AllRayTest: ";

	for (auto& it : evnt->m_obj_ids)
		std::cout << it << " ";

	std::cout << std::endl;
}

bool g_debug_draw = false;

void onButtonStateEvent(EventPtr e) {
	auto evnt = std::static_pointer_cast<ButtonStateEvent>(e);

	switch (evnt->getButtonId()) {
	case 0:
		if (!evnt->getState())
			g_debug_draw = !g_debug_draw;
		break;

	}
}

int g_screen_width = 800;
int g_screen_height = 600;

void onWindowResizeEvent(EventPtr e) {
	auto evnt = std::static_pointer_cast<WindowResizeEvent>(e);
	evnt->getSize(g_screen_width, g_screen_height);
}

void onLoopFinishedEvent(EventPtr e) {
	auto evnt = std::static_pointer_cast<LoopFinishedEvent>(e);

	if (g_debug_draw)
		PhysicsManager::getInstance().debugDraw();

	PhysicsManager::getInstance().pickingMotion(g_x, g_y, 1000.0f);

	std::stringstream ss;
	ss << "SBF: " <<
		std::setprecision(6) << evnt->getDeltaTime() << "\nFPS: " <<
		std::setprecision(3) << " " << evnt->getFrameRate();
	
	dynamic_cast<WritableComponent*>
		(GUIManager::getInstance().getControl("fpscounter"))->write(ss.str());

	auto xablau = g_entities[player].transforms[0];
	tpcamera.update(glm::vec3(xablau[3][0], xablau[3][1], xablau[3][2]));
}


#endif	// TEST