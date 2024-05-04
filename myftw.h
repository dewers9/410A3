// usings Stevens-Rago for traversing a directory tree                                                                                                                                                      
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h> // for reading directories                                                                                                                                                              
#include <limits.h>
#include "apue.h"
#include <errno.h>
#include <limits.h>

// declaration of function from pg. 132 of Stevens-Rago                                                                                                                                                     
typedef int Myfunc(const char *, const struct stat *, int);
static Myfunc myfunc;

static int myftw(char *, Myfunc *);
static int dopath(Myfunc *);
static char *pathalloc(size_t *sizep);

static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;

/*                                                                                                                                                                                                          
* Descend through the hierarchy, starting at "pathname".                                                                                                                                                    
* The caller’s func() is called for every file.                                                                                                                                                             
*/
#define FTW_F 1 /* file other than directory */
#define FTW_D 2 /* directory */
#define FTW_DNR 3 /* directory that can’t be read */
#define FTW_NS 4 /* file that we can’t stat */
static char *fullpath; /* contains full pathname for every file */
static size_t pathlen;

// page 50-51 of Stevens-Rago                                                                                                                                                                               
#ifdef PATH_MAX
static long pathmax = PATH_MAX;
#else
static long pathmax = 0;
#endif
static long posix_version = 0;
static long xsi_version = 0;

/* If PATH_MAX is indeterminate, no guarantee this is adequate */
#define PATH_MAX_GUESS 1024
