#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <iomanip>
#include <pwd.h>
#include <grp.h>

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

//return owner of the file by using the ownerID
char* owner(uid_t ownerID){
    struct passwd *pw = getpwuid(ownerID);
    return pw->pw_name;
}

//return group of the file by using the groupID
char* group(gid_t groupID){
    struct group *gr = getgrgid(groupID);
    return gr->gr_name;
}

//return file size in bytes
char* size(off_t size){
    auto sizeval = static_cast<char *>(malloc(sizeof(char) * 9 + 1));
    sprintf(sizeval, "%ld", size);
    return sizeval;
}

//return data of the last modification of the file in format YYYY-MM-DD
char* data(time_t time){
    auto timeval = static_cast<char *>(malloc(sizeof(char) * 9 + 1));
    strftime(timeval, 20, "%Y-%m-%d", localtime(&time));
    return timeval;
}

//return time of the last modification of the file in format HH:MM:SS
char* time(time_t time){
    auto timeval = static_cast<char *>(malloc(sizeof(char) * 9 + 1));
    strftime(timeval, 20, "%H:%M:%S", localtime(&time));
    return timeval;
}

//return if file is a directory, file or executable file
char* type(mode_t type){
    if (S_ISDIR(type)) return "d";
    if (S_ISREG(type)) return "-";
    if (S_ISLNK(type)) return "l";
    if (S_ISCHR(type)) return "c";
    if (S_ISBLK(type)) return "b";
    if (S_ISFIFO(type)) return "p";
    if (S_ISSOCK(type)) return "s";
    return "?";
}

//return name of the file or directory by knowing its type
// if it`s dirrectory, then add "/" at the begging of the name
// if it`s executable file, then add "*" at the begging of the name
char* name(mode_t type, char* name){
    if (S_ISDIR(type)) {
        auto nameval = static_cast<char *>(malloc(sizeof(char) * 9 + 1));
        sprintf(nameval, "/%s", name);
        return nameval;
    }
    if (S_ISREG(type)) {
        if (type & S_IXUSR) {
            auto nameval = static_cast<char *>(malloc(sizeof(char) * 9 + 1));
            sprintf(nameval, "*%s", name);
            return nameval;
        }
        return name;
    }
    if (S_ISLNK(type)) return name;
    if (S_ISCHR(type)) return name;
    if (S_ISBLK(type)) return name;
    if (S_ISFIFO(type)) return name;
    if (S_ISSOCK(type)) return name;
    return "?";
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
        while ((ent = readdir(dir)) != NULL) {
            stat(ent->d_name, &buf);
            //show all data about file lilke ls -l command
            std::cout << std::setw(10) << std::left << permissions(buf.st_mode)   //permissions
                        << std::setw(12) << std::left  << owner(buf.st_uid)             //owner
                        << std::setw(8) << std::right  <<  size(buf.st_size ) << " "                //file size in bytes
                        << std::setw(12) << std::left << data(buf.st_mtime)                 //data of the last modification of the file
                        << std::setw(12) << std::left  << time(buf.st_mtime)             //time of the last modification of the file
                        << std::setw(12) << std::left << name(buf.st_mode, ent->d_name) << std::endl;      //file name //ent->d_name
        }
        closedir(dir);
    } else {
        std::cout << "Error: could not open directory" << std::endl;
    }
    return 0;
}
