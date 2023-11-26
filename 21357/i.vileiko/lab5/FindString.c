#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct Line {
    off_t offset;
    off_t length;
} Line;

typedef struct Vector {
    Line* array;
    int count;
    int capacity;
} Vector;

void vector_init(Vector* vec) {
    vec->array = malloc(sizeof(Line));
    vec->count = 0;
    vec->capacity = 1;
}

void vector_pushBack(Vector* vec, Line elem) {
    if (vec->count == vec->capacity) {
        vec->capacity = vec->capacity * 2;
        vec->array = realloc(vec->array, vec->cap * sizeof(Line));

        if (vec->array == NULL) {
            perror("Error: Realloc has not done!");

            exit(1);
        }
    }

    vec->array[vec->count++] = elem;
}

void vector_destroy(Vector* vec) {
    free(vec->array);
    vec->array = NULL;
    vec->count = vec->capacity = 0;
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        printf("No filename was provided\n");
        return 1;
    }

    Vector* table = malloc(sizeof(Vector));
    vector_init(table);

    int fd = open(argv[1], O_RDONLY);

    if (fd == -1) {
        perror("Error: Couldnt open the file!");
        exit(1);
    }

    lseek(fd, 0LL, SEEK_SET);

    char c;
    off_t offset = 0,
	  length = 0,
    	  max_length = 0;

    while (read(fd, &c, 1) == 1) {

        if (c == '\n') {
            Line current = {offset, length};
            vector_pushBack(table, current);

            if (length > max_length) {
                max_length = length;
	    }

            offset += length + 1;
            length = 0;
        }
        else {
            length++;
        }
    }

    if (length > 0) {
        Line current = {offset, length};
        vector_pushBack(table, current);
    }

    Line line_info;
    char* str = malloc(max_length + 2 * sizeof(char));
    int n = 0;

    while (1) {
        printf("Enter the line number: \n");
        if (scanf("%d", &n) != 1) {
            perror("Error: Incorrect input!");

            exit(1);
        }

        if (n == 0) {
            break;
        }

        if ((table->count) < n) {
            printf("The file contains only %d lines! \n", table->count);
            continue;
        }

        line_info = table->array[n - 1];

        lseek(fd, line_info.offset, SEEK_SET);
        read(fd, str, line_info.length * sizeof(char));
        str[line_info.length] = 0;

        printf("%s\n", str);
    }

    close(fd);
    free(str);
    vector_destroy(table);

    return 0;
}
