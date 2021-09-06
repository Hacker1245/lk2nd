// SPDX-License-Identifier: GPL-2.0+
// © 2019 Mis012

#include <app.h>

#include <stdio.h>
#include <string.h>

#include <dev/fbcon.h>
#include <kernel/mutex.h>

#include <target.h>

#include "aboot/aboot.h"

#include "boot.h"
#include "config.h"
#include "menu.h"

struct global_config global_config = {0};
static int num_of_boot_entries;

bool FUGLY_boot_to_default_entry = 0;
extern bool boot_into_recovery;

void reboot_init(const struct app_descriptor *app)
{
	int ret;

	bool boot_into_fastboot = aboot_init();
	bool pon_reason_is_charger = target_pause_for_battery_charge();

	ret = parse_global_config("hd1p25", &global_config);
	if(ret < 0) {
		printf("falied to parse global config: %d\n", ret);

		global_config.default_entry_title = NULL;
		global_config.charger_entry_title = NULL;
		global_config.timeout = 0;
	}

	struct boot_entry *entry_list = NULL;
	ret = num_of_boot_entries = parse_boot_entries("hd1p25", &entry_list);
	if (ret < 0) {
		printf("falied to parse boot entries: %d\n", ret);
		num_of_boot_entries = 0;
	}

	if(pon_reason_is_charger && global_config.charger_entry_title) {
		global_config.default_entry = get_entry_by_title(entry_list, num_of_boot_entries, global_config.charger_entry_title);
		if(!global_config.default_entry && !boot_into_fastboot) { // attempt to use legacy boot instead
			boot_into_recovery = 0;
			boot_linux_from_mmc();
		}
	}
	else if(global_config.default_entry_title) {
		global_config.default_entry = get_entry_by_title(entry_list, num_of_boot_entries, global_config.default_entry_title);
	}
	else {
		global_config.default_entry = NULL;
	}

	if (!boot_into_fastboot && global_config.default_entry) {
		FUGLY_boot_to_default_entry = 1;
	}

	start_fastboot();

	thread_t *thread = thread_create("menu_thread", &menu_thread, &( (struct {struct boot_entry *entry_list; int num_of_boot_entries;}){entry_list, num_of_boot_entries} ), DEFAULT_PRIORITY, DEFAULT_STACK_SIZE);
	if (thread)
		printf("thread_resume ret: %d\n", thread_resume(thread));
	else
		printf("`thread_create` failed\n");
}

APP_START(reboot)
	.init = reboot_init,
APP_END
