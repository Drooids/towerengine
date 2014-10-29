#include "towerengine.h"

/*CVertexPosition::CVertexPosition(CMeshPosition *parent, CVertex *v, CVector p)
{
	this->parent = parent;
	chain_next = parent->pos_first;
	parent->pos_first = this;

	this->v = v;
	this->p = p;
}

CVertexPosition::~CVertexPosition(void)
{
}

void CVertexPosition::CopyPosition(void)
{
	p = (CVector)*v;
}

void CVertexPosition::ApplyPosition(void)
{
	v->SetVector(p);
}

void CVertexPosition::ApplyMixedPosition(CVector o, float mix)
{
	v->SetVector(Mix(p, o, mix));
}*/

//------------------------------------------------------------


tMeshPose::tMeshPose(tMesh *mesh)
{
	id = 0;
	this->mesh = mesh;
	vbo = mesh->CreateFloatVBO(3);
}

tMeshPose::~tMeshPose(void)
{
	if(mesh->GetCurrentPose() == this)
		mesh->ChangePose(mesh->GetIdlePose());
	delete vbo;
}

void tMeshPose::CopyFromVertices(void)
{
	tVertex *v;
	tVector p;
	int i;

	for(i=0; i<mesh->GetVertexCount(); i++)
	{
		v = mesh->GetVertex(i);
		p = *v;
		vertices.insert(pair<tVertex *, tVector>(v, p));
	}

	RefreshVBO();
}

void tMeshPose::Clear(void)
{
	vertices.clear();
}

int tMeshPose::Count(void)
{
	return vertices.size();
}

void tMeshPose::CopyFromData(int c, int *vert, tVector *vec)
{
	int i;//, vi;
	tVertex *v;

	for(i=0; i<c; i++)
	{
		v = mesh->GetVertexByID(vert[i]);
		if(!v)
			continue;
		vertices.insert(pair<tVertex *, tVector>(v, vec[i]));
	}
	RefreshVBO();
}


void tMeshPose::ApplyPoseToVertices(void)
{
	map<tVertex *, tVector>::iterator i;

	for(i=vertices.begin(); i!=vertices.end(); i++)
		i->first->SetVector(i->second);
}

void tMeshPose::ApplyMixedPoseToVertices(tMeshPose *o, float mix)
{
	map<tVertex *, tVector>::iterator i, oi;

	for(i=vertices.begin(); i!=vertices.end(); i++)
	{
		oi = o->vertices.find(i->first);
		if(oi == o->vertices.end())
			i->first->SetVector(i->second);
		else
			i->first->SetVector(i->second * mix + oi->second * (1.0 - mix));

	}
}


void tMeshPose::RefreshVBO(void)
{
	vector<tVertex *>::iterator v;
	tVertex *vt;
	float *p;
	map<tVertex *, tVector>::iterator vi;
	float *vertex_data;
	int i;

	mesh->AssignVertexArrayPositions();

	vbo->SetSize(mesh->GetVertexCount());
	vertex_data = vbo->GetData();

	for(i=0; i<mesh->GetVertexCount(); i++)
	{
		vt = mesh->GetVertex(i);
		vi = vertices.find(vt);
		if(vi==vertices.end())
			p = vt->v;
		else
			p = vi->second.v;
		memcpy(vertex_data + vt->index * 3, p, 3 * sizeof(float));
	}

	vbo->AssignData();
}













