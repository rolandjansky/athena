# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LArConditionsTest )

# Component(s) in the package:
atlas_add_component( LArConditionsTest
                     src/LArConditionsTest_entries.cxx
                     src/TestLArConditionsTools.cxx
                     src/LArCondDataTest.cxx
                     src/LArConditionsTestAlg.cxx
                     src/LArCablingTest.cxx
                     LINK_LIBRARIES AthenaBaseComps StoreGateLib Identifier GaudiKernel LArCablingLib LArElecCalib LArIdentifier LArRawConditions LArRawUtilsLib LArRecConditions CaloDetDescrLib CaloEvent CaloIdentifier AthenaKernel CaloInterfaceLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

function (larconditions_run_test testName)
  cmake_parse_arguments( ARG "" "DEPENDS" "" ${ARGN} )

  configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/test/larconditions_test.sh.in
                  ${CMAKE_CURRENT_BINARY_DIR}/larconditions_${testName}.sh
                  @ONLY )
  atlas_add_test( ${testName}
                  SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/larconditions_${testName}.sh
                  ENVIRONMENT ATLAS_REFERENCE_TAG=LArConditionsTest/LArConditionsTest-01-00-12
                  PROPERTIES TIMEOUT 1200
                  LOG_IGNORE_PATTERN "Data source lookup|Resolved path|Failed to connect to service|Release number|Sorting algorithm|Failed to connect|Failure while attempting to connect|Reading file|^CORAL/|being retired|^Domain|INFO GeoModelSvc|locate catalog|Cache alignment|COOL_DISABLE|Failed to get ContainerHandle|^RalSessionMgr Info|^RelationalDatabase Info|Bootstrap.py|MetaDataSvc|xAODMaker|Opening COOL connection|Disconnecting from|IOVDbSvc +INFO"
                   )
  if( ARG_DEPENDS )
    set_tests_properties( LArConditionsTest_${testName}_ctest
                          PROPERTIES DEPENDS LArConditionsTest_${ARG_DEPENDS}_ctest )
  endif()
endfunction (larconditions_run_test)

larconditions_run_test (LArConditionsTest)
larconditions_run_test (LArConditionsTestWriteNoReg)
larconditions_run_test (LArConditionsTestReadNoReg DEPENDS LArConditionsTestWriteNoReg)

# Remaining tests have been disabled since forever.
# LArConditionsTestReadAndReg
# LArConditionsTestReadTwoStep
# LArConditionsTestWriteNoCorr
# LArConditionsTestReadNoCorr
# LArConditionsTestWriteCorr
# LArConditionsTestReadCorr
# LArConditionsTestWriteOneStep
# LArConditionsTestReadOneStep
