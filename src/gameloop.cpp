/*
Program: GameLoop class
Description: This class represents the main gamplay loop.  This is taken out
of main to keep the flow of the game more clear, and to prevent a large main 
file.  The loop is to ask player 1 where they want to shoot, update their board
and player 2's board, then check if game is over.  If game is not over, then
player 2 gets their turn.
Author: Team 9
*/
#include "gameloop.hpp"

#include <iostream>
#include <limits>
#include <unistd.h>
#include <utility>

//Initialize gameplay loop by taking rvalue refs 
GameplayLoop::GameplayLoop(Player&& p1, Player&& p2) :  //use references to prevent memory issues
                           playerOne(std::move(p1)), playerTwo(std::move(p2)), //create players within this class.
                           currentTurn(1){} //this moves the player objects 

//Simple function that returns true if a shot is valid to take
bool GameplayLoop::verifyShot(const std::size_t row, const std::size_t col) const {
    //If the cell of the top board is X or O that means the shot has already been taken
    if ( !currentTurn ) {
        if ( playerOne.getCellTopBoard(row, col) == 'X' || playerOne.getCellTopBoard(row, col) == 'O' ) { //check if shot already taken
            return false; //verify shot as bad, don't continue
        }
    } else {
        if ( playerTwo.getCellTopBoard(row, col) == 'X' || playerTwo.getCellTopBoard(row, col) == 'O' ) { //check if already takena
            return false; //verify shot as bad, don't continue
        }
    }

    return true; //otherwise shot is good, continue
}

//This function gets a shot from the user in the format of RowCol
std::pair<std::size_t, std::size_t> GameplayLoop::getShot() const {
    size_t shot_row = 0; //Value of the input
    char column = 'a'; //Value of the input as a char

    while ( true ) {
        std::cout << "Please enter your shot's row: "; //min 1 - max 10
        //Loop to validate the rows
        //!(cin >> shot_row) means cin to fin has failed in some way, while also grabbing input
        while ( !(std::cin >> shot_row) || ( shot_row > 10 ) || ( shot_row < 1 ) ) {
            //Clears the failure state and then throws everything out of the input stream
            std::cin.clear();
            std::cin.ignore(numeric_limits<streamsize>::max(),'\n');
            std::cout << "Bad number (min 1, max 10) please try again: ";
        } 
        shot_row = shot_row - 1; //0 indexed now
        
        //then column
        std::cout << "Please enter your shot's column: "; //min a - max j

        //Loop to validate the column
        //!(cin >> column) means cin to fin has failed in some way, while also grabbing input
        while ( !(std::cin >> column ) || ( tolower(column) > 'j' ) || ( tolower(column) < 'a' ) ) {
            //Clears the failure state and then throws everything out of the input stream
            std::cin.clear();
            std::cin.ignore(numeric_limits<streamsize>::max(),'\n');
            std::cout << "Bad Letter ( a through j) please try again: ";
        } 

        //Now verify to see if the shot has already been taken
        //If the shot is valid return the coords from the function
        if ( !verifyShot(shot_row, playerOne.convert_chartoIndex( column ))) {
            std::cout << "You have already taken this shot! Try again.";
        } else {
            return std::make_pair(shot_row, playerOne.convert_chartoIndex( column )); //create pair to return for shot validation
        }
    }

}

void GameplayLoop::playerOneTurn() {
    //Player 1 takes their turn
    std::cout << "Player 1's Turn." << std::endl; //output player turn
    playerOne.print_Board(); //print player 1's board
    std::pair<std::size_t, std::size_t> coord = getShot();//pair that gets the shot from the user

    int flag = 0; //output ship

    for (int i = 1; i < playerOne.getNumShips() + 1; i++) {
        if (playerTwo.getShip(i)->valid_space(coord)) { //if is_hit, update board and ship
            flag = i; //figure out which ship has been marked
            break; //exit loop
        }
    }

    if (flag > 0) {
        //hit
        std::cout << "Player 1 Hit!" << std::endl;
        playerOne.top_board.update(coord, true); //update board
        playerTwo.getShip(flag)->hit(coord); //hit the ship
        playerTwo.bottom_board.update(coord, true); //update bottom board
    }
    else {
        std::cout << "Player 1 Miss!" << std::endl;
        playerOne.top_board.update(coord, false); //update with miss
        playerTwo.bottom_board.update(coord, false); //update with miss
    }

    playerOne.print_Board(); //reprint board(s)
    sleep(4); //sleep for hot seat, wait for next player
    system("clear"); //clear the terminal before the next action
}

void GameplayLoop::playerTwoTurn() {
    //Player 2 takes their turn
    std::cout << "Player 2's Turn." << std::endl; //output player's board
    playerTwo.print_Board(); //print player 2's board
    std::pair<std::size_t, std::size_t> coord = getShot(); //pair that gets the shot from the user

    int flag = 0; //output ship

    for (int i = 1; i < playerOne.getNumShips() + 1; i++) {
        if (playerOne.getShip(i)->valid_space(coord)) { //if is_hit, update board and ship
            flag = i; //which ship has been hit
            break; //exit loop
        }
    }

    if (flag > 0) { //ship has been hit
        std::cout << "Player 2 Hit!" << std::endl;
        playerTwo.top_board.update(coord, true); //update board
        playerOne.getShip(flag)->hit(coord); //hit the ship
        playerOne.bottom_board.update(coord, true); //update bottom board
    }
    else {
        std::cout << "Player 2 Miss!" << std::endl;
        playerTwo.top_board.update(coord, false); //update top board no hit
        playerOne.bottom_board.update(coord, false); //update bottom board no hit
    }

    playerTwo.print_Board(); //reprint board(s)
    sleep(4); //sleep for hot seat.
    system("clear"); //clear the terminal before the next action
}

bool GameplayLoop::gameOver() const {
    // Iterate through both players' ships and return true iff (if and onl if) all ships are sunk for one player
    const int numShips = playerOne.getNumShips(); //num ships to check for
    bool playerOneSunk = true; //set flag
    bool playerTwoSunk = true; //set flag
    for (int i = 1; i < numShips + 1; i++) { //check over all ships
        if (!(playerOne.getShip(i)->is_sunk())) { //check all ships are sunk
            playerOneSunk = false; //one ship is not sunk, game NOT over
        }
    }
    for (int i = 1; i < numShips + 1; i++) { //check player two's ships
        if (!(playerTwo.getShip(i)->is_sunk())) { //check all ships owned by player 2
            playerTwoSunk = false; //one ship not sunk, game NOT over
        }
    }
    if (playerOneSunk) { //check if player one loses
        std::cout << playerOne.returnName() << "'s ships have been sunk. " //use name to show who loses
                  << playerTwo.returnName() << " wins!" << std::endl; //show name to see who wins
        return true;  //for game play loop class
    }
    else if (playerTwoSunk) { //check if player two loses
        std::cout << playerTwo.returnName() << "'s ships have been sunk. " //use player two name to lose
                  << playerOne.returnName() << " wins!" << std::endl; //use player one name to lose
        return true; //there is a loss here
    }
    return false; //no loss
}

void GameplayLoop::start() {

    // Main loop
    while ( true ) {
        //Get the current player
        //(1 + 1) % 2 = 0 which is player one
        //(0 + 1) % 2 = 1 player two
        ++currentTurn %= 2;
        //if currentTurn == 0
        if ( !currentTurn ) {
            playerOneTurn(); //get the correct player's turn
        } else {
            playerTwoTurn();
        }
        //Check if game over 
        //If the game is over, exit the loop
        if( gameOver() ) {
            return;
        }

    }
}
