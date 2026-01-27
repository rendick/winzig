#include <hardware/regs/dreq.h>
#include <pico/stdio.h>
#include <pico/stdio_usb.h>
#include <pico/time.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "ff.h"
#include "sd_card.h"

FRESULT fr;
FATFS fs;
FILINFO fno;
FIL fil;
int ret;
char buf[100];

void init_sd_card()
{
	if (!sd_init_driver()) {
		printf("ERROR: Could not initialize SD card\r\n");
	}

	fr = f_mount(&fs, "0:", 1);
	if (fr != FR_OK) {
		printf("ERROR: Could not mount filesystem (%d)\r\n", fr);
	}
}

void parse_sd_dir()
{
	DIR dir;
	FILINFO fno;
	FRESULT fr;

	fr = f_opendir(&dir, "0:");
	if (fr != FR_OK) {
		printf("ERROR: Could not open directory (%d)\n", fr);
	}

	do {
		f_readdir(&dir, &fno);
		if (fno.fname[0] != 0) {
			printf("File: %s\n", fno.fname);
		}
	} while (fno.fname[0] != 0);

	f_closedir(&dir);
}

void read_sd_file(char *filename, char *l_test)
{
	fr = f_open(&fil, filename, FA_READ);
	if (fr != FR_OK) {
		printf("ERROR: Could not open file (%d)\r\n", fr);
	}

	fr = f_close(&fil);
	if (fr != FR_OK) {
		printf("ERROR: Could not close file (%d)\r\n", fr);
	}

	fr = f_open(&fil, filename, FA_READ);
	if (fr != FR_OK) {
		printf("ERROR: Could not open file (%d)\r\n", fr);
	}

	printf("Reading from file: %s\n", filename);
	printf("---\n");
	while (f_gets(buf, sizeof(buf), &fil)) {
		strcat(l_test, buf);
	}
	printf("\r\n---\r\n");

	printf("LTEST: %s\n", l_test);

	fr = f_close(&fil);
	if (fr != FR_OK) {
		printf("ERROR: Could not close file (%d)\r\n", fr);
	}

	f_unmount("0:");
}

