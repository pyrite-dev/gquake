#if defined(_PSP) || defined(_EE) || defined(__vita__)
#include <sys/socket.h>

typedef socklen_t SOCKLEN_T;
#define NO_FCNTL
#elif defined(_WIN32)
#define NO_FCNTL

typedef int SOCKLEN_T;
#else
typedef int SOCKLEN_T;
#endif
