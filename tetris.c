//   Note:
// * Add gravity
// * Add turning(will take some time) (in pause(learning matrix rotation)
//	 	- Note a square for the pieces denoting (x, y) as the top left of the piece size and checking will depend on each piece
// * Difficulty levels
// * Scoring (done)
// * Menu (in progress..)

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdint.h>
#include <termio.h>

//Not sure if i actually need this
#include <math.h>

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
	i32* data;
} piece_struct;

void board_init(u8 board[][SIZE_X])
{
	for(i32 i = 0; i < SIZE_Y; i++)
	{
		for(i32 j = 0; j < SIZE_X; j++)
		{
			board[i][j] = 0;
		}
	}
}

void board_display(u8 board[][SIZE_X], i32 score)
{
	fputs("\tTetris\n========================\n", stdout);
	for(i32 i = 0; i < SIZE_Y; i++)
	{
		fputs("||", stdout);
		for(i32 j = 0; j < SIZE_X; j++)
		{
			switch(board[i][j])
			{
				case 2:
				{
					fputs("[]", stdout);
				} break;

				case 1:
				{
					fputs("[]", stdout);
				} break;

				default:
				{
					fputs("  ", stdout);
				} break;
			}
		}

		if(i == 3)
		{
			printf("||    Score: %d\n", score);
		}
		else
		{
			fputs("||\n", stdout);
		}
	}

	fputs("========================\n", stdout);
}

void piece_spawn(piece_struct piece, u8 board[][SIZE_X])
{
	for(i32 i = 0; i < 4; i++)
	{
		i32 piece_y = piece.data[0 + (i * 2)];
		i32 piece_x = piece.data[1 + (i * 2)];

		board[piece_y][piece_x+4] = 2;
	}
}

void move_down(u8 board[][SIZE_X])
{
	for(i32 i = SIZE_Y; i > -1; i--)
	{
		for(i32 j = SIZE_X; j > -1; j--)
		{
			if(board[i][j] == 2)
			{
				if(board[i+1][j] == 1) { return; }
				board[i][j] = 0;
				board[i+1][j] = 2;
			}
		}
	}
}

void move_right(u8 board[][SIZE_X])
{
	for(i32 i = SIZE_Y; i > -1; i--)
	{
		for(i32 j = SIZE_X; j > -1; j--)
		{
			if(board[i][j] == 2)
			{
				if(j == (SIZE_X-1)) { return; }
				if(board[i][j+1] == 1) { return; }
				board[i][j] = 0;
				board[i][j+1] = 2;
			}
		}
	}
}

void move_left(u8 board[][SIZE_X])
{
	for(i32 i = SIZE_Y; i > -1; i--)
	{
		for(i32 j = 0; j < SIZE_X; j++)
		{
			if(board[i][j] == 2)
			{
				if(j == 0) { return; }
				if(board[i][j-1] == 1) { return; }
				board[i][j] = 0;
				board[i][j-1] = 2;
			}
		}
	}
}

bool bottomout_check(u8 board[][SIZE_X])
{
	for(i32 i = 0; i < SIZE_Y; i++)
	{
		for(i32 j = 0; j < SIZE_X; j++)
		{
			if(board[i][j] == 2)
			{
				if(i == SIZE_Y-1)
				{
					return true;
				}

				if(board[i+1][j] == 1)	
				{
					return true;
				}
			}
		}
	}
	return false;
}


u8 line_check(u8 board[][SIZE_X])
{
	u8 counter = 0;
	for(i32 i = 0; i < SIZE_Y; i++)
	{
		if(board[i][0] == 1)
		{
			bool verdict = true;
			for(i32 j = 0; j < SIZE_X; j++)
			{
				if(board[i][j] == 0) 
				{ 
					verdict = false;
					break;
				}
			}
			if(verdict)
			{
				for(i32 k = i; k > 0; k--)
				{
					for(i32 l = 0; l < SIZE_X; l++)
					{
						board[k][l] = board[k-1][l];
					}
				}
				counter++;
			}
		}
	}
	return counter;
}

void game_start()
{
	//Board init
	u8 board[20][10];
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
						if(board[i][j] == 2)
						{
							board[i][j] = 1;
						}
					}
				}

				u8 line_cleared = line_check(board);
				score += (line_cleared * 1000);

				//Spawns a new piece
				piece_spawn(L, board);
			}

			printf("You pressed %c\n", input);

			fputs("\033[H", stdout);
		}
	}
}

i32 menu(void)
{
	system("clear");

	//dummy board for menu
	u8 board[20][10];
	board_init(board);

	board_display(board, 0);
	fputs("\033[H", stdout);
	fputs("\033[15B\033[27C Controls: W - Turn piece", stdout);
	fputs("\033[1B\033[15D A - Move left", stdout);
	fputs("\033[1B\033[14D S - Move down", stdout);
	fputs("\033[1B\033[14D D - Move right", stdout);
	fputs("\033[13A\033[49D Press z to enter", stdout);
	fputs("\033[H", stdout);
	fputs("\033[6B\033[4C  [START GAME]", stdout);
	fputs("\033[1B\033[16D        QUIT", stdout);
	fputs("\033[16B", stdout);
	fputs("\033[H", stdout);

	u8 choice = 0;
	while(true)
	{
		u8 input;
		if(input == 'w')
		{
			fputs("\033[H", stdout);
			printf("\033[6B\033[4C   START GAME ");
			printf("\033[1B\033[16D       [QUIT]");
			printf("\033[16B");
		}
		if(input == 's')
		{
					fputs("\033[H", stdout);
					printf("\033[6B\033[4C  [START GAME]");
					printf("\033[1B\033[16D        QUIT ");
					printf("\033[16B");
		}

		ssize_t n = read(STDIN_FILENO, &input, 1);
		if(n > 0)
		{
			switch(input)
			{
				case 'w':
				{
					choice = 1;
					printf("w");
				} break;

				case 's':
				{
					choice = 2;
				} break;

				case 'z':
				{
					return choice;
				} break;
			}
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

	u8 user_choice = menu();
	switch(user_choice)
	{
		case 1:
		{
			system("clear");
//			game_start();
		} break;

	}



	tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
	
	return 0;
}

