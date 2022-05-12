# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( DataQualityInterfaces )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core Hist Tree RIO ROOTTPython )
find_package( tdaq-common COMPONENTS dqm_core_io dqm_core dqm_dummy_io dqm_dummy )
find_package( COOL COMPONENTS CoolKernel CoolApplication )
find_package( CORAL COMPONENTS CoralBase )
find_package( nlohmann_json )

# Component(s) in the package:
atlas_add_root_dictionary( DataQualityInterfaces
   DataQualityInterfacesDictSource
   ROOT_HEADERS  DataQualityInterfaces/MiniConfig.h
                 DataQualityInterfaces/MiniConfigTreeNode.h
                 DataQualityInterfaces/HanApp.h
                 DataQualityInterfaces/HanConfig.h
                 DataQualityInterfaces/HanConfigAlgLimit.h
                 DataQualityInterfaces/HanConfigAlgPar.h
                 DataQualityInterfaces/HanConfigParMap.h
                 DataQualityInterfaces/HanConfigAssessor.h
                 DataQualityInterfaces/HanConfigCompAlg.h
                 DataQualityInterfaces/HanConfigGroup.h
                 DataQualityInterfaces/HanConfigMetadata.h
                 DataQualityInterfaces/ConditionsSingleton.h
                 DataQualityInterfaces/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

atlas_add_library( DataQualityInterfaces
   DataQualityInterfaces/*.h
   src/HanAlgorithmConfig.cxx src/HanApp.cxx src/HanConfig.cxx
   src/HanConfigAlgLimit.cxx src/HanConfigAlgPar.cxx src/HanConfigParMap.cxx
   src/HanConfigAssessor.cxx src/HanConfigCompAlg.cxx src/HanConfigGroup.cxx
   src/HanConfigMetadata.cxx src/HanOutput.cxx src/MiniConfig.cxx
   src/DatabaseConfig.cxx src/MiniConfigTreeNode.cxx src/CompositeAlgorithm.cxx
   src/HanInputRootFile.cxx src/HanUtils.cxx src/ConditionsSingleton.cxx
   ${DataQualityInterfacesDictSource}
   PUBLIC_HEADERS DataQualityInterfaces
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
   ${TDAQ-COMMON_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} ${TDAQ-COMMON_LIBRARIES}
   ${COOL_LIBRARIES} ${CORAL_LIBRARIES} nlohmann_json::nlohmann_json )

atlas_add_executable( han
   src/han.cxx
   LINK_LIBRARIES DataQualityInterfaces )

atlas_add_executable( han-config-gen
   src/han_config_gen.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} DataQualityInterfaces CxxUtils )

atlas_add_executable( han-config-print
   src/han_config_print.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} DataQualityInterfaces )

atlas_install_python_modules( python/*.py
POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-select E11,W1 --extend-ignore E731 )

atlas_install_scripts( scripts/han_lark_tester.py )

atlas_install_data( data/* )

# Tests:
atlas_add_test( han_functions
    SCRIPT test/test_han_functions.sh ${CMAKE_CURRENT_SOURCE_DIR}/share
    PROPERTIES TIMEOUT 300
)