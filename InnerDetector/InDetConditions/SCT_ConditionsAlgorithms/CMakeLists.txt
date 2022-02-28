# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SCT_ConditionsAlgorithms )

# External dependencies:
find_package( Boost )

# Component(s) in the package:
atlas_add_component( SCT_ConditionsAlgorithms
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} AthenaBaseComps StoreGateLib Identifier DetDescrConditions GeoModelUtilities GeoPrimitives GaudiKernel SCT_ConditionsData SCT_CablingLib AthenaPoolUtilities InDetConditionsSummaryService InDetIdentifier InDetReadoutGeometry SCT_ReadoutGeometry TrkGeometry TrkSurfaces SCT_ConditionsToolsLib MuonReadoutGeometry TRT_ReadoutGeometry)

# Helper functions for test definitions:
function( _add_athena_test name )
   atlas_add_test( ${name}
      SCRIPT athena.py --threads=5 SCT_ConditionsAlgorithms/${name}.py
      POST_EXEC_SCRIPT nopost.sh
      PROPERTIES TIMEOUT 600
      ENVIRONMENT THREADS=5 )
endfunction()

function( _add_test name )
   atlas_add_test( ${name}
      SCRIPT python -m SCT_ConditionsAlgorithms.${name}
      POST_EXEC_SCRIPT nopost.sh
      PROPERTIES TIMEOUT 600 )
endfunction()

_add_athena_test( testCalibChipRead )
_add_athena_test( testCalibRead )
_add_athena_test( testConfig )
_add_athena_test( testDCSConditions )
_add_athena_test( testMajority )
_add_athena_test( testModuleVeto )
_add_athena_test( testMonRead )
_add_athena_test( testParameters )
_add_athena_test( testReadout )
_add_athena_test( testRodVeto )
_add_athena_test( testSensors )
_add_athena_test( testSilicon )
_add_athena_test( testStripVeto )
_add_athena_test( testSummary )
_add_athena_test( testTdaqEnabled )

_add_test( SCT_ReadCalibChipDataTestAlgConfig )
_add_test( SCT_ReadCalibDataTestAlgConfig )
_add_test( SCT_ConfigurationConditionsTestAlgConfig )
_add_test( SCT_DCSConditionsTestAlgConfig )
_add_test( SCT_MajorityConditionsTestAlgConfig )
_add_test( SCT_ModuleVetoTestAlgConfig )
_add_test( SCT_MonitorConditionsTestAlgConfig )
_add_test( SCT_ConditionsParameterTestAlgConfig )
_add_test( SCT_ReadoutTestAlgConfig )
_add_test( SCT_RODVetoTestAlgConfig )
_add_test( SCT_SensorsTestAlgConfig )
_add_test( SCT_StripVetoTestAlgConfig )
_add_test( SCT_ConditionsSummaryTestAlgConfig )
_add_test( SCT_TdaqEnabledTestAlgConfig )

atlas_add_test( TestLinkMasking
                SCRIPT share/TestLinkMasking.sh
                POST_EXEC_SCRIPT nopost.sh
                PROPERTIES TIMEOUT 600
                ENVIRONMENT THREADS=5 )

atlas_add_test( TestLinkMaskingNewConf
                SCRIPT share/TestLinkMaskingNewConf.sh
                POST_EXEC_SCRIPT nopost.sh
                PROPERTIES TIMEOUT 600 )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py )
atlas_install_scripts( share/*.sh )
