/*===========================================================================*
 * Arch Engine - "Graphics/graphicsManager.cpp"                              *
 *                                                                           *
 * TODO: description                                                         *
 *                                                                           *
 * Marcelo de Matos Menezes - marcelodmmenezes@gmail.com                     *
 * Created: 12/05/2018                                                       *
 * Last Modified: 19/05/2018                                                 *
 *===========================================================================*/


#include "graphicsManager.hpp"


using namespace Script;
using namespace Utils;


//------------------------------------------------------------ Lua Graphics API
int addMesh(lua_State* lua) {
	int argc = lua_gettop(lua);

#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_INFO
	ServiceLocator::getFileLogger()->log<LOG_INFO>(
		"Lua pushing context with " + std::to_string(argc) + " arguments");
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_INFO

#ifndef ARCH_ENGINE_REMOVE_ASSERTIONS
	assert(argc == 1);
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_INFO

	std::string path(lua_tostring(lua, lua_gettop(lua)));
	lua_pop(lua, 1);

	unsigned handle = Graphics::GraphicsManager::getInstance().addMesh(path);

	lua_pushnumber(lua, handle);

	// No values returned to Lua
	return 1;
}

int removeMesh(lua_State* lua) {
	int argc = lua_gettop(lua);

#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_INFO
	ServiceLocator::getFileLogger()->log<LOG_INFO>(
		"Lua popping context with " + std::to_string(argc) + " arguments");
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_INFO

#ifndef ARCH_ENGINE_REMOVE_ASSERTIONS
	assert(argc == 1);
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_INFO

	unsigned handle = (unsigned)(lua_tointeger(lua, lua_gettop(lua)));
	lua_pop(lua, 1);

	Graphics::GraphicsManager::getInstance().removeMesh(handle);

	// No values returned to Lua
	return 0;
}
//-------------------------------------------------------------------------


namespace Graphics {
	GraphicsManager::GraphicsManager() : m_state(CONSTRUCTED) {
#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_DEBUG
		ServiceLocator::getFileLogger()->log<LOG_DEBUG>(
			"GraphicsManager constructor");
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_DEBUG
	}

	GraphicsManager::~GraphicsManager() {
#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_DEBUG
		ServiceLocator::getFileLogger()->log<LOG_DEBUG>(
			"GraphicsManager constructor");
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_DEBUG
	}

	GraphicsManager& GraphicsManager::getInstance() {
		static GraphicsManager instance;
		return instance;
	}

	bool GraphicsManager::initialize(const glm::vec4& color) {
		// TODO
		glClearColor(color.r, color.g, color.b, color.a);

		return true;
	}

	bool GraphicsManager::initializeFromConfigFile(const std::string& path) {
		// TODO
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		return initialize(glm::vec4(0.05f, 0.08f, 0.07f, 1.0f));
	}

	void GraphicsManager::update(float delta_time) {
		// TODO
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//---------------------------------------------------------------- TEST

		m_cameras[0].m_delta_time = delta_time;

		for (auto& it : m_shaders) {
			it.bind();
			it.setMat4("u_projection", glm::perspective(
				glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f));
			it.setMat4("u_view", m_cameras[0].getViewMatrix());

			for (int i = 0; i < (int)m_meshes.size(); i++) {
				if (m_meshes[i].second > 0) {
					it.setMat4("u_model", glm::translate(glm::mat4(1.0f),
						glm::vec3((i - ((int)m_meshes.size() / 2)) * 5, 0.0f, 0.0f)));
					it.update();
					m_meshes[i].first.draw();
				}
			}
		}

		//---------------------------------------------------------------------
	}

	void GraphicsManager::destroy() {
		// TODO
	}

	//----------------------------------------------------------- Add functions
	unsigned GraphicsManager::addCamera(const DebugCamera& camera) {
		m_cameras.push_back(camera);
		return m_cameras.size() - 1;
	}

	unsigned GraphicsManager::addShader(const std::string& vs_path,
		const std::string& fs_path) {
		Shader shader;
		shader.initialize(vs_path, fs_path);
		m_shaders.push_back(shader);
		return m_shaders.size() - 1;
	}

	unsigned GraphicsManager::addShader(const std::string& vs_path,
		const std::string& gs_path, const std::string& fs_path) {
		Shader shader;
		shader.initialize(vs_path, gs_path, fs_path);
		m_shaders.push_back(shader);
		return m_shaders.size() - 1;
	}

	unsigned GraphicsManager::addMesh(const std::string& path) {
		auto it = m_mesh_path_to_handle.find(path);

		Mesh mesh;

		// If the mesh isn't in the vector
		if (it == this->m_mesh_path_to_handle.end()) {
			// TODO: create mesh

			//------------------------------------------------------------ TEST
			std::vector<BasicVertex> vertices;
			BasicVertex vertex;

			vertex.position = glm::vec3(-1.0f, -1.0f, 1.0f);
			vertex.normal = vertex.position;
			vertices.push_back(vertex);
			vertex.position = glm::vec3(1.0f, -1.0f, 1.0f);
			vertex.normal = vertex.position;
			vertices.push_back(vertex);
			vertex.position = glm::vec3(1.0f, 1.0f, 1.0f);
			vertex.normal = vertex.position;
			vertices.push_back(vertex);
			vertex.position = glm::vec3(-1.0f, 1.0f, 1.0f);
			vertex.normal = vertex.position;
			vertices.push_back(vertex);
			vertex.position = glm::vec3(-1.0f, -1.0f, -1.0f);
			vertex.normal = vertex.position;
			vertices.push_back(vertex);
			vertex.position = glm::vec3(1.0f, -1.0f, -1.0f);
			vertex.normal = vertex.position;
			vertices.push_back(vertex);
			vertex.position = glm::vec3(1.0f, 1.0f, -1.0f);
			vertex.normal = vertex.position;
			vertices.push_back(vertex);
			vertex.position = glm::vec3(-1.0f, 1.0f, -1.0f);
			vertex.normal = vertex.position;
			vertices.push_back(vertex);

			std::vector<unsigned> indices = {
				0, 1, 2,
				2, 3, 0,
				3, 2, 6,
				6, 7, 3,
				7, 6, 5,
				5, 4, 7,
				4, 5, 1,
				1, 0, 4,
				4, 0, 3,
				3, 7, 4,
				1, 5, 6,
				6, 2, 1,
			};

			mesh.create(vertices, indices, 0);
			//-----------------------------------------------------------------

			unsigned handle;

			// If there aren't spaces available from previous removes
			if (m_meshes_unused_spaces.empty()) {
				m_meshes.push_back(std::make_pair(std::move(mesh), 1));
				m_mesh_path_to_handle[path] = m_meshes.size() - 1;
				handle = m_meshes.size() - 1;
			}
			else {
				handle = m_meshes_unused_spaces.top();
				m_meshes_unused_spaces.pop();

				m_meshes[handle] = std::make_pair(std::move(mesh), 1);
				m_mesh_path_to_handle[path] = handle;
			}

			return handle;
		}

		// Increases mesh reference
		m_meshes[it->second].second++;
		return it->second;
	}

	unsigned GraphicsManager::addMaterial(const Material& material) {
		m_materials.push_back(material);
		return m_materials.size() - 1;
	}

	unsigned GraphicsManager::addDirectionalLight(
		const DirectionalLight& light) {
		m_directional_lights.push_back(light);
		return m_directional_lights.size() - 1;
	}

	unsigned GraphicsManager::addPointLight(
		const PointLight& light) {
		m_point_lights.push_back(light);
		return m_point_lights.size() - 1;
	}

	unsigned GraphicsManager::addSpotLight(
		const SpotLight& light) {
		m_spot_lights.push_back(light);
		return m_spot_lights.size() - 1;
	}

	//-------------------------------------------------------- Remove functions
	void GraphicsManager::removeCamera(unsigned handle) {
		if (handle >= m_cameras.size()) {
#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_WARNING
			ServiceLocator::getFileLogger()->log<LOG_WARNING>(
				"Attempt to remove Mesh outside boundaries");
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_WARNING
			return;
		}

		m_cameras.erase(m_cameras.begin() + handle);
	}

	void GraphicsManager::removeShader(unsigned handle) {
		if (handle >= m_shaders.size()) {
#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_WARNING
			ServiceLocator::getFileLogger()->log<LOG_WARNING>(
				"Attempt to remove Mesh outside boundaries");
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_WARNING
			return;
		}

		m_shaders[handle].destroy();
		m_shaders.erase(m_shaders.begin() + handle);
	}

	void GraphicsManager::removeMesh(unsigned handle) {
		if (handle >= m_meshes.size()) {
#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_WARNING
			ServiceLocator::getFileLogger()->log<LOG_WARNING>(
				"Attempt to remove Mesh outside boundaries");
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_WARNING
			return;
		}

		if (m_meshes[handle].second > 0)
			m_meshes[handle].second--;
#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_WARNING
		else
			ServiceLocator::getFileLogger()->log<LOG_WARNING>(
				"Attempt to remove Mesh with 0 references");
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_WARNING

		// If no one references the mesh anymore
		if (m_meshes[handle].second == 0) {
			m_meshes[handle].first.destroy();

			// Remove reference from map
			for (auto it = m_mesh_path_to_handle.begin();
				it != m_mesh_path_to_handle.end(); ++it) {
				if (it->second == handle) {
					m_mesh_path_to_handle.erase(it);
					break;
				}
			}

			m_meshes_unused_spaces.push(handle);
		}
	}

	void GraphicsManager::removeMaterial(unsigned handle) {
		if (handle >= m_materials.size()) {
#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_WARNING
			ServiceLocator::getFileLogger()->log<LOG_WARNING>(
				"Attempt to remove Material outside boundaries");
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_WARNING
			return;
		}

		m_materials.erase(m_materials.begin() + handle);
	}

	void GraphicsManager::removeDirectionalLight(unsigned handle) {
		if (handle >= m_directional_lights.size()) {
#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_WARNING
			ServiceLocator::getFileLogger()->log<LOG_WARNING>(
				"Attempt to remove Mesh outside boundaries");
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_WARNING
			return;
		}

		m_directional_lights.erase(m_directional_lights.begin() + handle);
	}

	void GraphicsManager::removePointLight(unsigned handle) {
		if (handle >= m_point_lights.size()) {
#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_WARNING
			ServiceLocator::getFileLogger()->log<LOG_WARNING>(
				"Attempt to remove Mesh outside boundaries");
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_WARNING
			return;
		}

		m_point_lights.erase(m_point_lights.begin() + handle);
	}

	void GraphicsManager::removeSpotLight(unsigned handle) {
		if (handle >= m_spot_lights.size()) {
#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_WARNING
			ServiceLocator::getFileLogger()->log<LOG_WARNING>(
				"Attempt to remove Mesh outside boundaries");
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_WARNING
			return;
		}

		m_spot_lights.erase(m_spot_lights.begin() + handle);
	}
}