#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

///AI TYPES:
// AI_RANDOM        : Moves Randomly
// AI_SIMPLE        : Moves towards the closest star
// AI_GRAPH_SIMPLE  : Creates a simple density graph, every square has a value initally 0,
//                    every star(and the player itself) increases the value of nearby squares, moves towards the most valued square
// AI_GRAPH_LINEAR  : Creates a density graph, every square has a value linearly proportional to how many stars are nearby 
//                    and its distance with the player, moves towards the highest value square
// AI_GRAPH_EXP     : Creates a density graph, every square has a value exponentially proportional to how many stars are nearby 
//                    and its distance with the player, moves towards the highest value square
// AI_GRAPH_SUPPORT : Creates a support graph, every star has a value that is inversely proportional to its distance between
//                    the player and other stars, moves towards the highest value star
// AI_GRAPH_SUPPORT_AGGR : Similar to the support graph, except that a star's value is also inversely proportional to its distance
//                         between the opponent player, moves towards the highest value star
// AI_GRAPH_SUPPORT_DEF  : Similar to the support graph, except that a star's value is also directly proportional to its distance
//                         between the opponent player, moves towards the highest value star
// AI_GRAPH_SUPPORT2: Similar to the support graph, except it calculates distance using manhattan distance
// AI_GRAPH_SUPPORT3: Similar to the support graph, except it calculates distance more accurately(non-euclidean)

///AI Rankings(from strongest to weakest):
// 1 - AI_GRAPH_SIMPLE
// 2 - AI_GRAPH_SUPPORT
// 3 - AI_SIMPLE
// 4 - AI_SIMPLE2
// 4 - AI_GRAPH_SUPPORT_AGGR
// 5 - AI_GRAPH_SUPPORT_DEF
// 6 - AI_GRAPH_EXP
// 7 - AI_GRAPH_LINEAR
// 8 - AI_RANDOM

const int MAP_WIDTH = 19;
const int MAP_HEIGHT = 18;
const int MAP_STARCOUNT = 20;
const int STRAIGHTTIME = 125;
const int DIAGTIME = 177;
enum{MAP_EMPTY,MAP_STAR,MAP_PLAYER1,MAP_PLAYER2};
enum{DIR_UP,DIR_DOWN,DIR_LEFT,DIR_RIGHT,DIR_UPRIGHT,DIR_UPLEFT,DIR_DOWNRIGHT,DIR_DOWNLEFT,DIR_UNDECIDED};
enum{AI_RANDOM,AI_SIMPLE,AI_SIMPLE2,AI_SIMPLE3,
	AI_GRAPH_SIMPLE,AI_GRAPH_LINEAR,AI_GRAPH_EXP,
	AI_GRAPH_SUPPORT,AI_GRAPH_SUPPORT_DEF,AI_GRAPH_SUPPORT_AGGR,
    AI_GRAPH_SUPPORT2,AI_GRAPH_SUPPORT2_DEF,AI_GRAPH_SUPPORT2_AGGR,
	AI_GRAPH_SUPPORT3,AI_GRAPH_SUPPORT3_DEF,AI_GRAPH_SUPPORT3_AGGR,
	AI_ASVIN,AI_SAYAN};
const int PLAYER1AI = AI_GRAPH_SUPPORT3;
const int PLAYER2AI = AI_GRAPH_SUPPORT;

#endif