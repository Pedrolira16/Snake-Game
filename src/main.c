#include "keyboard.h"
#include "screen.h"
#include "timer.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

bool ateFood = false;
bool startGame = false;
int snakeSpeed = 250000;
int page = 1;

typedef struct Segment
{
  int x;
  int y;
  struct Segment *next;
  struct Segment *prev;
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
  printf("1 - Fácil (1.0x)\n");
  printf("2 - Médio (1.5x)\n");
  printf("3 - Difícil (1.7x)\n");
  printf("4 - Insano (2.0x)\n");
  printf("5 - Sair\n");
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
        snakeSpeed = 200000;
        startGame = true;
        break;
      case '3':
        snakeSpeed = 170000;
        startGame = true;
        break;
      case '4':
        snakeSpeed = 140000;
        startGame = true;
        break;
      case '5':
        keyboardDestroy();
        return 0;
      }
    }

    if (startGame)
    {

      srand(time(NULL));
      screenInit(1);
      timerInit(100);

      Snake snake;
      Food food;

      snake.head = (Segment *)malloc(sizeof(Segment));
      snake.head->x = 10;
      snake.head->y = 10;
      snake.head->next = NULL;
      snake.head->prev = NULL;
      snake.dx = 1;
      snake.dy = 0;

      generateFoodInRandomLocal(&food);

      while (1)
      {

        if (keyhit())
        {
          char key = readch();

          switch (key)
          {
          case 'w':
            snake.dx = 0;
            snake.dy = -1;
            break;
          case 'a':
            snake.dx = -1;
            snake.dy = 0;
            break;
          case 's':
            snake.dx = 0;
            snake.dy = 1;
            break;
          case 'd':
            snake.dx = 1;
            snake.dy = 0;
            break;
          }
        }

        moveSnake(&snake, snake.dx, snake.dy, &food);
        displayGame(&snake, &food);
        usleep(snakeSpeed);

        if (hasItCollided(&snake))
        {
          break;
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
  newHead->prev = NULL;
  newHead->next = snake->head;

  if (snake->head != NULL)
  {
    snake->head->prev = newHead;
  }

  snake->head = newHead;

  if (newHead->x == food->x && newHead->y == food->y)
  {
    ateFood = true;
    generateFoodInRandomLocal(food);
  }
  else
  {
    Segment *current = snake->head;
    Segment *prevSegment = NULL;

    while (current->next != NULL)
    {
      prevSegment = current;
      current = current->next;
    }

    if (prevSegment != NULL)
    {
      prevSegment->next = NULL;
    }
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