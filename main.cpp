#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include <string>
#include <time.h>
#include <sstream>
#include "constants.h"
#include "AI.h"

using namespace std;

//Screen attributes
const int SCREEN_WIDTH = 32*MAP_WIDTH;
const int SCREEN_HEIGHT = 32*MAP_HEIGHT + 100;
const int SCREEN_BPP = 32;

int MAP[MAP_WIDTH][MAP_HEIGHT];

//The surfaces
SDL_Surface *background = NULL;
SDL_Surface *star = NULL;
SDL_Surface *grass = NULL;
SDL_Surface *player1 = NULL;
SDL_Surface *player2 = NULL;
SDL_Surface *message = NULL;
SDL_Surface *foo = NULL;
SDL_Surface *screen = NULL;

//The event structure
SDL_Event event;

//The font that's going to be used
TTF_Font *font = NULL;
SDL_Surface *upMessage = NULL;
SDL_Surface *downMessage = NULL;
SDL_Surface *rightMessage = NULL;
SDL_Surface *leftMessage = NULL;

//The music that will be played
Mix_Music *music = NULL;

//The sound effects that will be used
Mix_Chunk *scratch = NULL;
Mix_Chunk *high = NULL;
Mix_Chunk *med = NULL;
Mix_Chunk *low = NULL;

//The color of the font
SDL_Color textColor = { 255, 255, 255 };

std::string Int2Str(int n)
{
	return static_cast<std::ostringstream*>( &(std::ostringstream() << n) )->str();
}

SDL_Surface *load_image( std::string filename ) 
{
    //The image that's loaded
    SDL_Surface* loadedImage = NULL;
    
    //The optimized image that will be used
    SDL_Surface* optimizedImage = NULL;
    
    //Load the image
    loadedImage = IMG_Load( filename.c_str() );
    
    //If the image loaded
    if( loadedImage != NULL )
    {
        //Create an optimized image
        optimizedImage = SDL_DisplayFormat( loadedImage );
        
        //Free the old image
        SDL_FreeSurface( loadedImage );
		//If the image was optimized just fine
        if( optimizedImage != NULL )
        {
            //Map the color key
            Uint32 colorkey = SDL_MapRGB( optimizedImage->format, 0, 0xFF, 0xFF );
			//Set all pixels of color R 0, G 0xFF, B 0xFF to be transparent
            SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, colorkey );
        }
	}
	//Return the optimized image
    return optimizedImage;
}


void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL )
{
    //Holds offsets
    SDL_Rect offset;
    
    //Get offsets
    offset.x = x;
    offset.y = y;
    
    //Blit
    SDL_BlitSurface( source, clip, destination, &offset );
}

bool init()
{
    //Initialize all SDL subsystems
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return false;    
    }
    
    //Set up the screen
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );
    
    //If there was an error in setting up the screen
    if( screen == NULL )
    {
        return false;    
    }
    
    //Initialize SDL_ttf
    if( TTF_Init() == -1 )
    {
        return false;    
    }
    
    //Initialize SDL_mixer
    if( Mix_OpenAudio( 22050, MIX_DEFAULT_FORMAT, 2, 4096 ) == -1 )
    {
        return false;    
    }
    
    //Set the window caption
    SDL_WM_SetCaption( "Collector AI", NULL );
    
    //If everything initialized fine
    return true;
}

bool load_files()
{
    //Load the background image
    background = load_image( "background.png" );
	grass = load_image("grass.png");
	player1 = load_image("smile.png");
	player2 = load_image("sad.png");
	star = load_image("star.png");
    
    //Open the font
    font = TTF_OpenFont( "lazy.ttf", 30 );
    
    //If there was a problem in loading the background
    if( background == NULL || grass==NULL || player1==NULL || player2==NULL || star == NULL)
    {
        return false;    
    }
    
    //If there was an error in loading the font
    if( font == NULL )
    {
        return false;
    }
    
    //Load the music
    //music = Mix_LoadMUS( "beat.wav" );
    
    //If there was a problem loading the music
    //if( music == NULL )
    //{
    //    return false;    
    //}

    //Load the sound effects
    //scratch = Mix_LoadWAV( "scratch.wav" );
    //high = Mix_LoadWAV( "high.wav" );
    //med = Mix_LoadWAV( "medium.wav" );
    //low = Mix_LoadWAV( "low.wav" );
    
    //If there was a problem loading the sound effects
    //if( ( scratch == NULL ) || ( high == NULL ) || ( med == NULL ) || ( low == NULL ) )
    //{
    //    return false;    
    //}
    
    //If everything loaded fine
    return true;    
}

void clean_up()
{
    //Free the surfaces
    SDL_FreeSurface( background );
	SDL_FreeSurface( grass );
	SDL_FreeSurface( player2 );
	SDL_FreeSurface( player1 );
	SDL_FreeSurface( star );
    
    //Free the sound effects
    Mix_FreeChunk( scratch );
    Mix_FreeChunk( high );
    Mix_FreeChunk( med );
    Mix_FreeChunk( low );
    
    //Free the music
    Mix_FreeMusic( music );
    
    //Close the font
    TTF_CloseFont( font );
    
    //Quit SDL_mixer
    Mix_CloseAudio();
    
    //Quit SDL_ttf
    TTF_Quit();
    
    //Quit SDL
    SDL_Quit();
}

int initGame()
{
	//Game Data Init
	for(int i = 0;i<MAP_WIDTH;i++)
	{
		for(int j = 0;j<MAP_HEIGHT;j++)
		{
			MAP[i][j] = MAP_EMPTY;
		}
	}
	MAP[MAP_WIDTH/2][MAP_HEIGHT-1] = MAP_PLAYER1;
	MAP[MAP_WIDTH/2][0] = MAP_PLAYER2;
	for(int i = 0;i<MAP_STARCOUNT;i++)
	{
		int x = rand()%MAP_HEIGHT/2;
		int y = rand()%MAP_WIDTH;
		if(!(y==MAP_WIDTH/2 && (x==0 || x==MAP_HEIGHT-1)))
		{
		    MAP[y][x] = MAP_STAR;
		    MAP[y][MAP_HEIGHT-1-x] = MAP_STAR;
		}
		else
		{
			i--;
		}
	}
	return 0;
}

int main( int argc, char* args[] )
{
    //Quit flag
    bool quit = false;

    //Initialize
    if( init() == false )
    {
        return 1;
    }

    //Load the files
    if( load_files() == false )
    {
        return 2;
    }

    //Apply the background
    apply_surface( 0, 0, background, screen );

    //Update the screen
    if( SDL_Flip( screen ) == -1 )
    {
        return 6;
    }
	srand(time(0));
	initGame();

	int time = 0;
	int pausetime = 0;
	int player1time = 0;
	int player2time = 0;
	int player1ready = 1;
	int player2ready = 1;
	int player1dir = DIR_UNDECIDED;
	int player2dir = DIR_UNDECIDED;
	int player1stars = 0;
	int player2stars = 0;
	int player1score = 0;
	int player2score = 0;
	AI Player1(MAP_PLAYER1,PLAYER1AI);
	AI Player2(MAP_PLAYER2,PLAYER2AI);
	//While the user hasn't quit
    while( quit == false )
    {
		//make players think
		if(player1ready==1)
		{
			int x1=0,y1=0,x2=0,y2=0;
			for(int i = 0;i<MAP_WIDTH;i++)
			{
				for(int j = 0;j<MAP_HEIGHT;j++)
				{
					if(MAP[i][j] == MAP_PLAYER1)
					{
						x1=i;
						y1=j;
					}
				}
			}
			x2=x1;
			y2=y1;
			player1dir = Player1.think(MAP);
			if(player1dir==DIR_UP)
			{
				y2=y1-1;
				player1time = STRAIGHTTIME;
			}
			if(player1dir==DIR_DOWN)
			{
				y2=y1+1;
				player1time = STRAIGHTTIME;
			}
			if(player1dir==DIR_LEFT)
			{
				x2=x1-1;
				player1time = STRAIGHTTIME;
			}
			if(player1dir==DIR_RIGHT)
			{
				x2=x1+1;
				player1time = STRAIGHTTIME;
			}
			if(player1dir==DIR_DOWNRIGHT)
			{
				x2=x1+1;
				y2=y1+1;
				player1time = DIAGTIME;
			}
			if(player1dir==DIR_UPRIGHT)
			{
				x2=x1+1;
				y2=y1-1;
				player1time = DIAGTIME;
			}
			if(player1dir==DIR_UPLEFT)
			{
				x2=x1-1;
				y2=y1-1;
				player1time = DIAGTIME;
			}
			if(player1dir==DIR_DOWNLEFT)
			{
				x2=x1-1;
				y2=y1+1;
				player1time = DIAGTIME;
			}
			if(x2>=0 && y2>=0 && x2<MAP_WIDTH && y2<MAP_HEIGHT)
			{
				if(MAP[x2][y2]!=MAP_PLAYER2)
				{
					if(MAP[x2][y2]==MAP_STAR)
					{
						player1stars++;
					}
					MAP[x2][y2]=MAP_PLAYER1;
					MAP[x1][y1]=MAP_EMPTY;
				}
			}
			player1ready = 0;
		}
		if(player2ready==1)
		{
		    int x1=0,y1=0,x2=0,y2=0;
			for(int i = 0;i<MAP_WIDTH;i++)
			{
				for(int j = 0;j<MAP_HEIGHT;j++)
				{
					if(MAP[i][j] == MAP_PLAYER2)
					{
						x1=i;
						y1=j;
					}
				}
			}
			x2=x1;
			y2=y1;
			player2dir = Player2.think(MAP);
			if(player2dir==DIR_UP)
			{
				y2=y1-1;
				player2time = STRAIGHTTIME;
			}
			if(player2dir==DIR_DOWN)
			{
				y2=y1+1;
				player2time = STRAIGHTTIME;
			}
			if(player2dir==DIR_LEFT)
			{
				x2=x1-1;
				player2time = STRAIGHTTIME;
			}
			if(player2dir==DIR_RIGHT)
			{
				x2=x1+1;
				player2time = STRAIGHTTIME;
			}
			if(player2dir==DIR_DOWNRIGHT)
			{
				x2=x1+1;
				y2=y1+1;
				player2time = DIAGTIME;
			}
			if(player2dir==DIR_UPRIGHT)
			{
				x2=x1+1;
				y2=y1-1;
				player2time = DIAGTIME;
			}
			if(player2dir==DIR_UPLEFT)
			{
				x2=x1-1;
				y2=y1-1;
				player2time = DIAGTIME;
			}
			if(player2dir==DIR_DOWNLEFT)
			{
				x2=x1-1;
				y2=y1+1;
				player2time = DIAGTIME;
			}
			if(x2>=0 && y2>=0 && x2<MAP_WIDTH && y2<MAP_HEIGHT)
			{
				if(MAP[x2][y2]!=MAP_PLAYER1)
				{
					if(MAP[x2][y2]==MAP_STAR)
					{
						player2stars++;
					}
					MAP[x1][y1]=MAP_EMPTY;
					MAP[x2][y2]=MAP_PLAYER2;		
				}
			}
			player2ready = 0;
		}

		//update screen
		//Apply the background
        apply_surface( 0, 0, background, screen );
		for(int i = 0;i<MAP_WIDTH;i++)
		{
			for(int j = 0;j<MAP_HEIGHT;j++)
			{
				apply_surface( 32*i, 32*j, grass, screen );
				if(MAP[i][j]==MAP_STAR)
				{
					apply_surface( 32*i, 32*j, star, screen );
				}
				if(MAP[i][j]==MAP_PLAYER1)
				{
					apply_surface( 32*i, 32*j, player1, screen );
				}
				if(MAP[i][j]==MAP_PLAYER2)
				{
					apply_surface( 32*i, 32*j, player2, screen );
				}
			}
		}
		//Score
		string s = "Yellow Score: "+Int2Str(player1score)+"  "+"Red Score: "+Int2Str(player2score);
		message = TTF_RenderText_Solid( font, s.c_str(), textColor );
		if( message == NULL )
		{
			return 3;
		}
		apply_surface( 0, 32*MAP_HEIGHT, message, screen );
		SDL_FreeSurface( message );

		//Stars
		s = "Yellow Stars: "+Int2Str(player1stars)+"  "+"Red Stars: "+Int2Str(player2stars);
		message = TTF_RenderText_Solid( font, s.c_str(), textColor );
		if( message == NULL )
		{
			return 3;
		}
		apply_surface( 0, 32*MAP_HEIGHT+30, message, screen );
		SDL_FreeSurface( message );

		//Time
		s = "Time: "+Int2Str(time);
		message = TTF_RenderText_Solid( font, s.c_str(), textColor );
		if( message == NULL )
		{
			return 3;
		}
		apply_surface( 0, 32*MAP_HEIGHT+60, message, screen );
		SDL_FreeSurface( message );

		if( SDL_Flip( screen ) == -1 )
        {
            return 7;
        }

		//check if game is over
		int c = 0;
		for(int i = 0;i<MAP_WIDTH;i++)
		{
			for(int j = 0;j<MAP_HEIGHT;j++)
			{
				if(MAP[i][j] == MAP_STAR)
				{
					c++;				
				}
			}
		}
		if(c==0)
		{
			initGame();
			player1time = 0;
			player2time = 0;
			player1ready = 1;
			player2ready = 1;
			player1dir = DIR_UNDECIDED;
			player2dir = DIR_UNDECIDED;
			if(player1stars>player2stars)
				player1score+=2;
		    else if(player2stars>player1stars)
				player2score+=2;
			else
			{
				player1score++;
				player2score++;
			}
			player1stars = 0;
			player2stars = 0;
			Player1.initiate(MAP_PLAYER1,PLAYER1AI);
			Player2.initiate(MAP_PLAYER2,PLAYER2AI);
		}

		pausetime = SDL_GetTicks()-time; //timer starts
		while(quit==false)
		{
			//While there's events to handle
            while( SDL_PollEvent( &event ) )
            {
                //If the user has Xed out the window
                if( event.type == SDL_QUIT )
                {
                    //Quit the program
                    quit = true;
                }
            }
			if(SDL_GetTicks()-pausetime >= time+player1time) //player 1 has finished moving
			{
				player1ready = 1;
			}
			if(SDL_GetTicks()-pausetime >= time+player2time) //player 2 has finished moving
			{
				player2ready = 1;
			}
			if(player1ready==1 && player2ready==0)
			{
				player2time = time+player2time - SDL_GetTicks() + pausetime;
				time = SDL_GetTicks()-pausetime; //timer stops
				break;
			}
			if(player1ready==0 && player2ready==1)
			{
				player1time = time+player1time - SDL_GetTicks() + pausetime;
				time = SDL_GetTicks()-pausetime; //timer stops
				break;
			}
			if(player1ready==1 && player2ready==1)
			{
				time = SDL_GetTicks()-pausetime; //timer stops
				break;
			}
		}
    }

    //Free surfaces, fonts and sounds
    //then quit SDL_mixer, SDL_ttf and SDL
    clean_up();
    
    return 0;    
}