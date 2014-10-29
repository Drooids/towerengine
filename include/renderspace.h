
#ifndef _RENDERSPACE_H
#define _RENDERSPACE_H


class tRenderSpace
{
	public:
		set<tObject *> objects;

	public:
		void ClearObjects(void);

		void GeometryPass(void);
		void ForwardPass(void);
};


#endif
