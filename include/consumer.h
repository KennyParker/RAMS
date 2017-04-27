
#define RADS_OF (M_PI/180)*
#define DEGR_OF (180/M_PI)*

extern MYSQL *mysql;

void convert(struct laser *point, struct angle *angle1, struct angle *angle2, int *x, int *y, int *z);
void flat_convert(struct laser *point, struct angle *angle1, struct angle *angle2, int *x, int *y, int *z);
void triInitialize( struct consumeAll *all, struct laser *point, struct angle *angle1, struct angle *angle2, struct spectral *spectrum1, struct spectral *spectrum2, struct control *c);
