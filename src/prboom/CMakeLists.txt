#file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} "*.c")

idf_component_register(SRC_DIRS "."
                    PRIV_INCLUDE_DIRS "." "include" "../prboom-wad-tables/include")
	    
	   
	    
	    set(CMAKE_C_FLAGS  "-Wno-error -mlongcalls  -Wno-pointer-sign -Wno-maybe-uninitialized -Wno-parentheses -Wno-unused-value  -Wno-unused-variable -Wno-unused-but-set-parameter -Wno-nonnull -Wno-implicit-function-declaration -Wno-misleading-indentation -Wno-format-overflow -Wno-unused-const-variable -Wno-sizeof-pointer-div -Wno-duplicate-decl-specifier -Wno-dangling-else")
