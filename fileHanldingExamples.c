#include <stdio.h>
#include <sys/stat.h>

int main() {
    int isPathExists;
    struct stat st;
    
    isPathExists = stat("/home", &st);
    printf("%d\n", isPathExists);

    isPathExists = stat("/home/aishu", &st);
    printf("%d\n", isPathExists);

    isPathExists = stat("/home/Documents", &st);
    printf("%d\n", isPathExists);

    isPathExists = stat("/cartoon", &st);
    printf("%d\n", isPathExists);

    return 0;
}