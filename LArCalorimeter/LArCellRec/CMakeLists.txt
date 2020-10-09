################################################################################
# Package: LArCellRec
################################################################################

# Declare the package name:
atlas_subdir( LArCellRec )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloConditions
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloEvent
                          Calorimeter/CaloIdentifier
                          Calorimeter/CaloInterface
                          Calorimeter/CaloRec
                          Calorimeter/CaloUtils
                          Control/AthAllocators
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
			  Control/CxxUtils
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/Identifier
                          GaudiKernel
                          LArCalorimeter/LArElecCalib
                          LArCalorimeter/LArIdentifier
                          LArCalorimeter/LArRawEvent
                          LArCalorimeter/LArRecConditions
                          LArCalorimeter/LArCOOLConditions
                          LArCalorimeter/LArSimEvent
                          Event/xAOD/xAODTrigL1Calo
                          PRIVATE
                          Calorimeter/CaloTriggerTool
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODEventInfo
                          Generators/GeneratorObjects
                          LArCalorimeter/LArRecEvent
                          LArCalorimeter/LArCabling
                          LArCalorimeter/LArCOOLConditions
                          Trigger/TrigT1/TrigT1CaloCalibConditions
                          Trigger/TrigT1/TrigT1CaloCondSvc 
                          Generators/AtlasHepMC )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( TBB )

#Component(s) in the package:
atlas_add_component( LArCellRec
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}  ${TBB_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib ${TBB_LIBRARIES} CaloConditions CaloDetDescrLib CaloEvent CaloIdentifier 
		     CaloRecLib CaloUtilsLib AthAllocators AthenaBaseComps AthenaKernel StoreGateLib CxxUtils SGtests AtlasDetDescr Identifier GaudiKernel LArIdentifier 
		     LArRawEvent LArRecConditions LArCOOLConditions LArSimEvent CaloTriggerToolLib AthenaPoolUtilities xAODEventInfo xAODTrigL1Calo GeneratorObjects 
		     LArRecEvent LArCablingLib TrigT1CaloCalibConditions TrigT1CaloCondSvcLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )


#atlas_add_test( LArBadFebMaskingTool_test
#  SCRIPT test/LArBadFebMaskingTool_test.sh
#  PROPERTIES TIMEOUT 600
#  LOG_IGNORE_PATTERN "LArDetectorToolNV|is still valid|no data retrieved|Database being retired|Reading file|Unable to locate catalog|Resolved path|DigitizationFlags|^Domain|created CondCont|no dictionary for class|^ +[+]|Reading LArPedestalMC|IOV callback|^DetectorStore|Cache alignment" )


atlas_add_test( LArCellDeadOTXCorr_test
  SCRIPT test/LArCellDeadOTXCorr_test.sh
  PROPERTIES TIMEOUT 600
  LOG_IGNORE_PATTERN "LArDetectorToolNV|is still valid|no data retrieved|Database being retired|Reading file|Unable to locate catalog|Resolved path|DigitizationFlags|^Domain|created CondCont|no dictionary for class|^ +[+]|Reading LArPedestalMC|IOV callback|^DetectorStore|Cache alignment|object with key|recorded new|into Conditions|IOVDbSvc +INFO" )


