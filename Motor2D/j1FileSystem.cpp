#include "p2Defs.h"
#include "j1App.h"
#include "p2Log.h"
#include "j1FileSystem.h"
#include "SDL/include/SDL_rwops.h"
#include "SDL/include/SDL_filesystem.h"
#include "PhysFS/include/physfs.h"

#pragma comment( lib, "PhysFS/libx86/physfs.lib" )

j1FileSystem::j1FileSystem(const char* game_path) : j1Module()
{
	name.create("file_system");

	// TODO 1: Init PhysFS lib
	char* base_path = SDL_GetBasePath();
	PHYSFS_init(base_path);

	// TODO 2: Mount directory ".", then mount "data.zip"
	// files in the folder should take precendence over the zip file!
	PHYSFS_mount(".", game_path, 0);
	PHYSFS_mount("data.zip", "../Game/data.zip", 1);
}

// Destructor
j1FileSystem::~j1FileSystem()
{
	// TODO 1: stop PhysFS lib
	PHYSFS_deinit();
}

// Called before render is available
bool j1FileSystem::Awake()
{
	LOG("Loading File System");
	bool ret = true;

	// This is a good place to setup the write dir (homework)

	return ret;
}

// Read a whole file and put it in a new buffer
unsigned int j1FileSystem::Load(const char* file, char** buffer) const
{
	unsigned int ret = 0;

	// TODO 3: Read a file and allocate needed bytes to buffer and write all data into it
	// return the size of the data
	PHYSFS_file* file_being_read = PHYSFS_openRead(file);

	if (file_being_read != NULL) {

		PHYSFS_uint64 file_size = PHYSFS_fileLength(file_being_read);
		
		if (file_size > 0) {
			*buffer = new char[(uint)file_size];

			PHYSFS_uint64 data_read = PHYSFS_read(file_being_read, *buffer, 1, (PHYSFS_uint32)file_size);
			
			if (data_read != file_size)
			{
				LOG("File System error while reading from file %s: %s\n", file, PHYSFS_getLastError());
				RELEASE(buffer);
			}
			else
				ret = uint(data_read);
		}
		if (PHYSFS_close(file_being_read) == 0)
			LOG("File System error while closing file %s: %s\n", file, PHYSFS_getLastError());
	}
	else
		LOG("File System error while opening file %s: %s\n", file, PHYSFS_getLastError());

	return ret;
}

// Read a whole file and put it in a new buffer
SDL_RWops* j1FileSystem::Load(const char* file) const
{
	// TODO 4: Using our previous Load method
	// create a sdl rwops using SDL_RW_From_ConstMem()
	// and return it if everything goes well
	char* buffer;
	uint size = Load(file, &buffer);
	if (size > 0) {
		SDL_RWops* ret = SDL_RWFromMem(buffer, size);
		if (ret != NULL) {
			ret->close = close_sdl_rwops;
		}
		return(ret);
	}
	else
		return NULL;
}

// TODO 4: you will need to use this small helper function
// to make sure all data is freed automatically
int close_sdl_rwops(SDL_RWops *rw)
{
	RELEASE(rw->hidden.mem.base);
	SDL_FreeRW(rw);
	return 0;
}
