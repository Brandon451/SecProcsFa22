#include "util.h"

// mman library to be used for hugepage allocations (e.g. mmap or posix_memalign only)
#include <sys/mman.h>
#include <stdio.h>
#include <time.h>

uint64_t get_cache_set_index(ADDR_PTR phys_addr)
{
    uint64_t mask = ((uint64_t) 1 << 15) - 1;
    return (phys_addr & mask) >> 6;
}

int main(int argc, char *argv[]) {

	int set_id[8] = { 450, 100, 250, 180, 20, 90, 40, 8};

	uint64_t eviction_set[8][8];		//Number of sets to probe * Number of ways

	int n = 8;//CACHE_WAYS_L3;
    int o = 6;                          // log_2(64), where 64 is the line size
    int s = 14;                         // log_2(16384), where 16384 is the number of cache sets
    int two_o = 64;             // 64
    int two_o_s = 512;   // 1048576
    int b = 262144;                // size in bytes of the LLC = 8,388,608

    clock_t start_time,current_time;
    // Allocate a buffer of the size of the LLC
    char *buffer = malloc((size_t) b);

	volatile char tmp;
    volatile char input;

	uint64_t addr = (uint64_t) (buffer);
	printf("Buffer start address: %lx\n", addr);
    // Construct the eviction_set by taking the addresses that have cache set index 0
    // There should be 128 such addresses in our buffer: one per line per cache set 0 of each slice (8 * 16).
	for (int set_id_index = 0; set_id_index < 8; set_id_index++) {
		int address_count = 0;
		for (int set_index = 0; set_index < 512; set_index++) {
			for (int way_index = 0; way_index < 8; way_index++) {

				uint64_t addr = (uint64_t) (buffer + set_index * two_o_s + way_index * two_o);

				if (get_cache_set_index(addr) == set_id[set_id_index]) {
					//printf("Set id: %x, Set Address: %lx\n", set_id[set_id_index], addr);
					eviction_set[set_id_index][address_count] = addr;
					address_count++;
				}
			}
		}
    }

	// Put your covert channel setup code here

	for (int repeat=0; repeat < 1000; repeat++) {
      for (int ind = 0; ind < 8; ind++) {
		for (int way = 0; way < 8; way++){
			uint64_t *temp_pointer = (uint64_t *) eviction_set[ind][way]; // cast x to a pointer (treat it as an address)
			tmp = *temp_pointer; // access the data pointed to by pointer y.
		}
   }	
}

	//for (int way = 0; way < 8; way++){
	//	int time = measure_one_block_access_time(eviction_set[ind][way]);
	////	printf("Address: %d, Time for access: %d\n", way, time);
	//}

//	printf("Please press enter.\n");
//
    //char text_buf;
	//fgets(text_buf, sizeof(text_buf), stdin);
//    getchar();

	//for (int way = 0; way < 8; way++){
	//	int time = measure_one_block_access_time(eviction_set[ind][way]);
	////	printf("Address: %d, Time for access: %d\n", way, time);
	//}
    
    start_time = clock();

    current_time = start_time;         
	while (( current_time - start_time) < 10000000) {
		current_time = clock();
}

printf("Prime is done\n");
    start_time = clock();

    current_time = start_time;         
	while (( current_time - start_time) < 10000000) {
		current_time = clock();
}
printf("Done waiting\n");
//
//	printf("Please press enter.\n");

	//char text_buf[2];
	//fgets(text_buf, sizeof(text_buf), stdin);

//	printf("Please press enter.\n");
   // input =  getchar();
//    getchar();
//	for (int way = 0; way < 8; way++){
//		int time = measure_one_block_access_time(eviction_set[ind][way]);
//	//	printf("Address: %d, Time for access: %d\n", way, time);
//	}
	//printf("Receiver now listening.\n");
   size_t msg_len = 8;  
	bool listening = true;
    start_time = clock();

    current_time = start_time;         

	//while (listening && (current_time - start_time) < 10000000) {
	while (listening) {

      for (int ind = 0; ind < 4; ind++) {
		// Put your covert channel code here
		for (int way = 0; way < 8; way++){
			int time = measure_one_block_access_time(eviction_set[ind][way]);
         printf("Address: %d, Time for access: %d for set %d is\n", way, time, ind);
			//uint64_t *temp_pointer = (uint64_t *) eviction_set[ind][way]; // cast x to a pointer (treat it as an address)
			//tmp = *temp_pointer; // access the data pointed to by pointer y.

    current_time = clock();         
           listening = false;
}
		}
	        //getch();

	//listening = false;

	}

	printf("Receiver finished.\n");

	return 0;
}

