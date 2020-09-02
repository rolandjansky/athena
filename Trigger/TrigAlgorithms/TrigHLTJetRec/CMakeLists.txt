# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigHLTJetRec )

# External dependencies:
find_package( FastJet )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TrigHLTJetRec
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${FASTJET_INCLUDE_DIRS}
                     LINK_LIBRARIES ${FASTJET_LIBRARIES} ${ROOT_LIBRARIES} AsgTools AthenaBaseComps CaloEvent CxxUtils EventShapeToolsLib GaudiKernel JetEDM JetInterface JetRecLib JetRecToolsLib TrigInterfacesLib TrigParticle TrigSteeringEvent xAODBase xAODCaloEvent xAODCore xAODEventShape xAODJet xAODPFlow xAODTracking xAODTrigL1Calo )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
