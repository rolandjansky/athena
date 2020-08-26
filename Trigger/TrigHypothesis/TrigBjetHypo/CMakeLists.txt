# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigBjetHypo )

# External dependencies:
find_package( ROOT COMPONENTS MathCore )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( TrigBjetHypo
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AthContainers AthLinks AthViews AthenaBaseComps AthenaMonitoringKernelLib BTaggingLib BeamSpotConditionsData CxxUtils DecisionHandlingLib EventInfo EventPrimitives FourMomUtils GaudiKernel GeoPrimitives JetCalibToolsLib JetEvent Particle TrigCaloEvent TrigCompositeUtilsLib TrigInDetEvent TrigInterfacesLib TrigNavigationLib TrigParticle TrigSteeringEvent TrigInDetToolInterfacesLib VxSecVertex VxVertex xAODBTagging xAODBase xAODCore xAODJet xAODMuon xAODTracking xAODTrigger )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-extensions=ATL900,ATL901 )

# Unit tests:
atlas_add_test( TrigBjetBtagHypoTool SCRIPT python -m TrigBjetHypo.TrigBjetBtagHypoTool
   PROPERTIES TIMEOUT 300
   POST_EXEC_SCRIPT nopost.sh )
