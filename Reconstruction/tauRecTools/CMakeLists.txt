# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( tauRecTools )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core Tree Hist RIO )
find_package( FastJet COMPONENTS fastjetplugins fastjettools siscone
	        siscone_spherical )
find_package( FastJetContrib COMPONENTS VariableR )
find_package( lwtnn )

atlas_add_root_dictionary( tauRecToolsLib tauRecToolsLibCintDict
  ROOT_HEADERS
  tauRecTools/TauCalibrateLC.h
  tauRecTools/TauSubstructureVariables.h
  tauRecTools/TauCommonCalcVars.h
  tauRecTools/MvaTESVariableDecorator.h
  tauRecTools/MvaTESEvaluator.h
  tauRecTools/TauCombinedTES.h
  tauRecTools/TauTrackClassifier.h
  tauRecTools/TauTrackRNNClassifier.h
  tauRecTools/TauWPDecorator.h
  tauRecTools/TauJetBDTEvaluator.h
  tauRecTools/TauJetRNNEvaluator.h
  tauRecTools/TauIDVarCalculator.h
  tauRecTools/TauDecayModeNNClassifier.h
  tauRecTools/TauVertexedClusterDecorator.h
  Root/LinkDef.h
  EXTERNAL_PACKAGES ROOT
  )

# Component(s) in the package:
if( XAOD_STANDALONE OR XAOD_ANALYSIS )
   atlas_add_library( tauRecToolsLib
      tauRecTools/*.h Root/*.cxx tauRecTools/lwtnn/*.h Root/lwtnn/*.cxx  ${tauRecToolsLibCintDict}
      PUBLIC_HEADERS tauRecTools
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
      ${FASTJET_INCLUDE_DIRS}
      PRIVATE_INCLUDE_DIRS ${FASTJETCONTRIB_INCLUDE_DIRS} ${LWTNN_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} ${FASTJET_LIBRARIES} ${Boost_LIBRARIES}
      AthLinks AsgTools CxxUtils xAODCaloEvent xAODEventInfo xAODPFlow xAODTau
      xAODTracking xAODParticleEvent AsgDataHandlesLib MVAUtils
      PRIVATE_LINK_LIBRARIES ${FASTJETCONTRIB_LIBRARIES} ${LWTNN_LIBRARIES} FourMomUtils xAODJet
      PathResolver )
else()
   atlas_add_library( tauRecToolsLib
      tauRecTools/*.h Root/*.cxx tauRecTools/lwtnn/*.h Root/lwtnn/*.cxx  ${tauRecToolsLibCintDict}
      PUBLIC_HEADERS tauRecTools
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
      ${FASTJET_INCLUDE_DIRS}
      PRIVATE_INCLUDE_DIRS ${FASTJETCONTRIB_INCLUDE_DIRS} ${LWTNN_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} ${FASTJET_LIBRARIES} ${Boost_LIBRARIES}
      AthLinks AsgTools CxxUtils xAODCaloEvent xAODEventInfo xAODPFlow xAODTau
      xAODTracking xAODParticleEvent CaloUtilsLib Particle AsgDataHandlesLib MVAUtils
      PRIVATE_LINK_LIBRARIES ${FASTJETCONTRIB_LIBRARIES} ${LWTNN_LIBRARIES} FourMomUtils xAODJet BeamSpotConditionsData
      PathResolver )
endif()

if( NOT XAOD_STANDALONE )
   if( XAOD_ANALYSIS )
      atlas_add_component( tauRecTools
         src/*.h src/*.cxx src/components/*.cxx
         INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${LWTNN_INCLUDE_DIRS}
         LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${LWTNN_LIBRARIES} xAODTau
         xAODTracking AthContainers FourMomUtils xAODCaloEvent xAODJet
         xAODPFlow xAODParticleEvent MVAUtils GaudiKernel tauRecToolsLib )
   else()
      atlas_add_component( tauRecTools
         src/*.h src/*.cxx src/components/*.cxx
         INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${LWTNN_INCLUDE_DIRS}
         LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${LWTNN_LIBRARIES} CaloUtilsLib
         xAODTau xAODTracking AthContainers FourMomUtils NavFourMom
         xAODCaloEvent xAODJet xAODPFlow xAODParticleEvent MVAUtils GaudiKernel
         InDetRecToolInterfaces JetEDM Particle ITrackToVertex
         RecoToolInterfaces TrkLinks TrkParametersIdentificationHelpers
         TrkTrackSummary VxVertex TrkToolInterfaces TrkVertexFitterInterfaces
         TrkVertexFittersLib InDetTrackSelectionToolLib BeamSpotConditionsData
         tauRecToolsLib TrackVertexAssociationToolLib)
   endif()
endif()
