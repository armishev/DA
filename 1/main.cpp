#include <stdio.h>
#include <stdlib.h>
#include <string.h>


const unsigned int SIZE_N = 20;
const unsigned int SIZE_V = 64;

struct Record {
    char phone[21];
    char* value;
    unsigned long long phone_long;
};


void radix_sort(struct Record arr[], int n) {
    const int W = sizeof(long long) * 8;
    const int RADIX = 256;
    struct Record temp[n];
    int *count = calloc(RADIX, sizeof(int));
    int i, k;
    for (k = 0; k < W; k += 8) {
        memset(count, 0, RADIX * sizeof(int));
        for (i = 0; i < n; i++)
            count[(arr[i].phone_long >> k) & 0xFF]++;
        for (i = 1; i < RADIX; i++)
            count[i] += count[i - 1];
        for (i = n - 1; i >= 0; i--)
            temp[--count[(arr[i].phone_long >> k) & 0xFF]] = arr[i];
        for (int i = 0; i < n; i++) {
            arr[i] = temp[i];
        }
    }
    free(count);

}




int main() {
    FILE *file = fopen("input.txt", "r");
    int n = 0;
    struct Record* pairs =  (struct Record*) malloc(1 * sizeof(struct Record));
    char* line = realloc(NULL, sizeof(char)*(SIZE_N+SIZE_V+2));
    char* key;
    char* value;
    while (fgets(line, sizeof(char)*(SIZE_N+SIZE_V+2), file)) {
        int lenline = strlen(line);
        if(lenline<=1){
            continue;
        }
        char* new_line = realloc(line, sizeof(char) * lenline + 1);
        key = strtok(new_line, "\t");
        value = strtok(NULL, "\n");
        pairs[n].value = realloc(NULL, sizeof(char) * strlen(value)+1);
        strcpy(pairs[n].phone, key);
        strcpy(pairs[n].value, value);
        char digits[SIZE_N+1];
        int j = 0;
        for (int i = 0; pairs[n].phone[i] != '\0'; i++) {
            if (pairs[n].phone[i] >= '0' && pairs[n].phone[i] <= '9') {
                digits[j] = pairs[n].phone[i];
                j++;
            }
        }
        digits[j] = '\0';

        pairs[n].phone_long = atoll(digits);
        ++n;
        pairs =  (struct Record*) realloc(pairs, (n + 1) * sizeof(struct Record));
        line = realloc(NULL, sizeof(char)*(SIZE_N+SIZE_V+2));
    }



    radix_sort(pairs, n);

    for (int i = 0; i < n; i++) {
        fprintf(stdout, "%s\t%s\n",  pairs[i].phone, pairs[i].value);
    }

    free(pairs);


    return 0;
}
