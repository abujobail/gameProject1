               
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <vector>
#include <map>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

using namespace std;

const int CELL_SIZE = 60;
const int BOARD_PIXEL_SIZE = 600;
const int WINDOW_HEIGHT = BOARD_PIXEL_SIZE + 150; // extra space for dice/UI/turn

// Snakes and ladders
map<int,int> snakes = { {17,7},{54,34},{62,19},{87,36},{64,60},{93,73},{95,75},{98,79} };
map<int,int> ladders = { {1,38},{4,14},{9,31},{28,84},{21,42},{51,67},{80,99},{72,91} };

// Get pixel position of board cell
sf::Vector2f getPosition(int pos) {
    if(pos < 1) pos = 1;
    if(pos > 100) pos = 100;
    int row = (pos-1)/10;
    int col = (pos-1)%10;
    if(row % 2 == 1) col = 9 - col; // zig-zag
    int x = col * CELL_SIZE;
    int y = (9 - row) * CELL_SIZE;
    return sf::Vector2f(x, y);
}

// Roll dice (1-6)
int rollDice() {
    return rand() % 6 + 1;
}

int main() {
    srand(time(0));

    sf::RenderWindow window(sf::VideoMode(BOARD_PIXEL_SIZE, WINDOW_HEIGHT), "Snake & Ladder");

    // Load board
    sf::Texture boardTexture;
    if(!boardTexture.loadFromFile("assets/board.png")) {
        cout << "Failed to load board.png" << endl;
        return 1;
    }
    sf::Sprite boardSprite(boardTexture);
    boardSprite.setScale((float)BOARD_PIXEL_SIZE/boardTexture.getSize().x, (float)BOARD_PIXEL_SIZE/boardTexture.getSize().y);

    // Load dice textures
    vector<sf::Texture> diceTextures(6);
    for(int i=0;i<6;i++){
        if(!diceTextures[i].loadFromFile("assets/dice" + to_string(i+1) + ".png")){
            cout << "Failed to load dice" << i+1 << ".png" << endl;
            return 1;
        }
    }

    sf::Sprite diceSprite(diceTextures[0]);
    diceSprite.setScale((float)CELL_SIZE/60, (float)CELL_SIZE/60); // dice size 60x60
    diceSprite.setPosition(10, BOARD_PIXEL_SIZE + 40);

    // Player pieces with contrasting colors and outline
    sf::CircleShape player1(CELL_SIZE/3);
    player1.setFillColor(sf::Color::Blue);   
    player1.setOutlineThickness(2);
    player1.setOutlineColor(sf::Color::Black);

    sf::CircleShape player2(CELL_SIZE/3);
    player2.setFillColor(sf::Color(200,0,200)); // Magenta / Purple
    player2.setOutlineThickness(2);
    player2.setOutlineColor(sf::Color::Black);

    int pos1 = 1, pos2 = 1;
    player1.setPosition(getPosition(pos1));
    player2.setPosition(getPosition(pos2) + sf::Vector2f(CELL_SIZE/2,0)); // offset to avoid overlap

    bool turnPlayer1 = true;
    bool rolling = false;
    int diceValue = 0;

    // Load font
    sf::Font font;
    if(!font.loadFromFile("/System/Library/Fonts/Supplemental/Arial.ttf")){
        cout << "Failed to load font" << endl;
        return 1;
    }

    // Info text (dice result)
    sf::Text infoText;
    infoText.setFont(font);
    infoText.setCharacterSize(20);
    infoText.setFillColor(sf::Color::Black);
    infoText.setPosition(10, BOARD_PIXEL_SIZE + 100);
    infoText.setString("Press SPACE to roll the dice");

    // Turn indicator
    sf::Text turnText;
    turnText.setFont(font);
    turnText.setCharacterSize(24);
    turnText.setFillColor(sf::Color::Black);
    turnText.setPosition(10, BOARD_PIXEL_SIZE + 10);
    turnText.setString("Player 1's Turn");

    while(window.isOpen()){
        sf::Event event;
        while(window.pollEvent(event)){
            if(event.type == sf::Event::Closed)
                window.close();

            // Roll dice with animation
            if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space && !rolling){
                rolling = true;

                // Dice animation: spin 10 random dice quickly
                for(int i=0;i<10;i++){
                    int tempDice = rollDice();
                    diceSprite.setTexture(diceTextures[tempDice-1]);
                    window.clear(sf::Color::White);
                    window.draw(boardSprite);
                    window.draw(player1);
                    window.draw(player2);
                    window.draw(diceSprite);
                    window.draw(infoText);
                    window.draw(turnText);
                    window.display();
                    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 100ms delay
                }

                // Final dice value
                diceValue = rollDice();
                diceSprite.setTexture(diceTextures[diceValue-1]);
                infoText.setString("Dice: " + to_string(diceValue));

                // Move current player
                if(turnPlayer1){
                    pos1 += diceValue;
                    if(pos1 > 100) pos1 = 100;
                    if(snakes.find(pos1) != snakes.end()) pos1 = snakes[pos1];
                    else if(ladders.find(pos1) != ladders.end()) pos1 = ladders[pos1];
                    player1.setPosition(getPosition(pos1));
                } else {
                    pos2 += diceValue;
                    if(pos2 > 100) pos2 = 100;
                    if(snakes.find(pos2) != snakes.end()) pos2 = snakes[pos2];
                    else if(ladders.find(pos2) != ladders.end()) pos2 = ladders[pos2];
                    player2.setPosition(getPosition(pos2) + sf::Vector2f(CELL_SIZE/2,0));
                }

                // Check for win
                if(pos1 == 100) turnText.setString("Player 1 wins! Press ESC to exit.");
                else if(pos2 == 100) turnText.setString("Player 2 wins! Press ESC to exit.");
                else turnText.setString(turnPlayer1 ? "Player 2's Turn" : "Player 1's Turn");

                turnPlayer1 = !turnPlayer1;
                rolling = false;
            }

            if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape){
                window.close();
            }
        }

        window.clear(sf::Color::White);
        window.draw(boardSprite);
        window.draw(player1);
        window.draw(player2);
        window.draw(diceSprite);
        window.draw(infoText);
        window.draw(turnText);
        window.display();
    }

    return 0;
}

                //CODE 1


// #include <SFML/Graphics.hpp>
// #include <SFML/Window.hpp>
// #include <iostream>
// #include <vector>
// #include <map>
// #include <cstdlib>
// #include <ctime>

// using namespace std;

// const int CELL_SIZE = 60;
// const int BOARD_SIZE = 10;
// const int BOARD_PIXEL_SIZE = 600;
// const int WINDOW_HEIGHT = BOARD_PIXEL_SIZE + 120; // extra space for dice/UI

// // Snakes and ladders
// map<int,int> snakes = { {17,7},{54,34},{62,13},{87,24},{64,60},{93,73},{95,75},{98,79} };
// map<int,int> ladders = { {2,38},{4,14},{9,30},{28,84},{21,42},{51,67},{78,97},{71,91} };

// // Get pixel position of board cell
// sf::Vector2f getPosition(int pos) {
//     if(pos < 1) pos = 1;
//     if(pos > 100) pos = 100;
//     int row = (pos-1)/10;
//     int col = (pos-1)%10;
//     if(row % 2 == 1) col = 9 - col; // zig-zag
//     int x = col * CELL_SIZE;
//     int y = (9 - row) * CELL_SIZE;
//     return sf::Vector2f(x, y);
// }

// // Roll dice (1-6)
// int rollDice() {
//     return rand() % 6 + 1;
// }

// int main() {
//     srand(time(0));

//     sf::RenderWindow window(sf::VideoMode(BOARD_PIXEL_SIZE, WINDOW_HEIGHT), "Snake & Ladder");

//     // Load board
//     sf::Texture boardTexture;
//     if(!boardTexture.loadFromFile("assets/board.png")) {
//         cout << "Failed to load board.png" << endl;
//         return 1;
//     }
//     sf::Sprite boardSprite(boardTexture);
//     boardSprite.setScale((float)BOARD_PIXEL_SIZE/boardTexture.getSize().x, (float)BOARD_PIXEL_SIZE/boardTexture.getSize().y);

//     // Load dice textures
//     vector<sf::Texture> diceTextures(6);
//     for(int i=0;i<6;i++){
//         if(!diceTextures[i].loadFromFile("assets/dice" + to_string(i+1) + ".png")){
//             cout << "Failed to load dice" << i+1 << ".png" << endl;
//             return 1;
//         }
//     }

//     sf::Sprite diceSprite(diceTextures[0]);
//     diceSprite.setScale((float)CELL_SIZE/60, (float)CELL_SIZE/60); // dice size 60x60
//     diceSprite.setPosition(10, BOARD_PIXEL_SIZE + 20);

//     // Player pieces
//     sf::CircleShape player1(CELL_SIZE/3);
//     player1.setFillColor(sf::Color::Red);
//     sf::CircleShape player2(CELL_SIZE/3);
//     player2.setFillColor(sf::Color::Blue);

//     int pos1 = 1, pos2 = 1;
//     player1.setPosition(getPosition(pos1));
//     player2.setPosition(getPosition(pos2) + sf::Vector2f(CELL_SIZE/2,0)); // offset to avoid overlap

//     bool turnPlayer1 = true;
//     bool rolling = false;
//     int diceValue = 0;

//     sf::Font font;
//     if(!font.loadFromFile("/System/Library/Fonts/Supplemental/Arial.ttf")){
//         cout << "Failed to load font" << endl;
//         return 1;
//     }
//     sf::Text infoText;
//     infoText.setFont(font);
//     infoText.setCharacterSize(20);
//     infoText.setFillColor(sf::Color::Black);
//     infoText.setPosition(10, BOARD_PIXEL_SIZE + 80);
//     infoText.setString("Press SPACE to roll the dice");

//     while(window.isOpen()){
//         sf::Event event;
//         while(window.pollEvent(event)){
//             if(event.type == sf::Event::Closed)
//                 window.close();

//             if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space && !rolling){
//                 rolling = true;
//                 diceValue = rollDice();
//                 diceSprite.setTexture(diceTextures[diceValue-1]);
//                 infoText.setString("Dice: " + to_string(diceValue));

//                 // Move current player
//                 if(turnPlayer1){
//                     pos1 += diceValue;
//                     if(pos1 > 100) pos1 = 100;
//                     if(snakes.find(pos1) != snakes.end()) pos1 = snakes[pos1];
//                     else if(ladders.find(pos1) != ladders.end()) pos1 = ladders[pos1];
//                     player1.setPosition(getPosition(pos1));
//                 } else {
//                     pos2 += diceValue;
//                     if(pos2 > 100) pos2 = 100;
//                     if(snakes.find(pos2) != snakes.end()) pos2 = snakes[pos2];
//                     else if(ladders.find(pos2) != ladders.end()) pos2 = ladders[pos2];
//                     player2.setPosition(getPosition(pos2) + sf::Vector2f(CELL_SIZE/2,0));
//                 }

//                 // Check for win
//                 if(pos1 == 100) infoText.setString("Player 1 wins! Press ESC to exit.");
//                 else if(pos2 == 100) infoText.setString("Player 2 wins! Press ESC to exit.");
                
//                 turnPlayer1 = !turnPlayer1;
//                 rolling = false;
//             }

//             if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape){
//                 window.close();
//             }
//         }

//         window.clear(sf::Color::White);
//         window.draw(boardSprite);
//         window.draw(player1);
//         window.draw(player2);
//         window.draw(diceSprite);
//         window.draw(infoText);
//         window.display();
//     }

//     return 0;
// }









// // #include <SFML/Graphics.hpp>

// // int main()
// // {
// //     // Create a window
// //     sf::RenderWindow window(sf::VideoMode(800, 600), "SFML 2.x on Mac M1");

// //     // Create a green circle
// //     sf::CircleShape shape(100.f);
// //     shape.setFillColor(sf::Color::Green);
// //     shape.setPosition(350.f, 250.f);

// //     // Main loop
// //     while (window.isOpen())
// //     {
// //         sf::Event event;
// //         while (window.pollEvent(event))
// //         {
// //             if (event.type == sf::Event::Closed)
// //                 window.close();
// //         }

// //         window.clear(sf::Color::Cyan);
// //         window.draw(shape);
// //         window.display();
// //     }

// //     return 0;
// // }