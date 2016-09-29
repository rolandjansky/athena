# $Id: CMakeLists.txt 772349 2016-09-08 23:01:15Z griffith $ 
################################################################################
# Package: tauRecTools
################################################################################

# Declare the package name:
atlas_subdir( tauRecTools )

# Declare the package's dependencies:
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
   Reconstruction/MVAUtils
   Tools/PathResolver
   Tracking/TrkEvent/TrkLinks
   Tracking/TrkEvent/TrkParametersIdentificationHelpers
   Tracking/TrkEvent/TrkTrackSummary
   Tracking/TrkEvent/VxVertex
   Tracking/TrkTools/TrkToolInterfaces
   Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
   Tracking/TrkVertexFitter/TrkVertexFitters
   Tracking/TrkVertexFitter/TrkVxEdmCnv )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core Tree Hist RIO )

# Component(s) in the package:
atlas_add_library( tauRecToolsLib
   tauRecTools/*.h Root/*.cxx
   PUBLIC_HEADERS tauRecTools
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} AthLinks AsgTools
   CxxUtils xAODCaloEvent xAODEventInfo xAODPFlow xAODTau xAODTracking
   CaloUtilsLib Particle
   PRIVATE_LINK_LIBRARIES FourMomUtils xAODJet PathResolver MVAUtils )

atlas_add_component( tauRecTools
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} CaloUtilsLib xAODTau
   xAODTracking AthContainers FourMomUtils NavFourMom xAODCaloEvent xAODJet 
   xAODPFlow MVAUtils GaudiKernel InDetRecToolInterfaces JetEDM Particle ITrackToVertex
   RecoToolInterfaces TrkLinks TrkParametersIdentificationHelpers
   TrkTrackSummary VxVertex TrkToolInterfaces TrkVertexFitterInterfaces
   TrkVertexFittersLib TrkVxEdmCnvLib InDetTrackSelectionToolLib
   tauRecToolsLib )

# Install files from the package:
atlas_install_runtime( share/*.xml )
atlas_install_data( share/* )
