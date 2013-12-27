#ifndef AI_H_INCLUDED
#define AI_H_INCLUDED

#include "constants.h"

class Node
{
public:
	int x;
	int y;
	Node();
	Node(int a,int b);
};

class AI
{
public:
	int type;
	int MyPlayer;
	int OtherPlayer;

	AI();
	AI(int Player,int AItype);
	~AI();
	void initiate(int Player,int AItype);
	int think(int MAP[MAP_WIDTH][MAP_HEIGHT]);
	int Simple(int MAP[MAP_WIDTH][MAP_HEIGHT]);
	int Graph(int MAP[MAP_WIDTH][MAP_HEIGHT]);
	int Graph2(int MAP[MAP_WIDTH][MAP_HEIGHT]);
	int GraphSupport(int MAP[MAP_WIDTH][MAP_HEIGHT]);
	int GraphSupportManhattan(int MAP[MAP_WIDTH][MAP_HEIGHT]); //not used
	int NN2(int MAP[MAP_WIDTH][MAP_HEIGHT]);
	int NN3(int MAP[MAP_WIDTH][MAP_HEIGHT]);
	int Astar(Node a,Node b);
	int Heuristic(Node a,Node b);
};

#endif