#include"util.h"
// mman library to be used for hugepage allocations (e.g. mmap or posix_memalign only)
#include <sys/mman.h>
#include <time.h>
#define SAMPLES 10
 
uint64_t get_cache_set_index(ADDR_PTR phys_addr)
{
    uint64_t mask = ((uint64_t) 1 << 15) - 1;
    return (phys_addr & mask) >> 6;
}

int main(int argc, char *argv[]) {

 	int set_id[8] = {200, 152, 124, 109, 65, 44, 31, 20};	//Index for each bit.
 	uint64_t eviction_set[8][8];          	//Number of sets to probe * number of ways

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

 	int binary[8] = {0, 0, 0, 0, 0, 0, 0, 0};	//Store 8 bit number`
 
	//Prime L2
	for (int index = 0; index < 8; index++) {
          for (int way = 0; way < 8; way++){
              uint64_t *temp_pointer = (uint64_t *) eviction_set[index][way];
			  tmp = *temp_pointer;
	    }
   	}

	//Added delay to let sender access data
	start_time = clock();
	current_time = start_time;
	while (current_time - start_time < 10000000) current_time = clock();

	//Probe L2
	char ch;
	bool listening = true;

	for (int index = 0; index < 8; index++) {
		for (int way = 0; way < 8; way++){
        	start_time = clock();
            current_time = start_time;
	    	while (current_time - start_time < 200) {
            	int time = measure_one_block_access_time(eviction_set[index][way]);
	    
	  			// if (time < 700) {
            	if (time > 60) {
             		//misses[ind][way]++;
	     			binary[index] = 1;
	    		}
	   			//}	    
	    		else;
  				current_time = clock();      
      		}		    
		}
	}

	int secret_zero = 1;
	//Find the binary number based on time threshold calculation
  	for (int i = 1; i < 8; i++) {
    	if (binary[i]) secret_zero = 0;  
  	}

	if(secret_zero == 1) printf("Secret number: 0\n");
	else {
		int num = 0;
		int pow_2 = 1;
		for (int i = 0; i < 8; i++) {
			num += binary[i] * pow_2;
			pow_2 = 2 * pow_2;	
			//printf("Power of 2 is %d and binary number is %d\n", pow_2, binary[i]); 
		}
		printf("Secret number: %d\n", num);
	}
  	printf("Receiver finished.\n");
	
	return 0;
  
}


