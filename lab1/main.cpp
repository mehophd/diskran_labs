#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const int VALUE_SIZE = 64;
const int DATE_SIZE = 16;
const int START_CAPACITY = 16;
const int YEAR_COEFFICIENT = 10000;
const int MONTH_COEFFICIENT = 100;
const int CAPACITY_COEFFICIENT = 2;
const int DATE_FIELDS_COUNT = 3;
const int RADIX = 10;
const int BUFFER_SIZE = 256;


struct TRecord {
    int Key;
    char Date[DATE_SIZE];
    char Value[VALUE_SIZE];
};

struct TVector {
    TRecord *Data;
    int Size;
    int Capacity;
};

void InitTVector(TVector *v, int capacity) {
    v->Capacity = capacity;
    v->Size = 0;
    v->Data = (TRecord*)malloc(sizeof(TRecord) * capacity);

    if (v->Data == NULL) {
        exit(1);
    }
}

void FreeTVector(TVector *v) {
    if (v->Data) {
        free(v->Data);
        v->Data = NULL;
    }
    v->Size = 0;
    v->Capacity = 0;
}

void RecapacityTVector(TVector *v, int newCapacity) {
    if (newCapacity < 1) {
        newCapacity = 1;
    }

    TRecord *newData = (TRecord*)realloc(v->Data, sizeof(TRecord) * newCapacity);

    if (newData == NULL) {
        exit(1);
    }

    v->Data = newData;
    v->Capacity = newCapacity;
}

void PushTVector(TVector *v, const TRecord *rec) {
    if (v->Size >= v->Capacity) {
        RecapacityTVector(v, v->Capacity * CAPACITY_COEFFICIENT);
    }

    v->Data[v->Size] = *rec;
    v->Size++;
}

int ParseDate(const char *str, int *key) {
    int d = 0;
    int m = 0;
    int y = 0;

    if (sscanf(str, "%d.%d.%d", &d, &m, &y) != DATE_FIELDS_COUNT) {
        return 0;
    }

    *key = y * YEAR_COEFFICIENT + m * MONTH_COEFFICIENT + d;

    return 1;
}

int GetMaxKey(TRecord *arr, int n) {
    int maxKey = arr[0].Key;
    for (int i = 0; i < n; ++i) {
        if (arr[i].Key > maxKey) {
            maxKey = arr[i].Key;
        }
    }
    return maxKey;
}

void RadixSort(TRecord *arr, int n) {
    if (n <= 1) {
        return;
    }

    int maxKey = GetMaxKey(arr, n);

    TRecord *result = (TRecord*)malloc(sizeof(TRecord) * n);

    for (int exp = 1; maxKey / exp > 0; exp *= RADIX) {
        int count[RADIX];
        for (int i = 0; i < RADIX; ++i) {
            count[i] = 0;
        }

        for (int i = 0; i < n; ++i) {
            int digit = (arr[i].Key / exp) % RADIX;
            count[digit]++;
        }

        for (int i = 1; i < RADIX; ++i) {
            count[i] += count[i - 1];
        }

        for (int i = n - 1; i >= 0; --i) {
            int digit = (arr[i].Key / exp) % RADIX;
            result[count[digit] - 1] = arr[i];
            count[digit]--;
        }

        for (int i = 0; i < n; ++i) {
            arr[i] = result[i];
        }
    }

    free(result);
}

int main() {
    char buffer[BUFFER_SIZE];
    TVector records;
    InitTVector(&records, START_CAPACITY);

    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        if (buffer[0] == '\n' || buffer[0] == '\0') {
            continue;
        }

        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
            len--;
        }

        if (len > 0 && buffer[len - 1] == '\r') {
            buffer[len - 1] = '\0';
            len--;
        }

        char *tabPos = strchr(buffer, '\t');
        if (tabPos == NULL) {
            continue;
        }

        *tabPos = '\0';
        char *keyStr = buffer;
        char *valueStr = tabPos + 1;

        TRecord rec;
        memset(&rec, 0, sizeof(TRecord));

        if (!ParseDate(keyStr, &rec.Key)) {
            continue;
        }

        strncpy(rec.Date, keyStr, sizeof(rec.Date) - 1);
        rec.Date[sizeof(rec.Date) - 1] = '\0';
        memcpy(rec.Value, valueStr, VALUE_SIZE);

        PushTVector(&records, &rec);

    }

    RadixSort(records.Data, records.Size);

    for (int i = 0; i < records.Size; ++i) {
        printf("%s\t%.*s\n", records.Data[i].Date, VALUE_SIZE, records.Data[i].Value);
    }

    FreeTVector(&records);

    return 0;
}
