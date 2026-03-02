//   Note:
// * Add gravity
// * Add turning(will take some time)
//	 	- Note a square for the pieces denoting (x, y) as the top left of the piece size and checking will depend on each piece
// * Line Clear
// * Scoring
// * Menu

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>
#include <termio.h>

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef float f32;
typedef double f64;

#define SIZE_X 10
#define SIZE_Y 20

typedef struct
{
	u32* data;
} piece_struct;

void board_init(u8* board)
{
	for(i32 i = 0; i < SIZE_Y; i++)
	{
		for(i32 j = 0; j < SIZE_X; j++)
		{
			board[j + (SIZE_X * i)] = 0;
		}
	}
}

void board_display(u8 *board, i32 score)
{
	printf("\tTetris\n========================\n");
	for(i32 i = 0; i < SIZE_Y; i++)
	{
		printf("||");
		for(i32 j = 0; j < SIZE_X; j++)
		{
			switch(board[j + (SIZE_X * i)])
			{
				case 2:
				{
					printf("[]");
				} break;

				case 1:
				{
					printf("[]");
				} break;

				default:
				{
					printf("  ");
				} break;
			}
		}

		if(i == 3)
		{
			printf("||    Score: %d\n", score);
		}
		else
		{
			printf("||\n");
		}
	}

	printf("========================\n");
}

void piece_spawn(piece_struct piece, u8* board)
{
	for(i32 i = 0; i < 4; i++)
	{
		i32 piece_x = piece.data[0 + (i * 2)];
		i32 piece_y = piece.data[1 + (i * 2)];

		board[(piece_y + 4) + (piece_x * 10)] = 2;
	}
}

void move_down(u8* board)
{
	for(i32 i = SIZE_Y; i > -1; i--)
	{
		for(i32 j = SIZE_X; j > -1; j--)
		{
			if(board[j + (i * SIZE_X)] == 2)
			{
				if(board[j + ((i+1) * SIZE_X)] == 1) { return; }
				board[j + ((i) * SIZE_X)] = 0;
				board[j + ((i+1) * SIZE_X)] = 2;
			}
		}
	}
}

void move_right(u8* board)
{
	for(i32 i = SIZE_Y; i > -1; i--)
	{
		for(i32 j = SIZE_X; j > -1; j--)
		{
			if(board[j + (i * SIZE_X)] == 2)
			{
				if(j == (SIZE_X -1)) { return; }
				if(board[(j+1) + (i * SIZE_X)] == 1) { return; }
				board[j + ((i) * SIZE_X)] = 0;
				board[j+1 + ((i) * SIZE_X)] = 2;
			}
		}
	}
}

void move_left(u8* board)
{
	for(i32 i = SIZE_Y; i > -1; i--)
	{
		for(i32 j = 0; j < SIZE_X; j++)
		{
			if(board[j + (i * SIZE_X)] == 2)
			{
				if(j == 0) { return; }
				if(board[(j-1) + (i * SIZE_X)] == 1) { return; }
				board[j + ((i) * SIZE_X)] = 0;
				board[j-1 + ((i) * SIZE_X)] = 2;
			}
		}
	}
}

bool bottomout_check(u8* board)
{
	for(i32 i = 0; i < SIZE_Y; i++)
	{
		for(i32 j = 0; j < SIZE_X; j++)
		{
			if(board[j + (i * SIZE_X)] == 2)
			{
				if(i == SIZE_Y-1)
				{
					return true;
				}
				if(board[j + ((i+1) * SIZE_X)] == 1)	
				{
					return true;
				}
			}
		}
	}
	return false;
}

void game_start()
{
	//Board init
	u8* board = malloc(sizeof(u8) * SIZE_X * SIZE_Y);
	board_init(board);


	//L init
	piece_struct L;
	L.data = malloc(sizeof(i32) * 4 * 2);
	L.data[0] = 0;
	L.data[1] = 0;

	L.data[2] = 1;
	L.data[3] = 0;

	L.data[4] = 2;
	L.data[5] = 0;

	L.data[6] = 2;
	L.data[7] = 1;

	piece_spawn(L, board);

	i32 score = 0;
	bool running = true;
	while(running)
	{
		board_display(board, score);
		u8 input;
		ssize_t n = read(STDIN_FILENO, &input, 1);
		if(n > 0)
		{
			switch(input)
			{
				case '\033':
				{
					return;
				} break;

				case 'a':
				{
					move_left(board);
				} break;

				case 's':
				{
					move_down(board);
				} break;

				case 'd':
				{
					move_right(board);
				} break;
			}

			if(bottomout_check(board))
			{
				//Turns all 2's to 1's
				for(i32 i = 0; i < SIZE_Y; i++)
				{
					for(i32 j = 0; j < SIZE_X; j++)
					{
						if(board[j + (i * SIZE_X)] == 2)
						{
							board[j + (i * SIZE_X)] = 1;
						}
					}
				}

				//Spawns a new piece
				piece_spawn(L, board);
			}

			printf("You pressed %c\n", input);

			printf("\033[H");
		}
	}
}

int main(void)
{
	struct termios original_termios;
	tcgetattr(STDIN_FILENO, &original_termios);

	struct termios tetris_termios = original_termios;
	tetris_termios.c_iflag &= ~(ICRNL | IXON);
	tetris_termios.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);

	tcsetattr(STDIN_FILENO, TCSANOW, &tetris_termios);

	// board init

	system("clear");
	printf("\033[15B\033[27C Controls: W - Turn piece");
	printf("\033[1B\033[15D A - Move left");
	printf("\033[1B\033[14D S - Move down");
	printf("\033[1B\033[14D D - Move right");
	printf("\033[H");

	game_start();

	tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
	
	return 0;
}

