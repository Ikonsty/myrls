#include <iostream>
#include <dirent.h>
#include <sys/stat.h>

// implementation of the ls -il command in C++ using the dirent.h library and stat.h library
// to get the inode number, the file size of the file, owner and other data

int main(int argc, char *argv[]) {
    DIR *dir;
    struct dirent *ent;
    struct stat buf;
    if (argc == 1) {
        dir = opendir(".");
    } else {
        dir = opendir(argv[1]);
    }
    if (dir != NULL) {

        std::cout << "inode number | file size | ownerID | groupID | file name" << std::endl;
        while ((ent = readdir(dir)) != NULL) {
            stat(ent->d_name, &buf);
            // inode number, file size, ownerID, groupID, file name
            std::cout << buf.st_ino << " " << buf.st_size << " " << buf.st_uid << " " << buf.st_gid << " " << buf.st_mode << " " << buf << ent->d_name << std::endl;
        }
        closedir(dir);
    } else {
        std::cout << "Error: could not open directory" << std::endl;
    }
    return 0;
}

/*int main(int argc, char *argv[])
{
    DIR *dir;
    struct dirent *ent;
    struct stat buf;
    if ((dir = opendir (argv[1])) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            stat(ent->d_name, &buf);
            std::cout << ent->d_ino << " " << buf.st_size << " " << ent->d_name << std::endl;
        }
        closedir (dir);
    } else {
        perror ("");
        return EXIT_FAILURE;
    }
    return 0;
}*/
