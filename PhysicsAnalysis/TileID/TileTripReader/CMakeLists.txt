# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TileTripReader )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore )

# Component(s) in the package:
if( XAOD_STANDALONE )
        atlas_add_library( TileTripReaderLib
                   Root/*.cxx
                   PUBLIC_HEADERS TileTripReader
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES PATCoreAcceptLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

else()
        atlas_add_library( TileTripReaderLib
                   Root/*.cxx
                   PUBLIC_HEADERS TileTripReader
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps PATCoreAcceptLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

        atlas_add_component( TileTripReader
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel EventKernel xAODEventInfo GaudiKernel PathResolver TileTripReaderLib )
endif()

atlas_add_dictionary( TileTripReaderDict
   TileTripReader/TileTripReaderDict.h
   TileTripReader/selection.xml
   LINK_LIBRARIES TileTripReaderLib )

 # Install files from the package:
atlas_install_joboptions( share/*.py )
