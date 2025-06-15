# Include the source files from this subfolder
FILE(GLOB app_sources *.c)
target_sources(app PRIVATE ${app_sources})