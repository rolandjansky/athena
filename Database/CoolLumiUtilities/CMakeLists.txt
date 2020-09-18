# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CoolLumiUtilities )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( CoolLumiUtilitiesLib
                   src/*.cxx
                   PUBLIC_HEADERS CoolLumiUtilities
                   INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${COOL_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel AthenaPoolUtilities GaudiKernel
                   PRIVATE_LINK_LIBRARIES CxxUtils PersistentDataModel StoreGateLib )

atlas_add_component( CoolLumiUtilities
                     src/components/*.cxx
                     LINK_LIBRARIES CoolLumiUtilitiesLib )

atlas_add_dictionary( CoolLumiUtilitiesDict
                      CoolLumiUtilities/CoolLumiUtilitiesDict.h
                      CoolLumiUtilities/selection.xml
                      LINK_LIBRARIES CoolLumiUtilitiesLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8})
atlas_install_scripts( share/*.py )
atlas_install_joboptions( share/*.txt )

atlas_add_test( OnlineLumiCalibrationCondData_test
  SOURCES test/OnlineLumiCalibrationCondData_test.cxx
  LINK_LIBRARIES CoolLumiUtilitiesLib CxxUtils )

atlas_add_test( BunchGroupCondData_test
  SOURCES test/BunchGroupCondData_test.cxx
  LINK_LIBRARIES CoolLumiUtilitiesLib )

atlas_add_test( BunchLumisCondData_test
  SOURCES test/BunchLumisCondData_test.cxx
  LINK_LIBRARIES CoolLumiUtilitiesLib )

atlas_add_test( FillParamsCondData_test
  SOURCES test/FillParamsCondData_test.cxx
  LINK_LIBRARIES CoolLumiUtilitiesLib )

atlas_add_test( OnlineLumiCalibrationCondAlg_test
  SOURCES test/OnlineLumiCalibrationCondAlg_test.cxx
  LINK_LIBRARIES CoolLumiUtilitiesLib CxxUtils TestTools )

atlas_add_test( BunchGroupCondAlg_test
  SOURCES test/BunchGroupCondAlg_test.cxx
  LINK_LIBRARIES CoolLumiUtilitiesLib PersistentDataModel TestTools )

atlas_add_test( BunchLumisCondAlg_test
  SOURCES test/BunchLumisCondAlg_test.cxx
  LINK_LIBRARIES CoolLumiUtilitiesLib CxxUtils PersistentDataModel TestTools )

atlas_add_test( FillParamsCondAlg_test
  SOURCES test/FillParamsCondAlg_test.cxx
  LINK_LIBRARIES CoolLumiUtilitiesLib PersistentDataModel TestTools )

atlas_add_test( OnlineLumiCalibrationCondAlgConfig_test
                SCRIPT python -m CoolLumiUtilities.OnlineLumiCalibrationCondAlgConfig
                LOG_SELECT_PATTERN "ComponentAccumulator|^---|^IOVDbSvc" )

atlas_add_test( FillParamsCondAlgConfig_test
                SCRIPT python -m CoolLumiUtilities.FillParamsCondAlgConfig
                LOG_SELECT_PATTERN "ComponentAccumulator|^---|^IOVDbSvc" )

atlas_add_test( BunchLumisCondAlgConfig_test
                SCRIPT python -m CoolLumiUtilities.BunchLumisCondAlgConfig
                LOG_SELECT_PATTERN "ComponentAccumulator|^---|^IOVDbSvc" )

atlas_add_test( BunchGroupCondAlgConfig_test
                SCRIPT python -m CoolLumiUtilities.BunchGroupCondAlgConfig
                LOG_SELECT_PATTERN "ComponentAccumulator|^---|^IOVDbSvc" )

