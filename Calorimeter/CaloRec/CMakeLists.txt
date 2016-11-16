# $Id: CMakeLists.txt 773817 2016-09-19 09:17:19Z krasznaa $
################################################################################
# Package: CaloRec
################################################################################

# Declare the package name:
atlas_subdir( CaloRec )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Calorimeter/CaloConditions
   Calorimeter/CaloEvent
   Control/AthenaBaseComps
   Control/AthenaKernel
   Control/StoreGate
   Database/AthenaPOOL/AthenaPoolUtilities
   Event/xAOD/xAODCaloEvent
   GaudiKernel
   PRIVATE
   AtlasTest/TestTools
   Control/CxxUtils
   Control/AthAllocators
   Control/SGTools
   DetectorDescription/GeoModel/GeoModelInterfaces
   DetectorDescription/IdDictParser
   DetectorDescription/Identifier
   Event/EventKernel
   Event/FourMom
   Event/NavFourMom
   Calorimeter/CaloDetDescr
   Calorimeter/CaloGeoHelpers
   Calorimeter/CaloIdentifier
   Calorimeter/CaloInterface
   Calorimeter/CaloUtils
   LArCalorimeter/LArTools
   LArCalorimeter/LArElecCalib
   TestPolicy )

# External dependencies:
find_package( AIDA )
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( Eigen )
find_package( ROOT COMPONENTS Core MathCore MathMore Minuit Minuit2 Matrix )

# Component(s) in the package:
atlas_add_library( CaloRecLib
   CaloRec/*.h CaloRec/*.icc src/*.cxx
   PUBLIC_HEADERS CaloRec
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${AIDA_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   ${CORAL_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
   DEFINITIONS ${CLHEP_DEFINITIONS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloConditions CaloEvent CaloGeoHelpers
   CaloIdentifier AthenaBaseComps AthenaKernel CxxUtils AthenaPoolUtilities
   Identifier xAODCaloEvent GaudiKernel CaloDetDescrLib CaloUtilsLib
   StoreGateLib LArToolsLib
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES}
   ${EIGEN_LIBRARIES} AthAllocators DataModel SGTools IdDictParser EventKernel
   FourMom NavFourMom )

atlas_add_component( CaloRec
   src/components/*.cxx
   LINK_LIBRARIES CaloRecLib )

# Test(s) in the package:
atlas_add_test( CaloClusterProcessor_test
   SOURCES test/CaloClusterProcessor_test.cxx
   LINK_LIBRARIES CaloRecLib xAODCaloEvent TestTools AthenaBaseComps )

atlas_add_test( CaloCellFastCopyTool_test
   SOURCES test/CaloCellFastCopyTool_test.cxx
   LINK_LIBRARIES CaloRecLib CaloEvent CaloDetDescrLib CaloIdentifier
   IdDictParser AthenaBaseComps StoreGateLib CxxUtils GaudiKernel
   EXTRA_PATTERNS "Retrieved const handle|Service base class initialized|DEBUG Property update for OutputLevel" )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
