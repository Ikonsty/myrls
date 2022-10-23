#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <iomanip>

// implementation of the ls -il command in C++ using the dirent.h library and stat.h library
// to get the inode number, the file size of the file, owner and other data

char* permissions(mode_t perm){
    auto modeval = static_cast<char *>(malloc(sizeof(char) * 9 + 1));
    modeval[0] = (perm & S_IRUSR) ? 'r' : '-';
    modeval[1] = (perm & S_IWUSR) ? 'w' : '-';
    modeval[2] = (perm & S_IXUSR) ? 'x' : '-';
    modeval[3] = (perm & S_IRGRP) ? 'r' : '-';
    modeval[4] = (perm & S_IWGRP) ? 'w' : '-';
    modeval[5] = (perm & S_IXGRP) ? 'x' : '-';
    modeval[6] = (perm & S_IROTH) ? 'r' : '-';
    modeval[7] = (perm & S_IWOTH) ? 'w' : '-';
    modeval[8] = (perm & S_IXOTH) ? 'x' : '-';
    modeval[9] = '\0';
    return modeval;
}

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

        std::cout << "permisions | file size | ownerID | groupID | inode number | time | file name" << std::endl;
        while ((ent = readdir(dir)) != NULL) {
            stat(ent->d_name, &buf);
            //show owner name by using getpwuid or ownerID


            // inode number, file size, ownerID, groupID, file name
            std::cout << std::setw(10) << std::left << permissions(buf.st_mode)  << std::setw(10) << std::left  << buf.st_size
                      << std::setw(10) << std::left  << buf.st_uid << std::setw(10) << std::left  << buf.st_gid
                      << std::setw(10) << std::left  << buf.st_ino << std::setw(10) << std::left  << buf.st_ctimespec.tv_sec
                      << std::setw(22) << std::right << ent->d_name << std::endl;
        }
        closedir(dir);
    } else {
        std::cout << "Error: could not open directory" << std::endl;
    }
    return 0;
}
