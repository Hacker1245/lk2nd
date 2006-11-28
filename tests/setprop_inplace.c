/*
 * libfdt - Flat Device Tree manipulation
 *	Testcase for fdt_setprop_inplace()
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <fdt.h>
#include <libfdt.h>

#include "tests.h"
#include "testdata.h"

int main(int argc, char *argv[])
{
	struct fdt_header *fdt;
	uint32_t *intp;
	char *strp, *xstr;
	int xlen, i;
	int err;

	test_init(argc, argv);
	fdt = load_blob_arg(argc, argv);

	intp = check_getprop_typed(fdt, 0, "prop-int", TEST_VALUE_1);

	verbose_printf("Old int value was 0x%08x\n", *intp);
	err = fdt_setprop_inplace_typed(fdt, 0, "prop-int", ~TEST_VALUE_1);
	if (err)
		FAIL("Failed to set \"prop-int\" to 0x08%x: %s",
		     ~TEST_VALUE_1, fdt_strerror(err));
	intp = check_getprop_typed(fdt, 0, "prop-int", ~TEST_VALUE_1);
	verbose_printf("New int value is 0x%08x\n", *intp);
	
	strp = check_getprop(fdt, 0, "prop-str", strlen(TEST_STRING_1)+1,
			     TEST_STRING_1);

	verbose_printf("Old string value was \"%s\"\n", strp);
	xstr = strdup(strp);
	xlen = strlen(xstr);
	for (i = 0; i < xlen; i++)
		xstr[i] = toupper(xstr[i]);
	err = fdt_setprop_inplace(fdt, 0, "prop-str", xstr, xlen+1);
	if (err)
		FAIL("Failed to set \"prop-str\" to \"%s\": %s",
		     xstr, fdt_strerror(err));

	strp = check_getprop(fdt, 0, "prop-str", xlen+1, xstr);
	verbose_printf("New string value is \"%s\"\n", strp);	

	PASS();
}
