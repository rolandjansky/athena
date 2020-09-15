# $Id: CMakeLists.txt 744640 2016-05-03 19:08:14Z krasznaa $
################################################################################
# Package: PATCore
################################################################################

# Declare the package name:
atlas_subdir( PATCore )

# Extra dependencies, based on what environment we are in:
if( NOT XAOD_STANDALONE )
   set( extra_deps GaudiKernel )
   set( extra_libs GaudiKernel )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/AthToolSupport/AsgTools
   ${extra_deps} )

# External dependencies:
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_library( PATCoreAcceptLib
   Root/accept/*.cxx
   PUBLIC_HEADERS PATCore
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_library( PATCoreLib
   Root/*.cxx
   PUBLIC_HEADERS PATCore
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} PATCoreAcceptLib AthContainers AsgTools ${extra_libs} )

atlas_add_dictionary( PATCoreDict
   PATCore/PATCoreDict.h
   PATCore/selection.xml
   LINK_LIBRARIES PATCoreLib )

# second dictionary for Athena-only classes
if( NOT XAOD_STANDALONE )
   atlas_add_dictionary( PATCoreAthenaDict
      PATCore/PATCoreAthenaDict.h
      PATCore/selection_athena.xml
      LINK_LIBRARIES PATCoreLib )
endif ()

# Install files from the package:
atlas_install_python_modules( python/*.py )
