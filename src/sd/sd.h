
#include <hardware/regs/dreq.h>
#include <pico/stdio.h>
#include <pico/stdio_usb.h>
#include <pico/time.h>

#include "config.h"
#include "ff.h"

void init_sd_card();

void read_sd_file(char *filename, char *l_test);

void parse_sd_dir();
