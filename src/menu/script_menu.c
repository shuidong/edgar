/*
Copyright (C) 2009-2011 Parallel Realities

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "../headers.h"

#include "../audio/audio.h"
#include "../draw.h"
#include "../graphics/graphics.h"
#include "../init.h"
#include "../system/error.h"
#include "../system/pak.h"
#include "script_menu.h"
#include "widget.h"

extern Input input, menuInput;
extern Game game;
extern Control control;

static Menu menu;
static void (*yesAction)(void);
static void (*noAction)(void);

static void loadMenuLayout(char *);
static void doMenu(void);
static void doYes(void);
static void doNo(void);

void drawScriptMenu()
{
	int i;

	drawImage(menu.background, menu.x, menu.y, FALSE, 196);

	for (i=0;i<menu.widgetCount;i++)
	{
		drawWidget(menu.widgets[i], &menu, menu.index == i);
	}
}

static void doMenu()
{
	Widget *w;

	if (input.right == TRUE || menuInput.right == TRUE)
	{
		menu.index++;

		if (menu.index == menu.widgetCount)
		{
			menu.index = 1;
		}

		menuInput.right = FALSE;
		input.right = FALSE;

		playSound("sound/common/click.ogg");
	}

	else if (input.left == TRUE || menuInput.left == TRUE)
	{
		menu.index--;

		if (menu.index < 1)
		{
			menu.index = menu.widgetCount - 1;
		}

		menuInput.left = FALSE;
		input.left = FALSE;

		playSound("sound/common/click.ogg");
	}

	else if (input.attack == TRUE || menuInput.attack == TRUE)
	{
		w = menu.widgets[menu.index];

		menuInput.attack = FALSE;
		input.attack = FALSE;

		playSound("sound/common/click.ogg");

		if (w->clickAction != NULL)
		{
			w->clickAction();
		}
	}
}

static void loadMenuLayout(char *text)
{
	char *line, menuID[MAX_VALUE_LENGTH], menuName[MAX_VALUE_LENGTH], *token, *savePtr1, *savePtr2;
	unsigned char *buffer;
	int x, y, i;

	savePtr1 = NULL;

	i = 0;

	buffer = loadFileFromPak("data/menu/script_menu.dat");

	line = strtok_r((char *)buffer, "\n", &savePtr1);

	while (line != NULL)
	{
		if (line[strlen(line) - 1] == '\n')
		{
			line[strlen(line) - 1] = '\0';
		}

		if (line[strlen(line) - 1] == '\r')
		{
			line[strlen(line) - 1] = '\0';
		}

		if (line[0] == '#' || line[0] == '\n')
		{
			line = strtok_r(NULL, "\n", &savePtr1);

			continue;
		}

		token = strtok_r(line, " ", &savePtr2);

		if (strcmpignorecase(token, "WIDTH") == 0)
		{
			token = strtok_r(NULL, " ", &savePtr2);

			menu.w = atoi(token);
		}

		else if (strcmpignorecase(token, "HEIGHT") == 0)
		{
			token = strtok_r(NULL, " ", &savePtr2);

			menu.h = atoi(token);
		}

		else if (strcmpignorecase(token, "WIDGET_COUNT") == 0)
		{
			token = strtok_r(NULL, " ", &savePtr2);

			menu.widgetCount = atoi(token) + 1;

			menu.widgets = malloc(sizeof(Widget *) * menu.widgetCount);

			if (menu.widgets == NULL)
			{
				showErrorAndExit("Ran out of memory when creating Script Menu");
			}
		}

		else if (strcmpignorecase(token, "WIDGET") == 0)
		{
			if (menu.widgets != NULL)
			{
				if (i == 0)
				{
					menu.widgets[i++] = createWidget(text, NULL, NULL, NULL, NULL, -1, 20, FALSE, 255, 255, 255);
				}
				
				token = strtok_r(NULL, "\0", &savePtr2);

				sscanf(token, "%s \"%[^\"]\" %d %d", menuID, menuName, &x, &y);

				if (strcmpignorecase(menuID, "MENU_YES") == 0)
				{
					menu.widgets[i] = createWidget(_(menuName), NULL, NULL, NULL, doYes, x, y, TRUE, 255, 255, 255);
				}

				else if (strcmpignorecase(menuID, "MENU_NO") == 0)
				{
					menu.widgets[i] = createWidget(_(menuName), NULL, NULL, NULL, doNo, x, y, TRUE, 255, 255, 255);
				}

				else
				{
					showErrorAndExit("Unknown widget %s", menuID);
				}

				i++;
			}

			else
			{
				showErrorAndExit("Widget Count must be defined!");
			}
		}

		line = strtok_r(NULL, "\n", &savePtr1);
	}

	if (menu.w <= 0 || menu.h <= 0)
	{
		showErrorAndExit("Menu dimensions must be greater than 0");
	}
	
	menu.w = menu.widgets[0]->selectedState->w + 20;

	x = menu.widgets[1]->normalState->w + menu.widgets[2]->normalState->w + 20;

	menu.widgets[1]->x = (menu.w - x) / 2;
	menu.widgets[2]->x = menu.widgets[1]->x + menu.widgets[1]->selectedState->w + 20;

	menu.background = addBorder(createSurface(menu.w, menu.h), 255, 255, 255, 0, 0, 0);

	free(buffer);

	menu.x = (SCREEN_WIDTH - menu.background->w) / 2;
	menu.y = (SCREEN_HEIGHT - menu.background->h) / 2;
}

Menu *initScriptMenu(char *text, void (*yes)(void), void (*no)(void))
{
	menu.action = &doMenu;

	yesAction = yes;
	noAction = no;

	if (menu.widgets != NULL)
	{
		freeScriptMenu();
	}

	loadMenuLayout(text);

	menu.index = 2;

	menu.returnAction = NULL;

	return &menu;
}

void freeScriptMenu()
{
	int i;

	if (menu.widgets != NULL)
	{
		for (i=0;i<menu.widgetCount;i++)
		{
			freeWidget(menu.widgets[i]);
		}

		free(menu.widgets);
		
		menu.widgets = NULL;
	}

	if (menu.background != NULL)
	{
		SDL_FreeSurface(menu.background);

		menu.background = NULL;
	}
}

static void doYes()
{
	yesAction();
}

static void doNo()
{
	noAction();
}
