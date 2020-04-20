################################################################################
# Package: CoolLumiUtilities
################################################################################

# Declare the package name:
atlas_subdir( CoolLumiUtilities )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Control/StoreGate
                          GaudiKernel
                          PRIVATE
                          AtlasTest/TestTools
                          Database/AthenaPOOL/AthenaPoolUtilities )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( CoolLumiUtilitiesLib
                   src/*.cxx
                   PUBLIC_HEADERS CoolLumiUtilities
                   INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${COOL_LIBRARIES} ${CORAL_LIBRARIES} AthenaBaseComps AthenaKernel GaudiKernel StoreGateLib SGtests
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaPoolUtilities )

atlas_add_component( CoolLumiUtilities
                     src/components/*.cxx
                     INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${COOL_LIBRARIES} ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests GaudiKernel AthenaPoolUtilities CoolLumiUtilitiesLib )

atlas_add_dictionary( CoolLumiUtilitiesDict
                      CoolLumiUtilities/CoolLumiUtilitiesDict.h
                      CoolLumiUtilities/selection.xml
                      INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${COOL_LIBRARIES} ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests GaudiKernel AthenaPoolUtilities CoolLumiUtilitiesLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8})
atlas_install_scripts( share/*.py )
atlas_install_joboptions( share/*.txt )


atlas_add_test( OnlineLumiCalibrationCondData_test
  SOURCES test/OnlineLumiCalibrationCondData_test.cxx
  INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
  LINK_LIBRARIES ${COOL_LIBRARIES} ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests GaudiKernel AthenaPoolUtilities CoolLumiUtilitiesLib TestTools )

atlas_add_test( BunchGroupCondData_test
  SOURCES test/BunchGroupCondData_test.cxx
  INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
  LINK_LIBRARIES ${COOL_LIBRARIES} ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests GaudiKernel AthenaPoolUtilities CoolLumiUtilitiesLib TestTools )

atlas_add_test( BunchLumisCondData_test
  SOURCES test/BunchLumisCondData_test.cxx
  INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
  LINK_LIBRARIES ${COOL_LIBRARIES} ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests GaudiKernel AthenaPoolUtilities CoolLumiUtilitiesLib TestTools )

atlas_add_test( FillParamsCondData_test
  SOURCES test/FillParamsCondData_test.cxx
  INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
  LINK_LIBRARIES ${COOL_LIBRARIES} ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests GaudiKernel AthenaPoolUtilities CoolLumiUtilitiesLib TestTools )

atlas_add_test( OnlineLumiCalibrationCondAlg_test
  SOURCES test/OnlineLumiCalibrationCondAlg_test.cxx
  INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
  LINK_LIBRARIES ${COOL_LIBRARIES} ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests GaudiKernel AthenaPoolUtilities CoolLumiUtilitiesLib TestTools )

atlas_add_test( BunchGroupCondAlg_test
  SOURCES test/BunchGroupCondAlg_test.cxx
  INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
  LINK_LIBRARIES ${COOL_LIBRARIES} ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests GaudiKernel AthenaPoolUtilities CoolLumiUtilitiesLib TestTools )

atlas_add_test( BunchLumisCondAlg_test
  SOURCES test/BunchLumisCondAlg_test.cxx
  INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
  LINK_LIBRARIES ${COOL_LIBRARIES} ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests GaudiKernel AthenaPoolUtilities CoolLumiUtilitiesLib TestTools )

atlas_add_test( FillParamsCondAlg_test
  SOURCES test/FillParamsCondAlg_test.cxx
  INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
  LINK_LIBRARIES ${COOL_LIBRARIES} ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel StoreGateLib SGtests GaudiKernel AthenaPoolUtilities CoolLumiUtilitiesLib TestTools )


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

