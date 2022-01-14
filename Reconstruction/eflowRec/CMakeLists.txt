# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( eflowRec )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( eflowRec
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} CaloEvent CaloIdentifier CaloRecLib AthContainers AthLinks AthenaBaseComps CxxUtils AthenaKernel GeoPrimitives Identifier xAODBase 
		     xAODCaloEvent xAODCore xAODEgamma xAODMuon xAODPFlow xAODTau xAODTracking GaudiKernel InDetReadoutGeometry TRT_ReadoutGeometry Particle RecoToolInterfaces 
		     TrkParameters CaloDetDescrLib CaloUtilsLib StoreGateLib FourMomUtils PathResolver TrkCaloExtension TrkParametersIdentificationHelpers FourMomUtils 
		     InDetTrackSelectionToolLib AthenaMonitoringKernelLib ICaloTrkMuIdTools tauRecToolsLib)

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/rmsvsdr.dat )
