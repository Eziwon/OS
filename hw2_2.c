#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>


 int main(int argc, char *argv[]) {
    DIR *dir = NULL;
    struct dirent *ent = NULL;

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

        printf("%s, uid = %ld, gid = %ld, size = %ld, modified date = %ld\n", ent->d_name, (long)sb.st_uid, (long)sb.st_gid, (long)sb.st_size, sb.st_mtime);
    }

    // TO DO: close directory
    closedir(dir);

    return 0;
 }
 