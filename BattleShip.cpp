#include <iostream>
#include <vector>
#include <map>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <limits>
#include <conio.h>

using namespace std;

#include "class.h"
void displayWelcomeMessage() {
    cout << "====================================" << endl;
    cout << "      WELCOME TO BATTLESHIP GAME     " << endl;
    cout << "====================================" << endl << endl;
}

int getGameMode() {
    int choice;
    while (true) {
        cout << "Select game mode:\n";
        cout << "1. Player vs Player\n";
        cout << "2. Player vs Computer\n";
        cout << "3. Computer vs Computer (Simulation)\n";
        cout << "4. Exit Game\n";
        cout << "Enter your choice (1-4): ";

        cin >> choice;

        if (cin.fail() || choice < 1 || choice > 4) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please try again.\n";
        }
        else {
            return choice;
        }
    }
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));  // Random seed initialization

    displayWelcomeMessage();

    int choice = getGameMode();
    if (choice == 4) {
        cout << "Exiting game. Goodbye!" << endl;
        return 0;
    }

    const int boardSize = 10;
    auto board1 = make_shared<Board>(boardSize);
    auto board2 = make_shared<Board>(boardSize);

    unique_ptr<Player> player1;
    unique_ptr<Player> player2;

    // Oyun rejiminin seçilməsi
    switch (choice) {
    case 1:  // İnsan vs İnsan
        player1 = make_unique<HumanPlayer>(*board1);
        player2 = make_unique<HumanPlayer>(*board2);
        break;
    case 2:  // İnsan vs Kompüter
        player1 = make_unique<HumanPlayer>(*board1);
        player2 = make_unique<ComputerPlayer>(*board2);
        break;
    case 3:  // Kompüter vs Kompüter (simulyasiya)
        player1 = make_unique<ComputerPlayer>(*board1);
        player2 = make_unique<ComputerPlayer>(*board2);
        cout << "Running simulation..." << endl;
        break;
    }

    // Oyunu başlat
    Game game(player1.get(), player2.get());
    game.start();

    cout << "====================================" << endl;
    cout << "       THANKS FOR PLAYING!          " << endl;
    cout << "====================================" << endl;

    return 0;
}