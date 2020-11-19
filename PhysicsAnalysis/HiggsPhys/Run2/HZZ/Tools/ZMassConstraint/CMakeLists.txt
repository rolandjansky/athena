# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# The name of the package:
atlas_subdir( ZMassConstraint )

# Externals used by the package:
find_package( ROOT COMPONENTS Core Physics )

# Libraries in the package:
atlas_add_library( ZMassConstraintLib
   ZMassConstraint/*.h Root/*.cxx
   PUBLIC_HEADERS ZMassConstraint
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools EventPrimitives xAODBase xAODEgamma xAODMuon
   PRIVATE_LINK_LIBRARIES EgammaAnalysisInterfacesLib MuonAnalysisInterfacesLib xAODTracking )

if( NOT XAOD_STANDALONE )
   atlas_add_component( ZMassConstaint
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES AsgTools xAODEgamma xAODEventInfo xAODTracking ZMassConstraintLib )
endif()

atlas_add_dictionary( ZMassConstraintDict
   ZMassConstraint/ZMassConstraintDict.h
   ZMassConstraint/selection.xml
   LINK_LIBRARIES ZMassConstraintLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
