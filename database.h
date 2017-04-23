#define DROP_SCANS "DROP TABLE IF EXISTS scans"
#define DROP_SPECTRA "DROP TABLE IF EXISTS spectra"
#define DROP_VOXELS "DROP TABLE IF EXISTS voxels"

#define CREATE_SCANS "CREATE TABLE scans( id INT NOT NULL AUTO_INCREMENT, start_time INT, white_bal TEXT, PRIMARY KEY(id) )"
#define CREATE_SPECTRA "CREATE TABLE spectra( id INT NOT NULL AUTO_INCREMENT, scan INT REFERENCES scans(id), time INT, exposure INT, spectrum TEXT, PRIMARY KEY(id) )"
#define CREATE_VOXELS "CREATE TABLE voxels( scan INT REFERENCES scans(id), spectrum INT REFERENCES spectra(id), time INT, x INT, y INT, z INT )"

#define INSERT_SCAN "INSERT INTO scans( start_time, white_bal ) VALUES(?,?)"
#define BINDS_SCAN 2
#define INSERT_SPECTRA "INSERT INTO spectra( scan, time, exposure, spectrum ) VALUES(?,?,?,?)"
#define BINDS_SPECTRA 4
#define INSERT_VOXEL "INSERT INTO voxels( scan, spectrum, time, x, y, z ) VALUES(?,?,?,?,?,?)"
#define BINDS_VOXEL 6

#define server "localhost"
#define user "pi"
#define password "raspberry"
#define database "RAMS"

#define encode_start 128
#define encode_length PIXELS*2+1

enum data_type{ SCAN_TYPE, SPECTRA_TYPE, VOXEL_TYPE };

struct arg_struct
{
    int *id;
    int *spec_id;
    int *v_time;
    int *x;
    int *y;
    int *z;
    int *s_time;
    int *exposure;
    unsigned char *encode;
    int *encode_len;
};

void prepareTable( MYSQL *mysql, MYSQL_STMT *stmt, MYSQL_BIND *bind, enum data_type type, struct arg_struct *args, struct control *c);
void execute( MYSQL_STMT *stmt );
void array2string( double* spectrum, unsigned char* string );
void destroyAll( MYSQL *mysql );



