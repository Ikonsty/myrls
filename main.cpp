#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <iomanip>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include <boost/algorithm/string.hpp>

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

//  return owner of the file by using the ownerID
char* owner(uid_t ownerID){
    struct passwd *pw = getpwuid(ownerID);
    return pw->pw_name;
}

//  return group of the file by using the groupID
char* group(gid_t groupID){
    struct group *gr = getgrgid(groupID);
    return gr->gr_name;
}

//  return file size in bytes
char* size(off_t size){
    auto sizeval = static_cast<char *>(malloc(sizeof(char) * 9 + 1));
    sprintf(sizeval, "%ld", size);
    return sizeval;
}

//  return data of the last modification of the file in format YYYY-MM-DD
char* data(time_t time){
    auto timeval = static_cast<char *>(malloc(sizeof(char) * 9 + 1));
    strftime(timeval, 20, "%Y-%m-%d", localtime(&time));
    return timeval;
}

//  return time of the last modification of the file in format HH:MM:SS
char* time(time_t time){
    auto timeval = static_cast<char *>(malloc(sizeof(char) * 9 + 1));
    strftime(timeval, 20, "%H:%M:%S", localtime(&time));
    return timeval;
}

//  return if file is a directory, file or executable file
char* type(mode_t type){
    if (S_ISDIR(type)) return (char*)"d";
    if (S_ISREG(type)) return (char*)"-";
    if (S_ISLNK(type)) return (char*)"l";
    if (S_ISCHR(type)) return (char*)"c";
    if (S_ISBLK(type)) return (char*)"b";
    if (S_ISFIFO(type)) return (char*)"p";
    if (S_ISSOCK(type)) return (char*)"s";
    return (char*)"?";
}

// return name of the file or directory by knowing its type
// if it`s dirrectory, then add "/" at the begging of the name
// if it`s executable file, then add "*" at the begging of the name
// if it`s symbolic link, then add "@" at the begging of the name
// if it`s socket, then add "=" at the begging of the name
// if it`s pipe, then add "|" at the begging of the name
// else "?"
char* name(mode_t type, char* name, char* filePath, char **stack, int &stack_size){
    if (S_ISDIR(type)) {
        auto nameval = static_cast<char *>(malloc(sizeof(char) * 9 + 1));
        sprintf(nameval, "/%s", name);

        if (strcmp(name, ".") != 0) {
            if (strcmp(name, "..") != 0) {
                stack[stack_size] = filePath;
                stack_size++;
            }
        }

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
    if (S_ISLNK(type)) {
        auto nameval = static_cast<char *>(malloc(sizeof(char) * 9 + 1));
        sprintf(nameval, "@%s", name);
        return nameval;
    }
    if (S_ISCHR(type)) return name;
    if (S_ISBLK(type)) return name;
    if (S_ISFIFO(type)) {
        auto nameval = static_cast<char *>(malloc(sizeof(char) * 9 + 1));
        sprintf(nameval, "|%s", name);
        return nameval;
    }
    if (S_ISSOCK(type)) {
        auto nameval = static_cast<char *>(malloc(sizeof(char) * 9 + 1));
        sprintf(nameval, "=%s", name);
        return nameval;
    }
    return (char*)"?";
}

// return current dirrectory name
char* currentDir(){
    char* cwd = new char[1024];
    getcwd(cwd, sizeof(cwd));
    return cwd;
}

// to copy the string
char* copy_string(char s[])
{
    char* s2;
    s2 = (char*)malloc(strlen(s) + 1);

    strcpy(s2, s);
    return (char*)s2;
}

bool fileNameCmp (const std::vector<char*>& a, const std::vector<char*>& b) {
    std::string a_fl = std::string(a[0]);
    std::string b_fl = std::string(b[0]);
    const auto result = mismatch(a_fl.cbegin(), a_fl.cend(), b_fl.cbegin(), b_fl.cend(), [](const unsigned char a_fl, const unsigned char b_fl){return tolower(a_fl) == tolower(b_fl);});

    return result.second != b_fl.cend() && (result.first == a_fl.cend() || tolower(*result.first) < tolower(*result.second));
}

int main(int argc, char *argv[]) {
    DIR *dir;
    struct dirent *ent;
    struct stat buf;

    //initialize stack for directories
    char* stack[100];
    int stack_size = 0;

    // save name of the directory in the variable with FIFO queue structure
    auto dir_name = currentDir();
    // if there is no directory name in the command line, then use current directory
    if (argc == 1) {
        //add current directory to the stack
        stack[stack_size] = copy_string(dir_name);
        stack_size++;
        dir = opendir(dir_name);

    }
        // if there is directory name in the command line, then use it
    else if (argc == 2) {
        //add current directory to the stack
        stack[stack_size] = argv[1];
        stack_size++;

        dir = opendir(argv[1]);
        //dirName = argv[1];

    }
        // if there is more than one directory name in the command line, then print error
    else {
        std::cerr <<("Error: too many arguments") << std::endl;
        return 1;
    }
    // if directory is not found, then print error
    if (dir == NULL) {
        std::cerr << ("Error: directory not found") << std::endl;
        return 1;
    }

    // iterate through the stack while it is not empty
    while (stack_size > 0) {

        std::vector<std::vector<char*>> currentDirFiles;

        // get the last directory from the stack
        auto curr_dir_name = stack[stack_size - 1];
        // remove the last directory from the stack
        stack_size--;
        // open the directory
        dir = opendir(curr_dir_name);

        // show current directory name with ":" in the end
        std::cout << std::endl;
        std::cout << "./" << curr_dir_name << ":" << std::endl;

        // iterate through the directory

        while ((ent = readdir(dir)) != NULL) {
            // get the name of the file
            char* fileName = ent->d_name;

            std::vector<char*> fileInfo;

            // get the full path of the file
            char* filePath = (char*)malloc(strlen(curr_dir_name) + strlen(fileName) + 2);
            strcpy(filePath, curr_dir_name);
            strcat(filePath, "/");
            strcat(filePath, fileName);

            // get the information about the file
            stat(filePath, &buf);

            // print the information about the file
            stat(ent->d_name, &buf);
            //  check if buf is not empty
            if (buf.st_size != 0) {
                fileInfo.push_back(fileName);
                fileInfo.push_back(permissions(buf.st_mode));
                fileInfo.push_back(owner(buf.st_uid));
                fileInfo.push_back(size(buf.st_size));
                fileInfo.push_back(data(buf.st_mtime));
                fileInfo.push_back(time(buf.st_mtime));
                fileInfo.push_back(name(buf.st_mode, fileName, filePath, stack, stack_size));

                currentDirFiles.push_back(fileInfo);
            }
        }

        // sort items by fileName
        std::sort (currentDirFiles.begin(), currentDirFiles.end(), fileNameCmp);

        // print all information
        for(const auto& fi : currentDirFiles) {

            std::cout << std::setw(10) << std::left << fi[1]   //permissions
                      << std::setw(18) << std::left << fi[2]             //owner
                      << std::setw(8) << std::right << fi[3] << " "      //file size in bytes
                      << std::setw(12) << std::left
                      << fi[4]             //data of the last modification of the file
                      << std::setw(12) << std::left
                      << fi[5]             //time of the last modification of the file
                      << std::setw(12) << std::left
                      << fi[6] //file name //ent->d_name
                      << std::endl;
        }

        // close the directory
        closedir(dir);
    }
    return 0;
}
