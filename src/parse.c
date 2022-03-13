/**
 * Orginated file header below!
 * parse.c created by Bauer <bauer.github@gmail.com>
 *
 * \file test-parse.c
 * \brief Completely parse all files given on the command line.
 *
 * Copyright (C) 2007 Hans Ulrich Niedermann <gp@n-dimensional.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details. 
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA.
 *
 */

#include "libexif/exif-data.h"
#include "libexif/exif-system.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
char model[32];	int model_;
char make[32];	int make_;
char dt[32];	int dt_;
char dto[32];	int dto_;
char dtd[32];	int dtd_;

/** Callback function handling an ExifEntry. */
void content_foreach_func(ExifEntry *entry, void *callback_data);
void content_foreach_func(ExifEntry *entry, void *UNUSED(callback_data))
{
    char buf[2000];
    exif_entry_get_value(entry, buf, sizeof(buf));

    if (!strcmp(exif_format_get_name(entry->format), "ASCII")) {
        if (!strcmp(exif_tag_get_name(entry->tag), "Model")) {
            model_ = sprintf(model, "%s", exif_entry_get_value(entry, buf, sizeof(buf)));
        } else if (!strcmp(exif_tag_get_name(entry->tag), "Make")) {
            make_ = sprintf(make,  "%s", exif_entry_get_value(entry, buf, sizeof(buf)));
        } else if (!strcmp(exif_tag_get_name(entry->tag), "DateTime")) {
            dt_ = sprintf(dt,     "%s", exif_entry_get_value(entry, buf, sizeof(buf)));
        } else if (!strcmp(exif_tag_get_name(entry->tag), "DateTimeOriginal")) {
            dto_ = sprintf(dto,    "%s", exif_entry_get_value(entry, buf, sizeof(buf)));
        } else if (!strcmp(exif_tag_get_name(entry->tag), "DateTimeDigitized")) {
            dtd_ = sprintf(dtd,    "%s", exif_entry_get_value(entry, buf, sizeof(buf)));
        }
    }
}

/** Callback function handling an ExifContent (corresponds 1:1 to an IFD). */
void data_foreach_func(ExifContent *content, void *callback_data);
void data_foreach_func(ExifContent *content, void *callback_data)
{
//  printf("  Content %p: ifd=%d\n", content, exif_content_get_ifd(content));
  exif_content_foreach_entry(content, content_foreach_func, callback_data);
}

/** Run EXIF parsing test on the given file. */
int parse(const char *filename, char *str);
int parse(const char *filename, char *str)
{
	memset(model, 0, sizeof(model));
	memset(make, 0, sizeof(make));
	memset(dt, 0, sizeof(dt));
	memset(dto, 0, sizeof(dto));
	memset(dtd, 0, sizeof(dtd));
	model_ = 0;
	make_ = 0;
	dt_ = 0;
	dto_ = 0;
	dtd_ = 0;
 	ExifData *d;
//	printf("File %s\n", filename);
	d = exif_data_new_from_file(filename);
	exif_data_foreach_content(d, data_foreach_func, NULL);
	exif_data_unref(d);

	if (dto_) {
		strcat(str, dto);
	} else if (dtd_) {
		strcat(str, dtd);
	} else if (dt_) {
		strcat(str, dt);
	}
	if (make_) {
		strcat(str," ");
		strcat(str, make);
	}
	if (model_) {
		strcat(str," ");
		strcat(str, model);
	}

	if (model_ || make_ || dt_ || dto_ || dtd_)
		return sizeof(str);
	else
		return 0;		
}
