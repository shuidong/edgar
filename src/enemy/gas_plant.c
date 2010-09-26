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

#include "../audio/audio.h"
#include "../graphics/animation.h"
#include "../entity.h"
#include "../collisions.h"
#include "../custom_actions.h"
#include "../system/properties.h"
#include "../system/random.h"
#include "../system/error.h"
#include "../player.h"

extern Entity *self;

static void entityWait(void);
static void takeDamage(Entity *, int);
static void init(void);
static void sprayGas(void);

Entity *addGasPlant(int x, int y, char *name)
{
	Entity *e = getFreeEntity();

	if (e == NULL)
	{
		showErrorAndExit("No free slots to add a Gas Plant");
	}

	loadProperties(name, e);

	e->x = x;
	e->y = y;

	e->action = &init;
	e->draw = &drawLoopingAnimationToMap;
	e->die = &entityDie;
	e->touch = &entityTouch;
	e->takeDamage = &takeDamage;

	e->type = ENEMY;

	setEntityAnimation(e, STAND);

	return e;
}

static void init()
{
	self->mental = 0;
	
	self->action = &entityWait;
}

static void entityWait()
{
	checkToMap(self);
}

static void takeDamage(Entity *other, int damage)
{
	entityTakeDamageNoFlinch(other, damage);
	
	if (self->health > 0 && self->mental == 0)
	{
		self->mental = 1;
		
		self->thinkTime = 15;
		
		self->action = &sprayGas;
	}
}

static void sprayGas()
{
	self->thinkTime--;
	
	if (self->thinkTime <= 0)
	{
		self->mental = 0;
		
		self->action = &entityWait;
	}
	
	checkToMap(self);
}