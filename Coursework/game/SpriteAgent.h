#pragma once

class CSpriteAgent : public CSprite
{
	CVector wanderTarget;	// wander target position (relative)
	
public:
	CSpriteAgent(float x, float y, char* pFileBitmap, Uint32 time);
	CSpriteAgent(float x, float y, char* pFileBitmap, CColor colorKey, Uint32 time);
	CSpriteAgent(float x, float y, float w, float h, char* pFileBitmap, Uint32 time);
	CSpriteAgent(float x, float y, float w, float h, char* pFileBitmap, CColor colorKey, Uint32 time);
	~CSpriteAgent(void);

	// Behaviour Steerings
	CVector seek(CVector targetPos, float speed);
	CVector flee(CVector targetPos, float speed);
	CVector wander(float speed, float wanderDist = 1, float wanderRadius = 1);
	//CVector obstacleav(CVector obspos);

	// Screen function
	void wrapToScreen(int width, int height);
};
