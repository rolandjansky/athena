# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PanTauAlgs )

# External dependencies:
find_package( ROOT COMPONENTS Core RIO Tree MathCore MathMore Physics )

# Component(s) in the package.
atlas_add_library( PanTauAlgsLib
   PanTauAlgs/*.h Root/*.cxx
   PUBLIC_HEADERS PanTauAlgs
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgMessagingLib AsgTools xAODBase
   xAODParticleEvent xAODPFlow xAODTau tauRecToolsLib MVAUtils
   PRIVATE_LINK_LIBRARIES xAODTracking PathResolver )

atlas_add_dictionary( PanTauAlgsDict
   PanTauAlgs/PanTauAlgsDict.h PanTauAlgs/selection.xml
   LINK_LIBRARIES PanTauAlgsLib )

if( NOT XAOD_STANDALONE )
   atlas_add_component( PanTauAlgs
      src/components/*.cxx
      LINK_LIBRARIES PanTauAlgsLib )
endif()

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
