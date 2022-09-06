<div align="center">

  <img src="/images/logo.png" alt="logo" width="128"/>

</div>

<h1 align="center">Tic Tac Toe using the minimax algorithm</h1>

<div align="justify">

## About

Tic Tac Toe is a zero sum game with alternating turns and perfect information. This project makes use of a TCP connection and Linux's primitives to let the player (client) play against a computer (server).

With that being said, the minimax algorithm has been used in order to implement the computer. The latter is a recursive algorithm for finding the best move in a given situation. More particularly, it consists of a positional evaluation function that measures the quality of a position (or game state) and indicates how desirable it is for the given player to reach that position. The player then makes the move that minimizes the value of the best position attainable by the other player. Therefore, it assigns a value to each legal move, proportional to how much it decreases the value of the position for the other player.

## Stack

- C++
- Linux (TCP Sockets)
- Minimax algorithm

## Protocol

Client and server:

- communicate with each other
- use numbers between 1 and 9 to represent their move (cells)

Choices:

- client:

  - Choice (e.g., '1') - 1 character string (1 byte - 1 bit used)

  <img src="/images/client_choice.png" alt="Client choice" width="700"/>

- server:

  - Choice, Game_Status (e.g., '12') - 2 characters string (1 byte - 2 bits used)

  <img src="/images/server_choice.png" alt="Server choice" width="700"/>

Controls:

- server: 400
  - Invalid choice
- server: 500
  - Invalid choice, cell is not free

Quit:

- client: 100
  - Do not play again
- client: 200
  - Play again

## Game Flow

Client

1. Printing game
2. Move input
3. Wait for server choice

A visual representation can be seen in the following image:

<div align="center">

<img src="/images/game_flow.png" alt="Game flow" width="200"/>

</div>

Server

1. Printing game
2. Waiting for client choice
3. Processing own choice using the Minimax algorithm
4. Performing game status checks (e.g., win or loss, still being played)

## License

MIT

## Attributions

<a href="https://www.flaticon.com/free-icons/tic-tac-toe" title="tic tac toe icons">Tic tac toe icons created by Freepik - Flaticon</a>

</div>
