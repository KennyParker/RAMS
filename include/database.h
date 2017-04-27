#define DROP_SCANS "DROP TABLE IF EXISTS scan"
#define DROP_SPECTRA "DROP TABLE IF EXISTS spectrum"
#define DROP_VOXELS "DROP TABLE IF EXISTS voxel"

#define CREATE_SCANS "CREATE TABLE scan( id INT NOT NULL AUTO_INCREMENT, start_time INT, white_bal TEXT, PRIMARY KEY(id) )"
#define CREATE_SPECTRA "CREATE TABLE spectrum( id INT NOT NULL AUTO_INCREMENT, time INT, exposure INT, signature TEXT, scan_id INT REFERENCES scan(id), PRIMARY KEY(id) )"
#define CREATE_VOXELS "CREATE TABLE voxel(  id INT NOT NULL AUTO_INCREMENT, time INT, x SMALLINT, y SMALLINT, z SMALLINT, scan_id INT REFERENCES scan(id), spectrum_id INT REFERENCES spectrum(id), PRIMARY KEY(id) )"

#define INSERT_SCAN "INSERT INTO scan( start_time, white_bal ) VALUES(?,?)"
#define BINDS_SCAN 2
#define INSERT_SPECTRA "INSERT INTO spectrum( scan_id, time, exposure, signature ) VALUES(?,?,?,?)"
#define BINDS_SPECTRA 4
#define INSERT_VOXEL "INSERT INTO voxel( scan_id, spectrum_id, time, x, y, z ) VALUES(?,?,?,?,?,?)"
#define BINDS_VOXEL 6

#define server "localhost"
#define user "pi"
#define password "raspberry"
#define database "RAMS"

#define encode_start '$'
#define encode_length PIXELS*4

enum data_type{ SCAN_TYPE, SPECTRUM_TYPE, VOXEL_TYPE };

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



