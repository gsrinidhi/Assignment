#include "testdriver.h"

void testdriverfn(void) {

    int fd = open("/dev/pixxelDevice0", O_RDWR, 0644);
    if(fd < 0) {
        printf("Error opening file\n");
    }

    char buf[4];

    ssize_t ret = write(fd, "1234", 4);
    if (ret < 0) {
        perror("write");
    }

    ret = read(fd, buf, 4);
    if (ret < 0) {
        perror("read");
    }

    printf("Read data from file: %s\n",buf);

    close(fd);
}

int main(void) {
    testdriverfn();
    return 0;
}
