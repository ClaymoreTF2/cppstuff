/* User account manager + local database. Forever evolving project.
 * By Claymore @ https://steamcommunity.com/id/theclaymore
 * First modification: 	12-09-2016.
 * Second modification: 14-09-2016
 * Third modification: 	15-09-2016
 * Fourth modification: 20-09-2016 */
// SDL Includes for GUI:
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
// Standard C++ Includes:
#include <fstream>	// for file streams: output and input
#include <cstdio>	// for printf()
#include <curses.h>	// for getch()
#include <vector>	// for std::vector
#include <random>	// for mt19937
#include <functional>	// for std::bind
#include <iostream>	// for std::cin

// The max ammount of users the database can handle:
// Of course this number will expand as my program gets more people.
const unsigned MAX_USERS = 65536;
const unsigned HASH_SIZE = 24;

//Window Dimensions
const unsigned WIDTH 	= 640;
const unsigned HEIGHT 	= 480;

// Assigned automatically per user.
static unsigned userID;

// pRNG declaration.
std::mt19937 rnd(time(0));

// Memory block size in bytes
const static unsigned char Block = 128;

// These ANSI escape codes clear the whole screen and position the cursor at
// row 1, column 1.
#define CLEAR "\033[2J\033[1;1H"


namespace Hash
{
    unsigned SaltResult;
    unsigned HashTable;
    unsigned salt(unsigned ID, const std::string& pswrd)
    {
         unsigned sum = 0;
         for(unsigned i = ID; i<(pswrd[i]+ID); ++i)
         {
             sum+=static_cast<unsigned>(pswrd[i]);
             SaltResult = (sum+rnd())%pswrd.size();
         }
         return SaltResult;
    }
    unsigned hash(unsigned ID)
    {
        for(int pos = ID; pos<HASH_SIZE+ID; ++pos)
            HashTable += SaltResult%ID;
        return HashTable;
    }
}

struct User
{
    std::string login;		// User decides it (can contain any non-unicode symbol).
    std::string password;	// Same as above.
    // Unique per user.
    unsigned long uSalt = Hash::salt(userID, password);
    // Salting the hash will make it undecryptable(almost).
    unsigned uHash = Hash::hash(userID);
};

// We need two file variables. Output and input.
std::fstream output, input;

void Selection(); 	// Three options: Save, Load, Edit.
void SaveData();	// Saving user data.
void EditData();	// Editing user data.
void LoadData();	// Loading user data.

namespace GUI
{
    bool init();    // Initialize SDL.
    void ShowGUI();	// Show the user interface.
    void Quit();	// Destroy SDL and quit the program.
    SDL_Window* 	window = NULL;
    SDL_Renderer*	renderer = NULL;
    SDL_Texture*	texture = NULL;
    SDL_Surface* 	surface = NULL;
    bool init()
    {
        SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
        SDL_EventState(SDL_KEYUP, SDL_IGNORE);
        if(SDL_Init(SDL_INIT_VIDEO)<0)
            return false;
        window = SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                  WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE);
        renderer = SDL_CreateRenderer(window, -1, 0);
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
        return true;
    }
    int showMenu(SDL_Surface* screen, TTF_Font* font) 
    {
        int x, y;
        const int NUMMENU = 6;
        const char* labels[NUMMENU] = {"Master Login", "Master Password", "Save Data", "Load Data", "Edit Data", "Exit"};
        SDL_Surface* menus[NUMMENU];
        bool selected[NUMMENU] = {0, 0, 0, 0, 0, 0};
        SDL_Color color[6] = {{52, 25, 175}, {25, 80, 200}, {0, 100, 255},
                              {25, 52, 150}, {100, 25, 255}, {255, 150, 0}};
    }
}

void LoadData()
{

}

void SaveData()
{
    output.open("database.txt", std::ios::out | std::ios::binary);
    /* char* login, password;
    std::cin>>login>>password;
    User uout;
    uout.login      = login;
    uout.password   = password;
    UserID++;
    output<<UserID<<" | "<<uout.login<<" | "<<uout.password<<'\n'; */
}

void Selection()
{
    unsigned char s;
    Start:
        GUI::init();
        GUI::showMenu();
        std::cin>>s;
        switch(s)
        {
            case '1':
                SaveData();
                break;
            case '2':
                //EditData();
                break;
            case '3':
                //LoadData();
                break;
            case 'q':
            case 'Q':
                return;
            default:
            {
                printf("Unrecognized char %c.\nPlease use a valid selection character\n", s);
                getch();
                printf(CLEAR);
                goto Start;
            }
        }
}

/* SAVED DATA FORMAT :
 * AAAA H*24 S*24 U*24
 * where every character is a byte
 * A = UserID, H = Hash, S = Salt, U = Username	*/

int main()
{
    Selection();
    return 0;
}
