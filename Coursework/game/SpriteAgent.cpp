#include "stdafx.h"
#include "SpriteAgent.h"

// Constructors
CSpriteAgent::CSpriteAgent(float x, float y, char* pFileBitmap, Uint32 time) : CSprite(x, y, pFileBitmap, time) 
{ wanderTarget = CVector(0, 1); }

CSpriteAgent::CSpriteAgent(float x, float y, char* pFileBitmap, CColor colorKey, Uint32 time) : CSprite(x, y, pFileBitmap, colorKey, time)
{ wanderTarget = CVector(0, 1); }

CSpriteAgent::CSpriteAgent(float x, float y, float w, float h, char* pFileBitmap, Uint32 time) : CSprite(x, y, w, h, pFileBitmap, time)
{ wanderTarget = CVector(0, 1); }

CSpriteAgent::CSpriteAgent(float x, float y, float w, float h, char* pFileBitmap, CColor colorKey, Uint32 time) : CSprite(x, y, w, h, pFileBitmap, colorKey, time)
{ wanderTarget = CVector(0, 1); }

CSpriteAgent::~CSpriteAgent()  
{ }

// Behaviour Steerings

CVector CSpriteAgent::seek(CVector targetPos, float speed)
{
	CVector desiredVelocity, steering;
	// TO DO [1]: Implement the SEEK behaviour. It is used by the fox
	desiredVelocity = targetPos - GetPos();

	desiredVelocity = Normalize(desiredVelocity) * speed;

	steering = desiredVelocity - GetVelocity();
	
	return steering;

	
}

CVector CSpriteAgent::flee(CVector targetPos, float speed)
{
	// TO DO [2]: Implement the FLEE behaviour. It is used by the rabbit
	CVector desiredVelocity, steering;

	desiredVelocity = GetPos() - targetPos;

	desiredVelocity = Normalize(desiredVelocity) * speed;

	steering = desiredVelocity - GetVelocity();

	return steering;
}

float random()
{
	return ((float)rand() / (float)RAND_MAX);
}

CVector CSpriteAgent::wander(float speed, float wanderDist, float wanderRadius)
{
	// TO DO [4]: Implement the WANDER behaviour. It may also be used by the rabbit

	CVector realTarget;
	
	wanderTarget += CVector(random() - 0.5, random() - 0.5);

	wanderTarget = Normalize(wanderTarget);

	realTarget = GetPos() + Normalize(GetVelocity()) * wanderDist + wanderTarget * wanderRadius;

	
	return seek(realTarget, speed);
}

//My Attempt of a different type of Obstacle Avoidance
//CVector CSpriteAgent::obstacleav(CVector obspos) 
//{
//	CVector ahead, ahead2, steering, desiredVelocity,avoidf;
//
//	int force = 600;
//	ahead = GetPos() + Normalize(GetVelocity() * 5);
//	ahead2 = GetPos() + Normalize(GetVelocity() * 5 * 0.5);
//
//	avoidf = ahead - obspos;
//	avoidf = Normalize(avoidf) * force;
//	return avoidf;
//}
// Screen functions

void CSpriteAgent::wrapToScreen(int width, int height)
{
	if (GetX() < 0) SetX((float)width);
	if (GetX() > width) SetX(0);
	if (GetY() < 0) SetY((float)height);
	if (GetY() > height) SetY(0);

}
