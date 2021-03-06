/*===========================================================================*
 * Arch Engine - "Graphics/shader.cpp"                                       *
 *                                                                           *
 * TODO: description                                                         *
 *                                                                           *
 * Marcelo de Matos Menezes - marcelodmmenezes@gmail.com                     *
 * Created: 13/05/2018                                                       *
 * Last Modified: 22/06/2018                                                 *
 *===========================================================================*/


#include "shader.hpp"


using namespace Script;
using namespace Utils;


namespace Graphics {
	Shader::Shader() : m_has_normal_map(false), m_has_dir_lights(false),
		m_has_point_lights(false), m_has_spot_lights(false) {
#if defined(ARCH_ENGINE_HOT_RELOAD_ON)
		m_watch_file = true;
#endif	// ARCH_ENGINE_HOT_RELOAD_ON
		
#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_DEBUG
		ServiceLocator::getFileLogger()->log<LOG_DEBUG>(
			"Shader constructor");
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_DEBUG

	}

	Shader::~Shader() {
#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_DEBUG
		ServiceLocator::getFileLogger()->log<LOG_DEBUG>(
			"Shader destructor");
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_DEBUG
	}

	bool Shader::initialize(
		const std::string& vs_path,
		const std::string& fs_path) {
#if defined (ARCH_ENGINE_HOT_RELOAD_ON)
		m_vs_path = vs_path;
		m_gs_path = "";
		m_fs_path = fs_path;
#endif	// ARCH_ENGINE_HOT_RELOAD_ON

		unsigned vs, fs;

#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_INFO
		ServiceLocator::getFileLogger()->log<LOG_INFO>(
			"\n\n---------- BUILDING SHADER PROGRAM ----------\n");
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_INFO

		if (!compileShader(vs_path, GL_VERTEX_SHADER, vs))
			return false;

		if (!compileShader(fs_path, GL_FRAGMENT_SHADER, fs))
			return false;

		if (!linkProgram(vs, fs))
			return false;

		getAttributes();
		getUniforms();

#if defined(ARCH_ENGINE_HOT_RELOAD_ON)
		if (m_watch_file) {
			// Register for hot reload
			m_file_modified_listener.bind
				<Shader, &Shader::onFileModifiedEvent>(this);
			Core::EventManager::getInstance().addListener(
				m_file_modified_listener, Core::EVENT_FILE_MODIFIED);

			Core::EventPtr evnt = std::make_shared<WatchFileEvent>(
				WatchFileEvent(m_vs_path, true));
			Core::EventManager::getInstance().sendEvent(evnt);

			evnt = std::make_shared<WatchFileEvent>(
				WatchFileEvent(m_gs_path, true));
			Core::EventManager::getInstance().sendEvent(evnt);

			evnt = std::make_shared<WatchFileEvent>(
				WatchFileEvent(m_fs_path, true));
			Core::EventManager::getInstance().sendEvent(evnt);

			m_watch_file = false;
		}
#endif	// ARCH_ENGINE_HOT_RELOAD_ON

#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_INFO
		ServiceLocator::getFileLogger()->log<LOG_INFO>(
			"\n\n---------------------------------------------\n");
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_INFO

		return true;
	}

	bool Shader::initialize(
		const std::string& vs_path,
		const std::string& gs_path,
		const std::string& fs_path) {
#if defined (ARCH_ENGINE_HOT_RELOAD_ON)
		m_vs_path = vs_path;
		m_gs_path = gs_path;
		m_fs_path = fs_path;
#endif	// ARCH_ENGINE_HOT_RELOAD_ON

		unsigned vs, gs, fs;

#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_INFO
		ServiceLocator::getFileLogger()->log<LOG_INFO>(
			"\n\n---------- BUILDING SHADER PROGRAM ----------\n");
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_INFO

		if (!compileShader(vs_path, GL_VERTEX_SHADER, vs))
			return false;

		if (!compileShader(gs_path, GL_GEOMETRY_SHADER, gs))
			return false;

		if (!compileShader(fs_path, GL_FRAGMENT_SHADER, fs))
			return false;

		if (!linkProgram(vs, gs, fs))
			return false;

		getAttributes();
		getUniforms();

#if defined(ARCH_ENGINE_HOT_RELOAD_ON)
		if (m_watch_file) {
			// Register for hot reload
			m_file_modified_listener.bind
				<Shader, &Shader::onFileModifiedEvent>(this);
			Core::EventManager::getInstance().addListener(
				m_file_modified_listener, Core::EVENT_FILE_MODIFIED);

			Core::EventPtr evnt = std::make_shared<WatchFileEvent>(
				WatchFileEvent(m_vs_path, true));
			Core::EventManager::getInstance().sendEvent(evnt);

			evnt = std::make_shared<WatchFileEvent>(
				WatchFileEvent(m_gs_path, true));
			Core::EventManager::getInstance().sendEvent(evnt);

			evnt = std::make_shared<WatchFileEvent>(
				WatchFileEvent(m_fs_path, true));
			Core::EventManager::getInstance().sendEvent(evnt);

			m_watch_file = false;
		}
#endif	// ARCH_ENGINE_HOT_RELOAD_ON

#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_INFO
		ServiceLocator::getFileLogger()->log<LOG_INFO>(
			"\n\n---------------------------------------------\n");
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_INFO

		return true;
	}

#if defined(ARCH_ENGINE_HOT_RELOAD_ON)
	void Shader::onFileModifiedEvent(Core::EventPtr e) {
		auto evnt = std::static_pointer_cast<FileModifiedEvent>(e);

		// See if the modified file was one of this shader files
		if (evnt->getPath() != m_vs_path &&
			evnt->getPath() != m_gs_path &&
			evnt->getPath() != m_fs_path)
			return;

		if (glIsProgram(m_program_id))
			glDeleteProgram(m_program_id);

		m_dirty_uniforms.clear();
		m_uniforms_by_name.clear();

		if (m_gs_path == "")
			initialize(m_vs_path, m_fs_path);
		else
			initialize(m_vs_path, m_gs_path, m_fs_path);

#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_INFO
		ServiceLocator::getFileLogger()->log<LOG_INFO>(
			"Shader onFileModifiedEvent");
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_INFO
	}
#endif	// ARCH_ENGINE_HOT_RELOAD_ON

	void Shader::bind() {
		glUseProgram(m_program_id);
	}

	void Shader::unbindShaders() {
		glUseProgram(0);
	}

	void Shader::update() {
		for (auto& it : m_dirty_uniforms) {
			auto uniform = m_uniforms_by_name[it];
			if (uniform) uniform->update();

#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_ERROR
			checkOpenGLErrors(it);
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_ERROR
		}

		m_dirty_uniforms.clear();
	}

	void Shader::destroy() {
		if (glIsProgram(m_program_id))
			glDeleteProgram(m_program_id);
	}
	
	unsigned Shader::getProgramId() {
		return m_program_id;
	}

	bool Shader::hasNormalMap() {
		return m_has_normal_map;
	}

	bool Shader::hasDirLights() {
		return m_has_dir_lights;
	}

	bool Shader::hasPointLights() {
		return m_has_point_lights;
	}

	bool Shader::hasSpotLights() {
		return m_has_spot_lights;
	}
	
	void Shader::setBool(const std::string& name, bool value) {
		auto uniform = std::dynamic_pointer_cast<Uniform<bool>>(
			m_uniforms_by_name[name]);

		if (!uniform) {
#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_WARNING
			ServiceLocator::getFileLogger()->log<LOG_WARNING>(
				"Couldn't find " + name + " in shader " + m_vs_path);
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_WARNING
			return;
		}

		if (uniform->setValue(value))
			m_dirty_uniforms.push_back(name);
	}

	void Shader::setInt(const std::string& name, int value) {
		auto uniform = std::dynamic_pointer_cast<Uniform<int>>(
			m_uniforms_by_name[name]);

		if (!uniform) {
#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_WARNING
			ServiceLocator::getFileLogger()->log<LOG_WARNING>(
				"Couldn't find " + name + " in shader " + m_vs_path);
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_WARNING
			return;
		}

		if (uniform->setValue(value))
			m_dirty_uniforms.push_back(name);
	}

	void Shader::setFloat(const std::string& name, float value) {
		auto uniform = std::dynamic_pointer_cast<Uniform<float>>(
			m_uniforms_by_name[name]);

		if (!uniform) {
#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_WARNING
			ServiceLocator::getFileLogger()->log<LOG_WARNING>(
				"Couldn't find " + name + " in shader " + m_vs_path);
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_WARNING
			return;
		}

		if (uniform->setValue(value))
			m_dirty_uniforms.push_back(name);
	}

	void Shader::setVec3(const std::string& name, const glm::vec3& vec) {
		auto uniform = std::dynamic_pointer_cast<Uniform<glm::vec3>>(
			m_uniforms_by_name[name]);

		if (!uniform) {
#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_WARNING
			ServiceLocator::getFileLogger()->log<LOG_WARNING>(
				"Couldn't find " + name + " in shader " + m_vs_path);
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_WARNING
			return;
		}

		if (uniform->setValue(vec))
			m_dirty_uniforms.push_back(name);
	}

	void Shader::setVec4(const std::string& name, const glm::vec4& vec) {
		auto uniform = std::dynamic_pointer_cast<Uniform<glm::vec4>>(
			m_uniforms_by_name[name]);

		if (!uniform) {
#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_WARNING
			ServiceLocator::getFileLogger()->log<LOG_WARNING>(
				"Couldn't find " + name + " in shader " + m_vs_path);
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_WARNING
			return;
		}

		if (uniform->setValue(vec))
			m_dirty_uniforms.push_back(name);
	}

	void Shader::setMat3(const std::string& name, const glm::mat3& matrix) {
		auto uniform = std::dynamic_pointer_cast<Uniform<glm::mat3>>(
			m_uniforms_by_name[name]);

		if (!uniform) {
#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_WARNING
			ServiceLocator::getFileLogger()->log<LOG_WARNING>(
				"Couldn't find " + name + " in shader " + m_vs_path);
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_WARNING
			return;
		}

		if (uniform->setValue(matrix))
			m_dirty_uniforms.push_back(name);
	}

	void Shader::setMat4(const std::string& name, const glm::mat4& matrix) {
		auto uniform = std::dynamic_pointer_cast<Uniform<glm::mat4>>(
			m_uniforms_by_name[name]);

		if (!uniform) {
#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_WARNING
			ServiceLocator::getFileLogger()->log<LOG_WARNING>(
				"Couldn't find " + name + " in shader " + m_vs_path);
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_WARNING
			return;
		}

		if (uniform->setValue(matrix))
			m_dirty_uniforms.push_back(name);
	}

	bool Shader::compileShader(const std::string& path,
		GLenum type, unsigned& shader_handle) {
#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_INFO
		ServiceLocator::getFileLogger()->log<LOG_INFO>(
			path + " shader compilation:");
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_INFO

		std::string source_code;
		std::stringstream ss;
		std::fstream file;

		file.open(path);
		ss << file.rdbuf();
		file.close();
		source_code = ss.str();

		const char* src = source_code.c_str();

		shader_handle = glCreateShader(type);
		glShaderSource(shader_handle, 1, &src, nullptr);
		glCompileShader(shader_handle);
		
		int success;
		int info_log_length;
		char* info_log;

		glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &success);

		if (!success) {
			glGetShaderiv(shader_handle, GL_INFO_LOG_LENGTH, &info_log_length);

			info_log = new char[info_log_length + 1];
			glGetShaderInfoLog(shader_handle, info_log_length,
				nullptr, info_log);
			glDeleteShader(shader_handle);

#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_ERROR
			ServiceLocator::getFileLogger()->log<LOG_ERROR>(
				"\n\n    " + path + " shader compilation failed:\n"
				+ std::string(info_log) + "\n");
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_ERROR

			delete info_log;

			return false;
		}

#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_INFO
		ServiceLocator::getFileLogger()->log<LOG_INFO>(
			path + " sucessfully compiled");
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_INFO

		return true;
	}

	bool Shader::linkProgram(unsigned vs, unsigned fs) {
		m_program_id = glCreateProgram();
		
		glAttachShader(m_program_id, vs);
		glAttachShader(m_program_id, fs);

		glLinkProgram(m_program_id);

		glDetachShader(m_program_id, vs);
		glDetachShader(m_program_id, fs);

		glDeleteShader(vs);
		glDeleteShader(fs);

		int success;
		int info_log_length;
		char* info_log;

		glGetProgramiv(m_program_id, GL_LINK_STATUS, &success);

		if (!success) {
			glGetProgramiv(m_program_id, GL_INFO_LOG_LENGTH, &info_log_length);

			info_log = new char[info_log_length + 1];
			glGetProgramInfoLog(m_program_id, info_log_length,
				nullptr, info_log);
			glDeleteProgram(m_program_id);

#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_ERROR
			ServiceLocator::getFileLogger()->log<LOG_ERROR>(
				"\n\n    Program linking failed\n"
				+ std::string(info_log) + "\n");
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_ERROR

			delete info_log;

			return false;
		}

#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_INFO
		ServiceLocator::getFileLogger()->log<LOG_INFO>(
			"Program sucessfully linked");
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_INFO

		return true;
	}

	bool Shader::linkProgram(unsigned vs, unsigned gs, unsigned fs) {
		m_program_id = glCreateProgram();
		
		glAttachShader(m_program_id, vs);
		glAttachShader(m_program_id, gs);
		glAttachShader(m_program_id, fs);

		glLinkProgram(m_program_id);

		glDetachShader(m_program_id, vs);
		glDetachShader(m_program_id, gs);
		glDetachShader(m_program_id, fs);

		glDeleteShader(vs);
		glDeleteShader(gs);
		glDeleteShader(fs);

		int success;
		int info_log_length;
		char* info_log;

		glGetProgramiv(m_program_id, GL_LINK_STATUS, &success);

		if (!success) {
			glGetProgramiv(m_program_id, GL_INFO_LOG_LENGTH, &info_log_length);

			info_log = new char[info_log_length + 1];
			glGetProgramInfoLog(m_program_id, info_log_length,
				nullptr, info_log);
			glDeleteProgram(m_program_id);

#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_ERROR
			ServiceLocator::getFileLogger()->log<LOG_ERROR>(
				"\n\n    Program linking failed\n" +
				std::string(info_log));
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_ERROR

			delete info_log;

			return false;
		}

#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_INFO
		ServiceLocator::getFileLogger()->log<LOG_INFO>(
			"Program sucessfully linked");
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_INFO

		return true;
	}

	void Shader::getAttributes() {
		int count, size;
		GLenum type;

		const int BUF_SIZE = 32;
		char name[BUF_SIZE];
		int name_length;

		glGetProgramiv(m_program_id, GL_ACTIVE_ATTRIBUTES, &count);

#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_INFO
		ServiceLocator::getFileLogger()->log<LOG_INFO>(
			"Found " + std::to_string(count) + " attributes");
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_INFO

		for (unsigned i = 0; i < (unsigned)count; i++) {
			glGetActiveAttrib(m_program_id, i, BUF_SIZE,
				&name_length, &size, &type, name);

			// TODO
		}
	}

	void Shader::getUniforms() {
#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_INFO
		std::stringstream log_stream;
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_INFO

		int count, size;
		GLenum type;

		const int BUF_SIZE = 32;
		char name[BUF_SIZE];
		int name_length;

		glGetProgramiv(m_program_id, GL_ACTIVE_UNIFORMS, &count);

#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_INFO
		log_stream << "Found " + std::to_string(count) + " uniforms";
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_INFO

		for (unsigned i = 0; i < (unsigned)count; i++) {
			glGetActiveUniform(m_program_id, i, BUF_SIZE,
				&name_length, &size, &type, name);

			std::string name_str(name), array_name;

			// if array
			if (size > 1)
				array_name = name_str.substr(0, name_str.size() - 3);

			if (name_str == "u_texture_normals")
				m_has_normal_map = true;
			else if (name_str.substr(0, 12) == "u_dir_lights")
				m_has_dir_lights = true;
			else if (name_str.substr(0, 14) == "u_point_lights")
				m_has_point_lights = true;
			else if (name_str.substr(0, 13) == "u_spot_lights")
				m_has_spot_lights = true;

			for (unsigned i = 0; i < (unsigned)size; i++) { // for arrays
				if (size > 1)
					name_str = array_name + "[" + std::to_string(i) + "]";

				int location = glGetUniformLocation(
					m_program_id, name_str.c_str());

				m_dirty_uniforms.push_back(name_str);

#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_INFO
				log_stream << "\n    " << name_str;
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_INFO

				switch (type) {
				case GL_BOOL:
					m_uniforms_by_name.insert(std::make_pair(
						name_str, std::make_shared<Uniform<bool>>
							(name_str, location)));
					break;

				case GL_INT:
					m_uniforms_by_name.insert(std::make_pair(
						name_str, std::make_shared<Uniform<int>>
							(name_str, location)));
					break;

				case GL_FLOAT:
					m_uniforms_by_name.insert(std::make_pair(
						name_str, std::make_shared<Uniform<float>>
							(name_str, location)));
					break;

				case GL_FLOAT_VEC3:
					m_uniforms_by_name.insert(std::make_pair(
						name_str, std::make_shared<Uniform<glm::vec3>>
							(name_str, location)));
					break;

				case GL_FLOAT_VEC4:
					m_uniforms_by_name.insert(std::make_pair(
						name_str, std::make_shared<Uniform<glm::vec4>>
						(name_str, location)));
					break;

				case GL_FLOAT_MAT3:
					m_uniforms_by_name.insert(std::make_pair(
						name_str, std::make_shared<Uniform<glm::mat3>>
							(name_str, location)));
					break;

				case GL_FLOAT_MAT4:
					m_uniforms_by_name.insert(std::make_pair(
						name_str, std::make_shared<Uniform<glm::mat4>>
							(name_str, location)));
					break;

				case GL_SAMPLER_2D:
					m_uniforms_by_name.insert(std::make_pair(
						name_str, std::make_shared<Uniform<int>>
							(name_str, location)));
					break;

				case GL_SAMPLER_CUBE:
					m_uniforms_by_name.insert(std::make_pair(
						name_str, std::make_shared<Uniform<int>>
						(name_str, location)));
					break;
				}
			}
		}

#ifndef ARCH_ENGINE_LOGGER_SUPPRESS_INFO
		ServiceLocator::getFileLogger()->log<LOG_INFO>(log_stream);
#endif	// ARCH_ENGINE_LOGGER_SUPPRESS_INFO
	}
}