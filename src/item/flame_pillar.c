/*
Copyright (C) 2009-2010 Parallel Realities

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

#include "../graphics/animation.h"
#include "../system/properties.h"
#include "../entity.h"
#include "../system/error.h"
#include "../system/random.h"
#include "../collisions.h"
#include "../event/trigger.h"
#include "../graphics/decoration.h"
#include "../audio/audio.h"

extern Entity *self;

static void entityWait(void);
static void touch(Entity *);

Entity *addFlamePillar(int x, int y, char *name)
{
	Entity *e = getFreeEntity();

	if (e == NULL)
	{
		showErrorAndExit("No free slots to add a Flame Pillar");
	}

	loadProperties(name, e);

	e->x = x;
	e->y = y;

	e->type = KEY_ITEM;

	e->face = RIGHT;

	e->action = &entityWait;
	e->touch = &touch;

	e->draw = &drawLoopingAnimationToMap;

	setEntityAnimation(e, STAND);

	return e;
}

static void entityWait()
{
	checkToMap(self);
}

static void touch(Entity *other)
{
	/* Get killed by ice */

	if (other->type == KEY_ITEM && strcmpignorecase(other->name, "item/ice_cube") == 0)
	{
		self->health--;
		
		if (self->health <= 0)
		{
			fireTrigger(self->objectiveName);
			
			self->inUse = FALSE;
		}
		
		other->inUse = FALSE;
	}

	else
	{
		entityTouch(other);
	}
}