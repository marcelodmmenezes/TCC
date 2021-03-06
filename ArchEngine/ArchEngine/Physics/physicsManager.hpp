/*===========================================================================*
 * Arch Engine - "Physics/physicsManager.hpp"                                *
 *                                                                           *
 * Class responsible for representing the physics world.                     *
 * All physics objects, collisions and simulations are contained and         *
 * calculated in a physics world.                                            *
 *                                                                           *
 * Marcelo de Matos Menezes - marcelodmmenezes@gmail.com                     *
 * Created: 12/05/2018                                                       *
 * Last Modified: 14/06/2018                                                 *
 *===========================================================================*/


#ifndef PHYSICS_PHYSICS_MANAGER_HPP
#define PHYSICS_PHYSICS_MANAGER_HPP


#include "../API/entity.hpp"
#include "../Config/engineMacros.hpp"
#include "../Core/eventManager.hpp"
#include "../Graphics/graphicsManager.hpp"
#include "../Graphics/primitives.hpp"
#include "debugDraw.hpp"
#include "../Script/luaScript.hpp"
#include "../Utils/serviceLocator.hpp"

#include <glm/glm.hpp>
#include <btBulletDynamicsCommon.h>
#include <BulletSoftBody/btSoftBody.h>

#include <string>
#include <vector>


namespace Physics {
	enum WorldType {
		WORLD_DISCRETE_DYNAMICS,
		WORLD_SOFT_RIGID_DYNAMICS
	};

	enum CollisionConfigType {
		DEFAULT_COLLISION_CONFIGURATION,
		SOFT_RIGID_COLLISION_CONFIGURATION
	};

	enum BroadphaseType {
		AXIS_SWEEP_BROADPHASE,
		DBVT_BROADPHASE,
		MULTI_SAP_BROADPHASE,
		SIMPLE_BROADPHASE,
	};

	enum ConstraintSolverType {
		SEQUENTIAL_IMPULSE_SOLVER,
		MLCP_SOLVER,
		MULTI_BODY_SOLVER,
		NNGC_SOLVER
	};

	//----------------------------------- Bullet collision object encapsulation
	enum PhysicsObjectType {
		PHYSICS_OBJECT_CUBE,
		PHYSICS_OBJECT_SPHERE
	};

	struct PhysicsObject {
		PhysicsObjectType type;
		// User specified id
		long id;
		// Id in m_user_objects
		unsigned physics_manager_id;
		// World id
		unsigned m_world_id;
	};
	//-------------------------------------------------------------------------

	class PhysicsManager {
	public:
		~PhysicsManager();

		PhysicsManager(const PhysicsManager&) = delete;
		void operator=(const PhysicsManager&) = delete;

		static PhysicsManager& getInstance();

		bool initialize(
			WorldType world,
			CollisionConfigType collision_config,
			BroadphaseType broadphase,
			ConstraintSolverType solver);

		bool initializeFromConfigFile(const std::string& path);
		void update(float delta_time);
		void destroy();

		void setGravity(const glm::vec3& gravity);

		void closestObjectRayTest(const glm::vec3& pos,
			const glm::vec3& front);
		void allObjectsRayTest(const glm::vec3& pos, 
			const glm::vec3& front);

		//------------------------------------------------------- Mouse picking
		void createPickingConstraint(int x, int y,
			float ray_size, bool limit_angular_motion);
		void pickingMotion(int x, int y, float ray_size);
		void removePickingConstraint();
		//---------------------------------------------------------------------

		void setDebugDrawer(DebugDrawer* dd);
		void debugDraw();

		unsigned addCube(long id, const glm::vec3& sides,
			const glm::vec3& pos, float mass, float friction = 0.5f);

		unsigned addSphere(long id, float radius, const glm::vec3& pos,
			float mass, float friction = 0.5f);

		unsigned addStaticBody(
			const std::vector<Graphics::BasicVertex>& vertices,
			const std::vector<unsigned>& indices,
			const glm::mat4& transform);

		void applyForce(unsigned obj_id, const glm::vec3& dir,
			const glm::vec3& rel_pos,float intensity);
		void applyTorque(unsigned obj_id, const glm::vec3& dir,
			const glm::vec3& rel_pos, float intensity);
		void applyImpulse(unsigned obj_id, const glm::vec3& dir,
			const glm::vec3& rel_pos, float intensity);
		void applyTorqueImpulse(unsigned obj_id,
			const glm::vec3& dir, float intensity);
		void applyCentralForce(unsigned obj_id,
			const glm::vec3& dir, float intensity);
		void applyCentralImpulse(unsigned obj_id,
			const glm::vec3& dir, float intensity);

	private:
		enum State {
			CONSTRUCTED,
			INITIALIZED,
			SAFE_TO_DESTROY
		};

		PhysicsManager();
		
		State m_state;

		std::vector<PhysicsObject*> m_user_objects;

		//--------------------------------------------- Mouse picking variables
		btGeneric6DofConstraint* m_picking_constraint;
		btRigidBody* m_picked_obj;
		btScalar m_old_picking_dist;
		//---------------------------------------------------------------------

		btDynamicsWorld* m_world;
		btDispatcher* m_dispatcher;
		btCollisionConfiguration* m_collision_config;
		btBroadphaseInterface* m_broadphase;
		btConstraintSolver* m_solver;
	};
}


#endif	// PHYSICS_PHYSICS_MANAGER_HPP