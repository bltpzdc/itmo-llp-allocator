//
// Created by Honor on 26.11.2022.
//
#include "mem.h"
#include "tests.h"
static void print_error(char* message){
    fprintf(stderr, "%s", message);
}

static void print_info(char* message){
    fprintf(stdout, "%s", message);
}

static void unmap(void* heap, int bytes){
    munmap(heap, size_from_capacity((block_capacity){.bytes = bytes}).bytes);
}

static struct block_header* block_get_header(void* contents) {
    return (struct block_header*) (((uint8_t*)contents)-offsetof(struct block_header, contents));
}

void test1(void){
    print_info("----------Test #1----------");
    void* heap = heap_init(12345);
    if (!heap){
        print_error("Heap wasn't initialized.");
        return;
    }
    debug_heap(stdout, heap);
    void* someData = _malloc(1234);
    debug_heap(stdout, heap);
    if (!someData){
        print_error("Memory wasn't allocated.");
        return;
    }

    _free(someData);
    debug_heap(stdout, heap);
    unmap(heap, 12345);
    print_info("Success!");
}

void test2(void){
    print_info("----------Test #2----------");
    void* heap = heap_init(12345);
    if (heap == NULL){
        print_error("Heap wasn't initialized.");
        return;
    }
    debug_heap(stdout, heap);
    void* block1 = _malloc(1234);
    void* block2 = _malloc(1234);
    void* block3 = _malloc(1234);
    void* block4 = _malloc(1234);

    debug_heap(stdout, heap);

    if (!block1 || !block2 || !block3 || !block4){
        print_error("Memory wasn't allocated.");
        return;
    }

    _free(block2);

    if (!block1 || !block3 || !block4){
        print_error("Only the second block was released, but others were also affected.");
        return;
    }

    _free(block1);
    _free(block3);
    _free(block4);
    debug_heap(stdout, heap);

    unmap(heap, 12345);
    print_info("Success!");
}

void test3(void){
    print_info("----------Test #3----------");
    void* heap = heap_init(12345);
    if (heap == NULL){
        print_error("Heap wasn't initialized.");
        return;
    }
    debug_heap(stdout, heap);
    void* block1 = _malloc(1234);
    void* block2 = _malloc(1234);
    void* block3 = _malloc(1234);
    void* block4 = _malloc(1234);

    debug_heap(stdout, heap);

    if (!block1 || !block2 || !block3 || !block4){
        print_error("Memory wasn't allocated.");
        return;
    }

    _free(block2);
    _free(block3);

    if (!block1 || !block4){
        print_error("Second and third blocks were released, but others were also affected.");
        return;
    }

    _free(block1);
    _free(block4);
    debug_heap(stdout, heap);

    unmap(heap, 12345);
    print_info("Success!");
}

void test4(void){
    print_info("----------Test #4----------");
    void* heap = heap_init(1234);
    if (heap == NULL){
        print_error("Heap wasn't initialized.");
        return;
    }
    debug_heap(stdout, heap);

    void* block1 = _malloc(1024);
    void* block2 = _malloc(2056);

    debug_heap(stdout, heap);
    if (!block1 || !block2){
        print_error("Memory wasn't allocated.");
        return;
    }
    debug_heap(stdout, heap);

    struct block_header* header1 = block_get_header(block1);
    struct block_header* header2 = block_get_header(block2);

    if (header1->next != header2){
        print_error("Wrong allocation of regions");
        return;
    }

    _free(block1);
    _free(block2);
    debug_heap(stdout, heap);

    unmap(heap, 1234);
    print_info("Success!");
}

void test5(void){
    print_info("----------Test #5----------");
    void* heap = heap_init(1234);
    if (heap == NULL){
        print_error("Heap wasn't initialized.");
        return;
    }
    debug_heap(stdout, heap);

    void* block1 = _malloc(1024);
    if (!block1){
        print_error("Memory wasn't allocated for the first block.");
        return;
    }
    debug_heap(stdout, heap);
    struct block_header* header1 = block_get_header(block1);

    (void) mmap(header1->contents + header1->capacity.bytes, REGION_MIN_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_FIXED, -1, 0);

    void* block2 = _malloc(2048);
    if (!block2){
        print_error("Memory wasn't allocated for the second block.");
        return;
    }
    struct block_header* header2 = block_get_header(block2);

    if (header1->next != header2){
        print_error("Wrong allocation of regions");
        return;
    }

    munmap(header1->contents + header1->capacity.bytes, REGION_MIN_SIZE);

    _free(block1);
    _free(block2);
    debug_heap(stdout, heap);

    unmap(heap, 1234);
    print_info("Success!");
}