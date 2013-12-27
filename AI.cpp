#include "AI.h"
#include "constants.h"
#include <vector>
#include <time.h>

using namespace std;

double getDistance(int x1,int y1,int x2,int y2)
{
	int x = x1-x2;
	int y = y1-y2;
	return (sqrt(x*x + y*y));
}

double getManhattanDistance(int x1,int y1,int x2,int y2)
{
	int x = abs(x1-x2);
	int y = abs(y1-y2);
	return (x+y);
}

double getAccurateDistance(int x1,int y1,int x2,int y2)
{
	int x = abs(x1-x2);
	int y = abs(y1-y2);
	int a = y;
	int b = x;
	if(x>y)
	{
		b=y;
		a=x;
	}
	return (b*DIAGTIME/STRAIGHTTIME + (a-b));
}

Node::Node()
{
	x=0;
	y=0;
}

Node::Node(int a,int b)
{
	x=a;
	y=b;
}

AI::AI()
{
	MyPlayer = MAP_PLAYER1;
	OtherPlayer = MAP_PLAYER2;
	type = AI_SIMPLE;
}

AI::AI(int Player,int AItype)
{
	initiate(Player,AItype);
}

void AI::initiate(int Player,int AItype)
{
	MyPlayer = Player;
	if(Player==MAP_PLAYER1)
		OtherPlayer = MAP_PLAYER2;
	else
		OtherPlayer = MAP_PLAYER1;
	type = AItype;
}

AI::~AI()
{
}

int AI::think(int MAP[MAP_WIDTH][MAP_HEIGHT])
{
	if(type==AI_RANDOM)
	{
		srand(time(0));
		return rand()%8;
	}
	else if(type==AI_SIMPLE)
		return Simple(MAP);
	else if(type==AI_GRAPH_SIMPLE)
		return Graph(MAP);
    else if(type==AI_GRAPH_LINEAR || type==AI_GRAPH_EXP)
		return Graph2(MAP);
    else if(type==AI_GRAPH_SUPPORT)
		return GraphSupport(MAP);
	else if(type==AI_GRAPH_SUPPORT_DEF)
		return GraphSupport(MAP);
	else if(type==AI_GRAPH_SUPPORT_AGGR)
		return GraphSupport(MAP);
	else if(type==AI_GRAPH_SUPPORT2)
		return GraphSupport(MAP);
	else if(type==AI_GRAPH_SUPPORT2_AGGR)
		return GraphSupport(MAP);
	else if(type==AI_GRAPH_SUPPORT2_DEF)
		return GraphSupport(MAP);
	else if(type==AI_GRAPH_SUPPORT3)
		return GraphSupport(MAP);
	else if(type==AI_GRAPH_SUPPORT3_AGGR)
		return GraphSupport(MAP);
	else if(type==AI_GRAPH_SUPPORT3_DEF)
		return GraphSupport(MAP);
	else if(type==AI_SIMPLE2)
		return NN2(MAP);
	else if(type==AI_SIMPLE3)
		return NN3(MAP);
	return DIR_UNDECIDED; //should never happen
}

int AI::Simple(int MAP[MAP_WIDTH][MAP_HEIGHT])
{
	vector<Node> nodes(0);
	Node me;
	for(int i = 0;i<MAP_WIDTH;i++)
	{
		for(int j = 0;j<MAP_HEIGHT;j++)
		{
			if(MAP[i][j] == MAP_STAR)
			{
				Node n(i,j);
				nodes.push_back(n);
			}
			if(MAP[i][j] == MyPlayer)
			{
				me.x = i;
				me.y = j;
			}
		}
	}
	if(nodes.size()==0)
		return DIR_UNDECIDED;
	int nodeindex = 0;
	double dist = 9999999;
	for(int i = 0;i<nodes.size();i++)
	{
		Node t = nodes.at(i);
		double x = sqrt((me.x-t.x)*(me.x-t.x) + (me.y-t.y)*(me.y-t.y));
		if(x<dist)
		{
			dist = x;
			nodeindex = i;
		}
	}
	Node t = nodes.at(nodeindex);
	if(t.x > me.x && t.y > me.y)
		return DIR_DOWNRIGHT;
	else if(t.x > me.x && t.y < me.y)
		return DIR_UPRIGHT;
	else if(t.x > me.x && t.y == me.y)
		return DIR_RIGHT;
	else if(t.x < me.x && t.y > me.y)
		return DIR_DOWNLEFT;
	else if(t.x < me.x && t.y < me.y)
		return DIR_UPLEFT;
	else if(t.x < me.x && t.y == me.y)
		return DIR_LEFT;
	else if(t.x == me.x && t.y > me.y)
		return DIR_DOWN;
	else if(t.x == me.x && t.y < me.y)
		return DIR_UP;
	return DIR_UNDECIDED; //should never happen
}

int AI::Graph(int MAP[MAP_WIDTH][MAP_HEIGHT])
{
	vector<Node> nodes(0);
	Node me(0,0);
	double G[MAP_WIDTH][MAP_HEIGHT];

	//get stars
	for(int i = 0;i<MAP_WIDTH;i++)
	{
		for(int j = 0;j<MAP_HEIGHT;j++)
		{
			if(MAP[i][j] == MAP_STAR)
			{
				Node n(i,j);
				nodes.push_back(n);
			}
			if(MAP[i][j] == MyPlayer)
			{
				me.x = i;
				me.y = j;
			}
		}		
	}
	if(nodes.size()==0)
		return DIR_UNDECIDED;

	//create density graph
	for(int i = 0;i<MAP_WIDTH;i++)
	{
		for(int j = 0;j<MAP_HEIGHT;j++)
		{
			if(me.x!=i || me.y!=j)
			    G[i][j] = 500/((me.x-i)*(me.x-i) + (me.y-j)*(me.y-j));
		}		
	}

	for(int i = 0;i<nodes.size();i++)
	{
		Node* t = &nodes.at(i);
		//central node
		G[t->x][t->y] += 1000;
		//proximity nodes
		if(t->y < MAP_HEIGHT-1)
			G[t->x][t->y+1] += 20;
		if(t->y > 0)
			G[t->x][t->y-1] += 20;
		if(t->x < MAP_WIDTH-1)
		{
		    G[t->x+1][t->y] += 20;
		    if(t->y < MAP_HEIGHT-1)
		        G[t->x+1][t->y+1] += 20;
			if(t->y > 0)
		    G[t->x+1][t->y-1] += 20;
		}
		if(t->x > 0)
		{
		    G[t->x-1][t->y] += 20;
			if(t->y < MAP_HEIGHT-1)
				G[t->x-1][t->y+1] += 20;
			if(t->y > 0)
				G[t->x-1][t->y-1] += 20;
		}
		//secondary proximity nodes
		if(t->y < MAP_HEIGHT-2)
			G[t->x][t->y+2] += 20;
		if(t->x < MAP_WIDTH-2)
			G[t->x+2][t->y] += 20;
		if(t->y > 1)
			G[t->x][t->y-2] += 20;
		if(t->x > 1)
			G[t->x-2][t->y] += 20;
	}

	//find greatest density point
	Node t(0,0);
	int max=0;
	for(int i = 0;i<MAP_WIDTH;i++)
	{
		for(int j = 0;j<MAP_HEIGHT;j++)
		{
			if(G[i][j]>max)
			{
				max = G[i][j];
				t.x = i;
				t.y = j;
			}
		}
	}

	//move towards max density
	if(t.x < me.x && t.y > me.y)
		return DIR_DOWNLEFT;
	else if(t.x > me.x && t.y < me.y)
		return DIR_UPRIGHT;
	else if(t.x > me.x && t.y == me.y)
		return DIR_RIGHT;
	else if(t.x > me.x && t.y > me.y)
		return DIR_DOWNRIGHT;
	else if(t.x < me.x && t.y < me.y)
		return DIR_UPLEFT;
	else if(t.x < me.x && t.y == me.y)
		return DIR_LEFT;
	else if(t.x == me.x && t.y > me.y)
		return DIR_DOWN;
	else if(t.x == me.x && t.y < me.y)
		return DIR_UP;
	return DIR_UNDECIDED; //should never happen
}

int AI::Graph2(int MAP[MAP_WIDTH][MAP_HEIGHT])
{
	vector<Node> nodes(0);
	Node me(0,0);
	Node other(0,0);
	double G[MAP_WIDTH][MAP_HEIGHT];

	//get stars
	for(int i = 0;i<MAP_WIDTH;i++)
	{
		for(int j = 0;j<MAP_HEIGHT;j++)
		{
			if(MAP[i][j] == MAP_STAR)
			{
				Node n(i,j);
				nodes.push_back(n);
			}
			if(MAP[i][j] == MyPlayer)
			{
				me.x = i;
				me.y = j;
			}
			if(MAP[i][j] == OtherPlayer)
			{
				other.x = i;
				other.y = j;
			}
		}		
	}
	if(nodes.size()==0)
		return DIR_UNDECIDED;

	//create density graph
	for(int i = 0;i<MAP_WIDTH;i++)
	{
		for(int j = 0;j<MAP_HEIGHT;j++)
		{
			if(me.x!=i || me.y!=j)
			{
				double val = 0;
				if(type==AI_GRAPH_LINEAR)
					val = 1000-((me.x-i)*(me.x-i) + (me.y-j)*(me.y-j));
				else
					val = 1/((me.x-i)*(me.x-i) + (me.y-j)*(me.y-j));
			    G[i][j] = val/2;
			}
			if(other.x!=i || other.y!=j)
			{
				double val = 0;
				if(type==AI_GRAPH_LINEAR)
					val = 1000-((other.x-i)*(other.x-i) + (other.y-j)*(other.y-j));
				else
					val = 1/((other.x-i)*(other.x-i) + (other.y-j)*(other.y-j));
			    G[i][j] = -val;
			}
			for(int k = 0;k<nodes.size();k++)
			{
				if(nodes.at(k).x!=i || nodes.at(k).y!=j)
				{
					double val = 0;
					if(type==AI_GRAPH_LINEAR)
						val = 1000-((nodes.at(k).x-i)*(nodes.at(k).x-i) + (nodes.at(k).y-j)*(nodes.at(k).y-j));
					else
						val = 1/((nodes.at(k).x-i)*(nodes.at(k).x-i) + (nodes.at(k).y-j)*(nodes.at(k).y-j));
					G[i][j] += val;
				}
				else
				{
					G[i][j]+=99999;
				}
			}
		}		
	}

	for(int i = 0;i<nodes.size();i++)
	{
		Node* t = &nodes.at(i);
		//central node
		G[t->x][t->y] += 1000;
		//proximity nodes
		if(t->y < MAP_HEIGHT-1)
			G[t->x][t->y+1] += 20;
		if(t->y > 0)
			G[t->x][t->y-1] += 20;
		if(t->x < MAP_WIDTH-1)
		{
		    G[t->x+1][t->y] += 20;
		    if(t->y < MAP_HEIGHT-1)
		        G[t->x+1][t->y+1] += 20;
			if(t->y > 0)
		    G[t->x+1][t->y-1] += 20;
		}
		if(t->x > 0)
		{
		    G[t->x-1][t->y] += 20;
			if(t->y < MAP_HEIGHT-1)
				G[t->x-1][t->y+1] += 20;
			if(t->y > 0)
				G[t->x-1][t->y-1] += 20;
		}
		//secondary proximity nodes
		if(t->y < MAP_HEIGHT-2)
			G[t->x][t->y+2] += 20;
		if(t->x < MAP_WIDTH-2)
			G[t->x+2][t->y] += 20;
		if(t->y > 1)
			G[t->x][t->y-2] += 20;
		if(t->x > 1)
			G[t->x-2][t->y] += 20;
	}

	//find greatest density point
	Node t(0,0);
	int max=0;
	for(int i = 0;i<MAP_WIDTH;i++)
	{
		for(int j = 0;j<MAP_HEIGHT;j++)
		{
			if(G[i][j]>max)
			{
				max = G[i][j];
				t.x = i;
				t.y = j;
			}
		}
	}

	//move towards max density
	if(t.x < me.x && t.y > me.y)
		return DIR_DOWNLEFT;
	else if(t.x > me.x && t.y < me.y)
		return DIR_UPRIGHT;
	else if(t.x > me.x && t.y == me.y)
		return DIR_RIGHT;
	else if(t.x > me.x && t.y > me.y)
		return DIR_DOWNRIGHT;
	else if(t.x < me.x && t.y < me.y)
		return DIR_UPLEFT;
	else if(t.x < me.x && t.y == me.y)
		return DIR_LEFT;
	else if(t.x == me.x && t.y > me.y)
		return DIR_DOWN;
	else if(t.x == me.x && t.y < me.y)
		return DIR_UP;
	return DIR_UNDECIDED; //should never happen
}

int AI::GraphSupport(int MAP[MAP_WIDTH][MAP_HEIGHT])
{
	vector<Node> nodes(0);
	Node me(0,0);
	Node other(0,0);
	double G[MAP_WIDTH][MAP_HEIGHT];

	//get stars
	for(int i = 0;i<MAP_WIDTH;i++)
	{
		for(int j = 0;j<MAP_HEIGHT;j++)
		{
			if(MAP[i][j] == MAP_STAR)
			{
				Node n(i,j);
				nodes.push_back(n);
			}
			if(MAP[i][j] == MyPlayer)
			{
				me.x = i;
				me.y = j;
			}
			if(MAP[i][j] == OtherPlayer)
			{
				other.x = i;
				other.y = j;
			}
		}		
	}
	if(nodes.size()==0)
		return DIR_UNDECIDED;

	//create support graph
	double max = -9999;
	int maxid = 0;
	for(int i = 0;i<nodes.size();i++)
	{
		double val = 0;
		for(int j = 0;j<nodes.size();j++)
		{
			if(j==i) continue;
			if(type==AI_GRAPH_SUPPORT || type==AI_GRAPH_SUPPORT_AGGR || type==AI_GRAPH_SUPPORT_DEF)
			    val += 1/getDistance(nodes.at(j).x,nodes.at(j).y,nodes.at(i).x,nodes.at(i).y);
			else if(type==AI_GRAPH_SUPPORT2 || type==AI_GRAPH_SUPPORT2_AGGR || type==AI_GRAPH_SUPPORT2_DEF)
			    val += 1/getManhattanDistance(nodes.at(j).x,nodes.at(j).y,nodes.at(i).x,nodes.at(i).y);
			else if(type==AI_GRAPH_SUPPORT3 || type==AI_GRAPH_SUPPORT3_AGGR || type==AI_GRAPH_SUPPORT3_DEF)
			    val += 1/getAccurateDistance(nodes.at(j).x,nodes.at(j).y,nodes.at(i).x,nodes.at(i).y);
		}
		if(type==AI_GRAPH_SUPPORT || type==AI_GRAPH_SUPPORT_AGGR || type==AI_GRAPH_SUPPORT_DEF)
			val += 5/getDistance(me.x,me.y,nodes.at(i).x,nodes.at(i).y);
		else if(type==AI_GRAPH_SUPPORT2 || type==AI_GRAPH_SUPPORT2_AGGR || type==AI_GRAPH_SUPPORT2_DEF)
			val += 5/getManhattanDistance(me.x,me.y,nodes.at(i).x,nodes.at(i).y);
		else if(type==AI_GRAPH_SUPPORT3 || type==AI_GRAPH_SUPPORT3_AGGR || type==AI_GRAPH_SUPPORT3_DEF)
			val += 5/getAccurateDistance(me.x,me.y,nodes.at(i).x,nodes.at(i).y);
		if(type==AI_GRAPH_SUPPORT_AGGR || type==AI_GRAPH_SUPPORT2_AGGR || type==AI_GRAPH_SUPPORT3_AGGR)
		{
			if(type==AI_GRAPH_SUPPORT || type==AI_GRAPH_SUPPORT_AGGR || type==AI_GRAPH_SUPPORT_DEF)
			    val *= 20/getDistance(me.x,me.y,nodes.at(i).x,nodes.at(i).y);
			else if(type==AI_GRAPH_SUPPORT2 || type==AI_GRAPH_SUPPORT2_AGGR || type==AI_GRAPH_SUPPORT2_DEF)
			    val *= 20/getManhattanDistance(me.x,me.y,nodes.at(i).x,nodes.at(i).y);
			else if(type==AI_GRAPH_SUPPORT3 || type==AI_GRAPH_SUPPORT3_AGGR || type==AI_GRAPH_SUPPORT3_DEF)
			    val *= 20/getAccurateDistance(me.x,me.y,nodes.at(i).x,nodes.at(i).y);
		}
		else if(type==AI_GRAPH_SUPPORT_DEF || type==AI_GRAPH_SUPPORT2_DEF || type==AI_GRAPH_SUPPORT3_DEF)
		{
			if(type==AI_GRAPH_SUPPORT || type==AI_GRAPH_SUPPORT_AGGR || type==AI_GRAPH_SUPPORT_DEF)
			    val /= 50/getDistance(me.x,me.y,nodes.at(i).x,nodes.at(i).y);
			else if(type==AI_GRAPH_SUPPORT2 || type==AI_GRAPH_SUPPORT2_AGGR || type==AI_GRAPH_SUPPORT2_DEF)
			    val /= 50/getManhattanDistance(me.x,me.y,nodes.at(i).x,nodes.at(i).y);
			else if(type==AI_GRAPH_SUPPORT3 || type==AI_GRAPH_SUPPORT3_AGGR || type==AI_GRAPH_SUPPORT3_DEF)
			    val /= 50/getAccurateDistance(me.x,me.y,nodes.at(i).x,nodes.at(i).y);
		}
		if(val>max)
		{
			max = val;
		    maxid = i;
		}
	}

	Node t = nodes.at(maxid);
	//move towards max support
	if(t.x < me.x && t.y > me.y)
		return DIR_DOWNLEFT;
	else if(t.x > me.x && t.y < me.y)
		return DIR_UPRIGHT;
	else if(t.x > me.x && t.y == me.y)
		return DIR_RIGHT;
	else if(t.x > me.x && t.y > me.y)
		return DIR_DOWNRIGHT;
	else if(t.x < me.x && t.y < me.y)
		return DIR_UPLEFT;
	else if(t.x < me.x && t.y == me.y)
		return DIR_LEFT;
	else if(t.x == me.x && t.y > me.y)
		return DIR_DOWN;
	else if(t.x == me.x && t.y < me.y)
		return DIR_UP;
	return DIR_UNDECIDED; //should never happen
}

int AI::GraphSupportManhattan(int MAP[MAP_WIDTH][MAP_HEIGHT])
{
	vector<Node> nodes(0);
	Node me(0,0);
	Node other(0,0);
	double G[MAP_WIDTH][MAP_HEIGHT];

	//get stars
	for(int i = 0;i<MAP_WIDTH;i++)
	{
		for(int j = 0;j<MAP_HEIGHT;j++)
		{
			if(MAP[i][j] == MAP_STAR)
			{
				Node n(i,j);
				nodes.push_back(n);
			}
			if(MAP[i][j] == MyPlayer)
			{
				me.x = i;
				me.y = j;
			}
			if(MAP[i][j] == OtherPlayer)
			{
				other.x = i;
				other.y = j;
			}
		}		
	}
	if(nodes.size()==0)
		return DIR_UNDECIDED;

	//create support graph
	double max = -9999;
	int maxid = 0;
	for(int i = 0;i<nodes.size();i++)
	{
		double val = 0;
		for(int j = 0;j<nodes.size();j++)
		{
			if(j==i) break;
			val += 1/(abs(me.x-nodes.at(i).x)+abs(me.y-nodes.at(i).y));
		}
		val *= 300/(abs(me.x-nodes.at(i).x)+abs(me.y-nodes.at(i).y));
		if(type==AI_GRAPH_SUPPORT2_AGGR)
			val *= 20/(abs(me.x-nodes.at(i).x)+abs(me.y-nodes.at(i).y));
		else if(type==AI_GRAPH_SUPPORT2_DEF)
			val /= 50/(abs(me.x-nodes.at(i).x)+abs(me.y-nodes.at(i).y));
		if(val>max)
		{
			max = val;
		    maxid = i;
		}
	}

	Node t = nodes.at(maxid);
	//move towards max support
	if(t.x < me.x && t.y > me.y)
		return DIR_DOWNLEFT;
	else if(t.x > me.x && t.y < me.y)
		return DIR_UPRIGHT;
	else if(t.x > me.x && t.y == me.y)
		return DIR_RIGHT;
	else if(t.x > me.x && t.y > me.y)
		return DIR_DOWNRIGHT;
	else if(t.x < me.x && t.y < me.y)
		return DIR_UPLEFT;
	else if(t.x < me.x && t.y == me.y)
		return DIR_LEFT;
	else if(t.x == me.x && t.y > me.y)
		return DIR_DOWN;
	else if(t.x == me.x && t.y < me.y)
		return DIR_UP;
	return DIR_UNDECIDED; //should never happen
}

int AI::NN2(int MAP[MAP_WIDTH][MAP_HEIGHT])
{
	vector<Node> nodes(0);
	Node me;
	for(int i = 0;i<MAP_WIDTH;i++)
	{
		for(int j = 0;j<MAP_HEIGHT;j++)
		{
			if(MAP[i][j] == MAP_STAR)
			{
				Node n(i,j);
				nodes.push_back(n);
			}
			if(MAP[i][j] == MyPlayer)
			{
				me.x = i;
				me.y = j;
			}
		}
	}
	if(nodes.size()==0)
		return DIR_UNDECIDED;
	if(nodes.size()<2)
		return Simple(MAP);
	int nodeindex = 0;
	double dist = 9999999;
	for(int i = 0;i<nodes.size();i++)
	{
		Node t = nodes.at(i);
		double x = sqrt((me.x-t.x)*(me.x-t.x) + (me.y-t.y)*(me.y-t.y));
		for(int j = 0;j<nodes.size();j++)
		{
			Node l = nodes.at(j);
			double y = sqrt((me.x-l.x)*(me.x-l.x) + (me.y-l.y)*(me.y-l.y));
			if(y>x)
			{
				x += sqrt((l.x-t.x)*(l.x-t.x) + (l.y-t.y)*(l.y-t.y));
				if(x<dist)
				{
					dist = x;
					nodeindex = i;
				}
			}
		}
	}
	Node t = nodes.at(nodeindex);
	if(t.x > me.x && t.y > me.y)
		return DIR_DOWNRIGHT;
	else if(t.x > me.x && t.y < me.y)
		return DIR_UPRIGHT;
	else if(t.x > me.x && t.y == me.y)
		return DIR_RIGHT;
	else if(t.x < me.x && t.y > me.y)
		return DIR_DOWNLEFT;
	else if(t.x < me.x && t.y < me.y)
		return DIR_UPLEFT;
	else if(t.x < me.x && t.y == me.y)
		return DIR_LEFT;
	else if(t.x == me.x && t.y > me.y)
		return DIR_DOWN;
	else if(t.x == me.x && t.y < me.y)
		return DIR_UP;
	return DIR_UNDECIDED; //should never happen
}

int AI::NN3(int MAP[MAP_WIDTH][MAP_HEIGHT])
{
	vector<Node> nodes(0);
	Node me;
	for(int i = 0;i<MAP_WIDTH;i++)
	{
		for(int j = 0;j<MAP_HEIGHT;j++)
		{
			if(MAP[i][j] == MAP_STAR)
			{
				Node n(i,j);
				nodes.push_back(n);
			}
			if(MAP[i][j] == MyPlayer)
			{
				me.x = i;
				me.y = j;
			}
		}
	}
	if(nodes.size()==0)
		return DIR_UNDECIDED;
	if(nodes.size()<3)
		return Simple(MAP);
	int nodeindex = 0;
	double dist = 9999999;
	for(int i = 0;i<nodes.size();i++)
	{
		Node t = nodes.at(i);
		double x = sqrt((me.x-t.x)*(me.x-t.x) + (me.y-t.y)*(me.y-t.y));
		for(int j = 0;j<nodes.size();j++)
		{
			if(j!=i)
			{
				Node l = nodes.at(j);
				x += sqrt((l.x-t.x)*(l.x-t.x) + (l.y-t.y)*(l.y-t.y));
				for(int k = 0;k<nodes.size();k++)
				{
					if(k!=j && k!=i)
					{
						Node v = nodes.at(k);
						x += sqrt((l.x-v.x)*(l.x-v.x) + (l.y-v.y)*(l.y-v.y));
						if(x<dist)
						{
							dist = x;
							nodeindex = i;
						}
					}
				}
			}
		}
	}
	Node t = nodes.at(nodeindex);
	if(t.x > me.x && t.y > me.y)
		return DIR_DOWNRIGHT;
	else if(t.x > me.x && t.y < me.y)
		return DIR_UPRIGHT;
	else if(t.x > me.x && t.y == me.y)
		return DIR_RIGHT;
	else if(t.x < me.x && t.y > me.y)
		return DIR_DOWNLEFT;
	else if(t.x < me.x && t.y < me.y)
		return DIR_UPLEFT;
	else if(t.x < me.x && t.y == me.y)
		return DIR_LEFT;
	else if(t.x == me.x && t.y > me.y)
		return DIR_DOWN;
	else if(t.x == me.x && t.y < me.y)
		return DIR_UP;
	return DIR_UNDECIDED; //should never happen
}

int AIAsvin(int MAP[MAP_WIDTH][MAP_HEIGHT])
{
	//insert code here
	return DIR_UNDECIDED;
}

int AISayan(int MAP[MAP_WIDTH][MAP_HEIGHT])
{
	//insert code here
	return DIR_UNDECIDED;
}