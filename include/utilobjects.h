

#ifndef _UTILOBJECTS_H
#define _UTILOBJECTS_H


class tCoordinateSystemObject : public tObject
{
	private:
		tVector pos;
		tVector size;
		bool depth_test;

	public:
		tCoordinateSystemObject(bool depth_test = false);
		~tCoordinateSystemObject(void);

		void SetPosition(tVector pos)	{ this->pos = pos; }
		void SetSize(tVector size)		{ this->size = size; }

		void ForwardPass(tRenderer *renderer);

		tBoundingBox GetBoundingBox(void);
};


#endif
