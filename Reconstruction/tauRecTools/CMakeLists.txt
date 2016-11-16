# $Id: CMakeLists.txt 779210 2016-10-19 12:38:55Z krasznaa $ 
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
      Event/xAOD/xAODTracking
      Reconstruction/MVAUtils
      PRIVATE
      Event/FourMomUtils
      Event/xAOD/xAODJet
      Tools/PathResolver
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
      Event/xAOD/xAODTracking
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

# Component(s) in the package:
if( XAOD_STANDALONE OR XAOD_ANALYSIS )
   atlas_add_library( tauRecToolsLib
      tauRecTools/*.h Root/*.cxx
      PUBLIC_HEADERS tauRecTools
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
      ${FASTJET_INCLUDE_DIRS}
      PRIVATE_INCLUDE_DIRS ${FASTJETCONTRIB_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} ${FASTJET_LIBRARIES} ${Boost_LIBRARIES}
      AthLinks AsgTools CxxUtils xAODCaloEvent xAODEventInfo xAODPFlow xAODTau
      xAODTracking
      PRIVATE_LINK_LIBRARIES ${FASTJETCONTRIB_LIBRARIES} FourMomUtils xAODJet
      PathResolver MVAUtils )
else()
   atlas_add_library( tauRecToolsLib
      tauRecTools/*.h Root/*.cxx
      PUBLIC_HEADERS tauRecTools
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
      ${FASTJET_INCLUDE_DIRS}
      PRIVATE_INCLUDE_DIRS ${FASTJETCONTRIB_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} ${FASTJET_LIBRARIES} ${Boost_LIBRARIES}
      AthLinks AsgTools CxxUtils xAODCaloEvent xAODEventInfo xAODPFlow xAODTau
      xAODTracking CaloUtilsLib Particle
      PRIVATE_LINK_LIBRARIES ${FASTJETCONTRIB_LIBRARIES} FourMomUtils xAODJet
      PathResolver MVAUtils )
endif()

if( NOT XAOD_STANDALONE )
   if( XAOD_ANALYSIS )
      atlas_add_component( tauRecTools
         src/*.h src/*.cxx src/components/*.cxx
         INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
         LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} xAODTau
         xAODTracking AthContainers FourMomUtils xAODCaloEvent xAODJet 
         xAODPFlow MVAUtils GaudiKernel tauRecToolsLib )
   else()
      atlas_add_component( tauRecTools
         src/*.h src/*.cxx src/components/*.cxx
         INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
         LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} CaloUtilsLib
         xAODTau xAODTracking AthContainers FourMomUtils NavFourMom
         xAODCaloEvent xAODJet xAODPFlow MVAUtils GaudiKernel
         InDetRecToolInterfaces JetEDM Particle ITrackToVertex
         RecoToolInterfaces TrkLinks TrkParametersIdentificationHelpers
         TrkTrackSummary VxVertex TrkToolInterfaces TrkVertexFitterInterfaces
         TrkVertexFittersLib TrkVxEdmCnvLib InDetTrackSelectionToolLib
         tauRecToolsLib )
   endif()
endif()

# Install files from the package:
atlas_install_runtime( share/*.xml )
atlas_install_data( share/* )
