#include <stdio.h>
#include <stdlib.h>

#define INITIAL_TABLE_SIZE 10
#define EMPTY -1

typedef struct Node {
    int key;
    int value;
    struct Node* next;
} Node;

typedef struct {
    int size;
    int* keys;
    int* values;
    Node** chains;
} HashTable;


int mid_square_hash(int key, int size) {
    long long square = (long long)key * key;
    int mid = (square / 100) % 1000;
    return mid % size;
}

int division_hash(int key, int size) {
    return key % size;
}

int folding_hash(int key, int size) {
    int sum = 0;
    while (key > 0) {
        sum += key % 100;
        key /= 100;
    }
    return sum % size;
}

int digit_extraction_hash(int key, int size) {
    int extracted_digits = (key / 1000) % 10 * 10 + (key / 10) % 10;
    return extracted_digits % size;
}

int rotate_left(int num) {
    int last_digit = num % 10;
    num = num / 10;
    return last_digit * 1000 + num;
}

int rotation_hash(int key, int size) {
    key = rotate_left(key);
    return key % size;
}


void initialize(HashTable* ht, int size) {
    ht->size = size;
    ht->keys = (int*)malloc(size * sizeof(int));
    ht->values = (int*)malloc(size * sizeof(int));
    ht->chains = (Node**)malloc(size * sizeof(Node*));

    for (int i = 0; i < size; i++) {
        ht->keys[i] = EMPTY;
        ht->values[i] = 0;
        ht->chains[i] = NULL;
    }
}


void linear_probing_insert(HashTable* ht, int key, int value, int (*hash_function)(int, int)) {
    int index = hash_function(key, ht->size);

    while (ht->keys[index] != EMPTY) {
        index = (index + 1) % ht->size;
    }

    ht->keys[index] = key;
    ht->values[index] = value;
}


void chaining_insert(HashTable* ht, int key, int value, int (*hash_function)(int, int)) {
    int index = hash_function(key, ht->size);
    
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->key = key;
    newNode->value = value;
    newNode->next = ht->chains[index];

    ht->chains[index] = newNode;
}


void rehash(HashTable* ht, int (*hash_function)(int, int)) {
    int old_size = ht->size;
    int new_size = old_size * 2;

    int* old_keys = ht->keys;
    int* old_values = ht->values;
    Node** old_chains = ht->chains;

    initialize(ht, new_size);

    for (int i = 0; i < old_size; i++) {
        if (old_keys[i] != EMPTY) {
            linear_probing_insert(ht, old_keys[i], old_values[i], hash_function);
        }

        Node* current = old_chains[i];
        while (current) {
            chaining_insert(ht, current->key, current->value, hash_function);
            Node* temp = current;
            current = current->next;
            free(temp);
        }
    }

    free(old_keys);
    free(old_values);
    free(old_chains);

    printf("\nRehashed to new size: %d\n", new_size);
}

void display(HashTable* ht) {
    printf("\nHash Table:\n");
    for (int i = 0; i < ht->size; i++) {
        if (ht->keys[i] != EMPTY) {
            printf("Slot [%d]: (%d, %d)\n", i, ht->keys[i], ht->values[i]);
        } else {
            printf("Slot [%d]: empty\n", i);
        }

        Node* current = ht->chains[i];
        while (current) {
            printf("  -> (%d, %d)\n", current->key, current->value);
            current = current->next;
        }
    }
}

// Collision Handling Menu
void collision_menu(HashTable* ht, int key, int value, int (*hash_function)(int, int)) {
    int input2;
    do {
        printf("\nSelect collision handling:\n");
        printf("1. Linear Probing\n");
        printf("2. Chaining (Separate Chaining)\n");
        printf("3. Rehashing (Double Table Size)\n");
        printf("0. Exit\n");
        printf("Input: ");
        scanf("%d", &input2);

        if (input2 == 1) {
            linear_probing_insert(ht, key, value, hash_function);
            printf("Key %d inserted with value %d using Linear Probing.\n", key, value);
        } else if (input2 == 2) {
            chaining_insert(ht, key, value, hash_function);
            printf("Key %d inserted with value %d using Chaining.\n", key, value);
        } else if (input2 == 3) {
            rehash(ht, hash_function);
            linear_probing_insert(ht, key, value, hash_function);
        } else if (input2 == 0) {
            printf("Exiting collision handling menu.\n");
        } else {
            printf("Invalid option!\n");
        }
    } while (input2 != 0);
}

// Hashing Algorithm Selection Menu
void main_menu(HashTable* ht) {
    int input, key, value;
    do {
        printf("\nSelect hashing algorithm:\n");
        printf("1. Mid-Square\n");
        printf("2. Division\n");
        printf("3. Folding\n");
        printf("4. Digit Extraction\n");
        printf("5. Rotation\n");
        printf("0. Exit\n");
        printf("Input: ");
        scanf("%d", &input);

        if (input >= 1 && input <= 5) {
            printf("Enter key to insert: ");
            scanf("%d", &key);
            printf("Enter value: ");
            scanf("%d", &value);

            int (*hash_function)(int, int) = NULL;

            switch (input) {
                case 1: hash_function = mid_square_hash; break;
                case 2: hash_function = division_hash; break;
                case 3: hash_function = folding_hash; break;
                case 4: hash_function = digit_extraction_hash; break;
                case 5: hash_function = rotation_hash; break;
            }

            collision_menu(ht, key, value, hash_function);
        }

        display(ht);
    } while (input != 0);
}

int main() {
    HashTable hashtable;
    initialize(&hashtable, INITIAL_TABLE_SIZE);
    main_menu(&hashtable);
    return 0;
}
