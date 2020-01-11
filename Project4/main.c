#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "os_hw4_driver.h"
#include <math.h>

int fd;

// Obtain my cr3 value (a.k.a. PML4 table physical address)
uint64_t get_cr3_value()
{
	struct ioctl_arg cmd;
	int ret;
	cmd.request[0] = IO_CR3;
	ret = ioctl(fd, IOCTL_REQUEST, &cmd);
	return cmd.ret;
}

// Given a physical address, return the value
uint64_t read_physical_address(uint64_t physical_address)
{
	struct ioctl_arg cmd;
	int ret;
	cmd.request[0] = IO_READ;
	cmd.request[1] = physical_address;
	ret = ioctl(fd, IOCTL_REQUEST, &cmd);
	return cmd.ret;
}

// Write value to a physical address
void write_physical_address(uint64_t physical_address, uint64_t value)
{
	struct ioctl_arg cmd;
	int ret;
	cmd.request[0] = IO_WRITE;
	cmd.request[1] = physical_address;
	cmd.request[2] = value;
	ret = ioctl(fd, IOCTL_REQUEST, &cmd);
}
// Get the specifically bits from an address
uint64_t get_bit_range(uint64_t addr, int low, int high){
    uint64_t result;
    if(high != 0) result = addr % ((uint64_t)1<<high);
    else result = addr;
    result /= ((uint64_t)1<<low);
    return result;
}

// calculate entry from entry content
uint64_t cal_entry(uint64_t entry_content, uint64_t offset){
    entry_content %= (uint64_t)pow(16.0, 9);
    entry_content /= (uint64_t)pow(16.0, 3);
    entry_content *= (uint64_t)pow(16.0, 3);
    entry_content += (offset * 8);
    return entry_content;
}

// calculate target physical address
uint64_t cal_phy(uint64_t target){
    uint64_t PML4 = get_bit_range(target, 39, 0),
             PDPT = get_bit_range(target, 30, 39),
             PD = get_bit_range(target, 21, 30),
             PT = get_bit_range(target, 12, 21),
             base = get_cr3_value();
     
    base = cal_entry(base, PML4);
    base = read_physical_address(base);
    base = cal_entry(base, PDPT);
    base = read_physical_address(base);
    base = cal_entry(base, PD);
    base = read_physical_address(base);
    base = cal_entry(base, PT);
    return base;
}

int main()
{
	char *x = (char*)aligned_alloc(4096, 4096) + 0x123;
	char *y = (char*)aligned_alloc(4096, 4096) + 0x123;
	strcpy(x, "This is OS homework 4.");
	strcpy(y, "You have to modify my page table.");

	fd = open("/dev/os", O_RDONLY);
	if(fd < 0) 
	{
		printf("Cannot open device!\n");
		return 0;
	}
    
	printf("Before\n");
	printf("x : %s\n", x);
	printf("y : %s\n", y);

	/* TODO 1 */
	// ------------------------------------------------
	// Modify page table entry of y
	// Let y point to x's physical address
	// ------------------------------------------------
    uint64_t vir_x = x, vir_y = y;
    uint64_t phy_x = cal_phy(vir_x), phy_y = cal_phy(vir_y);
    uint64_t original_y = phy_y;
    phy_x = read_physical_address(phy_x);
    phy_y = read_physical_address(phy_y);

    write_physical_address(original_y, phy_x);
    /* TODO 1 done */
	getchar();

	printf("After modifying page table\n");
	printf("x : %s\n", x);
	printf("y : %s\n", y);

	getchar();

	strcpy(y, "When you modify y, x is modified actually.");
	printf("After modifying string y\n");
	printf("x : %s\n", x);
	printf("y : %s\n", y);

	/* TODO 2 */
	// ------------------------------------------------
	// Recover page table entry of y
	// Let y point to its original address
	// You may need to store y's original address at previous step
	// ------------------------------------------------
    write_physical_address(original_y, phy_y);
    /* TODO 2 done */
	getchar();

	printf("After recovering page table of y\n");
	printf("x : %s\n", x);
	printf("y : %s\n", y);

	close(fd);
}
