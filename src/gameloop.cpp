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
#include <utility>

//Initialize gameplay loop by taking rvalue refs 
GameplayLoop::GameplayLoop(Player&& p1, Player&& p2) : 
                           playerOne(std::move(p1)), playerTwo(std::move(p2)),
                           currentTurn(1){} //this moves the player objects 

//This function gets a shot from the user in the format of RowCol
std::pair<std::size_t, std::size_t> GameplayLoop::getShot() const {
    size_t shot_row = 0; //Value of the input
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
    char column = 'a'; //Value of the input as a char
    std::cout << "Please enter your shot's column: "; //min a - max j

    //Loop to validate the column
    //!(cin >> column) means cin to fin has failed in some way, while also grabbing input
    while ( !(std::cin >> column ) || ( tolower(column) > 'j' ) || ( tolower(column) < 'a' ) ) {
        //Clears the failure state and then throws everything out of the input stream
        std::cin.clear();
        std::cin.ignore(numeric_limits<streamsize>::max(),'\n');
        std::cout << "Bad Letter ( a through j) please try again: ";
    } 

    return std::make_pair(shot_row, playerOne.convert_chartoIndex( column ));
}

void GameplayLoop::playerOneTurn() {
    //Player 1 takes their turn
    std::cout << "Player 1's Turn." << std::endl;
    playerOne.print_Board(); //print player 1's board
    const auto [row, col] = getShot(); //Structured binding that gets the shot from the user
    system("clear"); //clear the terminal before the next action
}

void GameplayLoop::playerTwoTurn() {
    //Player 2 takes their turn
    std::cout << "Player 2's Turn." << std::endl;
    playerTwo.print_Board(); //print player 2's board
    const auto [row, col] = getShot(); //Structured binding that gets the shot from the user
    system("clear"); //clear the terminal before the next action
}

bool GameplayLoop::gameOver() const {
    // Iterate through both players' ships and return true iff all ships are sunk for one player
    int numShips = playerOne.getNumShips();
    bool playerOneSunk = true;
    bool playerTwoSunk = true;
    for (int i = 1; i < numShips + 1; i++) {
        if (!(playerOne.getShip(i)->is_sunk())) {
            playerOneSunk = false;
        }
    }
    for (int i = 1; i < numShips + 1; i++) {
        if (!(playerTwo.getShip(i)->is_sunk())) {
            playerTwoSunk = false;
        }
    }
    if (playerOneSunk) {
        std::cout << playerOne.returnName() << "'s ships have been sunk. "
                  << playerTwo.returnName() << " wins!" << std::endl;
        return true;
    }
    else if (playerTwoSunk) {
        std::cout << playerTwo.returnName() << "'s ships have been sunk. "
                  << playerOne.returnName() << " wins!" << std::endl;
        return true;
    }
    else {
        return false;
    }
}

void GameplayLoop::start() {

    // Main loop
    while ( true ) {
        //Get the current player
        //(1 + 1) % 2 = 0 which is player one
        //(0 + 1) % 2 = 1 player two
        ++currentTurn %= 2;
        //if currentTurn == 0
        if (!currentTurn) {
            playerOneTurn();
        } else {
            playerTwoTurn();
        }
        //Check if game over from player 1
        //If the game is over, exit the loop
        if( gameOver() ) {
            return;
        }
        //Player 2 takes their turn
        
        //Check if game over from player 2
        //If the game is over, exit the loop
        if( gameOver() ) {
            return;
        }
    }

}
