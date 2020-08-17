# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Module for setting up the runtime environment for numpy from an LCG release.
#

# Find the lapack installation that's part of LCG. Note that this call finds the
# CMake configuration included in the LAPACK installation, it does *not* use
# CMake's FindLAPACK.cmake module.
find_package( lapack )

# Extract the path of the library pointed to by the "lapack" imported library.
if( lapack_FOUND )
   get_target_property( _configs lapack IMPORTED_CONFIGURATIONS )
   list( GET _configs 0 _config )
   get_target_property( _libPath lapack IMPORTED_LOCATION_${_config} )
   get_filename_component( NumPyEnvironment_LIBRARY_DIRS ${_libPath}
      DIRECTORY )
   unset( _configs )
   unset( _config )
   unset( _libPath )
endif()
