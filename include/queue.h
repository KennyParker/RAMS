#define SIZE_S 10
#define SIZE_A 500
#define SIZE_L 8000

enum queue_type{ LASER_TYPE, ANGLE_TYPE, SPECTRAL_TYPE };

struct laser
{
    int time;
    int distance;
};

struct angle
{
    int time;
    float yaw;
    float pitch;
    float roll;
};

struct spectral
{
    int time;
    int exposure;
    double spectrum[PIXELS];
};

struct LamportQueue
{
    atomic_size_t front_;
    atomic_size_t back_;
    size_t cached_front_;
    size_t cached_back_;

    enum queue_type type;
    struct laser *l_data;
    struct angle *a_data;
    struct spectral *s_data;
};

struct controlQueue
{
    struct LamportQueue* queue;
    struct control* control;
};


struct consumeAll
{
    struct LamportQueue* lidar;
    struct LamportQueue* angles;
    struct LamportQueue* spectral;
    struct control* control;
};

void LamportQueue_init(struct LamportQueue *queue, enum queue_type type);
bool LamportQueue_push(struct LamportQueue *queue, void* elem);
bool LamportQueue_pop(struct LamportQueue *queue, void* elem);

