
struct control
{

    int *exposure;
    int *scan_id;
    int *start_time;

    bool *STOP;
    bool *DEBUG;
    bool *OUTPUT;

    bool *READY;
    bool *DESTROY; // IF DESTROY IS TRUE, ALL DATA IS CLEARED on every run.

    bool *WRITE_VOXEL;
    bool *WRITE_SPECTRA;

};

void init_control( struct control *control );