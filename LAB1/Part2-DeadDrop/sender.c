#include"util.h"
// mman library to be used for hugepage allocations (e.g. mmap or posix_memalign only)
#include <sys/mman.h>
#include <math.h>
#include <time.h>

// TODO: define your own buffer size
#define BUFF_SIZE (1<< 21)

//Calculate Index based on virtual address
uint64_t get_cache_set_index(ADDR_PTR phys_addr) {
    uint64_t mask = ((uint64_t) 1 << 15) - 1;	//Block offset + index = 15 bits
    return (phys_addr & mask) >> 6;				//Block offset = 6 bits
}


int main(int argc, char **argv) {

	// Allocate a buffer using huge page
 	// See the handout for details about hugepage management
 	void *buf= mmap(NULL, BUFF_SIZE, PROT_READ | PROT_WRITE, MAP_POPULATE | MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);  //Allocate a Page of 2MB
  
 	if (buf == (void*) - 1) {
     	perror("mmap() error\n");
     	exit(EXIT_FAILURE);
	}

 	int set_id[8] = {200, 152, 124, 109, 65, 44, 31, 20};	//Index for each bit.
 	uint64_t eviction_set[8][8];          	//Number of sets to probe * number of ways

 	int b = 262144;

	//Buffer with L2 size
 	char *buffer = malloc((size_t) b);   

	//uint64_t addr = (uint64_t) (buffer);
	//printf("Buffer start address: %lx\n", addr);
 	uint64_t addr;
    // Construct the eviction_set by taking the addresses that have fixed cache set index
    // There should be 8*8 such addresses in our buffer

	clock_t start_time, current_time;       //Timer

	//Find the eviction Set
 	for (int index = 0; index < 8; index++) {
     	int count = 0;
     	for (int set = 0; set < 512; set++) {
       		for (int way = 0; way < 8; way++) {
	  			addr = (uint64_t) (buffer + set*512 + way*64);
           		if ( get_cache_set_index(addr) == set_id[index]) {
		 			eviction_set[index][count] = addr;
		 			count++;
	   			}
         	}	 
       	}
 	}


 	*((char *)buf) = 1;
 
 	char text_buf[128];

 	bool sending = true;
 	uint64_t *temp_pointer;
 	volatile char temp;

 	//Read the input and start eviction
 	while (sending) {
    	printf("Please type a message.\n");
    	fgets(text_buf, sizeof(text_buf), stdin);
    	int msg = string_to_int(text_buf);
    	int msg_binary[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    	int i = 0;
  
		for ( ;msg > 0; ){
			msg_binary[i++] = msg % 2;
			msg /= 2;
		}

		for (int index = 0; index < 8; index++) {
			if (msg_binary[index] == 0) {;}
			else {
				for (int way = 0; way < 8; way++){
					start_time = clock();
					current_time = start_time;
					while (current_time - start_time < 200000) {//For each line of particular set evict multiple times. 
						temp_pointer = (uint64_t *) eviction_set[index][way];
						temp = *temp_pointer;
	//          		measure_one_block_access_time(eviction_set[index][way]);
	//	 				clflush(eviction_set[index][way]);
		 
						current_time = clock();
					}
				}
		
			}
		} 
		printf("Sender Access End\n");
	}
 	return 0;
}

