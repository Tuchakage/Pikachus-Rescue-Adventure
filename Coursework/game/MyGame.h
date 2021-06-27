#pragma once

#include "SpriteAgent.h"
#include "FVector.h"
struct CONNECTION
{
	int nEnd; // index of the destination node
	float cost; //cost of the transition
};

struct NODE
{
	CVector pos; // 2D Position of the node
	list<CONNECTION> conlist; // list of connections

	float costSoFar;
	int nConnection;
	bool open, closed;
};

class CMyGame : public CGame
{
	CSpriteAgent prey, m_player;
	CSprite hunter;
	CVector steerPlayer;
	CSprite mousetarget;
	CSpriteOval endtarget;
	CSpriteList m_nodes;			// Nodes
	vector <NODE> m_graph;
	CSprite* pKiller;
	CSpriteList obstacle;

	list<CVector> m_waypoints;

	CVector newpos;
	float xpos,ypos;
	int xtarget, ytarget; // Used so that the node the hunter goes to can change depending on the if statements
	bool arrivedonce = false; //Used for automatic waypoint system
	bool guided;
	bool dest;
	bool chase; // Used to see if the player is chasing the Prey
	bool flee; //Used to see if the Prey is running away from the player;
	bool lbuttondown;
	bool walkingl;//Used to see if the player is walking left
	bool preywalkingl; //Used to see if the prey is walking left

	//Prey and Player Movement Speed
	float preySpeed = 450;
	float preyWanderSpeed = 300;
	float playerWanderSpeed = 150;
	float playerChaseSpeed = 400;
	CGraphics grass;
public:
	CMyGame(void);
	~CMyGame(void);

	// Per-Frame Callback Funtions (must be implemented!)
	virtual void OnUpdate();
	virtual void OnDraw(CGraphics* g);

	// Game Life Cycle
	virtual void OnInitialize();
	virtual void OnDisplayMenu();
	virtual void OnStartGame();
	virtual void OnGameOver();
	virtual void OnTerminate();

	// Keyboard Event Handlers
	virtual void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	virtual void OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode);

	// Mouse Events Handlers
	virtual void OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle);
	virtual void OnLButtonDown(Uint16 x,Uint16 y);
	virtual void OnLButtonUp(Uint16 x,Uint16 y);
	virtual void OnRButtonDown(Uint16 x,Uint16 y);
	virtual void OnRButtonUp(Uint16 x,Uint16 y);
	virtual void OnMButtonDown(Uint16 x,Uint16 y);
	virtual void OnMButtonUp(Uint16 x,Uint16 y);

	virtual void hunterPath();
	CFVector playerobstacleav(double radius);
	CFVector preyobstacleav(double radius);
	virtual void playeravoidObstacle();
	virtual void preyavoidObstacle();
};
