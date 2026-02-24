#include <hardware/regs/dreq.h>
#include <pico/stdio.h>
#include <pico/stdio_usb.h>
#include <pico/time.h>
#include <stdio.h>
#include <string.h>

#include "f_util.h"

#include "config.h"
#include "ff.h"
#include "hw_config.h"
#include "sd_card.h"

FRESULT fr;
FATFS fs;
FILINFO fno;
FIL fil;
int ret;
char buf[512];

sd_card_t *pSD = sd_get_by_num(0);
void init_sd_card()
{
	if (!sd_init_driver()) {
		printf("SD init failure\n");
		while (1)
			;
	}
	sleep_ms(250);

	FRESULT fr = f_mount(&pSD->fatfs, pSD->pcName, 1);
	if (fr != FR_OK) {
		printf("1: ERROR: %s (%d)\r\n", FRESULT_str(fr), fr);
		return;
	}
}

void parse_sd_dir(char exist_files[][128], int *count)
{
	DIR dir;
	FILINFO fno;
	FRESULT fr;

	fr = f_opendir(&dir, "0:");
	if (fr != FR_OK) {
		printf("2: ERROR: %s (%d)\r\n", FRESULT_str(fr), fr);
	}

	int i = 0;
	do {
		f_readdir(&dir, &fno);
		if (fno.fname[0] != 0) {
			printf("File: %s\n", fno.fname);
			strcpy(exist_files[i], fno.fname);
			i++;
		}
	} while (fno.fname[0] != 0);

	f_closedir(&dir);
	*count = i;
}

void read_sd_file(char *filename, char *l_test)
{
	l_test[0] = '\0';
	buf[0] = '\0';
	fr = f_open(&fil, filename, FA_READ);
	if (fr != FR_OK) {
		printf("3: ERROR: %s (%d)\r\n", FRESULT_str(fr), fr);
		return;
	}

	while (f_gets(buf, sizeof(buf), &fil)) {
		strcat(l_test, buf);
	}


	f_close(&fil);

	printf("LTEST: %s\n", l_test);
}
