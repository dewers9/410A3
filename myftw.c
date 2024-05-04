#include "myftw.h"

static int
myfunc(const char *pathname, const struct stat *statptr, int type)
{
switch (type) {
case FTW_F:
switch (statptr->st_mode & S_IFMT) {
case S_IFREG: nreg++; break;
case S_IFBLK: nblk++; break;
case S_IFCHR: nchr++; break;
case S_IFIFO: nfifo++; break;
case S_IFLNK: nslink++; break;
case S_IFSOCK: nsock++; break;
case S_IFDIR: /* directories should have type = FTW_D */
printf("for S_IFDIR for %s", pathname);
}
break;
case FTW_D:
ndir++;
break;
case FTW_DNR:
printf("can’t read directory %s", pathname);
break;
case FTW_NS:
printf("stat error for %s", pathname);
break;
default:
printf("unknown type %d for pathname %s", type, pathname);
}
return(0);
}

int main(int argc, char *argv[]){
    int ret;
    if (argc != 2)
        printf("usage: ftw <starting-pathname>");
    ret = myftw(argv[1], myfunc); /* does it all */
    ntot = nreg + ndir + nblk + nchr + nfifo + nslink + nsock;
    if (ntot == 0)
    ntot = 1; /* avoid divide by 0; print 0 for all counts */
    
    //printf("%d %d %d %d %d %d %d", nreg, ndir, nblk, nchr, nfifo, nslink, nsock);
    
    FILE * plotPipe;
    
    if ((plotPipe = popen("./my-histogram.py", "w")) == 0){
        fprintf(stderr, "Error opening plot pipe\n");
        return 1;
    }

    /* Write to the pipe */
    fprintf(plotPipe, "%d %d %d %d %d %d %d", nreg, ndir, nblk, nchr, nfifo, nslink, nsock);
    // make sure that the above string gets flushed to stdout
    fflush(plotPipe);
    
    /* Close the pipe */
    if (pclose(plotPipe) != 0) {
        fprintf(stderr, "Error closing pipe\n");
        exit(EXIT_FAILURE);
    }

    exit(ret);
}

static int /* we return whatever func() returns */
myftw(char *pathname, Myfunc *func)
{
fullpath = path_alloc(&pathlen); /* malloc PATH_MAX+1 bytes */
/* (Figure 2.16) */
if (pathlen <= strlen(pathname)) {
pathlen = strlen(pathname) * 2;
if ((fullpath = realloc(fullpath, pathlen)) == NULL)
perror("realloc failed");
}
strcpy(fullpath, pathname);
return(dopath(func));
}



/*
* Descend through the hierarchy, starting at "fullpath".
* If "fullpath" is anything other than a directory, we lstat() it,
* call func(), and return. For a directory, we call ourself
* recursively for each name in the directory.
*/
static int /* we return whatever func() returns */
dopath(Myfunc* func)
{
struct stat statbuf;
struct dirent *dirp;
DIR *dp;
int ret, n;
if (lstat(fullpath, &statbuf) < 0) /* stat error */
return(func(fullpath, &statbuf, FTW_NS));
if (S_ISDIR(statbuf.st_mode) == 0) /* not a directory */
return(func(fullpath, &statbuf, FTW_F));
/*
* It’s a directory. First call func() for the directory,
* then process each filename in the directory.
*/
if ((ret = func(fullpath, &statbuf, FTW_D)) != 0)
return(ret);
n=strlen(fullpath);
if (n + NAME_MAX + 2 > pathlen) { /* expand path buffer */
pathlen *= 2;
if ((fullpath = realloc(fullpath, pathlen)) == NULL)
perror("realloc failed");
}
fullpath[n++] = '/';
fullpath[n] = 0;
if ((dp = opendir(fullpath)) == NULL) /* can’t read directory */
return(func(fullpath, &statbuf, FTW_DNR));
while ((dirp = readdir(dp)) != NULL) {
if (strcmp(dirp->d_name, ".") == 0 ||
strcmp(dirp->d_name, "..") == 0)
continue; /* ignore dot and dot-dot */
strcpy(&fullpath[n], dirp->d_name); /* append name after "/" */
if ((ret = dopath(func)) != 0) /* recursive */
break; /* time to leave */
}
fullpath[n-1] = 0; /* erase everything from slash onward */
if (closedir(dp) < 0)
printf("can’t close directory %s", fullpath);
return(ret);
}

char *
path_alloc(size_t *sizep) /* also return allocated size, if nonnull */
{
char *ptr;
size_t size;
if (posix_version == 0)
posix_version = sysconf(_SC_VERSION);
if (xsi_version == 0)
xsi_version = sysconf(_SC_XOPEN_VERSION);
if (pathmax == 0) { /* first time through */
  errno = 0;
if ((pathmax = pathconf("/", _PC_PATH_MAX)) < 0) {
if (errno == 0)
pathmax = PATH_MAX_GUESS; /* it’s indeterminate */
else
perror("pathconf error for _PC_PATH_MAX");
} else {
pathmax++; /* add one since it’s relative to root */
}
}
/*                                                                                                                                                                                                          
* Before POSIX.1-2001, we aren’t guaranteed that PATH_MAX includes                                                                                                                                          
* the terminating null byte. Same goes for XPG3.                                                                                                                                                            
*/
if ((posix_version < 200112L) && (xsi_version < 4))
size = pathmax + 1;
else
size = pathmax;
if ((ptr = malloc(size)) == NULL)
perror("malloc error for pathname");
if (sizep != NULL)
*sizep = size;
return(ptr);
}
