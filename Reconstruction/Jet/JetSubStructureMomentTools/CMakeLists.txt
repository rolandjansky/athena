# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( JetSubStructureMomentTools )

# External dependencies:
find_package( FastJet )
find_package( FastJetContrib COMPONENTS Nsubjettiness EnergyCorrelator )
find_package( ROOT COMPONENTS Core Hist )

# Component(s) in the package:
set( extra_srcs )
if( NOT XAOD_STANDALONE )
   set( extra_srcs src/*.cxx )
endif()
set( extra_libs )
if( NOT XAOD_STANDALONE )
   set( extra_libs GaudiKernel )
endif()
atlas_add_library( JetSubStructureMomentToolsLib
   JetSubStructureMomentTools/*.h Root/*.cxx ${extra_srcs}
   PUBLIC_HEADERS JetSubStructureMomentTools
   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${FASTJETCONTRIB_INCLUDE_DIRS} ${FASTJET_INCLUDE_DIRS}
   LINK_LIBRARIES AsgTools xAODCaloEvent xAODJet JetInterface JetRecLib
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${FASTJETCONTRIB_LIBRARIES} ${FASTJET_LIBRARIES} xAODMuon CxxUtils
   JetEDM JetSubStructureUtils ${extra_libs} )

if( NOT XAOD_STANDALONE )
   atlas_add_component( JetSubStructureMomentTools
      src/components/*.cxx
      LINK_LIBRARIES GaudiKernel JetSubStructureMomentToolsLib )
endif()

atlas_add_dictionary( JetSubStructureMomentToolsDict
   JetSubStructureMomentTools/JetSubStructureMomentToolsDict.h
   JetSubStructureMomentTools/selection.xml
   LINK_LIBRARIES JetSubStructureMomentToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
