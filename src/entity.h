void standardDie(void);
void freeEntities(void);
Entity *getFreeEntity(void);
void doEntities(void);
void drawEntities(int);
void removeEntity(void);
void doNothing(void);
void entityDie(void);
void standardDie(void);
void entityTakeDamage(Entity *, int);
void entityTouch(Entity *);
void pushEntity(Entity *);
int addEntity(Entity, int, int);
Entity *getEntityByObjectiveName(char *);
void activateEntitiesWithName(char *, int);
void interactWithEntity(int, int, int, int);
void initLineDefs(void);
void writeEntitiesToFile(FILE *);
void floatLeftToRight(void);
