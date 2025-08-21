file(GLOB_RECURSE cfiles "./src/*.c" "./Traffic/*.c") 
file(GLOB_RECURSE cppfiles "./src/*.cpp" "./Traffic/*.cpp") 
idf_component_register(SRCS main.c ${cfiles} ${cppfiles}
                                       INCLUDE_DIRS "inc")
target_compile_options(${COMPONENT_LIB} PRIVATE -w)

