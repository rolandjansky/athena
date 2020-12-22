# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
################################################################################
# Package: DerivationFrameworkBPhys
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkBPhys )
find_package( ROOT COMPONENTS Core MathCore )

# Component(s) in the package:
atlas_add_component( DerivationFrameworkBPhys
   DerivationFrameworkBPhys/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES xAODMuon AthenaBaseComps JpsiUpsilonToolsLib
   MuonSelectorToolsLib ${ROOT_LIBRARIES}
   xAODTracking xAODBPhysLib AthenaKernel RecoToolInterfaces EventPrimitives
   DerivationFrameworkInterfaces BPhysToolsLib TrackVertexAssociationToolLib
   xAODBase xAODMetaData  AsgTools CaloInterfaceLib TrackToCaloLib
   xAODEventInfo AthenaPoolUtilities xAODPrimitives TrigDecisionToolLib
     BeamSpotConditionsData TrkVertexAnalysisUtilsLib ITrackToVertex
   InDetTrackSelectionToolLib  InDetV0FinderLib)


# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8})
atlas_install_joboptions( share/*.py )
