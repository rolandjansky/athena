################################################################################
# Package: IdDict
################################################################################

# Declare the package name:
atlas_subdir( IdDict )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/Identifier )

# Component(s) in the package:
atlas_add_library( IdDict
                   src/IdDictMgr.cxx
                   src/IdDictFieldImplementation.cxx
                   PUBLIC_HEADERS IdDict
                   LINK_LIBRARIES Identifier )


# Code in this file makes heavy use of eigen and runs orders of magnitude
# more slowly without optimization.  So force this to be optimized even
# in debug builds.  If you need to debug it you might want to change this.
# Specifying optimization via an attribute on the particular
# function didn't work, because that still didn't allow inlining.
if ( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
  set_source_files_properties(
     ${CMAKE_CURRENT_SOURCE_DIR}/src/IdDictMgr.cxx
     PROPERTIES
     COMPILE_FLAGS "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}"
     COMPILE_DEFINITIONS "FLATTEN" )
endif()
