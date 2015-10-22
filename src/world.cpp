
#include "towerengine.h"

#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "BulletDynamics/Character/btKinematicCharacterController.h"

using namespace std;

tWorld::tWorld(void)
{
	ambient_color = Vec(0.1, 0.1, 0.1);
	sky_box = 0;

	physics.broadphase = new btDbvtBroadphase();
	physics.collision_configuration = new btDefaultCollisionConfiguration();
	physics.collision_dispatcher = new btCollisionDispatcher(physics.collision_configuration);
	physics.solver = new btSequentialImpulseConstraintSolver();
	physics.dynamics_world = new btDiscreteDynamicsWorld(physics.collision_dispatcher, physics.broadphase, physics.solver, physics.collision_configuration);
	physics.dynamics_world->setGravity(btVector3(0, -10, 0));
	//physics.dynamics_world->getDispatchInfo().m_allowedCcdPenetration=0.0001f;

	physics.broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
}

tWorld::~tWorld(void)
{
	delete physics.dynamics_world;
	delete physics.solver;
	delete physics.collision_dispatcher;
	delete physics.collision_configuration;
	delete physics.broadphase;
}

void tWorld::AddObject(tObject *o)
{
	vector<tObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
		if(*i == o)
			return;

	objects.push_back(o);
	o->AddedObjectToWorld(this);
}

void tWorld::RemoveObject(tObject *o)
{
	vector<tObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
		if(*i == o)
		{
			objects.erase(i);
			o->RemovedObjectFromWorld(this);
			return;
		}
}

void tWorld::AddPointLight(tPointLight *light)
{
	for(vector<tPointLight *>::iterator i = point_lights.begin(); i != point_lights.end(); i++)
		if((*i) == light)
			return;
	point_lights.push_back(light);
}

void tWorld::RemovePointLight(tPointLight *light)
{
	for(vector<tPointLight *>::iterator i = point_lights.begin(); i != point_lights.end(); i++)
		if((*i) == light)
		{
			point_lights.erase(i);
			return;
		}
}

void tWorld::AddDirectionalLight(tDirectionalLight *light)
{
	for(vector<tDirectionalLight *>::iterator i = dir_lights.begin(); i != dir_lights.end(); i++)
		if((*i) == light)
			return;
	dir_lights.push_back(light);
}

void tWorld::RemoveDirectionalLight(tDirectionalLight *light)
{
	for(vector<tDirectionalLight *>::iterator i = dir_lights.begin(); i != dir_lights.end(); i++)
		if((*i) == light)
		{
			dir_lights.erase(i);
			return;
		}
}



void tWorld::AddParticleSystem(tParticleSystem *ps)
{
	for(vector<tParticleSystem *>::iterator i = particle_systems.begin(); i != particle_systems.end(); i++)
		if(*i == ps)
			return;
	particle_systems.push_back(ps);
}

void tWorld::RemoveParticleSystem(tParticleSystem *ps)
{
	for(vector<tParticleSystem *>::iterator i = particle_systems.begin(); i != particle_systems.end(); i++)
		if(*i == ps)
		{
			particle_systems.erase(i);
			return;
		}
}



void tWorld::FillRenderObjectSpace(tRenderObjectSpace *space, tCulling **cullings, int cullings_count, bool clear, bool init_cullings)
{
	if(clear)
		space->Clear();

	if(init_cullings)
		for(int c=0; c<cullings_count; c++)
			cullings[c]->InitCulling();

	vector<tObject *>::iterator i;

	for(i=objects.begin(); i!=objects.end(); i++)
	{
		bool cull = true;

		for(int c=0; c<cullings_count; c++)
		{
			if(!cullings[c]->TestBoundingBoxCulling((*i)->GetBoundingBox()))
			{
				cull = false;
				break;
			}
		}

		if(cull)
			continue;

		space->objects.insert((*i));
	}
}


void tWorld::FillRenderSpace(tRenderSpace *space, tCulling **cullings, int cullings_count, bool init_cullings)
{
	tBoundingBox b;
	//tVector minv, maxv;
	tVector light_pos;
	float light_dist;
	vector<tObject *>::iterator i;

	if(init_cullings)
		for(int c=0; c<cullings_count; c++)
			cullings[c]->InitCulling();

	space->Clear();

	FillRenderObjectSpace(space, cullings, cullings_count, false, false);

	vector<tPointLight *>::iterator pi;

	for(pi=point_lights.begin(); pi!=point_lights.end(); pi++)
	{
		if(!(*pi)->GetEnabled())
			continue;

		if(!(*pi)->GetShadowInvalid())
		{
			light_pos = (*pi)->GetPosition();
			light_dist = (*pi)->GetDistance();

			bool cull = true;

			for(int c=0; c<cullings_count; c++)
			{
				if(!cullings[c]->TestSphereCulling(light_pos, light_dist))
				{
					cull = false;
					break;
				}
			}

			if(cull)
				continue;
		}

		space->point_lights.insert(*pi);
	}

	vector<tDirectionalLight *>::iterator di;

	for(di=dir_lights.begin(); di!=dir_lights.end(); di++)
	{
		space->dir_lights.insert(*di);

		if(!(*di)->GetShadowEnabled())
			continue;

		for(i=objects.begin(); i!=objects.end(); i++)
			(*di)->GetShadow()->GetRenderSpace()->objects.insert((*i)); // TODO: move to tRenderer
	}
}


void tWorld::Step(float time)
{
	if(time < 0.001)
		time = 0.001;

	physics.dynamics_world->stepSimulation(time, 10);
	//CProfileManager::dumpAll();

	for(vector<tParticleSystem *>::iterator psi = particle_systems.begin(); psi != particle_systems.end(); psi++)
		(*psi)->Step(time);
}




