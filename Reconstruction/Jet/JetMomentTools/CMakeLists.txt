# $Id: CMakeLists.txt 771040 2016-08-31 13:46:25Z ssnyder $
################################################################################
# Package: JetMomentTools
################################################################################

# Declare the package name:
atlas_subdir( JetMomentTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
  PUBLIC
  Calorimeter/CaloIdentifier
  Control/AthToolSupport/AsgTools
  Event/xAOD/xAODCaloEvent
  Event/xAOD/xAODJet
  Event/xAOD/xAODMissingET
  Event/xAOD/xAODTracking
  GaudiKernel
  Reconstruction/Jet/JetEDM
  Reconstruction/Jet/JetInterface
  Reconstruction/Jet/JetRec
  Reconstruction/Jet/JetRecCalo
  Reconstruction/Jet/JetUtils
  PRIVATE
  Calorimeter/CaloDetDescr
  Calorimeter/CaloEvent
  Calorimeter/CaloGeoHelpers
  Control/AthenaKernel
  Event/xAOD/xAODEventInfo
  Event/xAOD/xAODPFlow
  Tools/PathResolver )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core Hist )

# Component(s) in the package:
atlas_add_library( JetMomentToolsLib
  JetMomentTools/*.h src/*.h src/*.cxx Root/*.cxx src/components/*.cxx
  PUBLIC_HEADERS JetMomentTools
  PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} CaloDetDescrLib
  CaloEvent CaloIdentifier AsgTools AthenaKernel xAODCaloEvent xAODJet
  xAODMissingET xAODTracking GaudiKernel JetEDM JetInterface JetRecLib
  JetUtils CaloGeoHelpers xAODEventInfo xAODPFlow PathResolver JetRecCaloLib )


atlas_add_component( JetMomentTools
  JetMomentTools/*.h src/*.h src/*.cxx Root/*.cxx src/components/*.cxx
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} CaloDetDescrLib
  CaloEvent CaloIdentifier AsgTools AthenaKernel xAODCaloEvent xAODJet
  xAODMissingET xAODTracking GaudiKernel JetEDM JetInterface JetRecLib
  JetUtils CaloGeoHelpers xAODEventInfo xAODPFlow PathResolver JetRecCaloLib )

atlas_add_dictionary( JetMomentToolsDict
		      JetMomentTools/JetMomentToolsDict.h
                      JetMomentTools/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} 
		      CaloDetDescrLib CaloEvent CaloIdentifier AsgTools 
		      AthenaKernel xAODCaloEvent xAODJet xAODMissingET 
		      xAODTracking GaudiKernel JetEDM JetInterface JetRecLib
  		      JetUtils CaloGeoHelpers xAODEventInfo xAODPFlow 
		      PathResolver JetRecCaloLib JetMomentToolsLib )

# Install files from the package:
atlas_install_headers( JetMomentTools )
atlas_install_python_modules( python/*.py )
atlas_install_runtime( share/*.root )
atlas_install_data( share/*.root )
