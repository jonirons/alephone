/*
 *  wad_sdl.cpp - Additional map file handling, SDL implementation
 *
 *  Written in 2000 by Christian Bauer
 */

#include "cseries.h"
#include "FileHandler.h"
#include "find_files.h"

#include <SDL_endian.h>


// From shell_sdl.cpp
extern FileSpecifier global_data_dir, local_data_dir;

/*
 *  Find map file with specified checksum in path
 */

class FindByChecksum : public FileFinder {
public:
	FindByChecksum(uint32 checksum) : look_for_checksum(checksum) {}
	~FindByChecksum() {}

	FileSpecifier found_what;

private:
	bool found(FileSpecifier &file)
	{
		OpenedFile f;
		if (!file.Open(f))
			return false;
		f.SetPosition(0x44);
		SDL_RWops *p = f.GetRWops();
		uint32 checksum = SDL_ReadBE32(p);
		if (checksum == look_for_checksum) {
			found_what = file;
			return true;
		}
		return false;
	}

	uint32 look_for_checksum;
};

bool find_wad_file_that_has_checksum(FileSpecifier &matching_file, int file_type, short path_resource_id, uint32 checksum)
{
	FindByChecksum finder(checksum);
	bool found_it = finder.Find(global_data_dir, file_type);
	if (!found_it)
		found_it = finder.Find(local_data_dir, file_type);
	if (found_it) {
		matching_file = finder.found_what;
		return true;
	} else
		return false;
}


/*
 *  Find file with specified modification date in path
 */

class FindByDate : public FileFinder {
public:
	FindByDate(time_t date) : look_for_date(date) {}
	~FindByDate() {}

	FileSpecifier found_what;

private:
	bool found(FileSpecifier &file)
	{
		if (file.GetDate() == look_for_date) {
			found_what = file;
			return true;
		}
		return false;
	}

	TimeType look_for_date;
};

bool find_file_with_modification_date(FileSpecifier &matching_file, int file_type, short path_resource_id, TimeType modification_date)
{
	FindByDate finder(modification_date);
	bool found_it = finder.Find(global_data_dir, file_type);
	if (!found_it)
		found_it = finder.Find(local_data_dir, file_type);
	if (found_it) {
		matching_file = finder.found_what;
		return true;
	} else
		return false;
}
