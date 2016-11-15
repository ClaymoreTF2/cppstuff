/* User account manager + local database. Forever evolving project.
 * By Claymore @ https://steamcommunity.com/id/theclaymore */
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

//Window Dimensions and FPS
const unsigned WIDTH 	= 640;
const unsigned HEIGHT 	= 480;
const unsigned FPS      = 30;

// Assigned automatically per user.
static unsigned userID;

// pRNG declaration.
std::mt19937 rnd(time(0));

// Memory block size in bytes
const static unsigned char Block = 128;

// Clear CMD macros for Windows and UNIX(GNU/Linux, etc)
// TODO: Make it work
#ifdef __unix__
    #define CLEAR "\033[2J\033[1;1H"
#elif defined _WIN64
    #include <cstdlib>
    #define CLEAR system("cls")
#endif

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
    bool runnning = true;
    void ShowGUI();	// Show the user interface.
    void Quit();	// Destroy SDL and quit the program.
    SDL_Window* 	window = NULL;
    SDL_Renderer*	renderer = NULL;
    SDL_Texture*	texture = NULL;
    SDL_Surface* 	surface = NULL;
    bool init()
    {
        TTF_Font *font;
        font = TTF_OpenFont("Test.ttf",30);
        TTF_Init();
        SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
        SDL_EventState(SDL_KEYUP, SDL_IGNORE);
        if(SDL_Init(SDL_INIT_VIDEO)<0)
            return false;
        window = SDL_CreateWindow("test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                  WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE);
        renderer = SDL_CreateRenderer(window, -1, 0);
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
        screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_SWSURFACE);
        SDL_Color color = {0,0,0};
        SDL_Event event;
        return true;
    }
    int showMenu(SDL_Surface* screen, TTF_Font* font) 
    {
        Uint32 time; // we will store ticks in this.
        int x, y;   // Basic position pointers on a 2D plane
        const int NUMMENU = 6; // Number of menus
        const char* labels[NUMMENU] = {"Master Login", "Master Password", "Save Data", "Load Data", "Edit Data", "Exit"};
        SDL_Surface* menus[NUMMENU];
        bool selected[NUMMENU] = {0, 0, 0, 0, 0, 0};
        SDL_Color color[6] = {{52, 25, 175}, {25, 80, 200}, {0, 100, 255},
                              {25, 52, 150}, {100, 25, 255}, {255, 150, 0}};
        // Menus = Labels with set color and fonts
        menus[0] = TTF_RenderText_Solid(font, labels[0], color[0] );
        menus[1] = TTF_RenderText_Solid(font, labels[1], color[1] );
        menus[2] = TTF_RenderText_Solid(font, labels[2], color[2] );
        menus[3] = TTF_RenderText_Solid(font, labels[3], color[3] );
        menus[4] = TTF_RenderText_Solid(font, labels[4], color[4] );
        menus[5] = TTF_RenderText_Solid(font, labels[5], color[5] );
        SDL_Rect pos[NUMMENU];
        /* This long data structure is going to setup basic "boxes" for our inputs
         * I'm aware that there are more reliable and less prescribed methods to solve this
         * but this works "well" */
        pos[0].x = screen->clip_rect.w/2 - menus[0]->clip_rect.w/2;
        pos[0].y = screen->clip_rect.h/2 - menus[0]->clip_rect.h;
        pos[1].x = screen->clip_rect.w/2 - menus[1]->clip_rect.w/2;
        pos[1].y = screen->clip_rect.h/2 + menus[1]->clip_rect.h;
        pos[2].x = screen->clip_rect.w/2 - menus[2]->clip_rect.w/2;
        pos[2].y = screen->clip_rect.h/2 - menus[2]->clip_rect.h;
        pos[3].x = screen->clip_rect.w/2 - menus[3]->clip_rect.w/2;
        pos[3].y = screen->clip_rect.h/2 + menus[3]->clip_rect.h;
        pos[4].x = screen->clip_rect.w/2 - menus[4]->clip_rect.w/2;
        pos[4].y = screen->clip_rect.h/2 - menus[4]->clip_rect.h;
        pos[5].x = screen->clip_rect.w/2 - menus[5]->clip_rect.w/2;
        pos[5].y = screen->clip_rect.h/2 + menus[5]->clip_rect.h;
        SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0x00, 0x00, 0x00));
        SDL_Event event;
        if(running) {
            while(true) {
                time = SDL_GetTicks();
                while(SDL_PollEvent(&event)) {
                    switch(event.type) {
                        case SDL_Quit:
                            running = false;
                            SDL_FreeSurface(menus[0]);
                            SDL_FreeSurface(menus[1]);
                            SDL_FreeSurface(menus[2]);
                            SDL_FreeSurface(menus[3]);
                            SDL_FreeSurface(menus[4]);
                            SDL_FreeSurface(menus[5]);
                            return 1;
                        case SDL_MOUSE_MOTION:
                            x = event.motion.x;
                            y = event.motion.y;
                            for(int i = 0; i<NUMMENU; i++) {
                                if(x>=pos[i].x && x<=pos[i].x+pos[i].w && y>=pos[i].y && y<=pos[i].y+pos[i].h) {
                                    if(!selected[i]) {
                                        selected[i] = 1;
                                        SDL_FreeSurface(menus[i]);
                                        menus[i] = TTF_RenderText_Solid(font,labels[i],color[1]);
                                    }
                                } else {
                                    if(selected[i]) {
                                        selected[i] = 0;
                                        SDL_FreeSurface(menus[i]);
                                        menus[i] = TTF_RenderText_Solid(font,labels[i],color[0]);
                                    }
                                }
                            }
                            break;
                        case SDL_MOUSEBUTTONDOWN:
                            x = event.button.x;
                            y = event.button.y;
                            for(int i = 0; i < NUMMENU; i += 1) {
                                if(x>=pos[i].x && x<=pos[i].x+pos[i].w && y>=pos[i].y && y<=pos[i].y+pos[i].h) {
                                        SDL_FreeSurface(menus[0]);
                                        SDL_FreeSurface(menus[1]);
                                        SDL_FreeSurface(menus[2]);
                                        SDL_FreeSurface(menus[3]);
                                        SDL_FreeSurface(menus[4]);
                                        SDL_FreeSurface(menus[5]);
                                        return i;
                                }
                            }
                            break;
                        case SDL_KEYDOWN: 
                            if(event.key.keysym.sym == SDLK_ESCAPE) {
                                running = false;
                                SDL_FreeSurface(menus[0]);
                                SDL_FreeSurface(menus[1]);
                                SDL_FreeSurface(menus[2]);
                                SDL_FreeSurface(menus[3]);
                                SDL_FreeSurface(menus[4]);
                                SDL_FreeSurface(menus[5]);
                                return 0;
                            }
                    }
                }
            }
        }
        for(int i = 0; i<NUMMENU; i++) {
            SDL_BlitSurface(menus[i])
        }
        SDL_Flip(screen);
        if(1000/30 > (SDL_GetTicks() - time)) {
            SDL_Delay(1000/30 - (SDL_GetTicks() - time));
        }
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
