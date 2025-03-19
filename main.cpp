#include <iostream>
#include "clui.h"
#include <ctime>
#include <fstream>
#include "enums.h"
//@copyright && @author Shayan DashtiZad

using namespace std;
const int rows = get_window_rows();
const int columns = get_window_cols();


struct Game {
    Level game_level;
    int const_sudoku[9][9];
    int play_sudoku[9][9];
    int complete_sudoku[9][9];
    int remaining_time;
    int right = 0;
    int wrong = 0;
    int score = right - wrong;
};

struct Player {
    string username;
    Game game;
    int wins = 0;
    int loses = 0;
    bool has_saved_game = false;
    int total_game_play_time = 0;
    int score = 0;
};

/**
 * Save all the players
 */
void save_players(Player *players, int length) {
    ofstream outfile("saved_player.dat", std::ios::binary);
    if (outfile.is_open()) {
        for (int i = 0; i < length; ++i) {
            outfile.write(players[i].username.c_str(), players[i].username.size() + 1);
            outfile.write(reinterpret_cast<char *>(&players[i].wins), sizeof(int));
            outfile.write(reinterpret_cast<char *>(&players[i].loses), sizeof(int));
            outfile.write(reinterpret_cast<char *>(&players[i].has_saved_game), sizeof(bool));
            outfile.write(reinterpret_cast<char *>(&players[i].total_game_play_time), sizeof(int));
            outfile.write(reinterpret_cast<char *>(&players[i].score), sizeof(int));
        }
        outfile.close();
    } else {
        cerr << "Error opening file for writing!" << endl;
    }
}

/**
 * Load all the Players
 */
void load_players(Player *players, int length) {
    ifstream infile("saved_player.dat", std::ios::binary);
    if (infile.is_open()) {
        for (int i = 0; i < length; ++i) {
            char buffer[256];
            infile.getline(buffer, 256, '\0');
            players[i].username = string(buffer);
            infile.read(reinterpret_cast<char *>(&players[i].wins), sizeof(int));
            infile.read(reinterpret_cast<char *>(&players[i].loses), sizeof(int));
            infile.read(reinterpret_cast<char *>(&players[i].has_saved_game), sizeof(bool));
            infile.read(reinterpret_cast<char *>(&players[i].total_game_play_time), sizeof(int));
            infile.read(reinterpret_cast<char *>(&players[i].score), sizeof(int));
        }
        infile.close();
    } else {
        cerr << "Error opening file for reading!" << endl;
    }
}

/**
 * Load all the saved games to the players that had saved game
 */
void load_games(Player *players, int players_index) {
    ifstream infile("saved_game.dat", std::ios::binary);
    if (infile.is_open()) {
        for (int i = 0; i < players_index; ++i) {
            if (players[i].has_saved_game) {
                infile.read(reinterpret_cast<char *>(&players[i].game), sizeof(Game));
            }
        }
        infile.close();
    } else {
        cerr << "Error opening file for reading!" << endl;
    }
}

/**
 * Save all the games that player has_saved_game is true
 */
void save_games(Player *players, int players_index) {
    ofstream outfile("saved_game.dat", std::ios::binary);
    if (outfile.is_open()) {
        for (int i = 0; i < players_index; ++i) {
            if (players[i].has_saved_game) {
                outfile.write(reinterpret_cast<char *>(&players[i].game), sizeof(Game));
            }
        }
        outfile.close();
    } else {
        cerr << "Error opening file for writing!" << endl;
    }
}

/**
 * save the players_index
 */
void saveLength(int players_index) {
    std::ofstream outfile("length.dat", std::ios::binary);
    if (outfile.is_open()) {
        outfile.write(reinterpret_cast<char *>(&players_index), sizeof(int));
        outfile.close();
    } else {
        std::cerr << "Error opening file for writing!" << std::endl;
    }
}

/**
 * return the players_index loaded from the file
 */
int loadLength() {
    int players_index = 0;
    std::ifstream infile("length.dat", std::ios::binary);

    if (infile.is_open()) {
        // Check if the file is empty
        infile.seekg(0, std::ios::end);
        if (infile.tellg() == 0) {
            std::cout << "File is empty." << std::endl;
            infile.close();
            return players_index; // Return the default 0 if empty
        }
        infile.seekg(0, std::ios::beg); // Reset to start

        infile.read(reinterpret_cast<char *>(&players_index), sizeof(int));
        if (infile.fail()) {
            std::cerr << "Error reading from file or file smaller than int!" << std::endl;
            players_index = 0; //set back to zero if fail to read
        }
        infile.close();
    } else {
        std::cerr << "Error opening file for reading!" << std::endl;
    }
    return players_index;
}

/**
 * sort players base on their scores
 */
void sort_players(Player *players, int players_index) {
    for (int i = 0; i < players_index - 1; ++i) {
        for (int j = 0; j < players_index - i - 1; ++j) {
            if (players[j].score == players[j + 1].score) {
                if (players[j].total_game_play_time > players[j + 1].total_game_play_time) {
                    Player temp = players[j];
                    players[j] = players[j + 1];
                    players[j + 1] = temp;
                }
            } else if (players[j].score < players[j + 1].score) {
                Player temp = players[j];
                players[j] = players[j + 1];
                players[j + 1] = temp;
            }
        }
    }
}

/**
 * show leaderboard
 */
void show_leaderboard(Player *players, int players_index) {
    clear_screen();
    sort_players(players, players_index);
    cursor_to_pos(rows * 2 / 10, columns * 25 / 100);
    cout << "Name               |   Score    |   Total Time    | Right Moves | Wrong Moves | Loses | Wins" << endl;
    for (int i = 0; i < players_index; ++i) {
        cursor_to_pos(rows * 2 / 10 + (i * 2) + 1, columns * 25 / 100 - 3);
        printf("%d. %-18s | %-10d | %7d seconds | %11d | %11d | %5d | %d ", i + 1, players[i].username.c_str(),
               players[i].score,
               players[i].game.remaining_time,
               players[i].game.right, players[i].game.wrong,
               players[i].loses, players[i].wins);
    }
}

/**
 * calculate the score base on time and level and put it for the player
 */
void calculate_score(Player &player) {
    int score = 0;
    if (player.game.game_level == Easy)
        score = 1.25 * (player.game.remaining_time) + player.game.score;
    else if (player.game.game_level == Normal)
        score = 1.5 * (player.game.remaining_time) + player.game.score;
    else
        score = 2 * (player.game.remaining_time) + player.game.score;

    player.score = score;
}

/**
 * Print the sudoku game that is passed to it
 */
void print_sudoku(int sudoku[9][9]) {
    change_color_rgb(40, 237, 201);
    clear_screen();
    cout << "  -----------------------" << endl;
    for (int i = 0; i < 9; ++i) {
        cout << " | ";
        for (int j = 0; j < 9; ++j) {
            if (sudoku[i][j] == 0) {
                cout << "O ";
            } else {
                cout << sudoku[i][j] << " ";
            }
            if ((j + 1) % 3 == 0) {
                cout << "| ";
            }
        }
        cout << endl;
        if ((i + 1) % 3 == 0) {
            cout << "  -----------------------" << endl;
        }
    }
}

/**
 * Check if the move is valid or not
 */
bool is_valid_move(int sudoku[9][9], int row, int col, int num) {
    // Check row
    for (int i = 0; i < 9; ++i) {
        if (sudoku[row][i] == num) return false;
    }
    // Check column
    for (int i = 0; i < 9; ++i) {
        if (sudoku[i][col] == num) return false;
    }
    // Check 3x3 subgrid
    int startRow = row - row % 3;
    int startCol = col - col % 3;
    for (int i = startRow; i < startRow + 3; ++i) {
        for (int j = startCol; j < startCol + 3; ++j) {
            if (sudoku[i][j] == num) return false;
        }
    }
    return true;
}

/**
 * Check if the play sudoku is correctly completed or not
 */
bool is_complete(int play_sudoku[9][9], int complete_sudoku[9][9]) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (play_sudoku[i][j] != complete_sudoku[i][j])
                return false;
        }
    }
    return true;
}

/**
 * Print the first page when someone execute the game and ask for a username
 * then return the username
 */
string print_start_page() {
    clear_screen();
    string username;
    cursor_to_pos(rows * 3 / 10, columns * 4 / 10);
    cout << "Enter Your Username:" << endl;
    change_color_rgb(185, 224, 27);
    cursor_to_pos(rows * 3 / 10 + 1, columns * 4 / 10);
    getline(cin, username);
    reset_color();
    return username;
}

/**
 * check username and if it wasn't exist add it to the players list
 */
bool check_username(string username, Player *players, int &players_index) {
    for (int i = 0; i < players_index; ++i) {
        if (username == players[i].username) {
            return true;
        }
    }
    players[players_index++].username = username;
    return false;
}

/**
 * Print the menu and get player's option
 */
string print_menu(string username) {
    clear_screen();
    change_color_rgb(185, 69, 230);
    cursor_to_pos(rows * 2 / 10 - 1, columns * 48 / 100);
    cout << "Hi " << username << endl;
    change_color_rgb(27, 166, 68);
    cursor_to_pos(rows * 2 / 10, columns * 45 / 100);
    cout << "Welcome To Sudoku By Shayan DashtiZad" << endl;
    change_color_rgb(230, 97, 69);
    cursor_to_pos(rows * 4 / 10, columns * 5 / 100);
    cout << "1: Start A New Game" << endl;
    cursor_to_pos(rows * 4 / 10 + 1, columns * 5 / 100);
    cout << "2: Play A Saved Game" << endl;
    cursor_to_pos(rows * 4 / 10 + 2, columns * 5 / 100);
    cout << "3: Leaderboard" << endl;
    cursor_to_pos(rows * 4 / 10 + 3, columns * 5 / 100);
    cout << "4: Exit The Game" << endl;
    cursor_to_pos(rows * 4 / 10 + 5, columns * 10 / 100);
    cout << "Choose :" << endl;
    string option;
    cursor_to_pos(rows * 4 / 10 + 5, columns * 10 / 100 + 9);
    getline(cin, option);
    return option;
}

/**
 * Explain the username is already used
 * and after 5 seconds return the user to the first page
 */
void print_username_already_used(string username) {
    clear_screen();
    cout << "The Username " << username << " is already in use." << endl;
    cout << "Please choose another username." << endl;
    for (int i = 5; i > 0; --i) {
        cursor_to_pos(rows * 35 / 100, columns * 3 / 10);
        cout << "You will be sent back to first page after { " << i << " } seconds" << endl;
        delay(1000);
    }
}

/**
 * Explain the option is not valid
 * and after 5 seconds return the user to choose a valid option
 */
void print_option_is_not_valid() {
    clear_screen();
    cout << "The Option You have Chosen Is Not Valid" << endl;
    for (int i = 5; i > 0; --i) {
        cursor_to_pos(rows * 35 / 100, columns * 3 / 10);
        cout << "You will be sent back to choose a valid option after { " << i << " } seconds" << endl;
        delay(1000);
    }
}

/**
 * generate sudoku for the player
 */
void generate_sudoku(Player &player) {
    int grids[15][9][9] = {
            {
                    {5, 3, 4, 6, 7, 8, 9, 1, 2},
                    {6, 7, 2, 1, 9, 5, 3, 4, 8},
                    {1, 9, 8, 3, 4, 2, 5, 6, 7},
                    {8, 5, 9, 7, 6, 1, 4, 2, 3},
                    {4, 2, 6, 8, 5, 3, 7, 9, 1},
                    {7, 1, 3, 9, 2, 4, 8, 5, 6},
                    {9, 6, 1, 5, 3, 7, 2, 8, 4},
                    {2, 8, 7, 4, 1, 9, 6, 3, 5},
                    {3, 4, 5, 2, 8, 6, 1, 7, 9}
            },
            {
                    {8, 3, 5, 4, 1, 6, 9, 2, 7},
                    {2, 9, 7, 8, 5, 3, 4, 6, 1},
                    {4, 6, 1, 2, 7, 9, 8, 3, 5},
                    {1, 7, 6, 9, 3, 4, 5, 8, 2},
                    {9, 5, 3, 6, 8, 2, 1, 7, 4},
                    {2, 4, 8, 1, 2, 7, 3, 5, 6},
                    {7, 8, 2, 3, 9, 5, 6, 1, 4},
                    {3, 1, 9, 7, 4, 8, 2, 5, 6},
                    {5, 2, 4, 1, 6, 7, 3, 8, 9}
            },
            {
                    {1, 2, 3, 4, 5, 6, 7, 8, 9},
                    {4, 5, 6, 7, 8, 9, 1, 2, 3},
                    {7, 8, 9, 1, 2, 3, 4, 5, 6},
                    {2, 3, 1, 5, 6, 4, 8, 9, 7},
                    {5, 6, 4, 8, 9, 7, 2, 3, 1},
                    {8, 9, 7, 2, 3, 1, 5, 6, 4},
                    {3, 1, 2, 6, 4, 5, 9, 7, 8},
                    {6, 4, 5, 9, 7, 8, 3, 1, 2},
                    {9, 7, 8, 3, 1, 2, 6, 4, 5}
            },
            {
                    {9, 1, 4, 3, 6, 8, 2, 7, 5},
                    {3, 6, 8, 2, 7, 5, 9, 4, 1},
                    {2, 7, 5, 9, 1, 4, 3, 6, 8},
                    {5, 4, 2, 6, 3, 9, 1, 8, 7},
                    {6, 8, 3, 1, 4, 7, 5, 9, 2},
                    {1, 9, 7, 5, 8, 2, 6, 3, 4},
                    {4, 5, 6, 8, 2, 3, 7, 1, 9},
                    {8, 2, 1, 7, 9, 6, 4, 5, 3},
                    {7, 3, 9, 4, 5, 1, 8, 2, 6}
            },
            {
                    {4, 8, 3, 9, 2, 1, 5, 7, 6},
                    {9, 2, 6, 7, 4, 5, 3, 1, 8},
                    {7, 1, 5, 6, 3, 8, 9, 4, 2},
                    {6, 3, 4, 1, 7, 9, 2, 8, 5},
                    {1, 7, 8, 2, 5, 6, 4, 9, 3},
                    {2, 5, 9, 8, 4, 3, 7, 6, 1},
                    {8, 4, 7, 3, 6, 2, 1, 5, 9},
                    {3, 9, 1, 5, 8, 4, 6, 2, 7},
                    {5, 6, 2, 4, 9, 7, 8, 3, 1}
            },
            {
                    {6, 1, 7, 2, 8, 9, 5, 3, 4},
                    {9, 4, 5, 3, 6, 1, 7, 2, 8},
                    {2, 8, 3, 5, 4, 7, 1, 6, 9},
                    {4, 2, 1, 6, 7, 3, 8, 9, 5},
                    {3, 5, 8, 9, 1, 2, 4, 7, 6},
                    {7, 9, 6, 4, 5, 8, 2, 1, 3},
                    {1, 3, 4, 7, 9, 6, 5, 8, 2},
                    {5, 6, 2, 8, 3, 4, 9, 7, 1},
                    {8, 7, 9, 1, 2, 5, 6, 4, 3}
            },
            {
                    {7, 3, 8, 9, 1, 2, 5, 4, 6},
                    {1, 9, 5, 3, 6, 4, 8, 7, 2},
                    {4, 2, 6, 7, 5, 8, 9, 3, 1},
                    {8, 7, 1, 2, 4, 9, 6, 5, 3},
                    {3, 6, 4, 5, 8, 1, 7, 2, 9},
                    {9, 5, 2, 6, 3, 7, 4, 1, 8},
                    {2, 4, 9, 1, 7, 5, 3, 6, 8},
                    {5, 8, 7, 4, 2, 3, 1, 9, 6},
                    {6, 1, 3, 8, 9, 6, 2, 8, 7}
            },
            {
                    {1, 2, 3, 7, 8, 9, 4, 5, 6},
                    {4, 5, 6, 1, 2, 3, 7, 8, 9},
                    {7, 8, 9, 4, 5, 6, 1, 2, 3},
                    {3, 1, 2, 6, 4, 5, 9, 7, 8},
                    {6, 4, 5, 9, 7, 8, 3, 1, 2},
                    {9, 7, 8, 3, 1, 2, 6, 4, 5},
                    {2, 3, 1, 5, 6, 4, 8, 9, 7},
                    {5, 6, 4, 8, 9, 7, 2, 3, 1},
                    {8, 9, 7, 2, 3, 1, 5, 6, 4}
            },
            {
                    {6, 4, 8, 9, 1, 2, 5, 7, 3},
                    {2, 9, 7, 5, 6, 3, 8, 1, 4},
                    {5, 3, 1, 7, 8, 4, 2, 6, 9},
                    {7, 6, 2, 4, 5, 9, 3, 8, 1},
                    {9, 8, 3, 1, 2, 7, 6, 4, 5},
                    {4, 1, 5, 6, 3, 8, 7, 9, 2},
                    {1, 2, 9, 8, 4, 5, 7, 3, 6},
                    {3, 7, 6, 2, 9, 1, 4, 5, 8},
                    {8, 5, 4, 3, 7, 6, 1, 2, 9}
            },
            {
                    {5, 3, 1, 7, 8, 2, 6, 4, 9},
                    {7, 2, 6, 4, 1, 9, 3, 8, 5},
                    {8, 4, 9, 3, 5, 6, 7, 2, 1},
                    {3, 1, 5, 8, 7, 4, 9, 6, 2},
                    {6, 8, 7, 2, 9, 5, 4, 1, 3},
                    {4, 9, 2, 6, 3, 1, 8, 5, 7},
                    {1, 7, 8, 5, 4, 3, 2, 9, 6},
                    {2, 6, 3, 9, 8, 7, 1, 5, 4},
                    {9, 5, 4, 1, 6, 2, 3, 7, 8}
            },
            {
                    {9, 5, 1, 4, 3, 8, 6, 7, 2},
                    {8, 7, 2, 6, 1, 5, 9, 4, 3},
                    {4, 6, 3, 9, 2, 7, 8, 1, 5},
                    {1, 3, 9, 5, 8, 4, 7, 2, 6},
                    {7, 2, 8, 1, 6, 9, 4, 3, 5},
                    {6, 4, 5, 2, 7, 3, 1, 8, 9},
                    {2, 9, 6, 8, 4, 1, 5, 3, 7},
                    {5, 8, 7, 3, 9, 6, 2, 1, 4},
                    {3, 1, 4, 7, 5, 2, 6, 9, 8}
            },
            {
                    {8, 9, 6, 5, 3, 7, 2, 4, 1},
                    {3, 2, 5, 1, 4, 8, 9, 7, 6},
                    {1, 4, 7, 6, 9, 2, 3, 8, 5},
                    {9, 7, 4, 8, 6, 1, 5, 3, 2},
                    {2, 6, 3, 9, 5, 4, 1, 8, 7},
                    {5, 8, 1, 2, 7, 3, 6, 9, 4},
                    {7, 1, 9, 3, 8, 6, 4, 5, 2},
                    {4, 3, 2, 7, 1, 5, 8, 6, 9},
                    {6, 5, 8, 4, 2, 9, 7, 1, 3}
            },
            {
                    {5, 2, 8, 6, 9, 4, 1, 7, 3},
                    {1, 6, 9, 8, 7, 3, 2, 4, 5},
                    {7, 3, 4, 2, 1, 5, 6, 8, 9},
                    {2, 9, 5, 4, 6, 7, 3, 1, 8},
                    {4, 8, 1, 3, 2, 9, 7, 5, 6},
                    {6, 7, 3, 5, 8, 1, 9, 2, 4},
                    {9, 4, 7, 1, 5, 2, 8, 3, 6},
                    {3, 1, 6, 7, 4, 8, 5, 9, 2},
                    {8, 5, 2, 9, 3, 6, 4, 1, 7}
            },
            {
                    {1, 6, 8, 3, 2, 7, 4, 5, 9},
                    {2, 9, 4, 5, 1, 8, 6, 3, 7},
                    {3, 5, 7, 9, 4, 6, 1, 2, 8},
                    {6, 1, 3, 7, 5, 2, 9, 8, 4},
                    {4, 8, 9, 1, 6, 3, 7, 2, 5},
                    {7, 2, 5, 4, 8, 9, 3, 6, 1},
                    {9, 3, 2, 6, 7, 4, 5, 1, 8},
                    {5, 7, 6, 8, 9, 1, 2, 4, 3},
                    {8, 4, 1, 2, 3, 5, 7, 9, 6}
            },
            {
                    {4, 2, 6, 8, 5, 7, 3, 9, 1},
                    {9, 3, 5, 1, 6, 4, 7, 2, 8},
                    {7, 8, 1, 2, 3, 9, 5, 6, 4},
                    {2, 5, 4, 3, 9, 8, 1, 7, 6},
                    {6, 1, 7, 4, 2, 5, 8, 3, 9},
                    {8, 9, 3, 6, 7, 1, 2, 4, 5},
                    {5, 7, 9, 6, 1, 3, 4, 8, 2},
                    {3, 4, 8, 5, 2, 6, 9, 1, 7},
                    {1, 6, 2, 7, 8, 4, 6, 5, 3}
            }
    };


    srand(time(0));
    int randomIndex = rand() % 15;
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            player.game.complete_sudoku[i][j] = grids[randomIndex][i][j];
            player.game.const_sudoku[i][j] = grids[randomIndex][i][j];
        }
    }

    srand(time(0));


    for (int k = 0; k < 50; ++k) {
        int row = rand() % 9;
        int col = rand() % 9;
        if (player.game.const_sudoku[row][col] != 0) {
            player.game.const_sudoku[row][col] = 0;
        } else
            k--;
    }

    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            player.game.play_sudoku[i][j] = player.game.const_sudoku[i][j];
        }
    }
}

/**
 * Start a new game , first get the difficulty level then generate a sudoku to play
 */
void start_new_game(Player &player) {
    clear_screen();
    string level_option;
    Level level;
    while (true) {
        cursor_to_pos(rows * 2 / 10, columns * 4 / 10);
        cout << "choose a level:" << endl;
        cursor_to_pos(rows * 3 / 10, columns * 4 / 10);
        cout << "1: Easy" << endl;
        cursor_to_pos(rows * 3 / 10 + 1, columns * 4 / 10);
        cout << "2: Normal" << endl;
        cursor_to_pos(rows * 3 / 10 + 2, columns * 4 / 10);
        cout << "3: Hard" << endl;
        cursor_to_pos(rows * 3 / 10 + 3, columns * 4 / 10);
        cout << "Choose : ";
        cursor_to_pos(rows * 3 / 10 + 3, columns * 4 / 10 + 9);
        getline(cin, level_option);

        if (level_option == "1") {
            level = Easy;
            break;
        } else if (level_option == "2") {
            level = Normal;
            break;
        } else if (level_option == "3") {
            level = Hard;
            break;
        } else {
            print_option_is_not_valid();
        }
    }

    player.game.game_level = level;
    generate_sudoku(player);
    if (level == Easy) {
        player.game.remaining_time = player.game.remaining_time + 300;
        player.total_game_play_time += 300;
    } else if (level == Normal) {
        player.game.remaining_time = player.game.remaining_time + 180;
        player.total_game_play_time += 180;
    } else {
        player.game.remaining_time = player.game.remaining_time + 120;
        player.total_game_play_time += 120;
    }
    player.has_saved_game = false;
}

/**
 * find the index of the player via username
 * @return
 */
int find_player_via_username(Player *players, string username, int players_index) {
    for (int i = 0; i < players_index; ++i) {
        if (players[i].username == username)
            return i;
    }
    return -1;
}

/**
 * clear the screen and explain the option for the user
 */
void print_pause_menu() {
    clear_screen();
    cursor_to_pos(rows * 3 / 10, columns * 3 / 10);
    cout << "Pause Menu Press ESC to Continue the Game or S to Save the game" << endl;
}

/**
 * clear the screen and explain the user that it hasn't a saved game
 */
void print_has_not_save_game() {
    clear_screen();
    cursor_to_pos(rows * 3 / 10, columns * 3 / 10);
    cout << "You don't Have a Saved Game" << endl;
    delay(3500);
}

/**
 * after finishing the game show the result
 */
void print_end_game(bool win, Player &player) {
    clear_screen();
    cursor_to_pos(rows * 3 / 10, columns * 4 / 10);
    if (win) {
        change_color_rgb(40, 237, 53);
        cout << "Wow Congratulations! You won the game!";
    } else {
        change_color_rgb(102, 53, 69);
        cout << "Time is up! You lost the game.";
    }
    cursor_to_pos(rows * 4 / 10, columns * 15 / 100);
    cout << "Name               |   Score    |    Time     | Right Moves | Wrong Moves" << endl;
    cursor_to_pos(rows * 4 / 10 + 1, columns * 15 / 100 - 3);
    printf("%-18s | %10d | %3d seconds | %d | %d", player.username.c_str(), player.score, player.game.remaining_time,
           player.game.right, player.game.wrong);
}

/**
 * Saves everything and exit the game.
 */
void save(Player *players, int players_index) {
    saveLength(players_index);
    save_players(players, players_index);
    save_games(players, players_index);
}

/**
 * start the game and show the sudoku to play
 * show the remaining time to user
 * evaluate the answers and count the score
 */
void play_game(Player &player, Player *players, int players_index) {
    int x = 0, y = 0;
    time_t start_time = time(nullptr);
    clear_screen();
    print_sudoku(player.game.play_sudoku);
    bool is_save = false;
    while (player.game.remaining_time > 0 && !is_save) {
        time_t current_time = time(nullptr);
        int time_elapsed = (int) difftime(current_time, start_time);
        player.game.remaining_time -= time_elapsed;
        start_time = current_time;

        cursor_to_pos(2 + y + (y / 3), 4 + (x * 2) + (2 * (x / 3)));

        change_color_rgb(230, 55, 100);
        printf("%d", player.game.play_sudoku[y][x]);
        reset_color();
        cursor_backward(1);

        flush();

        if (is_keyboard_hit()) {
            int key = getch();

            if (key == 224) {
                // Arrow keys
                key = getch();
                switch (key) {
                    case 72: // Up arrow
                        if (y > 0)
                            y--;
                        break;
                    case 80: // Down arrow
                        if (y < 8)
                            y++;
                        break;
                    case 75: // Left arrow
                        if (x > 0)
                            x--;
                        break;
                    case 77: // Right arrow
                        if (x < 8)
                            x++;
                        break;
                }
            } else if (key >= '1' && key <= '9') {
                int num = key - '0';
                if (player.game.const_sudoku[y][x] == 0) {
                    if (is_valid_move(player.game.play_sudoku, y, x, num)) {
                        player.game.play_sudoku[y][x] = num;
                        if (player.game.play_sudoku[y][x] == player.game.complete_sudoku[y][x]) {
                            player.game.right++;
                        } else {
                            player.game.wrong++;
                        }
                    } else {
                        //Invalid Move
                        cursor_to_pos(rows * 7 / 10, columns * 8 / 10);
                        change_color_rgb(200, 10, 10);
                        cout << "Wrong Moves" << endl;
                        delay(1000);
                        cursor_to_pos(rows * 7 / 10, columns * 8 / 10);
                        cout << "           " << endl;
                        reset_color();
                    }
                } else {
                    //Invalid Move in Const
                    cursor_to_pos(rows * 7 / 10, columns * 8 / 10);
                    change_color_rgb(200, 10, 10);
                    cout << "It's not Possible to change" << endl;
                    delay(1000);
                    cursor_to_pos(rows * 7 / 10, columns * 8 / 10);
                    cout << "                           " << endl;
                    reset_color();
                }
            } else if (key == 27) {
                print_pause_menu();
                while (true) {
                    int new_key = getch();
                    if (new_key == 27) {
                        time_t current_time = time(nullptr);
                        start_time = current_time;
                        break;
                    }
                    if (new_key == 115) {
                        //sort_players(players, players_index);
                        player.has_saved_game = true;
                        //save(players, players_index);
                        is_save = true;
                        break;
                    }
                }
            }
            print_sudoku(player.game.play_sudoku);
        }

        if (is_complete(player.game.play_sudoku, player.game.complete_sudoku)) {
            print_end_game(true, player);
            reset_color();
            player.wins++;
            player.game.score = player.game.right - player.game.wrong;
            calculate_score(player);
            delay(3000);
            break;
        }
        if (player.game.wrong > 5)break;

        cursor_to_pos(rows * 9 / 10, columns * 3 / 10);
        cout << "Time Remaining: " << player.game.remaining_time << " seconds" << endl;
        player.game.score = player.game.right - player.game.wrong;
        //cursor_to_pos(rows * 9 / 10 + 1, columns * 3 / 10);
        //cout << "Score: " << player.game.score << " Hit" << endl;
    }

    if (player.game.remaining_time <= 0 || player.game.wrong > 5) {
        print_end_game(false, player);
        player.loses++;
        player.total_game_play_time -= player.game.remaining_time;
        delay(10000);
    }
}

/**
 * load players and link their games to them if they has saved game
 */
void initial_game(Player *players, int players_index) {
    load_players(players, players_index);
    load_games(players, players_index);
}

int main() {
    int players_index = 0;
    players_index = loadLength();
    Player *players = new Player[100];
    initial_game(players, players_index);
    string username;
    while (true) {
        username = print_start_page();
        bool check = check_username(username, players, players_index);
        if (!check) {
            break;
        }
        print_username_already_used(username);
    }
    while (true) {
        string option;
        while (true) {
            option = print_menu(username);
            if (option == "1" || option == "2" || option == "3") {
                break;
            }
            if (option == "4") {
                sort_players(players, players_index);
                save(players, players_index);
                delete[] players;
                quit();
            }
            print_option_is_not_valid();
        }
        if (option == "1") {
            start_new_game(players[find_player_via_username(players, username, players_index)]);
            play_game(players[find_player_via_username(players, username, players_index)], players, players_index);
        } else if (option == "2") {
            if (players[find_player_via_username(players, username, players_index)].has_saved_game) {
                play_game(players[find_player_via_username(players, username, players_index)], players, players_index);
            } else {
                print_has_not_save_game();
            }
        } else {
            show_leaderboard(players, players_index);
            getch();
            flush();
            while (true) {
                int new_key = getch();
                if (new_key == 109) {
                    break;
                }
            }
            delay(2000);
        }
    }
    return 0;
}
