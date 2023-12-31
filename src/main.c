#include "keyboard.h"
#include "screen.h"
#include "timer.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define RESET   "\x1b[0m"
#define BLUE    "\033[1;36m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define ORANGE  "\x1b[38;5;208m"
#define RED     "\x1b[31m"

bool ateFood = false;
bool startGame = false;
bool lostGame = false;
int snakeSpeed = 250000;
int cont = 0;

FILE *fptr;

typedef struct Segment
{
  int x;
  int y;
  struct Segment *next;
} Segment;

typedef struct Snake
{
  Segment *head;
  int dx;
  int dy;
} Snake;

typedef struct Food
{
  int x;
  int y;
} Food;

void moveSnake(Snake *snake, int dx, int dy, Food *food);
void generateFoodInRandomLocal(Food *food);
void displayGame(Snake *snake, Food *food);
bool hasItCollided(Snake *snake);

int main()
{
  keyboardInit();
  screenInit(0);

  printf("Escolha uma dificuldade: \n");
  printf(BLUE "1 - Super fácil (1.0x) \n");
  printf(GREEN "2 - Fácil (1.7x) \n");
  printf(YELLOW "3 - Médio (2.0x) \n");
  printf(ORANGE "4 - Difícil (2.4x) \n");
  printf(RED "5 - Impossível (2.9x) \n");
  printf(RESET "6 - Sair\n");
  printf("Pressione a tecla correspondente.\n");

  while (1)
  {

    if (keyhit())
    {
      char key = readch();
      switch (key)
      {
      case '1':
        snakeSpeed = 250000;
        startGame = true;
        break;
      case '2':
        snakeSpeed = 170000;
        startGame = true;
        break;
      case '3':
        snakeSpeed = 140000;
        startGame = true;
        break;
      case '4':
        snakeSpeed = 100000;
        startGame = true;
        break;
      case '5':
        snakeSpeed = 50000;
        startGame = true;
        break;

      case '6':
        keyboardDestroy();
        return 0;
      }
    }

    if (startGame)
    {

      srand(time(NULL));
      screenInit(1);

      Snake snake;
      Food food;

      snake.head = (Segment *)malloc(sizeof(Segment));
      snake.head->x = 10;
      snake.head->y = 10;
      snake.head->next = NULL;

      snake.dx = 1; // -1 esquerda, 1 direita
      snake.dy = 0; // -1 cima, 1 baixo

      generateFoodInRandomLocal(&food);

      while (startGame)
      {

        if (keyhit() && !lostGame)
        {
          int key = readch();
          int new_dx = snake.dx;
          int new_dy = snake.dy;

          switch (key)
          {
          case 119: // w
            new_dx = 0;
            new_dy = -1;
            break;
          case 97: // a
            new_dx = -1;
            new_dy = 0;
            break;
          case 115: // s
            new_dx = 0;
            new_dy = 1;
            break;
          case 100: // d
            new_dx = 1;
            new_dy = 0;
            break;
          case 27: // ESC
            startGame = false;
            break;
          }
          if (!(new_dx == -snake.dx && new_dy == -snake.dy)) {
            snake.dx = new_dx;
            snake.dy = new_dy;
          }
        }

        moveSnake(&snake, snake.dx, snake.dy, &food);
        displayGame(&snake, &food);
        usleep(snakeSpeed);

        if (hasItCollided(&snake))
        {
          lostGame = true;
          screenInit(0);
          fptr = fopen("ascii.txt", "r");
          char text = fgetc(fptr);
          while (text != EOF)
          {
            printf("%c", text);
            text = fgetc(fptr);
          }
          fclose(fptr);

          printf("\n\n Você deseja continuar jogando? (s/n)\n");

          while (1)
          {
            if (keyhit())
            {

              char key = readch();
              screenInit(0);
              switch (key)
              {
              case 's':
                snake.head = (Segment *)malloc(sizeof(Segment));
                snake.head->x = 10;
                snake.head->y = 10;
                snake.head->next = NULL;

                snake.dx = 1;
                snake.dy = 0;

                generateFoodInRandomLocal(&food);
                ateFood = false;
                lostGame = false;

                cont = 0;
                break;
              case 'n':
                lostGame = false;
                startGame = false;
                break;
              }
              if (lostGame == false)
              {
                break;
              }
            }
          }
        }
      }

      screenDestroy();
      keyboardDestroy();
      return 0;
    }
  }
}


void moveSnake(Snake *snake, int dx, int dy, Food *food)
{
  Segment *newHead = (Segment *)malloc(sizeof(Segment));
  newHead->x = snake->head->x + dx;
  newHead->y = snake->head->y + dy;
  newHead->next = snake->head;

  snake->head = newHead;

  if (newHead->x == food->x && newHead->y == food->y)
  {
    ateFood = true;
    generateFoodInRandomLocal(food);
    cont ++;
  }
  else
  {
    Segment *current = snake->head->next;
    Segment *prevSegment = snake->head;

    while (current->next != NULL)
    {
      prevSegment = current;
      current = current->next;
    }

    prevSegment->next = NULL;
    free(current);
  }
}

void generateFoodInRandomLocal(Food *food)
{
  food->x = rand() % (MAXX - 3) + 2;
  food->y = rand() % (MAXY - 2) + 2;
}

void displayGame(Snake *snake, Food *food)
{

  screenInit(1);
  printf("  W - Frente   S - Trás   A - Esquerda   D - Direita   ESC - Sair\t Comida: %d", cont);

  Segment *segment = snake->head;
  while (segment != NULL)
  {
    screenGotoxy(segment->x, segment->y);
    printf("O");
    segment = segment->next;
  }

  screenGotoxy(food->x, food->y);
  printf("*");

  screenUpdate();
}

bool hasItCollided(Snake *snake)
{
  if (snake->head->x <= 1 || snake->head->x > MAXX - 2 || snake->head->y <= 1 || snake->head->y > MAXY - 1)
  {
    return true;
  }

  Segment *segment = snake->head->next;
  while (segment != NULL)
  {
    if (snake->head->x == segment->x && snake->head->y == segment->y)
    {
      return true;
    }
    segment = segment->next;
  }

  return false;
}