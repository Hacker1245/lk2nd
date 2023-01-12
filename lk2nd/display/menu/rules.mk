# SPDX-License-Identifier: BSD-3-Clause
LOCAL_DIR := $(GET_LOCAL_DIR)

# The device module is required for the gpio-keys driver
MODULES += \
	lk2nd/device

OBJS += \
	$(LOCAL_DIR)/menu.o
