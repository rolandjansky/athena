# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArCellRec )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( TBB )

#Component(s) in the package:
atlas_add_component( LArCellRec
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}  ${TBB_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${TBB_LIBRARIES} CaloConditions CaloDetDescrLib CaloEvent CaloIdentifier CaloInterfaceLib
		             CaloUtilsLib AthAllocators AthenaBaseComps AthenaKernel StoreGateLib CxxUtils Identifier GaudiKernel LArIdentifier
		             LArRawEvent LArRecConditions CaloTriggerToolLib AthenaPoolUtilities xAODEventInfo xAODTrigL1Calo
		             LArRecEvent LArCablingLib LArElecCalib TrigT1CaloCalibConditions TrigT1CaloCondSvcLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

#atlas_add_test( LArBadFebMaskingTool_test
#  SCRIPT test/LArBadFebMaskingTool_test.sh
#  PROPERTIES TIMEOUT 600
#  LOG_IGNORE_PATTERN "LArDetectorToolNV|is still valid|no data retrieved|Database being retired|Reading file|Unable to locate catalog|Resolved path|DigitizationFlags|^Domain|created CondCont|no dictionary for class|^ +[+]|Reading LArPedestalMC|IOV callback|^DetectorStore|Cache alignment" )

atlas_add_test( LArCellDeadOTXCorr_test
  SCRIPT test/LArCellDeadOTXCorr_test.sh
  PROPERTIES TIMEOUT 600
  LOG_IGNORE_PATTERN "LArDetectorToolNV|is still valid|no data retrieved|Database being retired|Reading file|Unable to locate catalog|Resolved path|DigitizationFlags|^Domain|created CondCont|no dictionary for class|^ +[+]|Reading LArPedestalMC|IOV callback|^DetectorStore|Cache alignment|object with key|recorded new|into Conditions" )

