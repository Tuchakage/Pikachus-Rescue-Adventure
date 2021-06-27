#include "stdafx.h"
#include "MyGame.h"
#include <iostream>

using std::cout;
using std::endl;

float Coords[][2] =
{
	{236,144},
	{323, 115},
	{395,123},
	{480,168},
	{536, 145},
	{536, 194},
	{628,230},
	{567, 280},
	{596,374},
	{515,401},
	{565,465},
	{410, 531},
	{306,454},
	{205,344},
	{303,257},

};

int Connections[][2] =
{
	{ 0, 1 },
	{ 0, 14 },
	{ 1, 2 },
	{ 2, 3 },
	{ 2, 4 },
	{ 3, 4 },
	{ 3, 5 },
	{ 4, 5 },
	{ 5, 6 },
	{ 5, 7 },
	{ 6, 7 },
	{ 7, 8 },
	{ 8, 9 },
	{ 8, 10 },
	{ 9, 10 },
	{ 10, 11 },
	{ 11, 12 },
	{ 12, 13 },
	{ 13, 14 },
};

CMyGame::CMyGame(void)
	: grass("grass-texture.jpg"),
	  prey(50, 50, 32, 64, "bulbasaur.bmp", CColor::Blue(), 0),
	  m_player(0, 0, "pikachu.png", 0), 
	  mousetarget(0,0,50,50,"crosshair.png",0),
	  hunter(0,0, "guard", 0),
	  endtarget(1184, 480, 15, CColor::Black(), 0)

{
	//create graph structure 
	for (float* coord : Coords)
	{
		m_graph.push_back(NODE{ CVector(coord[0], coord[1]) });
	}

	//Create Graph structure - connections
	for (int* conn : Connections)
	{
		int ind1 = conn[0];
		int ind2 = conn[1];
		NODE& node1 = m_graph[ind1];
		NODE& node2 = m_graph[ind2];
		float dist = Distance(node1.pos, node2.pos);

		//Creates a connection from node 1 to node 2 
		node1.conlist.push_back(CONNECTION{ ind2, dist });
		//Creates a connection from node 2 to node 1 
		node2.conlist.push_back(CONNECTION{ ind1, dist });
	}

	pKiller = NULL;
}

CMyGame::~CMyGame(void)
{
}

/////////////////////////////////////////////////////
// Per-Frame Callback Funtions (must be implemented!)

void CMyGame::OnUpdate()
{
	// Restart the play automatically - 5 secs after Game Over
	if (IsGameOver())
	{
		if (GetTime() > 5000)
			NewGame();
		return;
	}

	//If The Player spawns in the object then find a different position to spawn in
	for (CSprite* pObstacle : obstacle) 
	{
		if (m_player.HitTest(pObstacle)) 
		{
			m_player.SetPosition(rand() % 1260, rand() % 500);
		}
	}
	
	// NPC: follow the waypoints
	if (!m_waypoints.empty())
	{
		// If NPC not moving, start moving to the first waypoint
		if (hunter.GetSpeed() < 1)
		{
			hunter.SetSpeed(100);		
			hunter.SetDirection(m_waypoints.front() - hunter.GetPosition());

			if (hunter.GetXVelocity() > 0) 
			{
				hunter.SetAnimation("walkr");
			}
			else
			{
				hunter.SetAnimation("walkl");
			}
		}

		// Passed the waypoint?
		CVector v = m_waypoints.front() - hunter.GetPosition();
		if (Dot(hunter.GetVelocity(), v) < 0)
		{
			// Stop movement
			m_waypoints.pop_front();
			if (m_waypoints.empty())
				hunter.SetAnimation("idle");
			hunter.SetVelocity(0, 0);
			hunter.SetRotation(0);
			
		}
	}

	if (Distance(hunter.GetPosition(), m_player.GetPosition()) < 150) 
	{
		//pKiller will be set to the hunter Sprite
		pKiller = &hunter;

		//If the player is in sight
		if (pKiller)
		{
			// Additional test - only killing if the player within 60 degrees from the guard's front (they have no eyes in the back of their head)
			CVector v = m_player.GetPosition() - hunter.GetPosition();


			//If The Dot Product of the displacement vector and the guards velocity vector is les than or equal to 0.5 then pKiller will be equal to NULL
			if (Dot(Normalise(v), Normalise(hunter.GetVelocity())) <= 0.5)
			{
				pKiller = NULL;
			}

			if (pKiller) 
			{
				GameOver();
			}
		}
	}
	else 
	{
		pKiller = NULL;
	}

	//Prey Steering Forces
	CVector steerPrey;

	// Calculate the Player Steering

	//If the player is near the Prey then it will chase it
	if (Distance(m_player.GetPosition(), prey.GetPosition()) < 300)
	{
		steerPlayer = m_player.seek(prey.GetPosition(), playerChaseSpeed);
		chase = true;
	}
	else //If the player is not within range then it will wander around
	{
		if (!guided)
		{
			steerPlayer = m_player.wander(playerWanderSpeed);
			chase = false;
		}
		else if (guided)
		{

			steerPlayer = m_player.seek(mousetarget.GetPosition(), playerWanderSpeed);

			if (m_player.HitTest(&mousetarget))
			{
				guided = false;
			}
		}
	}

	//My Attempt of a different type of Obstacle Avoidance
	//for (CSprite* pObstacle : obstacle)
	//{
	//	//Distance between player and Obstacle
	//	float distance = sqrt(pow(m_player.GetX() - pObstacle->GetX(), 2) + pow(pObstacle->GetY() - m_player.GetY(), 2));

	//	if (distance < sqrt(pow(pObstacle->GetW(), 2) + pow(pObstacle->GetH(), 2)/ 1.5))
	//	{
	//		steerPlayer = m_player.obstacleav(pObstacle->GetPosition());
	//	}

	//	//Distance between Prey and Obstacle
	//	float pdistance = sqrt(pow(prey.GetX() - pObstacle->GetX(), 2) + pow(pObstacle->GetY() - prey.GetY(), 2));

	//	if (pdistance < sqrt(pow(pObstacle->GetW(), 2) + pow(pObstacle->GetH(), 2) / 1.5))
	//	{
	//		steerPrey = prey.obstacleav(pObstacle->GetPosition());
	//	}

	//}
	//if (chase)
	//{
	//	if (m_player.GetSpeed() > playerChaseSpeed)
	//	{
	//		m_player.SetSpeed(playerChaseSpeed / 2);
	//	}
	//}
	//else
	//{
	//	if (m_player.GetSpeed() > playerWanderSpeed)
	//	{
	//		m_player.SetSpeed(playerWanderSpeed / 2);
	//	}
	//}

	//if (flee)
	//{
	//	if (prey.GetSpeed() > preySpeed)
	//	{
	//		prey.SetSpeed(preySpeed / 2);
	//	}
	//}
	//else
	//{
	//	if (prey.GetSpeed() > preyWanderSpeed)
	//	{
	//		prey.SetSpeed(preyWanderSpeed / 2);
	//	}
	//}


	//Obstacle avoidance function for the Player and Prey
	playeravoidObstacle();
	preyavoidObstacle();
	



	// If The Prey it close to the player then the Prey will flee and if it is not near the player then it will wander
	if (Distance(prey.GetPosition(), m_player.GetPosition()) < 150)
	{
		steerPrey = prey.flee(m_player.GetPosition(), preySpeed);
		flee = true;
	}
	else 
	{
		steerPrey = prey.wander(preyWanderSpeed);
		flee = false;
	}

	//Player Animation
	if (m_player.GetXVelocity() > 0 && walkingl) 
	{
		m_player.SetAnimation("pwalkr");
		walkingl = false;
	}
	else if (m_player.GetXVelocity() < 0 && !walkingl)
	{
		m_player.SetAnimation("pwalkl");
		walkingl = true;
	}

	//Prey Animation
	if (prey.GetXVelocity() > 0 && preywalkingl)
	{
		prey.SetAnimation("walkr");
		preywalkingl = false;
	}
	else if (prey.GetXVelocity() < 0 && !preywalkingl)
	{
		prey.SetAnimation("walkl");
		preywalkingl = true;
	}
	
	// Apply the Steering Forces
	prey.Accelerate(steerPrey* (float)GetDeltaTime() / 1000.f);
	m_player.Accelerate(steerPlayer * (float)GetDeltaTime() / 1000.f);
	

	// Update Sprites
	prey.Update(GetTime());
	m_player.Update(GetTime());
	hunter.Update(GetTime());
	for (CSprite* pObstacle : obstacle) pObstacle->Update(GetTime());



	/*If Statements are here because when i try to just put the function in OnUpdate() by itself it will start glitching out on the first node so i tried to make a switch like its pressing 
	 the mouse button to activate the function*/
	if (!dest && !arrivedonce) 
	{
		hunterPath();
		dest = true;
	}
	else if (hunter.HitTest(&endtarget) && dest && !arrivedonce) 
	{
		hunterPath();
		arrivedonce = true;
		dest = false;

	}
	else if (!dest && arrivedonce && hunter.HitTest(&endtarget)) 
	{
		hunterPath();
		dest = true;

	}
	else if (dest && arrivedonce && hunter.HitTest(&endtarget)) 
	{
		hunterPath();
		dest = false;
		arrivedonce = false;
	}
	// keep the prey and the player within the scene
	extern CGameApp app;
	prey.wrapToScreen(app.GetWidth(), app.GetHeight());
	m_player.wrapToScreen(app.GetWidth(), app.GetHeight());

	//If the Player touches the prey then they win the game and GameOver is called
	if (m_player.HitTest(&prey)) 
	{
		GameOver();
	}

}

void CMyGame::OnDraw(CGraphics* g)
{
	// Draw the Background
	g->Blit(CVectorI(0, 0), grass);

	for (CSprite* pObstacle : obstacle) pObstacle->Draw(g);
	prey.Draw(g);
	m_player.Draw(g);
	hunter.Draw(g);
	/*The Graph and nodes
	for (NODE n : m_graph)
		for (CONNECTION c : n.conlist)
			g->DrawLine(n.pos, m_graph[c.nEnd].pos, CColor::Black());
	m_nodes.for_each(&CSprite::Draw, g);*/


	if (guided) 
	{
		mousetarget.Draw(g);
	}
	
	//Used to Test if certain things are working
	//*g << font(40) << color(CColor::Red()) << vcenter << center << xpos << "," << ypos << endl << guided;
	//*g << font(40) << color(CColor::Blue()) << vcenter << center << Distance(m_player.GetPosition(), prey.GetPosition());
	//*g << font(40) << color(CColor::Blue()) << vcenter << center << dest;
	//*g << font(40) << color(CColor::Blue()) << vcenter << center << endl << arrivedonce;
	//*g << font(40) << color(CColor::Blue()) << vcenter << center << m_player.GetXVelocity();
	//endtarget.Draw(g);
	//*g << font(40) << color(CColor::Blue()) << vcenter << center <<"Player Speed: " <<  m_player.GetSpeed() << endl << "Prey Speed: " << prey.GetSpeed();
	//*g << font(40) << color(CColor::Blue()) << vcenter << center << "Player walking left is:" << walkingl;

	if (m_player.HitTest(&prey)) 
	{
		*g << font(40) << color(CColor::Blue()) << vcenter << center << "You Saved The Bulbasaur!";
	}
	
	if (pKiller) 
	{
		g->DrawLine(hunter.GetPosition(), m_player.GetPosition(), 4, CColor::Red());
		if (IsGameOver())
			*g << font(48) << center << vcenter << color(CColor::White()) << "You have been Shot!" << endl << font(20) << "PRESS F2 TO RESTART";
	}


	
}

CFVector CMyGame::playerobstacleav(double radius) 
{
	CFVector vec;

	CRectangle rect;
	for each (CSprite * pSprite in obstacle)
	{
		pSprite->GetBoundingRect(rect);
		rect.Grow(2 * (Uint16)radius);

		//For The Player
		if (rect.Contains(m_player.GetCenter()))
		{
			if (m_player.GetX() < pSprite->GetLeft())
				vec.m_x = (pSprite->GetLeft() - 2 * radius - m_player.GetX()) * abs(m_player.GetXVelocity()) / radius;
			if (m_player.GetX() > pSprite->GetRight())
				vec.m_x = (pSprite->GetRight() + 2 * radius - m_player.GetX()) * abs(m_player.GetXVelocity()) / radius;
			if (m_player.GetY() > pSprite->GetTop())
				vec.m_y = (pSprite->GetTop() + 2 * radius - m_player.GetY()) * abs(m_player.GetYVelocity()) / radius;
			if (m_player.GetY() < pSprite->GetBottom())
				vec.m_y = (pSprite->GetBottom() - 2 * radius - m_player.GetY()) * abs(m_player.GetYVelocity()) / radius;

		}
	}

	return vec;
}

CFVector CMyGame::preyobstacleav(double radius) 
{
	CFVector vec;

	CRectangle rect;
	for each (CSprite * pSprite in obstacle)
	{
		pSprite->GetBoundingRect(rect);
		rect.Grow(2 * (Uint16)radius);

		//For The Prey
		if (rect.Contains(prey.GetCenter()))
		{
			if (prey.GetX() < pSprite->GetLeft())
				vec.m_x = (pSprite->GetLeft() - 2 * radius - prey.GetX()) * abs(prey.GetXVelocity()) / radius;
			if (prey.GetX() > pSprite->GetRight())
				vec.m_x = (pSprite->GetRight() + 2 * radius - prey.GetX()) * abs(prey.GetXVelocity()) / radius;
			if (prey.GetY() > pSprite->GetTop())
				vec.m_y = (pSprite->GetTop() + 2 * radius - prey.GetY()) * abs(prey.GetYVelocity()) / radius;
			if (prey.GetY() < pSprite->GetBottom())
				vec.m_y = (pSprite->GetBottom() - 2 * radius - prey.GetY()) * abs(prey.GetYVelocity()) / radius;
		}
	}

	return vec;
}

void CMyGame::playeravoidObstacle() 
{
	double r = 20;


	CFVector vec;

	CFVector obs = playerobstacleav(r);
	obs *= 2.0;
	vec += obs;

	if (chase) 
	{
		if (m_player.GetSpeed() > playerChaseSpeed)
		{
			m_player.SetSpeed(playerChaseSpeed /2 );
		}
	}
	else 
	{
		if (m_player.GetSpeed() > playerWanderSpeed)
		{
			m_player.SetSpeed(playerWanderSpeed / 2);
		}
	}

	m_player.SetVelocity(m_player.GetXVelocity() + vec.GetX(), m_player.GetYVelocity() + vec.GetY());
}

void CMyGame::preyavoidObstacle() 
{
	double r = 20;


	CFVector vec;

	CFVector obs = preyobstacleav(r);
	obs *= 2.0;
	vec += obs;

	if (flee) 
	{
		if (prey.GetSpeed() > preySpeed)
		{
			prey.SetSpeed(preySpeed / 2);
		}
	}
	else 
	{
		if (prey.GetSpeed() > preyWanderSpeed)
		{
			prey.SetSpeed(preyWanderSpeed / 2 );
		}
	}


	prey.SetVelocity(prey.GetXVelocity() + vec.GetX(), prey.GetYVelocity() + vec.GetY());
}
/////////////////////////////////////////////////////
// Game Life Cycle

// one time initialisation
void CMyGame::OnInitialize()
{
	m_player.LoadAnimation("pikachul.bmp", "pwalkl", CSprite::Sheet(3, 1).Row(0).From(0).To(2), CColor::Blue());
	m_player.LoadAnimation("pikachur.bmp", "pwalkr", CSprite::Sheet(3, 1).Row(0).From(2).To(0), CColor::Blue());
	m_player.LoadImage("pikachuidle.bmp", "idle", CSprite::Sheet(1, 1).Tile(0, 0), CColor::Blue());

	prey.LoadImage("bulbasaur.bmp", "idle", CSprite::Sheet(3, 1).Tile(0,0), CColor::Blue());
	prey.LoadAnimation("bulbasaurR.bmp", "walkr", CSprite::Sheet(3, 1).Row(0).From(0).To(2), CColor::Blue());
	prey.LoadAnimation("bulbasaurl.bmp", "walkl", CSprite::Sheet(3, 1).Row(0).From(0).To(2), CColor::Blue());

	hunter.LoadAnimation("trwalkr.bmp", "walkr", CSprite::Sheet(3, 1).Row(0).From(0).To(2), CColor::Blue());
	hunter.LoadAnimation("trwalkl.bmp", "walkl", CSprite::Sheet(3, 1).Row(0).From(2).To(0), CColor::Blue());
	hunter.LoadImage("tr.bmp", "idle", CSprite::Sheet(1, 1).Tile(0, 0), CColor::Blue());

	// Create Nodes
	int i = 0;
	for (NODE n : m_graph)
	{
		stringstream s;
		s << i++;
		m_nodes.push_back(new CSpriteOval(n.pos, 12, CColor::White(), CColor::Black(), 0));
		m_nodes.push_back(new CSpriteText(n.pos, "arial.ttf", 14, s.str(), CColor::Black(), 0));
	}
}

bool PathFind(vector<NODE>& graph, int nStart, int nGoal, vector<int>& path)
{
	//List of Open Nodes
	list<int> open;

	//Mark all the nodes as unvisited
	for (unsigned i = 0; i < graph.size(); i++)
	{
		graph[i].open = false;
	}

	//Open The Start Node
	graph[nStart].open = true;
	graph[nStart].costSoFar = 0;
	graph[nStart].nConnection = -1;
	//Adding Start node to the open list
	open.push_back(nStart);

	while (open.size() > 0)
	{

		//This code means that the current node will be the node with the lowest costSoFar
		list<int>::iterator iCurrent = min_element(open.begin(), open.end(), [graph](int i, int j) -> bool {
			return graph[i].costSoFar < graph[j].costSoFar;
			});

		int curNode = *iCurrent;

		if (curNode == nGoal) break;

		for each (CONNECTION c in graph[curNode].conlist)
		{
			int endNode = c.nEnd;

			int newCostSoFar = graph[curNode].costSoFar + c.cost;

			//For open nodes, ignore them if the current cost so far is less than the newCostSo far 
			if (graph[endNode].open && graph[endNode].costSoFar <= newCostSoFar) continue;


			graph[endNode].costSoFar = newCostSoFar;
			graph[endNode].nConnection = curNode;

			if (!graph[endNode].open)
			{
				graph[endNode].open = true;
				//Putting the End Node in the open list
				open.push_back(endNode);
			}
		}

		//Close the current node
		graph[curNode].closed = true;
		open.erase(iCurrent);
	}


	if (open.size() < 1) return false; //Path not found

	int i = nGoal;

	while (graph[i].nConnection >= 0)
	{
		path.push_back(i);
		i = graph[i].nConnection;
	}

	path.push_back(i);

	reverse(path.begin(), path.end());

	/*path.push_back(nStart);
	path.push_back(nGoal);*/
	return true;
}

void CMyGame::hunterPath()
{ 
	
	
	//If the hunter is not at the destination then set its destination to this
	if (!dest && !arrivedonce) 
	{
		xtarget = 410;
		ytarget = 531;
		endtarget.SetPosition(xtarget, ytarget);
	}

	else if (!dest && arrivedonce)
	{
		xtarget = 50;
		ytarget = 100;
		endtarget.SetPosition(xtarget, ytarget);
	}

	else if (dest && !arrivedonce) // When you are at the destination you will go back to the starting position 
	{
		xtarget = 567;
		ytarget = 280;
		endtarget.SetPosition(xtarget, ytarget);

	}


	CVector v(xtarget, ytarget);	// destination
	


// find the first node: the closest to the NPC
	vector<NODE>::iterator iFirst = min_element(m_graph.begin(), m_graph.end(), [this](NODE& n1, NODE& n2) -> bool {
		return Distance(n1.pos, hunter.GetPos()) < Distance(n2.pos, hunter.GetPos());
		});
	int nFirst = iFirst - m_graph.begin();
	// find the last node: the closest to the destination
	vector<NODE>::iterator iLast = min_element(m_graph.begin(), m_graph.end(), [v](NODE& n1, NODE& n2) -> bool {
		return Distance(n1.pos, v) < Distance(n2.pos, v);
		});
	int nLast = iLast - m_graph.begin();
	// remove the current way points and reset the NPC
	if (!m_waypoints.empty())
	{
		m_waypoints.clear();
		hunter.SetVelocity(0, 0);
	}
	// call the path finding algorithm to complete the waypoints
	vector<int> path;

	if (PathFind(m_graph, nFirst, nLast, path))
	{
		for (int i : path)
		{
			m_waypoints.push_back(m_graph[i].pos);
		}
		m_waypoints.push_back(v);
	}
}
// called at the start of a new game - display menu here
void CMyGame::OnDisplayMenu()
{
	prey.SetVelocity(0, 0);
	prey.SetRotation(0);
	prey.SetPosition(50, 550);
	prey.SetImage("idle");

	m_player.SetPosition(rand() % 1260, rand() % 500);
	m_player.SetVelocity(0, 0);
	m_player.SetImage("idle");
	guided = false;
	chase = false;

	hunter.SetPosition(50, 100);
	hunter.SetVelocity(0, 0);
	hunter.SetAnimation("idle");
	dest = false;
	arrivedonce = false;

	CSprite* pObstacle;
	pObstacle = new CSpriteRect(900, 384, 100, 100, CColor::Black(), 0);
	obstacle.push_back(pObstacle);

	pObstacle = new CSpriteRect(900, 600, 40, 40, CColor::Black(), 0);
	obstacle.push_back(pObstacle);

	StartGame();	// this allows to start the game immediately
}

// called when Game Mode entered
void CMyGame::OnStartGame()
{
}

// called when Game is Over
void CMyGame::OnGameOver()
{
}

// one time termination code
void CMyGame::OnTerminate()
{
}

/////////////////////////////////////////////////////
// Keyboard Event Handlers

void CMyGame::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode)
{
	if (sym == SDLK_F4 && (mod & (KMOD_LALT | KMOD_RALT)))
		StopGame();
	if (sym == SDLK_SPACE)
		PauseGame();
	if (sym == SDLK_F2)
		NewGame();
}

void CMyGame::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode)
{
}


/////////////////////////////////////////////////////
// Mouse Events Handlers

void CMyGame::OnMouseMove(Uint16 x,Uint16 y,Sint16 relx,Sint16 rely,bool bLeft,bool bRight,bool bMiddle)
{
	
	if (lbuttondown) 
	{
		xpos = x;
		ypos = y;
		mousetarget.SetPosition(xpos, ypos);
	}

}

void CMyGame::OnLButtonDown(Uint16 x,Uint16 y)
{
	if (!IsGameOver()) 
	{
		guided = true;

		xpos = x;
		ypos = y;
		mousetarget.SetPosition(xpos, ypos);
		lbuttondown = true;
	}

}

void CMyGame::OnLButtonUp(Uint16 x,Uint16 y)
{
	lbuttondown = false;

}

void CMyGame::OnRButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnRButtonUp(Uint16 x,Uint16 y)
{
	//If you feel like the player isn't catching up to the Rabbit when chasing then press this button to slow the player down
	m_player.SetVelocity(0, 0);
}

void CMyGame::OnMButtonDown(Uint16 x,Uint16 y)
{
}

void CMyGame::OnMButtonUp(Uint16 x,Uint16 y)
{
}
