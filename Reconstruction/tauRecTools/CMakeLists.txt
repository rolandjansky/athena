# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( tauRecTools )

# External dependencies:
find_package( Boost )
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree Hist RIO )
find_package( lwtnn )

# Optional dependencies.
set( extra_public_libs )
set( extra_private_libs )
if( NOT XAOD_ANALYSIS )
   set( extra_public_libs CaloEvent Particle )
   set( extra_private_libs GaudiKernel StoreGateLib CaloIdentifier
      CaloInterfaceLib CaloUtilsLib TrkCaloExtension TrkLinks
      TrkParametersIdentificationHelpers TrkToolInterfaces
      TrackVertexAssociationToolLib TrkVertexFitterInterfaces
      TrkVertexFittersLib RecoToolInterfaces ITrackToVertex
      InDetTrackSelectionToolLib BeamSpotConditionsData VxVertex JetEDM
      Particle )
endif()

# Component(s) in the package:
atlas_add_library( tauRecToolsLib
   tauRecTools/*.h Root/*.cxx tauRecTools/lwtnn/*.h Root/lwtnn/*.cxx
   PUBLIC_HEADERS tauRecTools
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
   ${LWTNN_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${EIGEN_LIBRARIES} ${LWTNN_LIBRARIES}
   ${ROOT_LIBRARIES}
   CxxUtils AthLinks AsgMessagingLib AsgDataHandlesLib AsgTools xAODCaloEvent
   xAODEventInfo xAODJet xAODParticleEvent xAODPFlow xAODTau xAODTracking
   MVAUtils ${extra_public_libs}
   PRIVATE_LINK_LIBRARIES CaloGeoHelpers FourMomUtils PathResolver )

atlas_add_dictionary( tauRecToolsDict
   tauRecTools/tauRecToolsDict.h tauRecTools/selection.xml
   LINK_LIBRARIES tauRecToolsLib )

if( NOT XAOD_STANDALONE )
   atlas_add_component( tauRecTools
      src/*.h src/*.cxx src/components/*.cxx
      INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
      LINK_LIBRARIES ${Boost_LIBRARIES} AsgTools AsgMessagingLib xAODBase
      xAODCaloEvent xAODJet xAODPFlow xAODTau FourMomUtils tauRecToolsLib
      ${extra_private_libs} )
endif()
