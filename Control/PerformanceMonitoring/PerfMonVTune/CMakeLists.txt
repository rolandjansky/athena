################################################################################
# Package: PerfMonVTune
################################################################################

# Declare the package name:
atlas_subdir( PerfMonVTune )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel )

####
# VTune hack for the time-being
find_program( VTUNE_EXECUTABLE amplxe-cl )
get_filename_component( VTUNE_DIR ${VTUNE_EXECUTABLE} PATH )
set( ITT_PREFIX ${VTUNE_DIR}/.. )

find_path( ITT_INCLUDE_DIR NAMES ittnotify.h HINTS ${ITT_PREFIX}/include )
find_library( ITT_LIBRARY NAMES ittnotify HINTS ${ITT_PREFIX}/lib64 )

include_directories(${ITT_INCLUDE_DIR})
####

# Component(s) in the package:
atlas_add_component( PerfMonVTune
                     src/*.cxx
                     src/components/*.cxx 
                     LINK_LIBRARIES GaudiKernel AthenaBaseComps AthenaKernel ${ITT_LIBRARY} ${CMAKE_DL_LIBS} )

# Install files from the package:
atlas_install_headers( PerfMonVTune )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( scripts/*.py )
