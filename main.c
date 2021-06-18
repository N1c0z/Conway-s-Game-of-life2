#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <time.h>
typedef struct _Square {
	long PosX;
	long PosY;
} Square;

typedef struct _ListArray
{
    size_t size;
    size_t actual_size;
    Square *content;
} ListArray;

#define TSIZEE 60 //This is grid size, change to any number you'd like
uint8_t TABLE[TSIZEE][TSIZEE];
uint8_t TABLE2[TSIZEE][TSIZEE];
ListArray LIVE_SQUARES;
ListArray LIVE_SQUARES2;
const char PrintableSquare = 171 + '0';
//const uint8_t PrintableSquare = 171;

void create_array(ListArray *d);
void element_append(ListArray *d, Square v);
void destroy_array(ListArray *d);
size_t array_size(ListArray *d);
uint8_t delete(ListArray *v, int index);
uint8_t surrounding_cells(Square* square);
uint8_t surrounding_cellss(long PosXX, long PosYY);
void do_stuff();
uint8_t cell_exists(long PosX, long PosY);
void print_table();
void generate_new_cells(Square* square);
void create_random_startup();

int main()
{
	srand(time(NULL)); 
    	create_array(&LIVE_SQUARES);
    	create_array(&LIVE_SQUARES2);
	create_random_startup();
	print_table();
	while (1)
	{
		do_stuff();
		print_table();
		sleep(1);
	}

    destroy_array(&LIVE_SQUARES);
    destroy_array(&LIVE_SQUARES2);
}

void create_array(ListArray *d)
{
	Square* temp = malloc(sizeof(struct Square*));
	if (!temp)
    {
        fprintf(stderr, "Failed to allocate array");            
	exit(EXIT_FAILURE);
    }
	d->actual_size = d->size = 0;
	d->content= temp;
}

void element_append(ListArray *d, Square v)
{
    if (d->size+1 > d->actual_size)
    {
        size_t new_size;
        if (!d->actual_size) 
        { 
            new_size = 1;
        }
        else
        {
            new_size = d->actual_size * 2;
        }
        Square *temp = realloc(d->content, sizeof(Square) * new_size);
        if (!temp)
        {
            fprintf(stderr, "Failed to extend array (new_size=%zu)\n", new_size);
            exit(EXIT_FAILURE);
        }
        d->actual_size = new_size;
        d->content = temp;
    }
    d->content[d->size] = v;
    d->size++;
}

void destroy_array(ListArray *d)
{
    free(d->content);
    d->content = NULL;
    d->size = d->actual_size = 0;
}

size_t array_size(ListArray *d)
{
    return d->size;
}

uint8_t delete(ListArray *v, int index)
{
    if (index < 0 || index >= v->size)
        return -1;

    memmove(v->content + index, v->content + index + 1, (v->size - index - 1) * sizeof(Square));
    v->size--;
    if (v->actual_size / 2 > v->size)
    {
        size_t new_size;
        new_size = v->actual_size / 2;
        Square *temp = realloc(v->content, sizeof(Square) * new_size);
        if (!temp)
        {
            fprintf(stderr, "Failed to extend array (new_size=%zu)\n", new_size);
            exit(EXIT_FAILURE);
        }
        v->actual_size = new_size;
        v->content = temp;
    }
    return 0;
}

void create_random_startup() {

	for (long i = 0; i < TSIZEE; i++)
	{
		for (long k = 0; k < TSIZEE; k++)
		{
			TABLE[i][k] = (rand() % 2) && (rand() % 2); 
			if (TABLE[i][k])
			{
				Square square;
				square.PosX = i;
				square.PosY = k;
				element_append(&LIVE_SQUARES, square);
			}
		}
	}

	for (size_t i = 0; i < TSIZEE; i++)
		for (size_t k = 0; k < TSIZEE; k++)
			TABLE2[i][k] = TABLE[i][k];

}
uint8_t surrounding_cells(Square* square) {
	long numOfCells = 0;
	long posx, posy;
	for (long i = -1; i < 2; i++)
	{
		posx = square->PosX + i;
		for (long k = -1; k < 2; k++)
		{
			posy = square->PosY + k;
			if (cell_exists(posx, posy))
			{
				if (TABLE[posx][posy])
				{
					if (!((posx == square->PosX)&&(posy == square->PosY)))
					{
						numOfCells++;
					}
					
				}
			}
		}
	}
	return numOfCells;
}

uint8_t surrounding_cellss(long PosXX, long PosYY) {
	long numOfCells = 0;
	long posx, posy;
	for (long i = -1; i < 2; i++)
	{
		posx = PosXX + i;
		for (long k = -1; k < 2; k++)
		{
			posy = PosYY + k;
			if (cell_exists(posx, posy))
			{
				if (TABLE[posx][posy])
				{
					if (!((posx == PosXX)&&(posy == PosYY)))
					{
						numOfCells++;
					}
				}

			}
		}
	}
	return numOfCells;
}

void do_stuff() {
	long stuff;
	for (size_t i = 0; i < LIVE_SQUARES.size; i++)
	{
		stuff = surrounding_cells(&LIVE_SQUARES.content[i]);
		if ((stuff != 2) && (stuff != 3))
		{
			TABLE2[LIVE_SQUARES.content[i].PosX][LIVE_SQUARES.content[i].PosY] = 0;
		}
		else
		{
			element_append(&LIVE_SQUARES2, LIVE_SQUARES.content[i]);
		}
	}
	for (size_t i = 0; i < LIVE_SQUARES.size; i++)
	{
		generate_new_cells(&LIVE_SQUARES.content[i]);
	}

	for (size_t i = 0; i < TSIZEE; i++)
		for (size_t k = 0; k < TSIZEE; k++)
			TABLE[i][k] = TABLE2[i][k];

    	destroy_array(&LIVE_SQUARES);
    	create_array(&LIVE_SQUARES);
    
	LIVE_SQUARES = LIVE_SQUARES2;

    	create_array(&LIVE_SQUARES2);
}

void generate_new_cells(Square* square) {
	long posx, posy;
	for (long i = -1; i < 2; i++)
	{
		posx = square->PosX + i;
		for (long k = -1; k < 2; k++)
		{
			posy = square->PosY + k;
			if (cell_exists(posx, posy))
			{
				if (!TABLE2[posx][posy])
				{
					if (surrounding_cellss(posx, posy) == 3)
					{
						Square cSquare;
						cSquare.PosX = posx;
						cSquare.PosY = posy;
						TABLE2[posx][posy] = 1;
                        			element_append(&LIVE_SQUARES2, cSquare);
					}
				}
			}
		}
	}
	return;
}

void print_table() {

	//system("CLS"); //to clean console in windoze
	system("clear"); //to clean console in linucs

	for (size_t i = 0; i < TSIZEE; i++)
	{
		for (size_t k = 0; k < TSIZEE; k++)
		{
			if (TABLE[i][k])
			{
				printf("%c", PrintableSquare);
			}
			else
			{
				printf(" ");
			}
		}
		printf("\n");
	}
	putchar('\n');
}

uint8_t cell_exists(long PosX, long PosY) {

	if (PosX < 0 || PosY < 0 || PosX >= TSIZEE || PosY >= TSIZEE)
	{
		return 0;
	}
	return 1;
}
