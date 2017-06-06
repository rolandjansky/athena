# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: AnalysisBaseFunctions.cmake 787191 2016-11-29 19:41:54Z krasznaa $
#
# This module collects CMake functions that are useful when setting up an
# analysis release.
#

# Function generating a RootCore/Packages.h header
#
# This function should be called in the main CMakeLists.txt file of the
# project, after the atlas_project(...) call, in order to generate a header file
# called "RootCore/Packages.h", in the format that RootCore generates it in.
# With one define statement per package that was found in he release.
#
# Usage: analysisbase_generate_release_header()
#
function( analysisbase_generate_release_header )

   # Get the list of packages that were found:
   get_property( _packages GLOBAL PROPERTY ATLAS_EXPORTED_PACKAGES )

   # Generate a "RootCore/Package.h" file, in the same format that
   # RootCore does/did:
   set( _packagesFileName
      ${CMAKE_BINARY_DIR}/RootCore/include/RootCore/Packages.h )
   file( MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/RootCore/include/RootCore )
   file( WRITE ${_packagesFileName}
      "// Auto-generated file, please do not edit\n"
      "#ifndef ROOTCORE_PACKAGES_H\n"
      "#define ROOTCORE_PACKAGES_H\n\n" )
   foreach( pkgname ${_packages} )
      file( APPEND ${_packagesFileName}
         "#define ROOTCORE_PACKAGE_${pkgname}\n" )
   endforeach()
   file( APPEND ${_packagesFileName} "\n#endif // not ROOTCORE_PACKAGES_H\n" )
   unset( _packagesFileName )
   unset( _packages )

   # Install the header in the usual place:
   install( DIRECTORY ${CMAKE_BINARY_DIR}/RootCore/include/
      DESTINATION RootCore/include )

endfunction( analysisbase_generate_release_header )
