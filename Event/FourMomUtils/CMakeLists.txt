# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( FourMomUtils )

# External dependencies:
find_package( CLHEP QUIET )

# Component(s) in the package:
if( XAOD_STANDALONE OR XAOD_ANALYSIS )
   if( CLHEP_FOUND )
      atlas_add_library( FourMomUtils
         FourMomUtils/*.h Root/*.cxx
         PUBLIC_HEADERS FourMomUtils
         PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
         PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
         LINK_LIBRARIES AthContainers CxxUtils xAODBase xAODMissingET
         PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} )
   else()
      atlas_add_library( FourMomUtils
         FourMomUtils/*.h Root/*.cxx
         PUBLIC_HEADERS FourMomUtils
         LINK_LIBRARIES AthContainers CxxUtils xAODBase xAODMissingET )
   endif()
else()
   atlas_add_library( FourMomUtils
      FourMomUtils/*.h src/*.cxx Root/*.cxx
      PUBLIC_HEADERS FourMomUtils
      PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
      PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
      LINK_LIBRARIES AthContainers CxxUtils EventKernel NavFourMom xAODBase
      xAODMissingET
      PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} FourMom )
endif()

atlas_add_dictionary( FourMomUtilsDict
   FourMomUtils/FourMomUtilsDict.h
   FourMomUtils/selection.xml
   LINK_LIBRARIES FourMomUtils )

# Install files from the package:
atlas_install_python_modules( python/*.py )
