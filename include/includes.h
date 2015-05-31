#ifndef _INCLUDES_H
#define _INCLUDES_H

#define _USE_MATH_DEFINES
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdexcept>


#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif

//#define MAX_LINE_LEN 300

#ifdef _WIN32
#include <GL/glew.h>
#include <GL/wglew.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif


#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

#endif
