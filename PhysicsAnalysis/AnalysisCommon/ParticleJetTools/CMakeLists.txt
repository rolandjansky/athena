# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ParticleJetTools )

find_package( ROOT )

# Component(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_library( ParticleJetToolsLib
      ParticleJetTools/*.h Root/*.cxx src/*.cxx
      PUBLIC_HEADERS ParticleJetTools
      LINK_LIBRARIES AsgTools EventPrimitives xAODJet xAODTracking xAODTruth
      JetInterface MCTruthClassifierLib JetRecLib AsgDataHandlesLib
      PRIVATE_LINK_LIBRARIES AthContainers TruthUtils )
else()
   atlas_add_library( ParticleJetToolsLib
      ParticleJetTools/*.h Root/*.cxx src/*.cxx
      PUBLIC_HEADERS ParticleJetTools
      LINK_LIBRARIES AsgDataHandlesLib AsgTools AthContainers AthenaBaseComps EventPrimitives
      GaudiKernel JetInterface JetRecLib MCTruthClassifierLib
      xAODBase xAODEventInfo xAODJet xAODTruth
      PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AsgMessagingLib AthAnalysisBaseCompsLib TruthUtils xAODTracking )
endif()

if( NOT XAOD_STANDALONE )
   atlas_add_component( ParticleJetTools
      src/components/*.cxx
      LINK_LIBRARIES ParticleJetToolsLib )
endif()

atlas_add_dictionary( ParticleJetToolsDict
  ParticleJetTools/ParticleJetToolsDict.h
  ParticleJetTools/selection.xml
  LINK_LIBRARIES ParticleJetToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
