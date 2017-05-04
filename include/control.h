extern MYSQL *mysql;

struct control
{

    int *exposure;
    int *scan_id;
    int *start_time;

    bool *STOP;
    bool *DEBUG;
    bool *OUTPUT;

    bool *READY;
    bool *FINISHED;
    bool *CLEAR; // IF DESTROY IS TRUE, ALL DATA IS CLEARED on every run.

    bool *RUN_LASER;
    bool *RUN_SPECTROMETER;

};
 
void init_control( struct control *control, int argc, char *argv[] );
int parse_commands( struct control *control, int argc, char *argv[] );