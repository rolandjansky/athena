# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigTauHypo )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore Hist )

# Component(s) in the package:
atlas_add_component( TrigTauHypo
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthViews AthenaBaseComps AthenaMonitoringKernelLib CaloEvent CxxUtils DecisionHandlingLib EventInfo GaudiKernel ITrackToVertex MuidEvent Particle PathResolver StoreGateLib TrigCaloEvent TrigCompositeUtilsLib TrigInDetEvent TrigInterfacesLib TrigMuonEvent TrigNavStructure TrigParticle TrigSteeringEvent TrigTopoEvent TrkTrack TrkTrackSummary VxVertex egammaEvent tauEvent xAODBase xAODCaloEvent xAODEgamma xAODJet xAODMuon xAODTau xAODTracking xAODTrigger )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-extensions=ATL900,ATL901 )
