# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( GammaORTools )

# External dependencies:
find_package( Boost )

# Libraries in the package:
atlas_add_library( GammaORToolsLib
   GammaORTools/*.h Root/*.cxx
   PUBLIC_HEADERS GammaORTools
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} AsgTools xAODTruth MCTruthClassifierLib )

if( NOT XAOD_STANDALONE )
  atlas_add_component( GammaORTools
    src/components/*.cxx
    LINK_LIBRARIES GammaORToolsLib)
endif()

atlas_add_dictionary( GammaORToolsDict
  GammaORTools/GammaORToolsDict.h
  GammaORTools/selection.xml
  LINK_LIBRARIES GammaORToolsLib )
