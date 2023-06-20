// contains everything the game needs to function
#include "GameUtility.h"
// namespace to contain specific assets used during gameplay
namespace tetris
{
	typedef std::string Block;
	
	// stores the action the user wants to perform on the game
	char actionCommand = '\0';
	bool dropped = false;
	
	// available kinds of tetrominoes
    enum class Type {
    	Undefined,Chord,Square,TBlock,LBlock,RLBlock,ZBlock,RZBlock
    };
    // the state defines the current rotation state of the tetronino
	enum class State {
		Undefined,Up,Down,Left,Right
	};
	
	enum class Movement {
		Undefined,Up,Down,Left,Right
	};
	
	enum class Drop {
		Normal,Instant
	};
	
	// position of each block that makes up the tetromino
	struct bit {
		// coordinates
		short row,column;
		
		// constructors
		bit() {}
		bit(short r,short c) : row(r),column(c) {}
		
		// operator
		bool operator==(const bit& coordinate) {
			return (this->row == coordinate.row && this->column == coordinate.column);
		}
	};
	
	// array of bit to store the coordinates or positions that are not free
	std::vector<bit> bitsArray;
	
	// array of Block to store a shape and where it's located on the matrix
	std::vector<Block> blocksArray;
	
	State shapeStateInfo = State::Undefined;
	
	// stores the kind of movement action being made
	Movement movementType = Movement::Undefined;
	Drop dropType = Drop::Normal;
	
	// stores previous coordinates of a tetromino
	short rowBitStorage[4],colBitStorage[4];
	
	// determines if a line has been formed somewhere and if the matrix is full, respectively
	bool lineIsFormed = false,full = false;
	
	// different kinds of tetromino will be built with this class
	class Tetromino
	{
		private:
		    // checks for collision and handles it
		    bool collision(bool = false),bitSet = false;
		protected:
		    // a block has a shape and color
		    Block brick = Block("[]");
		    bcgColor brickColor; // color of the tetromino
		    
		    // what kind of tetromino
		    Type shapeType = Type::Undefined;
		    // stores current rotation state
		    State shapeState = State::Undefined; 
		    // coordinates(row and column) of each block that makes up the tetromino
		    int initialColumn = 22, rbits[4] = {9,0,0,0}, cbits[4] = {initialColumn,0,0,0};
		    
		    // set increment to individual bits
		    void setbits(int,int,int,int,int,int,int,int);
        public:
            // set block color 
            void setBrickColor(const bcgColor& c) { this->brickColor = c; }
            void setShapeState(const State& value) { this->shapeState = value; }
            
            inline void modifyRBit(const int& index,const int& value) { rbits[index] = value; }
            inline void modifyCBit(const int& index,const int& value) { cbits[index] = value; }
            inline void modifyRBit(const int& value) { for (int i = 0; i < 4; ++i) rbits[i] += value; }
            inline void modifyCBit(const int& value) { for (int i = 0; i < 4; ++i) cbits[i] += value; }
            inline void setBitSet(bool value) { this->bitSet = value; }
            
            inline int getrbits(const int& index) const { return this->rbits[index]; }
            inline int getcbits(const int& index) const { return this->cbits[index]; }
            inline int getInitialColumn() const { return this->initialColumn; }
            
            // erase the previous view of the shape
            inline void erase() const { for (int i = 0; i < 4; ++i) std::cout << color() << cursor(rbits[i],cbits[i]) << std::string(2,' '); }
            
            // movement actions
            void moveLeft(),moveRight(),turn(),storeCurrentPos(),stayAtCurrentPos();
            int drop();
            
            virtual Block getShape() = 0;
            virtual Block rotate() = 0;
	};
	
	// a chord shaped tetromino
	class Chord : public Tetromino
	{
		public:
		    // constructor
		    Chord(bcgColor c) {
		        setBrickColor(c); shapeType = Type::Chord; shapeState = State::Up; cbits[0] = initialColumn = 20;
		    }
		    
		    // returns the shape based on its state
		    Block getShape() {
		    	// the leftmost col will always be the column of the first block irrespective of rotation
		    	 ;
		    	switch (shapeState) {
                	case State::Up:    setbits(0,0,0,2,0,4,0,6); return (Block(color(white,brickColor)+cursor(rbits[0],cbits[0])+brick+brick+brick+brick)); break;
                	case State::Right: setbits(0,0,1,0,2,0,3,0); return (Block(color(white,brickColor)+cursor(rbits[0],cbits[0])+brick+cursor(rbits[1],cbits[1])+brick+cursor(rbits[2],cbits[2])+brick+cursor(rbits[3],cbits[3])+brick)); break;		    	   
		    	    case State::Down:  setbits(0,0,0,2,0,4,0,6); return (Block(color(white,brickColor)+cursor(rbits[0],cbits[0])+brick+brick+brick+brick)); break;
		            case State::Left:  setbits(0,0,1,0,2,0,3,0); return (Block(color(white,brickColor)+cursor(rbits[0],cbits[0])+brick+cursor(rbits[1],cbits[1])+brick+cursor(rbits[2],cbits[2])+brick+cursor(rbits[3],cbits[3])+brick)); break;
		    	}
		    }
		    // specifically rotate this shape
		    Block rotate() {
		    	// clean the previous shape
		    	erase();
		    	
		    	// rotate the shape 90 degrees clockwise relative to its previous state
		    	switch (shapeState) {
		    		case State::Up: shapeState = State::Right;   setbits(-1,2,1,0,2,0,3,0); return getShape(); break;		    	   
		    	    case State::Right: shapeState = State::Down; setbits(1,-4,0,2,0,4,0,6); return getShape(); break;
		            case State::Down: shapeState = State::Left;  setbits(-2,4,1,0,2,0,3,0); return getShape(); break;
		    	}
		    	// return initial shape
		        shapeState = State::Up; setbits(2,-2,0,2,0,4,0,6); return (getShape());
		    }
	};
	
	// a square shaped tetromino
	class Square : public Tetromino
	{
		public:
		    // constructor 
		    Square(bcgColor c) {
		        setBrickColor(c); shapeType = Type::Square;
		    }
		    
		    // a square has one state : undefined
		    Block getShape() { setbits(0,0,0,2,1,0,1,2); return (Block(color(white,brickColor)+cursor(rbits[0],cbits[0])+brick+brick+cursor(rbits[2],cbits[2])+brick+brick)); }
		    
		    Block rotate() {
		    	// clean the previous shape
		    	erase();
		    	
		    	// a square is the same despite rotation
		    	return (getShape());
		    }
	};
	
	// a T shaped tetromino
	class TBlock : public Tetromino
	{
		public:
		    // constructor 
		    TBlock(bcgColor c) {
		        setBrickColor(c); shapeType = Type::TBlock; shapeState = State::Up;
		    }
		    
		    Block getShape() {
		    	switch (shapeState) {
                    case State::Up:    setbits(0,0,0,2,0,4,1,2);  return (Block(color(white,brickColor)+cursor(rbits[0],cbits[0])+brick+brick+brick+cursor(rbits[3],cbits[3])+brick)); break;
		    	    case State::Right: setbits(0,0,1,-2,1,0,2,0); return (Block(color(white,brickColor)+cursor(rbits[0],cbits[0])+brick+cursor(rbits[1],cbits[1])+brick+brick+cursor(rbits[3],cbits[3])+brick)); break;
		    	    case State::Down:  setbits(0,0,1,-2,1,0,1,2); return (Block(color(white,brickColor)+cursor(rbits[0],cbits[0])+brick+cursor(rbits[1],cbits[1])+brick+brick+brick)); break;
		    	    case State::Left:  setbits(0,0,1,0,1,2,2,0);  return (Block(color(white,brickColor)+cursor(rbits[0],cbits[0])+brick+cursor(rbits[1],cbits[1])+brick+brick+cursor(rbits[3],cbits[3])+brick)); break;
		    	}
		    }
		    // specifically rotate this shape
		    Block rotate() {
		    	// clean the previous shape
		    	erase();
		    	
		    	// rotate the shape 90 degrees clockwise relative to its previous state
		    	switch (shapeState) {
		    	    case State::Up: shapeState = State::Right;   setbits(-1,2,1,-2,1,0,2,0); return getShape(); break;
		    	    case State::Right: shapeState = State::Down; setbits(0,0,1,-2,1,0,1,2);  return getShape(); break;
		    	    case State::Down: shapeState = State::Left;  setbits(0,0,1,0,1,2,2,0);   return getShape(); break;
		    	}
		    	// return initial shape
		    	shapeState = State::Up; setbits(1,-2,0,2,0,4,1,2); return (getShape());
		    }
	};
	
	// an L shaped tetromino
	class LBlock : public Tetromino
	{
		public:
		    // constructor 
		    LBlock(bcgColor c) {
		        setBrickColor(c); shapeType = Type::LBlock; shapeState = State::Up;
		    }
		    
		    Block getShape() {
		    	switch (shapeState) {
		    		case State::Up:    setbits(0,0,0,2,0,4,1,0);   return (Block(color(white,brickColor)+cursor(rbits[0],cbits[0])+brick+brick+brick+cursor(rbits[3],cbits[3])+brick)); break;
		    		case State::Right: setbits(0,0,0,2,1,2,2,2);   return (Block(color(white,brickColor)+cursor(rbits[0],cbits[0])+brick+brick+cursor(rbits[2],cbits[2])+brick+cursor(rbits[3],cbits[3])+brick)); break;
		    	    case State::Down:  setbits(0,0,1,-4,1,-2,1,0); return (Block(color(white,brickColor)+cursor(rbits[0],cbits[0])+brick+cursor(rbits[1],cbits[1])+brick+brick+brick)); break;
		    	    case State::Left:  setbits(0,0,1,0,2,0,2,2);   return (Block(color(white,brickColor)+cursor(rbits[0],cbits[0])+brick+cursor(rbits[1],cbits[1])+brick+cursor(rbits[2],cbits[2])+brick+brick)); break;
		    	}
		    }
		    // specifically rotate this shape
		    Block rotate() {
		    	// clean the previous shape
		    	erase();
		    	
		    	// rotate the shape 90 degrees clockwise relative to its previous state
		    	switch (shapeState) {
		    		case State::Up: shapeState = State::Right;   setbits(-1,0,0,2,1,2,2,2);  return getShape(); break;
		    	    case State::Right: shapeState = State::Down; setbits(0,4,1,-4,1,-2,1,0); return getShape(); break;
		    	    case State::Down: shapeState = State::Left;  setbits(0,-2,1,0,2,0,2,2);  return getShape(); break;
		    	}
		    	// return initial shape
		        shapeState = State::Up; setbits(1,-2,0,2,0,4,1,0); return (getShape());
		    }
	};
	
	// a reversed L shaped tetromino
	class RLBlock : public Tetromino
	{
		public:
		    // constructor 
		    RLBlock(bcgColor c) {
		        setBrickColor(c); shapeType = Type::RLBlock; shapeState = State::Up;
		    }
		    
		    Block getShape() {
		    	switch (shapeState) {
		    		case State::Up:    setbits(0,0,0,2,0,4,1,4);  return (Block(color(white,brickColor)+cursor(rbits[0],cbits[0])+brick+brick+brick+cursor(rbits[3],cbits[3])+brick)); break;
		    		case State::Right: setbits(0,0,1,0,2,-2,2,0); return (Block(color(white,brickColor)+cursor(rbits[0],cbits[0])+brick+cursor(rbits[1],cbits[1])+brick+cursor(rbits[2],cbits[2])+brick+brick)); break;
		    	    case State::Down:  setbits(0,0,1,0,1,2,1,4);  return (Block(color(white,brickColor)+cursor(rbits[0],cbits[0])+brick+cursor(rbits[1],cbits[1])+brick+brick+brick)); break;
		    	    case State::Left:  setbits(0,0,0,2,1,0,2,0);  return (Block(color(white,brickColor)+cursor(rbits[0],cbits[0])+brick+brick+cursor(rbits[2],cbits[2])+brick+cursor(rbits[3],cbits[3])+brick)); break;
		    	}
		    }
		    // specifically rotate this shape
		    Block rotate() {
		    	// clean the previous shape
		    	erase();
		    	
		    	// rotate the shape 90 degrees clockwise relative to its previous state
		    	switch (shapeState) {
		    		case State::Up: shapeState = State::Right;   setbits(-1,2,1,0,2,-2,2,0); return getShape(); break;
		    	    case State::Right: shapeState = State::Down; setbits(0,-2,1,0,1,2,1,4);  return getShape(); break;
		    	    case State::Down: shapeState = State::Left;  setbits(0,2,0,2,1,0,2,0);   return getShape(); break;
		    	}
		    	// return initial shape
		    	shapeState = State::Up; setbits(1,-2,0,2,0,4,1,4); return (getShape());
		    }
	};
	
	// a Z shaped tetromino
	class ZBlock : public Tetromino
	{
		public:
		    // constructor 
		    ZBlock(bcgColor c) {
		        setBrickColor(c); shapeType = Type::ZBlock; shapeState = State::Up;
		    }
		    
		    Block getShape() {
		    	switch (shapeState) {
		    		case State::Up:    setbits(0,0,0,2,1,2,1,4);   return (Block(color(white,brickColor)+cursor(rbits[0],cbits[0])+brick+brick+cursor(rbits[2],cbits[2])+brick+brick)); break;
		    		case State::Right: setbits(0,0,1,-2,1,0,2,-2); return (Block(color(white,brickColor)+cursor(rbits[0],cbits[0])+brick+cursor(rbits[1],cbits[1])+brick+brick+cursor(rbits[3],cbits[3])+brick)); break;
		    	    case State::Down:  setbits(0,0,0,2,1,2,1,4);   return (Block(color(white,brickColor)+cursor(rbits[0],cbits[0])+brick+brick+cursor(rbits[2],cbits[2])+brick+brick)); break;
		    	    case State::Left:  setbits(0,0,1,-2,1,0,2,-2); return (Block(color(white,brickColor)+cursor(rbits[0],cbits[0])+brick+cursor(rbits[1],cbits[1])+brick+brick+cursor(rbits[3],cbits[3])+brick)); break;
		    	}
		    }
		    // specifically rotate this shape
		    Block rotate() {
		    	// erase the previous shape
		    	erase();
		    	
		    	// rotate the shape 90 degrees clockwise relative to its previous state
		    	switch (shapeState) {
		    		case State::Up: shapeState = State::Right;   setbits(-1,2,1,-2,1,0,2,-2); return getShape(); break;
		    	    case State::Right: shapeState = State::Down; setbits(0,-2,0,2,1,2,1,4);   return getShape(); break;
		    	    case State::Down: shapeState = State::Left;  setbits(0,4,1,-2,1,0,2,-2);  return getShape(); break;
		    	}
		    	// return initial shape
		    	shapeState = State::Up; setbits(1,-4,0,2,1,2,1,4); return (getShape());
		    }
	};
	
	// a reversed Z shaped tetromino
	class RZBlock : public Tetromino
	{
		public:
		    // constructor 
		    RZBlock(bcgColor c) {
		        setBrickColor(c); shapeType = Type::RZBlock; shapeState = State::Up; cbits[0] = initialColumn = 24;
		    }
		    
		    Block getShape() {
		    	switch (shapeState) {
		    		case State::Up:    setbits(0,0,0,2,1,-2,1,0); return (Block(color(white,brickColor)+cursor(rbits[0],cbits[0])+brick+brick+cursor(rbits[2],cbits[2])+brick+brick)); break;
		    		case State::Right: setbits(0,0,1,0,1,2,2,2);  return (Block(color(white,brickColor)+cursor(rbits[0],cbits[0])+brick+cursor(rbits[1],cbits[1])+brick+brick+cursor(rbits[3],cbits[3])+brick)); break;
		    		case State::Down:  setbits(0,0,0,2,1,-2,1,0); return (Block(color(white,brickColor)+cursor(rbits[0],cbits[0])+brick+brick+cursor(rbits[2],cbits[2])+brick+brick)); break;
		    		case State::Left:  setbits(0,0,1,0,1,2,2,2);  return (Block(color(white,brickColor)+cursor(rbits[0],cbits[0])+brick+cursor(rbits[1],cbits[1])+brick+brick+cursor(rbits[3],cbits[3])+brick)); break;
		    	}
		    }
		    // specifically rotate this shape
		    Block rotate() {
		    	// erase the previous shape
		    	erase();
		    	
		    	// rotate the shape 90 degrees clockwise relative to its previous state
		    	switch (shapeState) {
		    		case State::Up: shapeState = State::Right;   setbits(-1,-2,1,0,1,2,2,2); return getShape(); break;
		    		case State::Right: shapeState = State::Down; setbits(0,2,0,2,1,-2,1,0);  return getShape(); break;
		    		case State::Down: shapeState = State::Left;  setbits(0,0,1,0,1,2,2,2);   return getShape(); break;
		    	}
		    	// return initial shape
		    	shapeState = State::Up; setbits(1,0,0,2,1,-2,1,0); return (getShape());
		    }
	};
	
	class ShapeContainer
    {
    	private:
    	    // create the tetrominoes
	        Chord chord = Chord(Red); Square square = Square(Pink); TBlock tblock = TBlock(Green); LBlock lblock = LBlock(Yellow);  ZBlock zblock = ZBlock(Blue); RLBlock rlblock = RLBlock(Cyan); RZBlock rzblock = RZBlock(DarkGray);
	    public:
	        // randomly selects a shape
	        auto selectShape() {
	        	static std::vector<Tetromino*> tetrominoArray {&chord,&square,&tblock,&lblock,&rlblock,&zblock,&rzblock};
	        	// random number generation process
	        	static std::random_device randomDevice;
	        	static std::uniform_int_distribution dist(0,6);
	        	
	        	// select a type
	        	return tetrominoArray[dist(randomDevice)];
	        }
    };
    
    // create an object that contains the tetrominoes
    ShapeContainer shapes;
    
    // sets the tetromino to its initial details
    void reset(Tetromino* tetromino) {
		tetromino->modifyRBit(0,9);
		tetromino->modifyCBit(0,tetromino->getInitialColumn());
		for (int i = 1; i <= 3; ++i) {
		    tetromino->modifyRBit(i,0); tetromino->modifyCBit(i,0);
	    }
	    tetromino->setShapeState(State::Up);
    }
    
    // set all game resources to default values
    void clearResources() {
    	// clear the array that stores the position of each tetromino block
    	bitsArray.clear();
        // clear the array that stores each tetromino shape and their position on screen
    	blocksArray.clear();
    	// the matrix has been cleared so it has nothing in it
    	full = lineIsFormed = dropped = false;
    }
    
    // gets user command and performs an action based on it
    int getActionCommand(Tetromino* tetromino,bool access = false) {
    	if (kbhit()||access) {
    		
    		// get user input
        	actionCommand = getch();
        	
        	// perform an action
            switch (actionCommand) {
            	case '#': reset(tetromino); clearResources(); return 1; // break;
    	    	case '4': tetromino->moveLeft(); break;
        		case '6': tetromino->moveRight(); break;
    	    	case '5': tetromino->turn(); break;
    	    	case '0': dropType = Drop::Instant; break;
    	    	/* case '8':
    	    	    // store the current position
    	    	    tetromino->storeCurrentPos();
    	    	    // erase the previous shape
    	    	    tetromino->erase();
    	    	    // drop the shape one row below irrespective of delay period
    	    	    tetromino->modifyRBit(0,tetromino->getrbits(0)+1);
    	    	    // display the shape
    	    	    screen.display(tetromino->getShape());
    	    	    tetromino->setBitSet(false); dropType = Drop::Normal;
    	    	break; */
    	        // pause the game
    	        default: return getActionCommand(tetromino,true); break;
    	    }
    	}
    	return 0;
    }
    
    // updates the scores during gameplay
    void updateScores() {
    	screen.display(color(pink)+center("Score: "+color(green)+std::to_string(tetrisData->getScore()),15,42,62,green));
		screen.display(color(pink)+center("Lines: "+color(green)+std::to_string(tetrisData->getLinesCleared()),18,42,62,green));
    }
    
    // determines if a shape cannot enter the matrix
    bool matrixIsFull(Tetromino *tetromino) {
    	// store the current coordinates of the tetromino
    	tetromino->storeCurrentPos();
    	// check if the shape can be pushed into the matrix without colliding
    	tetromino->getShape(); // doesn't display. It just invisibly does the same thing as it would when displayed
    	tetromino->setBitSet(false);
    	
    	// return the result(true if it can fit in the matrix,otherwise false)
    	return full;
    }
    
    inline void clearMatrix() {
    	for (int i = 9; i < 29; ++i) std::cout << cursor(i,14) << color() << std::string(20,' ');
    }
    
    void GameOver() {
        Sleep(500); screen.clear();
    	screen.display("G A M E  O V E R!",17,27,green); std::cout << std::flush;
    	unsigned score = tetrisData->getScore(),lines = tetrisData->getLinesCleared();
    	clearResources();
    	// store the scores if they are greater than the one in storage
    	tetrisData->createGameData();
    	if (score > tetrisData->getHighestScore()) tetrisData->setHighestScore(score);
    	if (lines > tetrisData->getHighestLines()) tetrisData->setHighestLines(lines);
    	tetrisData->incrementTotalLinesCleared();
    	tetrisData->incrementGamesPlayed();
    	tetrisData->createGameData();
    	// reset the current game scores
    	tetrisData->setDefaultGameScores();
    	    
    	Sleep(5000);
    	actionCommand = '#';
    }
    
    // checks if a line has been formed
    void checkLine(Tetromino* tetromino) {
    	for (int i = 0; i < 4; ++i) {
    		
    		// assume a line has been formed
    		lineIsFormed = true;
    		// the row where the line was formed
    		int lineRow = tetromino->getrbits(i);
    		
    		// check if a line has really been formed
    		for (int column = 14; column <= 32; column += 2) {
    			if (std::find(bitsArray.begin(),bitsArray.end(),bit(lineRow,column)) == bitsArray.end()) {
    				lineIsFormed = false; break;
    			}
    		}
    		
    		// clear the line if it has been formed
    		if (lineIsFormed) {
    			// erase the line
    			std::cout << cursor(lineRow,14) << color() << std::string(20,' ') << std::flush;
    			for (int column = 14; column <= 32; column += 2) {
    				// erase that row and its column from the array
    			    bitsArray.erase(find(bitsArray.begin(),bitsArray.end(),bit(lineRow,column)));
    			}
    		    
    			// now that the line is out of the array, move every block bit above the line down by 1 row
    		    for (auto& bitObject : bitsArray) {
    		    	// row 8 and row 29 are border rows so they should not be checked
    				if (bitObject.row < lineRow && bitObject.column != 12 && bitObject.column != 34 && bitObject.row != 8 && bitObject.row != 29) ++(bitObject.row);
    			}
    		    
    		    // clear the matrix of old block positions
    		    clearMatrix();
    		    
    		    // remove a few blocks from the line cleared in blocksArray
    			for (auto& e : blocksArray) {
    				long long index = e.find("\033["+std::to_string(lineRow)+";");
    				// if a block from the line cleared is in a Block object the array...
    				if (index != Block::npos) {
    					long long index1 = e.find("\033["+std::to_string(lineRow+1)+";"),index2 = e.find("\033["+std::to_string(lineRow-1)+";");
    					// ...remove the block from the Block object in the array
    					if (index1 == Block::npos && index2 == Block::npos) {
    							e = "";
    					} else if ((index1 != Block::npos && index2 != Block::npos)||index2 == Block::npos) {
    						e = e.erase(index,index1-index);
    					} else if (index1 == Block::npos) {
    						e.erase(index);
    					}
    				}
    			    
    				Block tempBlock = "";
    				// shift the blocks one row downwards
    		        while ((index = e.rfind("\033[")) != Block::npos) {
    		        	std::string row;
    		        	if (index != 0) {
    		        		if (e[index+3] != ';' && std::stoi(row = e.substr(index+2,2)) < lineRow) {
    		        			// increment the row of the block by 1
    		        		    e.replace(index+2,2,std::to_string(std::stoi(row)+1));
    		        		} else if (e[index+3] == ';' && std::stoi(row = e.substr(index+2,1)) < lineRow) {
    		        			// increment the row of the block by 1
    		        			e.replace(index+2,1,std::to_string(std::stoi(row)+1));
    		        		}
    		        		// insert the modified part of the bloxk at the begining of tempBlock
    		        		tempBlock.insert(0,e.substr(index));
    		        		// remove the modified part of the block from the object
    		        		e = e.substr(0,index);
    		        	} else {
    		        	    tempBlock.insert(0,e); e = tempBlock; break;
    		            }
    		        }
    		        std::cout << e;
    			}
    			
    			tetrisData->incrementLinesCleared();
    			tetrisData->incrementScore();
    			
    			// update the score
    			updateScores();
    		}
    	}
    }
    
    // sets the position of each block that makes up the tetromino
    void Tetromino::setbits(int r0,int c0,int r1,int c1,int r2,int c2,int r3,int c3) {
    	if (!bitSet) {
        	rbits[0] += r0; // where r1 is the increment or modification
            cbits[0] += c0; // where c1 is the increment or modification
            rbits[1] = rbits[0]+r1; cbits[1] = cbits[0]+c1;
            rbits[2] = rbits[0]+r2; cbits[2] = cbits[0]+c2;
            rbits[3] = rbits[0]+r3; cbits[3] = cbits[0]+c3;
            // check if a collision occurs from changing the position of the tetromino
            collision();
            bitSet = true;
    	}
    }
    
    // detects a collision and handles it
    bool Tetromino::collision(bool return_val) {
        // assume the shape has not collide with anything
        bool hasCollision = false;
        bool BitCollision[4] = {false,false,false,false};
        // check if the shape collides with anything
        for (int i = 0; i <= 3; ++i) {
            if (std::find(bitsArray.begin(),bitsArray.end(),bit(getrbits(i),getcbits(i))) != bitsArray.end()) {
   		     hasCollision = true; BitCollision[i] = true;
   	     }
        }
   	 if (hasCollision) {
   	 	if (return_val != false) return hasCollision;
   	     // how did the collision occur?
   	     switch (movementType) {
   	         // from dropping down? then the shape has landed
   		     case Movement::Down:  if (getrbits(0) == 9/*initialPosition()*/) { full = true; } stayAtCurrentPos(); dropped = true; break;
   			 // from moving to the left?
   			 case Movement::Left:  stayAtCurrentPos(); break;
   			 // from moving to the right?
   			 case Movement::Right: stayAtCurrentPos(); break;
   			 // from rotation?
   			 case Movement::Undefined:
   			     // rotation collision for a chord
   			     if (shapeType == Type::Chord) {
   			         if (shapeState == State::Up) {
   			     	    if (BitCollision[0]) {
   			     	        modifyCBit(2);
   			     	    } else if (BitCollision[2]) {
   			     	    	modifyCBit(-4);
   			     	    } else if (BitCollision[3]) {
   			     	    	modifyCBit(-2);
   			     	    }
   			         } else if (shapeState == State::Right) {
   			             if (BitCollision[0]) {
   			     	        modifyRBit(1);
   			     	    } else if (BitCollision[2]) {
   			     	    	modifyRBit(-2);
   			     	    } else if (BitCollision[3]) {
   			     	    	modifyRBit(-1);
   			     	    }
   			         } else if (shapeState == State::Down) {
   			             if (BitCollision[3]) {
   			     	        modifyCBit(-2);
   			     	    } else if (BitCollision[1]) {
   			     	    	modifyCBit(4);
   			     	    } else if (BitCollision[0]) {
   			     	    	modifyCBit(2);
   			     	    }
   			         } else if (shapeState == State::Left) {
   			         	if (BitCollision[3]) {
   			     	        modifyRBit(-1);
   			     	    } else if (BitCollision[1]) {
   			     	    	modifyRBit(2);
   			     	    } else if (BitCollision[0]) {
   			     	    	modifyRBit(1);
   			     	    }
   			         }
   			         // if collision still occurs after modification then don't rotate the shape
   			     	if (collision(true)) { stayAtCurrentPos(); }
   			     // rotation collision for a tblock
   			     } else if (shapeType == Type::TBlock) {
   			     	if (shapeState == State::Up) {
   			     		modifyCBit(2);
   			     	} else if (shapeState == State::Right) {
   			     		modifyRBit(1);
   			     	} else if (shapeState == State::Down) {
   			     		modifyCBit(-2);
   			     	} else if (shapeState == State::Left) {
   			     		modifyRBit(-1);
   			     	}
   			     	// if collision still occurs after modification then don't rotate the shape
   			     	if (collision(true)) { stayAtCurrentPos(); }
   			     } else if (shapeType == Type::LBlock) {
   			     	if (shapeState == State::Up) {
   			     		if (BitCollision[2]) {
   			     			modifyCBit(-2);
   			     		} else if (BitCollision[0] || BitCollision[3]) {
   			     			modifyCBit(2);
   			     		}
   			     	} else if (shapeState == State::Right) {
   			     		if (BitCollision[3]) {
   			     			modifyRBit(-1);
   			     		} else if (BitCollision[0] || BitCollision[1]) {
   			     		    modifyRBit(1);
   			     		}
   			     	} else if (shapeState == State::Down) {
   			     		if (BitCollision[1]) {
   			     			modifyCBit(2);
   			     		} else if (BitCollision[0] || BitCollision[3]) {
   			     		    modifyCBit(-2);
   			     		}
   			     	} else if (shapeState == State::Left) {
   			     		if (BitCollision[0]) {
   			     			modifyRBit(1);
   			     		} else if (BitCollision[2] || BitCollision[3]) {
   			     		    modifyRBit(-1);
   			     		}
   			     	}
   			     	// if collision still occurs after modification then don't rotate the shape
   			     	if (collision(true)) { stayAtCurrentPos(); }
   			     } else if (shapeType == Type::RLBlock) {
   			     	if (shapeState == State::Up) {
   			     		if (BitCollision[0]) {
   			     			modifyCBit(2);
   			     		} else if (BitCollision[2] || BitCollision[3]) {
   			     			modifyCBit(-2);
   			     		}
   			     	} else if (shapeState == State::Right) {
   			     		if (BitCollision[0]) {
   			     			modifyRBit(1);
   			     		} else if (BitCollision[2] || BitCollision[3]) {
   			     		    modifyRBit(-1);
   			     		}
   			     	} else if (shapeState == State::Down) {
   			     		if (BitCollision[3]) {
   			     			modifyCBit(-2);
   			     		} else if (BitCollision[0] || BitCollision[1]) {
   			     		    modifyCBit(2);
   			     		}
   			     	} else if (shapeState == State::Left) {
   			     		if (BitCollision[3]) {
   			     			modifyRBit(-1);
   			     		} else if (BitCollision[0] || BitCollision[1]) {
   			     		    modifyRBit(1);
   			     		}
   			     	}
   			     	// if collision still occurs after modification then don't rotate the shape
   			     	if (collision(true)) { stayAtCurrentPos(); }
   			     } else if (shapeType == Type::ZBlock) {
   			     	if (shapeState == State::Up) {
   			     		if (BitCollision[0]) {
   			     			modifyCBit(2);
   			     		} else if (BitCollision[3]) {
   			     			modifyCBit(-2);
   			     		}
   			     	} else if (shapeState == State::Right) {
   			     		if (BitCollision[0]) {
   			     			modifyRBit(1);
   			     		} else if (BitCollision[3]) {
   			     		    modifyRBit(-1);
   			     		}
   			     	} else if (shapeState == State::Down) {
   			     		if (BitCollision[3]) {
   			     			modifyCBit(-2);
   			     		} else if (BitCollision[0]) {
   			     		    modifyCBit(2);
   			     		}
   			     	} else if (shapeState == State::Left) {
   			     		if (BitCollision[3]) {
   			     			modifyRBit(-1);
   			     		} else if (BitCollision[0]) {
   			     		    modifyRBit(1);
   			     		}
   			     	}
   			     	// if collision still occurs after modification then don't rotate the shape
   			     	if (collision(true)) { stayAtCurrentPos(); }
   			     } else if (shapeType == Type::RZBlock) {
   			     	if (shapeState == State::Up) {
   			     		if (BitCollision[3]) {
   			     			modifyCBit(4);
   			     		} else if (BitCollision[2]) {
   			     			modifyCBit(2);
   			     		}
   			     	} else if (shapeState == State::Right) {
   			     		if (BitCollision[1]) {
   			     			modifyRBit(2);
   			     		} else if (BitCollision[0]) {
   			     		    modifyRBit(1);
   			     		}
   			     	} else if (shapeState == State::Down) {
   			     		if (BitCollision[0]) {
   			     			modifyCBit(-4);
   			     		} else if (BitCollision[1]) {
   			     		    modifyCBit(-2);
   			     		}
   			     	} else if (shapeState == State::Left) {
   			     		if (BitCollision[2]) {
   			     			modifyRBit(-2);
   			     		} else if (BitCollision[3]) {
   			     		    modifyRBit(-1);
   			     		}
   			     	}
   			     	// if collision still occurs after modification then don't rotate the shape
   			     	if (collision(true)) { stayAtCurrentPos(); }
   			     }
   			 break;
   	     }
        }
        // to avoid segfault
        return false;
    }
    
    // stores the current coordinates (here,anything without "this" does not belong to the class)
    void Tetromino::storeCurrentPos() {
    	for (int i = 0; i < 4; ++i) { rowBitStorage[i] = this->getrbits(i); colBitStorage[i] = this->getcbits(i); }
	    shapeStateInfo = this->shapeState;
    }
    
    // restores the last saved position of the tetromino (here,anything without "this" does not belong to the class)
    void Tetromino::stayAtCurrentPos() {
        for (int i = 0; i < 4; ++i) { this->modifyRBit(i,rowBitStorage[i]); this->modifyCBit(i,colBitStorage[i]); }
	    this->shapeState = shapeStateInfo;
    }
    
    void Tetromino::turn() {
    	movementType = Movement::Undefined;
        screen.display(rotate()); std::cout << std::flush;
        this->storeCurrentPos(); this->bitSet = false;
    }
    
    // fall a tetromino of any type
    int Tetromino::drop() {
    	if (!matrixIsFull( this )) {
        	// wait for user input while waiting to drop the tetromino
        	for (int x = 2,i = x; dropType == Drop::Normal && i < delay; i += x) { Sleep(x); if (getActionCommand(this) == 1) return 1; }
        	movementType = Movement::Down;
        	// clean the previous shape
        	erase();
        	// increment the row and print the shape
            switch (dropType) {
            	case Drop::Instant: while (!dropped) { storeCurrentPos(); modifyRBit(0,getrbits(0)+1); /* getShape checks collision */ getShape(); bitSet = false; } break;
            	case Drop::Normal:  modifyRBit(0,getrbits(0)+1); break;
            }
        	// display the tetromino on the next row
        	screen.display(getShape()); std::cout << std::flush;
        	storeCurrentPos(); bitSet = false; dropType = Drop::Normal;
    	}
    	return 0;
    }
    
    // moves a tetromino to the left
    void Tetromino::moveRight() {
    	movementType = Movement::Right;
    	erase();
    	
    	modifyCBit(0,getcbits(0)+2);
    	// display the shape
    	screen.display(getShape()); std::cout << std::flush;
    	storeCurrentPos(); bitSet = false;
    }
    
    void Tetromino::moveLeft() {
    	movementType = Movement::Left;
    	erase();
    	
    	modifyCBit(0,getcbits(0)-2);
    	// display the shape
    	screen.display(getShape()); std::cout << std::flush;
    	storeCurrentPos(); bitSet = false;
    }
    
    // performs an action based on user command
    int performAction() {
    	// store the next shape to fall
        static Tetromino *NextShape;
        auto tetromino = (NextShape != nullptr)? NextShape : shapes.selectShape();
        
        // clear the previous shape in the next shape box
        for (int i = 7; i <= 8; ++i) { std::cout << cursor(i,48) << color() << std::string(10,' '); }
        
         // select and display the next shape to fall
        NextShape = shapes.selectShape(); NextShape->modifyRBit(-2); NextShape->modifyCBit(28);
        screen.display(NextShape->getShape()); NextShape->setBitSet(false);
        reset(NextShape);
        
        // end the game if the matrix is full
        if (matrixIsFull(tetromino)) { GameOver(); NextShape = nullptr; return 0; }
        
        // display the current falling shape
        screen.display(tetromino->getShape()); std::cout << std::flush;
        tetromino->setBitSet(false);
        
        // fall the shape
    	do {
    		// the tetromino will always drop but the return statement
    		// will only run if drop() returns 1
    		if(tetromino->drop() == 1) return 1; // drop the tetromino
    	} while (!dropped);
    	
    	// code block to maintain reserves for the vectors
        {
        	if (bitsArray.size() == bitsArray.capacity()) bitsArray.reserve(bitsArray.capacity()+100);
        	if (blocksArray.size() == blocksArray.capacity()) blocksArray.reserve(blocksArray.capacity()+100);
    	}
    	
    	// the shape has landed. store its position in the vector
    	for (int i = 0; i < 4; ++i) {
    	    bitsArray.push_back(bit(tetromino->getrbits(i),tetromino->getcbits(i)));
    	}
    	
    	// push the shape to the array of Blocks
    	blocksArray.push_back(tetromino->getShape()); tetromino->setBitSet(false);
    	
    	// check if a line has been cleared
    	checkLine(tetromino);
    	reset(tetromino); dropped = false;
    	// return the cursor to the initial position
    	std::cout << cursor() << color() << std::flush;
    	return 0;
    }
    
    // stops the current game and returns to menu
    void endCurrentGame() {
    	//.....
    	actionCommand = '\0'; screen.clear(); interface::menu();
    }
    
	
}

// gets user commands in game screen which in turn drives the game
void startNewGame() {
	tetris::bitsArray.reserve(1000);
	tetris::blocksArray.reserve(1000);
	
	// set default limits
	for (int i = 8; i <= 29; ++i) { tetris::bitsArray.push_back(tetris::bit(i,12)); tetris::bitsArray.push_back(tetris::bit(i,34)); }
	for (int i = 12; i <= 34; i+=2) { tetris::bitsArray.push_back(tetris::bit(8,i)); tetris::bitsArray.push_back(tetris::bit(29,i)); }
	
	while (tetris::actionCommand != '#') tetris::performAction();
	
	// loop reaches here when the user presses #
	tetris::endCurrentGame();
}

// code execution starts from here
int main()
{
	// start the game application
	runGame();
}