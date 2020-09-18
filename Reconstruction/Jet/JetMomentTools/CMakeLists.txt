# $Id: CMakeLists.txt 784099 2016-11-14 17:18:18Z khoo $
################################################################################
# Package: JetMomentTools
################################################################################

# Declare the package name:
atlas_subdir( JetMomentTools )

# Declare the package's dependencies:
if( XAOD_STANDALONE OR XAOD_ANALYSIS )
   set( extra_deps )
   if( NOT XAOD_STANDALONE )
      set( extra_deps GaudiKernel )
   endif()
   atlas_depends_on_subdirs(
      PUBLIC
      Control/AthToolSupport/AsgTools
      Event/xAOD/xAODCaloEvent
      Event/xAOD/xAODJet
      Event/xAOD/xAODMissingET
      Event/xAOD/xAODTracking
      Event/xAOD/xAODTruth
      InnerDetector/InDetRecTools/InDetTrackSelectionTool
      InnerDetector/InDetRecTools/TrackVertexAssociationTool
      Reconstruction/Jet/JetEDM
      Reconstruction/Jet/JetInterface
      Reconstruction/Jet/JetRec
      Reconstruction/Jet/JetUtils
      Reconstruction/Jet/JetCalibTools
      Reconstruction/PFlow/PFlowUtils
      PRIVATE
      Calorimeter/CaloGeoHelpers
      Event/xAOD/xAODEventInfo
      Event/xAOD/xAODPFlow
      Tools/PathResolver
      Event/xAOD/xAODMetaData
      ${extra_deps} )
else()
   atlas_depends_on_subdirs(
      PUBLIC
      Calorimeter/CaloIdentifier
      Control/AthToolSupport/AsgTools
      Event/xAOD/xAODCaloEvent
      Event/xAOD/xAODJet
      Event/xAOD/xAODMissingET
      Event/xAOD/xAODTracking
      Event/xAOD/xAODTruth
      InnerDetector/InDetRecTools/InDetTrackSelectionTool
      InnerDetector/InDetRecTools/TrackVertexAssociationTool
      Reconstruction/Jet/JetEDM
      Reconstruction/Jet/JetInterface
      Reconstruction/Jet/JetRec
      Reconstruction/Jet/JetCalibTools
      Reconstruction/Jet/JetRecCalo
      Reconstruction/Jet/JetUtils
      PRIVATE
      Reconstruction/PFlow/PFlowUtils
      Calorimeter/CaloDetDescr
      Calorimeter/CaloEvent
      Calorimeter/CaloGeoHelpers
      Control/AthenaKernel
      Event/xAOD/xAODEventInfo
      Event/xAOD/xAODPFlow
      Tools/PathResolver
      Event/xAOD/xAODMetaData
      GaudiKernel )
endif()

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core Hist RIO TMVA)

# Component(s) in the package:
atlas_add_library( JetMomentToolsLib
   JetMomentTools/*.h Root/*.cxx
   PUBLIC_HEADERS JetMomentTools
   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES}
   AsgTools xAODCaloEvent xAODJet xAODMissingET xAODTracking xAODTruth JetEDM JetInterface TrackVertexAssociationToolLib
   JetRecLib JetUtils PFlowUtilsLib JetCalibToolsLib
   xAODMetaData InDetTrackSelectionToolLib AsgDataHandlesLib
   PRIVATE_LINK_LIBRARIES CaloGeoHelpers xAODEventInfo xAODPFlow PathResolver )

if( NOT XAOD_STANDALONE )
   set( extra_libs )
   if( NOT XAOD_ANALYSIS )
      set( extra_libs JetRecCaloLib CaloDetDescrLib CaloEvent AthenaKernel )
   endif()
   atlas_add_component( JetMomentTools
      src/*.h src/*.cxx src/components/*.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} CaloIdentifier xAODCaloEvent xAODJet xAODMetaData
      GaudiKernel JetRecLib JetUtils PathResolver JetMomentToolsLib JetCalibToolsLib
      ${extra_libs} )
endif()

#if( XAOD_STANDALONE )
atlas_add_dictionary( JetMomentToolsDict
	Root/JetMomentToolsDict.h
	Root/selection.xml
	LINK_LIBRARIES JetMomentToolsLib )
#endif()

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_runtime( share/*.root )
atlas_install_data( share/*.root )

