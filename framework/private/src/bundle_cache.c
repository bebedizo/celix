/**
 *Licensed to the Apache Software Foundation (ASF) under one
 *or more contributor license agreements.  See the NOTICE file
 *distributed with this work for additional information
 *regarding copyright ownership.  The ASF licenses this file
 *to you under the Apache License, Version 2.0 (the
 *"License"); you may not use this file except in compliance
 *with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *Unless required by applicable law or agreed to in writing,
 *software distributed under the License is distributed on an
 *"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 *specific language governing permissions and limitations
 *under the License.
 */
/*
 * bundle_cache.c
 *
 *  \date       Aug 6, 2010
 *  \author    	<a href="mailto:dev@celix.apache.org">Apache Celix Project Team</a>
 *  \copyright	Apache License, Version 2.0
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#include "bundle_cache_private.h"
#include "bundle_archive.h"
#include "constants.h"
#include "celix_log.h"

static celix_status_t bundleCache_deleteTree(bundle_cache_pt cache, char * directory);

celix_status_t bundleCache_create(properties_pt configurationMap, bundle_cache_pt *bundle_cache) {
	celix_status_t status;
	bundle_cache_pt cache;

	if (configurationMap == NULL || *bundle_cache != NULL) {
		return CELIX_ILLEGAL_ARGUMENT;
	}

	cache = (bundle_cache_pt) calloc(1, sizeof(*cache));
	if (cache == NULL) {
		status = CELIX_ENOMEM;
	} else {
		char* cacheDir = (char*)properties_get(configurationMap, (char *) OSGI_FRAMEWORK_FRAMEWORK_STORAGE);
		cache->configurationMap = configurationMap;
		if (cacheDir == NULL) {
			cacheDir = ".cache";
		}
		cache->cacheDir = cacheDir;

		*bundle_cache = cache;
		status = CELIX_SUCCESS;
	}

	framework_logIfError(logger, status, NULL, "Failed to create bundle cache");

	return status;
}

celix_status_t bundleCache_destroy(bundle_cache_pt *cache) {

	free(*cache);
	*cache = NULL;

	return CELIX_SUCCESS;
}

celix_status_t bundleCache_delete(bundle_cache_pt cache) {
	return bundleCache_deleteTree(cache, cache->cacheDir);
}

celix_status_t bundleCache_getArchives(bundle_cache_pt cache, array_list_pt *archives) {
	celix_status_t status = CELIX_SUCCESS;

	DIR *dir;
	struct stat st;

	dir = opendir(cache->cacheDir);

	if (dir == NULL && errno == ENOENT) {
		if(mkdir(cache->cacheDir, S_IRWXU) == 0 ){
			dir = opendir(cache->cacheDir);
		}
	}

	if (dir != NULL) {
		array_list_pt list = NULL;
		arrayList_create(&list);

		struct dirent *dp;

		errno = 0;
		dp = readdir(dir);
		while (dp != NULL) {
			char archiveRoot[512];

			snprintf(archiveRoot, sizeof(archiveRoot), "%s/%s", cache->cacheDir, dp->d_name);

			if (stat (archiveRoot, &st) == 0) {
				if (S_ISDIR (st.st_mode)
						&& (strcmp((dp->d_name), ".") != 0)
						&& (strcmp((dp->d_name), "..") != 0)
						&& (strncmp(dp->d_name, "bundle", 6) == 0)
						&& (strcmp(dp->d_name, "bundle0") != 0)) {

					bundle_archive_pt archive = NULL;
					status = bundleArchive_recreate(archiveRoot, &archive);
					if (status == CELIX_SUCCESS) {
						arrayList_add(list, archive);
					}
				}
			}

			errno = 0;
			dp = readdir(dir);
		}

		if (errno != 0) {
			fw_log(logger, OSGI_FRAMEWORK_LOG_ERROR, "Error reading dir");
			status = CELIX_FILE_IO_EXCEPTION;
		} else {
			status = CELIX_SUCCESS;
		}

		closedir(dir);

		if (status == CELIX_SUCCESS) {
			*archives = list;
		}
		else{
			int idx = 0;
			for(;idx<arrayList_size(list);idx++){
				bundleArchive_destroy((bundle_archive_pt)arrayList_get(list,idx));
			}
			arrayList_destroy(list);
			*archives = NULL;
		}

	} else {
		status = CELIX_FILE_IO_EXCEPTION;
	}

	framework_logIfError(logger, status, NULL, "Failed to get bundle archives");

	return status;
}

celix_status_t bundleCache_createArchive(bundle_cache_pt cache, long id, const char * location, const char *inputFile, bundle_archive_pt *bundle_archive) {
	celix_status_t status = CELIX_SUCCESS;
	char archiveRoot[512];

	if (cache && location) {
		snprintf(archiveRoot, sizeof(archiveRoot), "%s/bundle%ld",  cache->cacheDir, id);
		status = bundleArchive_create(archiveRoot, id, location, inputFile, bundle_archive);
	}

	framework_logIfError(logger, status, NULL, "Failed to create archive");

	return status;
}

static celix_status_t bundleCache_deleteTree(bundle_cache_pt cache, char * directory) {
	DIR *dir;
	celix_status_t status = CELIX_SUCCESS;
	struct stat st;

	dir = opendir(directory);
	if (dir == NULL) {
		status = CELIX_FILE_IO_EXCEPTION;
	} else {
		struct dirent *dp;
		dp = readdir(dir);
		while (dp != NULL) {
			if ((strcmp((dp->d_name), ".") != 0) && (strcmp((dp->d_name), "..") != 0)) {
				char subdir[512];
				snprintf(subdir, sizeof(subdir), "%s/%s", directory, dp->d_name);

				if (stat(subdir, &st) == 0) {
					if (S_ISDIR (st.st_mode)) {
						status = bundleCache_deleteTree(cache, subdir);
					} else {
						if (remove(subdir) != 0) {
							status = CELIX_FILE_IO_EXCEPTION;
							break;
						}
					}
				}
			}
			dp = readdir(dir);
		}

		if (closedir(dir) != 0) {
			status = CELIX_FILE_IO_EXCEPTION;
		}
		if (status == CELIX_SUCCESS) {
			if (rmdir(directory) != 0) {
				status = CELIX_FILE_IO_EXCEPTION;
			}
		}
	}

	framework_logIfError(logger, status, NULL, "Failed to delete tree at dir '%s'", directory);

	return status;
}
