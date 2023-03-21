#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>


 int main(int argc, char *argv[]) {
    DIR *dir = NULL;
    struct dirent *ent = NULL;
    struct tm *lt;
    struct stat st;

    // TO DO: open current directory using opendir()
    dir = opendir(".");

    // TO DO:
	// 	repeat until there is more more directory entry
	//		read a directory entry using readdir()
	// 		get the file state using stat()
	//		convert the modified time into struct tm
	//		display the name, uid, gid, size, and modified date (year, month, day)
    if (dir == NULL) {
        printf("Could not open current directory");
        return 0;
    }

    while ((ent = readdir(dir)) != NULL) {
        stat(ent->d_name, &st);
        lt = localtime(&st.st_mtime);
        printf("%s, uid = %d, gid = %d, size = %d, modified date = %d/%d/%d\n", ent->d_name, (int)st.st_uid, (int)st.st_gid, (int)st.st_size, (int)lt->tm_year+1900, (int)lt->tm_mon+1, (int)lt->tm_mday);
    }

    // TO DO: close directory
    closedir(dir);
    return 0;
 }
 