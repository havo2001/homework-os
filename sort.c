#include <sys/time.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>



////////////////////////////////////////////////////////////////////////////////////////////////////
///                                             SORT                                             ///
////////////////////////////////////////////////////////////////////////////////////////////////////

typedef int(*comparator_ptr)(const void*, const void*);

int sort (void* buffer, size_t count, size_t type_size, comparator_ptr comparator);


////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          COMPARATORS                                         ///
////////////////////////////////////////////////////////////////////////////////////////////////////

size_t comparison_counter = 0;

int int_compare (const void* first_value, const void* second_value) {
    ++ comparison_counter;
    int first_int = *(const int*)first_value;
    int second_int = *(const int*)second_value;

    return
        first_int < second_int ? -1 :
        first_int == second_int ? 0 :
        1;
}

int string_compare (const void* first_value, const void* second_value) {
    ++ comparison_counter;
    return strcmp(*(const char**)first_value, *(const char**)second_value);
}


int main () {
    const size_t max_string_size = 32;

    char data_type[16];
    scanf("%s", data_type);

    const size_t type_size =
        strcmp(data_type, "int") == 0 ? sizeof(int) :
        strcmp(data_type, "string") == 0 ? sizeof(char*) :
        0;

    if (type_size == 0) {
        printf("TEST CRASHED: unsupported data type\n");
        return 1;
    }

    size_t count;
    scanf("%d", &count);

    void* const arr = malloc(count * type_size);

    for (size_t i = 0; i < count; i ++) {
        if (strcmp(data_type, "int") == 0) {
            int* const arr_casted = (int*)arr;
            scanf("%d", &arr_casted[i]);
        }

        else if (strcmp(data_type, "string") == 0) {
            char** const arr_casted = (char**)arr;
            arr_casted[i] = malloc(max_string_size);
            memset(arr_casted[i], 0, max_string_size);
            scanf("%s", arr_casted[i]);
        }
    }

    struct timeval end, start;
    gettimeofday(&start, NULL);

    sort(arr, count, type_size,
        strcmp(data_type, "int") == 0 ? &int_compare :
        strcmp(data_type, "string") == 0 ? &string_compare :
        NULL
    );

    gettimeofday(&end, NULL);
    const size_t elapsed_time = (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec;

    for (size_t i = 0; i < count; i ++) {
        if (strcmp(data_type, "int") == 0) {
            int* arr_casted = (int*)arr;
            printf("%d", arr_casted[i]);
        }

        else if (strcmp(data_type, "string") == 0) {
            char** arr_casted = (char**)arr;
            printf("%s", arr_casted[i]);
            free(arr_casted[i]);
        }

        if (i != count - 1)
            printf(" ");
    }

    printf("\n");

    printf("Execution time in mcs: %d\n", elapsed_time);
    printf("Comparisons: %d\n", comparison_counter);
    free(arr);

    return 0;
}
