# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LumiBlockComps )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel )
find_package( CORAL COMPONENTS CoralBase )
find_package( ROOT COMPONENTS Core Tree RIO )

# Component(s) in the package:
set( extra_libs )
set( extra_srcs )
if( NOT XAOD_ANALYSIS )
   set( extra_libs LumiBlockCoolQuery CoolLumiUtilitiesLib )
   set( extra_srcs src/*.cxx )
endif()
atlas_add_library( LumiBlockCompsLib
   LumiBlockComps/*.h src/*.h Root/*.cxx
   src/CreateLumiBlockCollectionFromFile.cxx 
   ${extra_srcs}
   PUBLIC_HEADERS LumiBlockComps
   INCLUDE_DIRS ${CORAL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   PRIVATE_INCLUDE_DIRS ${COOL_INCLUDE_DIRS}
   LINK_LIBRARIES ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps
   AthenaKernel xAODLuminosity GaudiKernel StoreGateLib AthenaPoolUtilities ${extra_libs}
   PRIVATE_LINK_LIBRARIES ${COOL_LIBRARIES} DBDataModel
   GoodRunsListsLib xAODRootAccess )

atlas_add_component( LumiBlockComps
   src/components/*.cxx
   LINK_LIBRARIES GaudiKernel LumiBlockCompsLib AthenaPoolUtilities )

atlas_add_dictionary( LumiBlockCompsDict
   LumiBlockComps/LumiBlockCompsDict.h
   LumiBlockComps/selection.xml
   LINK_LIBRARIES LumiBlockCompsLib )

atlas_add_executable( getLumi
   util/getLumi.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} xAODLuminosity GoodRunsListsLib
   xAODRootAccess LumiBlockCompsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py
   POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_joboptions( share/*.txt )

atlas_add_test( LBDurationCondAlg_test
   SOURCES test/LBDurationCondAlg_test.cxx
   LINK_LIBRARIES GaudiKernel LumiBlockCompsLib )

atlas_add_test( LuminosityCondAlg_test
   SOURCES test/LuminosityCondAlg_test.cxx
   LINK_LIBRARIES GaudiKernel LumiBlockCompsLib AthenaPoolUtilities )

atlas_add_test( TrigLiveFractionCondAlg_test
   SOURCES test/TrigLiveFractionCondAlg_test.cxx
   LINK_LIBRARIES GaudiKernel LumiBlockCompsLib )

atlas_add_test( LuminosityCondAlgConfig_test
                SCRIPT python -m LumiBlockComps.LuminosityCondAlgConfig
                LOG_SELECT_PATTERN "ComponentAccumulator|^---|^IOVDbSvc" )

atlas_add_test( LBDurationCondAlgConfig_test
                SCRIPT python -m LumiBlockComps.LBDurationCondAlgConfig
                LOG_SELECT_PATTERN "ComponentAccumulator|^---|^IOVDbSvc" )

atlas_add_test( TrigLiveFractionCondAlgConfig_test
                SCRIPT python -m LumiBlockComps.TrigLiveFractionCondAlgConfig
                LOG_SELECT_PATTERN "ComponentAccumulator|^---|^IOVDbSvc" )

atlas_add_test( LumiBlockMuWriterConfig_test
                SCRIPT python -m LumiBlockComps.LumiBlockMuWriterConfig
                LOG_SELECT_PATTERN "ComponentAccumulator|^---|^IOVDbSvc" )

atlas_add_test( BunchCrossingCondAlg 
                SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/python/BunchCrossingCondAlgTest.py
                POST_EXEC_SCRIPT " /usr/bin/diff BCData.txt ${CMAKE_CURRENT_SOURCE_DIR}/share/BCData.txt.ref > BC_diff.log " )
