# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( METReconstruction )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

include_directories(src)

# Component(s) in the package:
atlas_add_component( METReconstruction
                     Root/*.cxx src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools FourMomUtils xAODCaloEvent xAODEgamma xAODJet xAODMissingET xAODMuon xAODPFlow xAODTracking xAODTruth GaudiKernel RecoToolInterfaces CaloEvent CaloConditions AthContainers AthenaBaseComps EventPrimitives xAODTau TruthUtils METRecoInterface PFlowUtilsLib StoreGateLib InDetTrackSelectionToolLib TrackVertexAssociationToolLib )

# Install files from the package:
atlas_install_python_modules( python/*.py python/LegacyRunII/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
