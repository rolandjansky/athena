# $Id: CMakeLists.txt 796274 2017-02-08 17:13:03Z griffith $ 
################################################################################
# Package: tauRecTools
################################################################################

# Declare the package name:
atlas_subdir( tauRecTools )

# Declare the package's dependencies:
if( XAOD_STANDALONE OR XAOD_ANALYSIS )
   set( extra_deps )
   if( XAOD_ANALYSIS )
      set( extra_deps GaudiKernel )
   endif()
   atlas_depends_on_subdirs(
      PUBLIC
      Control/AthLinks
      Control/AthToolSupport/AsgTools
      Control/CxxUtils
      Event/xAOD/xAODCaloEvent
      Event/xAOD/xAODEventInfo
      Event/xAOD/xAODPFlow
      Event/xAOD/xAODTau
      Event/xAOD/xAODEgamma
      Event/xAOD/xAODTracking
      PhysicsAnalysis/MCTruthClassifier
      Reconstruction/MVAUtils
      PRIVATE
      Event/FourMomUtils
      Event/xAOD/xAODJet
      Tools/PathResolver
      PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools
      ${extra_deps} )
else()
   atlas_depends_on_subdirs(
      PUBLIC
      Calorimeter/CaloUtils
      Control/AthLinks
      Control/AthToolSupport/AsgTools
      Control/CxxUtils
      Event/xAOD/xAODCaloEvent
      Event/xAOD/xAODEventInfo
      Event/xAOD/xAODPFlow
      Event/xAOD/xAODTau
      Event/xAOD/xAODEgamma
      Event/xAOD/xAODTracking
      PhysicsAnalysis/MCTruthClassifier
      Reconstruction/Particle
      Reconstruction/MVAUtils
      PRIVATE
      Calorimeter/CaloInterface
      Control/AthContainers
      Event/FourMomUtils
      Event/NavFourMom
      Event/xAOD/xAODJet
      GaudiKernel
      InnerDetector/InDetRecTools/InDetRecToolInterfaces
      InnerDetector/InDetRecTools/InDetTrackSelectionTool
      Reconstruction/Jet/JetEDM
      Reconstruction/RecoTools/ITrackToVertex
      Reconstruction/RecoTools/RecoToolInterfaces
      Tools/PathResolver
      PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools	
      Tracking/TrkEvent/TrkLinks
      Tracking/TrkEvent/TrkParametersIdentificationHelpers
      Tracking/TrkEvent/TrkTrackSummary
      Tracking/TrkEvent/VxVertex
      Tracking/TrkTools/TrkToolInterfaces
      Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
      Tracking/TrkVertexFitter/TrkVertexFitters
      Tracking/TrkVertexFitter/TrkVxEdmCnv )
endif()

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core Tree Hist RIO )
find_package( FastJet COMPONENTS fastjetplugins fastjettools siscone
	        siscone_spherical )
find_package( FastJetContrib COMPONENTS VariableR )

atlas_add_root_dictionary( tauRecToolsLib tauRecToolsLibCintDict
  ROOT_HEADERS
  tauRecTools/TauCalibrateLC.h
  tauRecTools/TauSubstructureVariables.h
  tauRecTools/TauCommonCalcVars.h
  tauRecTools/TauGenericPi0Cone.h
  tauRecTools/TauIDPileupCorrection.h
  tauRecTools/TauTrackFilter.h
  tauRecTools/MvaTESVariableDecorator.h
  tauRecTools/MvaTESEvaluator.h
  tauRecTools/CombinedP4FromRecoTaus.h
  tauRecTools/TauTrackClassifier.h
  tauRecTools/TauChargedPFOCreator.h
  tauRecTools/TauWPDecorator.h
  tauRecTools/TauJetBDTEvaluator.h
  tauRecTools/TauIDVarCalculator.h
  tauRecTools/TauEleOLRDecorator.h
  tauRecTools/TauProcessorTool.h
  Root/LinkDef.h
  EXTERNAL_PACKAGES ROOT
  )

# Component(s) in the package:
if( XAOD_STANDALONE OR XAOD_ANALYSIS )
   atlas_add_library( tauRecToolsLib
      tauRecTools/*.h Root/*.cxx  ${tauRecToolsLibCintDict}
      PUBLIC_HEADERS tauRecTools
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
      ${FASTJET_INCLUDE_DIRS}
      PRIVATE_INCLUDE_DIRS ${FASTJETCONTRIB_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} ${FASTJET_LIBRARIES} ${Boost_LIBRARIES}
      AthLinks AsgTools CxxUtils xAODCaloEvent xAODEventInfo xAODPFlow xAODEgamma xAODTau
      xAODTracking MCTruthClassifierLib
      PRIVATE_LINK_LIBRARIES ${FASTJETCONTRIB_LIBRARIES} FourMomUtils xAODJet
      PathResolver MVAUtils ElectronPhotonSelectorToolsLib )
else()
   atlas_add_library( tauRecToolsLib
      tauRecTools/*.h Root/*.cxx  ${tauRecToolsLibCintDict}
      PUBLIC_HEADERS tauRecTools
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
      ${FASTJET_INCLUDE_DIRS}
      PRIVATE_INCLUDE_DIRS ${FASTJETCONTRIB_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} ${FASTJET_LIBRARIES} ${Boost_LIBRARIES}
      AthLinks AsgTools CxxUtils xAODCaloEvent xAODEventInfo xAODPFlow xAODEgamma xAODTau
      xAODTracking MCTruthClassifierLib CaloUtilsLib Particle
      PRIVATE_LINK_LIBRARIES ${FASTJETCONTRIB_LIBRARIES} FourMomUtils xAODJet
      PathResolver MVAUtils ElectronPhotonSelectorToolsLib)
endif()

if( NOT XAOD_STANDALONE )
   if( XAOD_ANALYSIS )
      atlas_add_component( tauRecTools
         src/*.h src/*.cxx src/components/*.cxx
         INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
         LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} xAODTau
         xAODTracking xAODEgamma AthContainers FourMomUtils xAODCaloEvent xAODJet 
         xAODPFlow MVAUtils ElectronPhotonSelectorToolsLib GaudiKernel tauRecToolsLib )
   else()
      atlas_add_component( tauRecTools
         src/*.h src/*.cxx src/components/*.cxx
         INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
         LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} CaloUtilsLib
         xAODTau xAODTracking xAODEgamma AthContainers FourMomUtils NavFourMom
         xAODCaloEvent xAODJet xAODPFlow MVAUtils ElectronPhotonSelectorToolsLib GaudiKernel
         InDetRecToolInterfaces JetEDM Particle ITrackToVertex
         RecoToolInterfaces TrkLinks TrkParametersIdentificationHelpers
         TrkTrackSummary VxVertex TrkToolInterfaces TrkVertexFitterInterfaces
         TrkVertexFittersLib TrkVxEdmCnvLib InDetTrackSelectionToolLib
         tauRecToolsLib )
   endif()
endif()

# Install files from the package:
atlas_install_runtime( share/*.xml )
atlas_install_runtime( share/*.root )
atlas_install_data( share/* )
