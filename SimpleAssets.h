#ifndef SIMPLE_ASSETS_H
#define SIMPLE_ASSETS_H
//=================================================================================================================================//
// needed header files
#include <iostream>
#include <algorithm>
#include <conio.h>
#include <fstream>
#include <random>
#include <string>
#include <sys/stat.h>
#include <vector>
#if defined(__linux__)||defined(__linux)||defined(linux)
#include <unistd.h>
#define Sleep(milliseconds) (usleep(milliseconds*1000))
#define createDirectory(n) (mkdir(n,'-p'))
#else
#include <windows.h> /* prototype for Sleep() */
#define createDirectory(n) (n)
#endif
//==================================================================================================================================//

// prototypes
void userOption(const int& = 0);
void runGame();
void startNewGame();

// namespace to contain all the tools the game needs
namespace SimpleAssets
{
	// stores available text color
	enum textColor { 
	    red = 31,green = 32,yellow = 33,blue = 34,pink = 35,cyan = 36, normal = 39,darkgray = 90,white = 97
	};
	
	// stores available background color
	enum bcgColor { 
	    Black = 40, Red = 41,Green = 42,Yellow = 43,Blue = 44,Pink = 45,Cyan = 46,Normal = 49,DarkGray = 100,White = 107
	};
	
	// store available interfaces
    enum class Page {
        Undefined,Menu,NewGame,Difficulty,Score,Instructions,Settings
    };
	
	// sets cursor position
	inline std::string cursor(const int& row = 35, const int& column = 1) {
		return ("\033["+std::to_string(row)+";"+std::to_string(column)+"H");
	}
	
	// sets text and background color
	inline std::string color(const textColor& c = normal,const bcgColor& b = Normal) {
		return ("\033[1;"+std::to_string(c)+";"+std::to_string(b)+"m");
	}
	
	// positions an item in a way that it moves inwards when the length increases
	inline std::string space(unsigned var,unsigned n) {
		return std::string(n-std::to_string(var).length(),' ');
	}
	
	// allign a string approximately at the center
	inline std::string center(std::string n,int row,int colBegin,int colEnd,textColor clr) {
		return (cursor(row,(colEnd-(colEnd-colBegin)/2)-(n.length()-color(clr).length())/2)+n);
	}
	
	// class that handles how the gamedata is processed
	class Data
	{
		private:
		    // for manipulating the game's data in file
		    std::fstream gameData;
		    std::string data[5];
		    
		    // where to store the game data file on the device
		    std::string path = "/storage/emulated/0/Android/data/";
		    char *directory = "/storage/emulated/0/Android/data/com.nunugames.tg/";
		    std::string folder = directory;
		    
		    // user data to store
		    unsigned highestLines = 0, linesCleared = 0;
	        unsigned totalLinesCleared = 0,highestScore = 0,gamesPlayed = 0;
	        unsigned score = 0;
	        bool hasData = false;
	        
	    public:
	        Data() {} /* constructor */
	        
	        // setter methods
	        inline void setHighestLines(const unsigned& n) { highestLines = n; }
	        inline void setHighestScore(const unsigned& n) { highestScore = n; }
	        inline void incrementLinesCleared() { linesCleared++; }
	        inline void incrementTotalLinesCleared() { totalLinesCleared += linesCleared; }
	        inline void incrementGamesPlayed() { gamesPlayed++; }
	        inline void incrementScore() { score += (3 * this->linesCleared); }
	        inline void setDefaultGameScores() { linesCleared = score = 0; }
	        
	        // getter methods
	        inline unsigned getHighestLines() { return this->highestLines; }
	        inline unsigned getLinesCleared() { return this->linesCleared; }
	        inline unsigned getTotalLinesCleared() { return this->totalLinesCleared; }
	        inline unsigned getGamesPlayed() { return this->gamesPlayed; }
	        inline unsigned getHighestScore() { return this->highestScore; }
	        inline unsigned getScore() { return this->score; }
	        inline bool getHasData() { return this->hasData; }
	        
	        // stores game data in a file
	        void createGameData() {
	        	// open the file to save the scores
	        	gameData.open(folder+"tetris.dat",std::ios::out);
	            // write the scores to the file
	            gameData << "[Tetris scores]\n";
	            gameData << "Highest lines in one game :   data[ " << highestLines << " ]\n";
	            gameData << "Total lines cleared :"+std::string(9,' ') << "data[ " << totalLinesCleared << " ]\n";
	            gameData << "Games played :"+std::string(16,' ') << "data[ " << gamesPlayed << " ]\n";
	            gameData << "Highest score :"+std::string(15,' ') << "data[ " << highestScore << " ]";
	            
	            // close the file
	            gameData.close();
	            // reopen the file for reading
	            gameData.open(folder+"tetris.dat",std::ios::in);
	            // store the details written in the file to data
	            for (auto& val : data) {
	            	std::getline(gameData,val);
	            }
	            gameData.close();
	        }
	        
	        // retrieves the current game data from the file it is stored
	        void retrieveGameData() {
	        	// check if the path exist on the device
	        	gameData.open(path+".test",std::ios::out);
	        	gameData.close();
	        	if (!gameData.fail()) {
	        		// if the path exists then open the game file to retrieve the scores
	                gameData.open(folder+"tetris.dat",std::ios::in);
	            	if (gameData.fail()) {
	            		// if the game file doesn't already exist then it means the folder
	            		// doesn't exist, so create the folder then create a game data file
	        	        createDirectory(directory); createGameData();
	            	}
	            // if the path doesn't exist on the device then set file storage to default
	            // and create a new game data file
	            } else {
	        		folder = ""; createGameData();
	        	}
	        	// retrieve the saved scores and store in data
	        	for (auto& val : data) {
	        	    std::getline(gameData,val);
	            }
	            
	            // close the file if it is still open
	            if (gameData.is_open()) gameData.close();
	            
	        	// the exception may occur if user messes with the file to negate how data
	        	// is passed to the variables below in the try statement
	        	try {
	        		// pass the scores from data to their respective holders
	            	highestLines = std::stoi(data[1].substr(data[1].find("data")+6)); totalLinesCleared = std::stoi(data[2].substr(data[2].find("data")+6));
	            	gamesPlayed = std::stoi(data[3].substr(data[3].find("data")+6)); highestScore = std::stoi(data[4].substr(data[4].find("data")+6));
	            // if the scores can't be gotten due to the above reason then keep overwriting
	            // user data with default data when the game runs
	        	} catch (const std::invalid_argument& ex) {
	        	    // creating data before retrieving successfully resets game data to default values
	        	    createGameData();
	        	}
	        	hasData = true;
	        }
	        
	        // sets game data to default values
	        void deleteGameData() {
	        	// set the scores to default values
	        	highestLines = 0; totalLinesCleared = 0; highestScore = 0; gamesPlayed = 0;
	        	// create a new game data to save these scores
	        	createGameData();
	        }
	        
	};
	
	// create an object to hold and manipulate game data
	Data *tetrisData = new Data();
	
	// construct the selector for option selection operations
    class Selector
    {   
        private:
            unsigned short width,row,col;
            unsigned short upperLimit, lowerLimit;
            unsigned short count = 0;
        
            // stores the option last highlighted and its column
            std::string tempOption = ""; int tempCol = 0;
        
        public:
            // constructor for Selector
            Selector(unsigned short w,unsigned short r,unsigned short c,unsigned short u,unsigned short l):
                width(w),row(r),col(c),upperLimit(u),lowerLimit(l) {
            }
            
            // getters only for members used outside this class
            inline int getRow()        const { return this->row; }
            inline int getLowerLimit() const { return this->lowerLimit; }
            inline int getCount()      const { return this->count; }
    	
            // updates the position of the selector
            void indicateOption(const std::string& option = "",const int& option_col = 0) {
                // display the selector
                std::cout << cursor(row,col) << color(green) << ">[";
                std::cout << cursor(row,col+2) << color(white,Red) << std::string(width,' ');
                std::cout << cursor(row,col+width) << color(green) << "]<";
                    
                // highlight the option
                std::cout << cursor(row,option_col) << color(white,Red) << option << std::flush;
                tempOption = option; tempCol = option_col;
            }
        
            // erases the former position of the selector
        	void erase() {
    		    // erase the previous position of the selector
                std::cout << cursor(row,col) << color(normal) << std::string(width+2,' ');
                std::cout << cursor(row,tempCol) << color(cyan) << tempOption;
            }
        	
            // moves the selector to another row
            void scroll(const char& command) {
                // check if limit hasn't been reached before scrolling
                if (row < lowerLimit && command == '8') {
                	// move selector down to the next option
                    row += 3; count++;
                } else if (row > upperLimit && command == '2') {
                	// move selector up to the next option
                    row -= 3; count--;
                }
            }
        
            // takes the selector back to the top on reload
            void reset() { row = upperLimit; count = 0; }	
    };
	
	// for common screen operations
	class Screen 
	{
		// not accessible outside this scope
		private:
		    int startY = 3, endY = 36;
    	    int startX = 7, endX = 62;
    	    
    	    // distance between the borders
    	    int dy = this->endY - this->startY;
    	    int dx = this->endX - this->startX;
    	    
    	    // stores user input
    	    char command = '2';
    	    
    	    // default screen page
        	Page page = Page::Undefined;
        	
        	// index of the current page
            short pageIndex = 0;
        	
    	    // selectors-----------> menu selector ---------> Difficulty selector --> Settings selector <--
    	    Selector selectors[3] = {Selector(21,11,38,11,23),Selector(11,11,9,11,26),Selector(0,0,0,0,0)};
    	     // this takes one of the three selectors as value
            Selector *selector;
            
    	    // all selectable options available in each selection page
    	    std::string options[3][6] = {{"START GAME","DIFFICULTY LEVEL","HIGH SCORE","INSTRUCTIONS","SETTINGS"},{"Level 1","Level 2","Level 3","Level 4","Level 5","Level 6"},{""}};                   
    	    // columns where each option is placed with respect to options above
    	    int options_cols[3][6] = {{47,41,47,45,49},{12,12,12,12,12,12},{0}};
    	    
    	    // where the cursor is placed in a certain screen page and the color
    	    int cursorDefaultRow = 35,cursorDefaultCol = 1;
    	    textColor cursorDefaultColor = normal;
    	    
    	// accessible from anywhere    
    	public:
    	    Screen() {} /* constructor */
    	    
    	    // sets the Selector object to be used
    	    inline void setSelector(const int& index) { selector = &selectors[(pageIndex = index)]; if (page == Page::Difficulty || page == Page::Settings) selector->reset(); }
    	    // sets default details for the cursor
            inline void setCursorDefaults(const int& cRow,const int& cCol,const textColor& cColor) {
            	cursorDefaultRow = cRow; cursorDefaultCol = cCol; cursorDefaultColor = cColor;
            }
            // sets the current page of the screen object
            inline void setPage(Page page) { this->page = page; }
            
    	    // get user input commands
            inline void setCommand() {
    	        std::cout << cursor() << std::flush;
    	
            	// wait for input
    	        this->command = getch();
            }
    	    
    	    // returns the input command the user enters
    	    inline char getCommand() {
    	    	return this->command;
    	    }
    	    
    	    // returns the current screen page
    	    inline Page getPage() { return this->page; }
    	    
    	    // returns the current selector(functions below are const so that the private members returned retain their immunity to modifications external to the class)
    	    inline const Selector *getSelector() { return selector; }
    	    // returns the options for a particular selection page
    	    inline const std::string *getOptions(const int& index) { return options[index]; }
    	    inline const int *getOptionsCols(const int& index) { return options_cols[index]; }
    	    
    	    // erases the screen but not the borders
    	    void clear() {
    	    	for (int y = this->dy; y >= this->startY; y--) {
    	    		std::cout << cursor(y,this->startX) << color() << std::string(this->dx,' ') << cursor() << std::flush;
    	    	}
    	    }
    	    
    	    // display to screen
    	    void display(const std::string& n = "",const int& r = 34,const int& c = 4,const textColor& clr = blue,const bcgColor& bcg = Normal) {
    	    	// set position to print to and print to the screen
    	    	std::cout << cursor(r,c) << color(clr,bcg) << n << cursor(cursorDefaultRow,cursorDefaultCol) << color(cursorDefaultColor);
    	    }
            
            // creates a box container
            void createContainer(int width,int height,int start_row,int start_col,textColor clr,const char& tborder = '_',const char& bborder = '"',const char& sborder = '|') {
            	// build top border
            	display(std::string(width-2,tborder),start_row,start_col+1,clr);
            	
            	// build side borders
            	for (int i = 1; i <= height; i++) {
			        display(std::string(1,sborder),++start_row,start_col,clr);
			        display(std::string(1,sborder),start_row,(start_col+width-1),clr);
            	}
            	// build bottom border
            	display(std::string(width-2,bborder),start_row+1,start_col+1,clr);
            }
        	
            void initialize_selection() { /* this highlights and processes a selectable option */
            	// highlight the option
            	if (page == Page::Menu || page == Page::Difficulty) {
            	    selector->indicateOption(options[pageIndex][selector->getCount()],options_cols[pageIndex][selector->getCount()]);
            	}
                // wait for user input
                setCommand();
               
                // only checks if the user wants to go back
                if (page != Page::Menu) userOption();
                
            	if (page == Page::Menu || page == Page::Difficulty) {
            		if (command == '8' || command == '2') {
            		    // erase the previous position of the selector
            		    selector->erase();
            			
            			// change the selector's position
                        selector->scroll(command);
            		
            		// selected an option
            	    } else if (command == '5') {
            	    	// process the option the user selected
            	    	userOption(selector->getRow());
            	    }
            	}
            }
    }; /* end of class Screen */
	
	// create a Screen object
	Screen screen = Screen();
	
	// time to wait before moving a tetromino
	unsigned short delay = 1000; /* milliseconds */
	
	// available game levels
	enum class Level {
		level1 = 11,level2 = 14,level3 = 17,level4 = 20,level5 = 23,level6 = 26
	};
	
	// stores the game's current level with 0 as undefined
	int GameLevelNumber = 0;
	
	// stores the game's current level
	Level level = Level::level2;
	
	Level set_level() {
		// set the level based on the selector's row position
		switch (screen.getSelector()->getRow()) {
			case 11: return Level::level1; break;
			case 14: return Level::level2; break;
			case 17: return Level::level3; break;
			case 20: return Level::level4; break;
			case 23: return Level::level5; break;
			case 26: return Level::level6; break;
		}
	}
	
	// has a level been defined?
	bool levelSet = true;
	
	int setDifficulty() { /* sets the game's difficulty */
		if (!levelSet) level = set_level();
		// set game difficulty based on level
	    if (level == Level::level1) {
	    	delay = 1500; GameLevelNumber = 1;
	    } else if (level == Level::level2) {
	    	delay = 1000; GameLevelNumber = 2;
	    } else if (level == Level::level3) {
	    	delay = 800;  GameLevelNumber = 3;
	    } else if (level == Level::level4) {
	    	delay = 500;  GameLevelNumber = 4;
	    } else if (level == Level::level5) {
	    	delay = 300;  GameLevelNumber = 5;
	    } else if (level == Level::level6) {
	    	delay = 100;  GameLevelNumber = 6;
	    }
	    
	    // don't update the indicators if not at difficulty screen
	    if (screen.getPage() != Page::Difficulty) return 0;
	    
	    // where to place the level indicator
	    unsigned short level_row = static_cast<unsigned short>(level);
	    // the row where the options begin from
	    unsigned short position = 11;
	    
	    // unmark the rest options
	    do {
	    	screen.display("o",position,55,white);
	    	position += 3;
	    	
	    // while position hasn't reached the lower limit
	    } while (position <= screen.getSelector()->getLowerLimit());
	    // mark the level option selected
	    screen.display("√",level_row,55,pink);
	    return 0;
	}
	
} /* end of namespace SimpleAssets */
//==============================================================================================================================================================//
#endif