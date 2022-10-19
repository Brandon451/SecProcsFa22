#include "util.h"

// mman library to be used for hugepage allocations (e.g. mmap or posix_memalign only)
#include <sys/mman.h>

#define BUFF_SIZE (1<<21)

uint64_t get_cache_set_index(ADDR_PTR phys_addr)
{
    uint64_t mask = ((uint64_t) 1 << 15) - 1;
    return (phys_addr & mask) >> 6;
}

int main(int argc, char *argv[]) {

	// Allocate a buffer using huge page
	// See the handout for details about hugepage management
	void *buf= mmap(NULL, BUFF_SIZE, PROT_READ | PROT_WRITE, MAP_POPULATE | MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB, -1, 0);

	if (buf == (void*) - 1) {
	 perror("mmap() error\n");
	 exit(EXIT_FAILURE);
	}

	int set_id[8] = {450, 100, 250, 180, 20, 90, 40, 8};
	uint64_t eviction_set[8][8];		//Number of sets to probe * Number of ways

	volatile char tmp; 

	int n = 8;//CACHE_WAYS_L3;
    int o = 6;                          // log_2(64), where 64 is the line size
    int s = 14;                         // log_2(16384), where 16384 is the number of cache sets
    int two_o = 64;             // 64
    int two_o_s = 512;   // 1048576
    int b = 262144;                // size in bytes of the LLC = 8,388,608

    // Allocate a buffer of the size of the LLC
    char *buffer = malloc((size_t) b);

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

	// The first access to a page triggers overhead associated with
	// page allocation, TLB insertion, etc.
	// Thus, we use a dummy write here to trigger page allocation
	// so later access will not suffer from such overhead.
	*((char *)buf) = 1; // dummy write to trigger page allocation

	// TODO:
	// Put your covert channel setup code here

	printf("Please type a message.\n");

	bool sending = true;
	while (sending) {
	  	char text_buf[128];
	  	fgets(text_buf, sizeof(text_buf), stdin);


		// Convert that message to binary
        //char *msg = string_to_binary(text_buf);

		// Convert that message to binary
        int msg = string_to_int(text_buf);
		int msg_bin[8] = {0, 0, 0, 0, 0, 0, 0, 0};
		int i = 0;
		for ( ;msg > 0; ){
		  msg_bin[i++] = msg % 2;
		  msg /= 2;
		}

		// Send the message bit by bit
        //size_t msg_len = strlen(msg);
		//printf("Message length: %ld\n", msg_len);
		//printf("Message: %s\n", msg);
        size_t msg_len = 8;
        for (int ind = 0; ind < msg_len; ind++) {
            if (msg_bin[ind] == 0) {
                ;//No access
            }
			else {
            printf("Sender is evicting for set %d\n",ind);
              for (int k = 0; k < 1000; k++) {
                //Access all 8 ways in set
				for (int way = 0; way < 8; way++){
					//printf("Set Address: %lx\n", eviction_set[ind][way]);
					uint64_t *temp_pointer = (uint64_t *) eviction_set[ind][way]; // cast x to a pointer (treat it as an address)
					tmp = *temp_pointer; // access the data pointed to by pointer y.
				}
			
           	}
            }
        }

 printf("Sender Access end\n");

		// TODO:
		// Put your covert channel code here
	}

	printf("Sender finished.\n");

	return 0;
}

