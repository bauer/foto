/**
 * Copyright (C) 2017 Marcus Bauer <bauer.github@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_PATH_SZ 512

#define FILE_RENAME_ERROR (-1)
#define FILE_RENAME_OK    ( 0)
#define FILE_RENAME_SKIP  ( 1)

#define OPT_E_JPG (0)
#define OPT_E_AVI (1)
#define OPT_E_MP4 (2)
#define OPT_E_THM (3)
#define OPT_M_JPG (4)
#define OPT_M_MOV (5)
#define OPT_SIZE  (6)

extern int parse(const char *filename, char *str);

int set_newname(const char *oldname, char *mtime, char *size)
{
	struct stat sb;
	char s[200];
	size_t max = sizeof(s);
	struct tm *tm;
	/* RFC 2822-compliant date format */
	//const char *fmt = "%a, %d %b %Y %T %z";
	/* RFC 822-compliant date format */
	//const char *fmt = "%a, %d %b %y %T %z";
	/* My foto format 2000-01-01 02.17.29 */
	const char *fmt = "%Y-%m-%d %H.%M.%S";

	if (stat(oldname, &sb) == -1) {
		perror("stat");
		return -1;
	}
	tm = localtime(&sb.st_mtime);
	if (tm == NULL) {
		perror("localtime");
		return -2;
	}

	if (strftime(s, max, fmt, tm) == 0) {
		fprintf(stderr, "strftime returned 0");
		return -3;
	}
	/* time of last modification */
	strcat(mtime, s);

	/* total size, in bytes */
	if (sb.st_size < 1000)
		sprintf(size, "(%ldB)", sb.st_size);
	else if (sb.st_size < 1000 * 1000)
		sprintf(size, "(%ldKB)", (sb.st_size/1000));
	else if (sb.st_size < 1000 * 1000 *1000)
		sprintf(size, "(%ldMB)", (sb.st_size/(1000*1000)));
	else
		sprintf(size, "(%ldGB)", (sb.st_size/(1000*1000*1000)));
	return 0;
}

int rename_it(char *path, const char *name, int *opt)
{
	int rval = FILE_RENAME_OK;
	int exif_only = 0;
	char end[5] = "";
#ifdef TEST
	const char *tag = "Test-";
#else
	const char *tag = NULL;
#endif
	char newname_exif[128] = "";
	char newname[MAX_PATH_SZ];
	char oldname[MAX_PATH_SZ];
	size_t pathn = strlen(path);
	size_t namen = strlen(name);
	size_t tagn = 0;
	if (tag != NULL)
		tagn = strlen(tag);
#ifdef DEBUG	
	printf("\nrename: %s/%s\n", path, name);
#endif
	if ((opt[OPT_E_JPG] || opt[OPT_M_JPG]) && (name[namen-4] == '.') &&
		((name[namen-3] == 'j') || (name[namen-3] == 'J')) &&
		((name[namen-2] == 'p') || (name[namen-2] == 'P')) &&
		((name[namen-1] == 'g') || (name[namen-1] == 'G'))) {
		strcat(end, ".jpg");
	} else 	if ((opt[OPT_E_MP4]) && (name[namen-4] == '.') &&
		((name[namen-3] == 'm') || (name[namen-3] == 'M')) &&
		((name[namen-2] == 'p') || (name[namen-2] == 'P')) &&
		((name[namen-1] == '4') || (name[namen-1] == '4'))) {
		strcat(end, ".mp4");
	} else 	if ((opt[OPT_E_AVI]) && (name[namen-4] == '.') &&
		((name[namen-3] == 'a') || (name[namen-3] == 'A')) &&
		((name[namen-2] == 'v') || (name[namen-2] == 'V')) &&
		((name[namen-1] == 'i') || (name[namen-1] == 'I'))) {
		strcat(end, ".avi");
	} else 	if ((opt[OPT_E_THM]) && (name[namen-4] == '.') &&
		((name[namen-3] == 't') || (name[namen-3] == 'T')) &&
		((name[namen-2] == 'h') || (name[namen-2] == 'H')) &&
		((name[namen-1] == 'm') || (name[namen-1] == 'M'))) {
		strcat(end, ".thm");
	} else 	if ((opt[OPT_M_MOV]) && (name[namen-4] == '.') &&
		((name[namen-3] == 'm') || (name[namen-3] == 'M')) &&
		((name[namen-2] == 'o') || (name[namen-2] == 'O')) &&
		((name[namen-1] == 'v') || (name[namen-1] == 'V'))) {
		strcat(end, ".mov");		
	} else {
		rval = FILE_RENAME_SKIP;
		goto exit;
	}

	/* add path oldname */
	if (pathn + namen + 2 < MAX_PATH_SZ) {
		strcpy(oldname, path);
		strcat(oldname, "/");
		strcat(oldname, name);
	} else {
		rval = FILE_RENAME_ERROR;
		goto exit;
	}

	/* add path newname */
	if (pathn + tagn + 2 < MAX_PATH_SZ) {
		strcpy(newname, path);
		strcat(newname, "/");
		if (tag != NULL)
			strcat(newname, tag);
	} else {
		rval = FILE_RENAME_ERROR;
		goto exit;
	}

	/* newname from exif or from time, without ':' in name */
	if (parse(oldname, newname_exif)) {
		int i;
		for (i=0; i<sizeof(newname_exif); i++) {
			if (newname_exif[i] == ':') {
				if (exif_only < 2 )
					newname_exif[i] = '-';
				else
					newname_exif[i] = '.';
				exif_only++;
			}
		}
	}

	if (exif_only) {
		/* protect against unset date cameras with 0000-00-00 00.00.00 date */
		if (newname_exif[0] == '0')  {
			printf("Skip '%s', invalid new name with zeros\n", oldname);
			rval = FILE_RENAME_SKIP;
			goto exit;
		}
		/* use the exif name as the new name */
		strcat(newname, newname_exif);
	} else {
		/* protection, -mjpg required if jpg */
		if ((!opt[OPT_M_JPG]) && (!strcmp(end, ".jpg"))) {
			printf("Skip '%s', -mjpg not set\n", oldname);
			rval = FILE_RENAME_SKIP;
			goto exit;
		}
		char mtime[128] = "";
		char size[128] = "";
		set_newname(oldname, mtime, size);
		strcat(newname, mtime);
		strcat(newname, newname_exif);
		strcat(newname, size);
	}
	strcat(newname, end);

	if (!strcmp(oldname, newname)) {
		printf("Skip '%s', identical old and new name\n", oldname);
		rval = FILE_RENAME_SKIP;
		goto exit;
	}

	rval = rename(oldname, newname);
	if (rval) {
		perror("Rename error");
		printf("Renamed failed, '%s' to '%s'\n", oldname, newname);
		goto exit;
	}
	printf("'%s' ->\n'%s'\n", oldname, newname);
exit:
#ifdef DEBUG
	printf("rval: %d", rval);
#endif
	return rval;
}

int main(int argc, char *argv[])
{
	clock_t start, end;
	time_t start_time, end_time;
 	double ct;
	int rval;
	int fr_total_nr = 0;
	int fr_error_nr = 0;
	int fr_ok_nr    = 0;
	int fr_skip_nr  = 0;
	int opt[OPT_SIZE] = {0, 0, 0, 0, 0};
	DIR *my_dir;
	struct dirent *my_dirent;

	if ((argc == 1) || (argc > 7)) {
		printf("Usage: %s \"path to your photo directory\" <options>\n", argv[0]);
		printf(" Options available: \t\t\t\t\t\tExample:\n");
		printf(" -e<file extension to be included in rename operation>\t\t-ejpg -eavi -emp4 -ethm\n");
		printf(" -m<use default data mtime & size if picture data fails>\t-mjpg -mmov\n");
		return 0;
	} else if (argc == 2) {
			/* Default options if none are selected, only jpg from date pic taken */
			opt[OPT_E_JPG] = 1;
	} else if (argc > 2) {
		int i;
		for (i=2;i<argc;i++){
			if (!strcmp(argv[i], "-ejpg")) { opt[OPT_E_JPG] = 1; }
			if (!strcmp(argv[i], "-eavi")) { opt[OPT_E_AVI] = 1; }
			if (!strcmp(argv[i], "-emp4")) { opt[OPT_E_MP4] = 1; }
			if (!strcmp(argv[i], "-ethm")) { opt[OPT_E_THM] = 1; }
			if (!strcmp(argv[i], "-mjpg")) { opt[OPT_M_JPG] = 1; }						
			if (!strcmp(argv[i], "-mmov")) { opt[OPT_M_MOV] = 1; }									
		}
	}
	printf(" options selected: ");
	printf(" %s", opt[OPT_E_JPG] < 1 ? "" : "-ejpg");
	printf(" %s", opt[OPT_E_AVI] < 1 ? "" : "-eavi");
	printf(" %s", opt[OPT_E_MP4] < 1 ? "" : "-emp4");
	printf(" %s", opt[OPT_E_THM] < 1 ? "" : "-ethm");
	printf(" %s", opt[OPT_M_JPG] < 1 ? "" : "-mjpg");
	printf(" %s", opt[OPT_M_MOV] < 1 ? "" : "-mmov");	
	printf("\n");
	
	my_dir = opendir(argv[1]);
	if (my_dir == NULL) {
		perror("Failed to open directory");
		return 0;
	}

	start = clock();
	start_time = time(NULL);
	while ((my_dirent = readdir(my_dir)) != NULL) {
		if (!strcmp(my_dirent->d_name, ".")) {
			continue;
		}
		if (!strcmp(my_dirent->d_name, "..")) {
			continue;
		}
		rval = rename_it(argv[1], my_dirent->d_name, opt);
		fr_total_nr++;
		if (rval == FILE_RENAME_OK) {
			fr_ok_nr++;
		} else if (rval == FILE_RENAME_SKIP) {
			fr_skip_nr++;
		} else {
			fr_error_nr++;
		}
	}
	end_time = time(NULL);
	end = clock();
	ct = ((double) (end - start)) / CLOCKS_PER_SEC;
	printf("Total number of files was %d, and resulted in\n", fr_total_nr);
	printf("\t%d ok\n", fr_ok_nr);
	printf("\t%d skipped\n", fr_skip_nr);
	printf("\t%d error\n", fr_error_nr);
	printf("using %.02f seconds cpu time and took in total %ld seconds\n\n", ct, end_time - start_time);
	return 0;
}
