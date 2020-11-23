# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PATCore )

# Extra dependencies, based on what environment we are in:
if( NOT XAOD_STANDALONE )
   set( extra_libs GaudiKernel )
endif()

# External dependencies:
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_library( PATCoreAcceptLib
   Root/accept/*.cxx
   PUBLIC_HEADERS PATCore
   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_library( PATCoreLib
   Root/*.cxx
   PUBLIC_HEADERS PATCore
   LINK_LIBRARIES PATCoreAcceptLib AthContainers AsgTools ${extra_libs} )

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
