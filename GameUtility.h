#ifndef GAME_UTILITY_H
#define GAME_UTILITY_H
#include "SimpleAssets.h"
//===================================================================================================================================================//

// for convienience...
using namespace SimpleAssets;

// namespace to contain the game's interface
namespace interface
{
	
	void menu() {
		// set the current screen page
		screen.setPage(Page::Menu);
		// set the specific selector and its options for this page
		screen.setSelector(0);
		screen.setCursorDefaults(35,1,normal);
		
		// display the game's name;
		screen.display("oooo [",4,16,green);
		screen.display("T E T R I S   G A M E",4,23,yellow);
		screen.display("] oooo",4,45,green);
        
        // display user selectable contents and other
        for (int opt = 0,row = 11; opt < 5; opt++,row += 3) {
            screen.display(screen.getOptions(0)[opt],row,screen.getOptionsCols(0)[opt],cyan);
        }
        // display tip info
        screen.display("Press 5 to select an option",29,20,darkgray);
        
        // retrieve the game data if it hasn't been retrieved. This happens once per game run
        if (!tetrisData->getHasData()) { tetrisData->retrieveGameData(); }
	}
	
	void newGame() {
		screen.setPage(Page::NewGame);
		// create the matrix
		screen.createContainer(22,20,8,13,blue);
		// create a margin to separate the matrix from the game info
		for (int i = 3; i <= 33; i++) {
			screen.display("|",i,41,blue);
		}
		// interfaces
		screen.createContainer(15,5,4,45,blue);
		screen.display("Game Level: "+color(green)+std::to_string(GameLevelNumber),12,46,yellow);
		screen.display(std::string(20,'_'),13,42,blue);
		screen.display(color(pink)+center("Score: "+color(green)+std::to_string(tetrisData->getScore()),15,42,62,green));
		screen.display(color(pink)+center("Lines: "+color(green)+std::to_string(tetrisData->getLinesCleared()),18,42,62,green));
		screen.display(std::string(20,'_'),19,42,blue);
		screen.display(color(yellow)+center("Key Pressed: "+color(green)+"0",21,42,62,green));
		screen.display(std::string(20,'_'),22,42,blue);
		// set the cursor derails for this page
		screen.setCursorDefaults(21,58,green);
		std::cout << std::flush; startNewGame();
	}
	
	void difficulty() {
		// set the current screen page
		screen.setPage(Page::Difficulty);
		// set the specific selector and its options for this page
		screen.setSelector(1);
		// level will always be set when this interface runs
		levelSet = true;
		// display the current screen page title
		screen.display(" DIFFICULTY ",3,29,white,Red);
		screen.display("Select Level",6,10,green);
		// draw content borders
		screen.createContainer(50,21,7,9,blue,'_','_','\0');
		// display selectable options
		for (int opt = 0,row = 11; opt <= 5; opt++,row += 3) {
		    screen.display(screen.getOptions(1)[opt]+cursor(row,55)+color(white)+"o",row,12,cyan);
		}
		screen.display("Press # to go back",31,25,darkgray);
		setDifficulty();
		
		// the user may want to modify the level so allow modification
		levelSet = false;
	}
	
	void scores() {
		screen.setPage(Page::Score);
		screen.display(" SCORES ",3,31,white,Red);
		// draw borders for content
		screen.createContainer(50,23,5,9,blue,'_','_','\0');
		screen.display("Single Game: ",7,10,white);
	    screen.display("Highest lines cleared:"+space(tetrisData->getHighestLines(),26)+color(green)+std::to_string(tetrisData->getHighestLines()),9,10,pink);
		screen.display("Lifetime: ",13,10,white);
		screen.display("Games Played:"+space(tetrisData->getGamesPlayed(),35)+color(green)+std::to_string(tetrisData->getGamesPlayed()),15,10,pink);
		screen.display("Total lines Cleared:"+space(tetrisData->getTotalLinesCleared(),28)+color(green)+std::to_string(tetrisData->getTotalLinesCleared()),17,10,pink);
		screen.display("Highest score:"+space(tetrisData->getHighestScore(),34)+color(green)+std::to_string(tetrisData->getHighestScore()),24,10,cyan);
		screen.display("Press # to go back",31,25,darkgray);
	}
	
	void instructions() {
		// set the current screen page
		screen.setPage(Page::Instructions);
		
		// display the current screen page title
		screen.display(" INSTRUCTIONS ",3,28,white,Red);
		screen.display("Controlling the Tetromino",7,9,green);
		
		screen.display("Moving a tetromino can be done with the directional",9,9,darkgray);
		screen.display("keys 4 and 6. With 4 to move the tetromino left and",10,9,darkgray);
		screen.display("6 to move the tetromino right. You can also rotate",11,9,darkgray);
		screen.display("the tetromino by pressing 5, and perform a quicker",12,9,darkgray);
		screen.display("drop by continously pressing 8. An instant drop is",13,9,darkgray);
		screen.display("done when you press 0.",14,9,darkgray);
		
		// display info on the gameplay
		screen.display(" Gameplay and Objective ",18,9,green);
		screen.display("The game is played on a 10x20 grid with tetrominoes",20,9,darkgray);
		screen.display("falling from above. Arrange them as they fall so as",21,9,darkgray);
		screen.display("to form an horizontal line. A line will be cleared",22,9,darkgray);
	    screen.display("off as soon as it is formed. The game ends when the",23,9,darkgray);
	    screen.display("tetrominoes are piled up to the top of the grid.",24,9,darkgray);
	    screen.display("Clear the highest number of lines before it ends!",25,9,darkgray);
	    screen.display("Press # to go back",31,25,darkgray);
	}
	
	void settings() {
		screen.setPage(Page::Settings);
		screen.display(" SETTINGS ",3,30,white,Red);
	}
	
} /* end of namespace interface */

// processes the option selected by the user
void userOption(const int& selectorPosition) {
	// the user wants to go back
	if (screen.getCommand() == '#' && screen.getPage() != Page::Menu) { screen.clear(); interface::menu(); }
	// the user wants to select an option
	else if (screen.getCommand() == '5' && screen.getPage() == Page::Menu) {
	    screen.clear();
	    // load the screen option the user selects
	    switch (selectorPosition) {
		    case 11: interface::newGame(); break;
		    case 14: interface::difficulty(); break;
		    case 17: interface::scores(); break;
	    	case 20: interface::instructions(); break;
			case 23: interface::settings(); break;
		}
	// the user wants to set a difficulty
	} else if (screen.getCommand() == '5' && screen.getPage() == Page::Difficulty) { setDifficulty(); }
}

// starts executing the program
void runGame() {
	// hide the cursor
	std::cout << "\033[?25l" << std::endl;
	// create a screen container for display
	screen.createContainer(59,33,1,5,blue);
	// create an inner screen container
	screen.createContainer(57,31,2,6,blue,'"','_','"');
	interface::menu();
	setDifficulty();
	// never stop running the game
	// NOTE: no form of recursion is used to keep the game running
	// control will ALWAYS come back to this loop no matter where it goes
	while (true) {
		screen.initialize_selection();
	}
}
//===================================================================================================================================================//
#endif