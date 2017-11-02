################################################################################
# Package: TrigConfStorage
################################################################################

# Declare the package name:
atlas_subdir( TrigConfStorage )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Trigger/TrigConfiguration/TrigConfBase
                          Trigger/TrigConfiguration/TrigConfHLTData
                          Trigger/TrigConfiguration/TrigConfL1Data
                          PRIVATE
                          Trigger/TrigConfiguration/TrigConfJobOptData
                          Trigger/TrigT1/L1Topo/L1TopoConfig )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( COOL COMPONENTS CoolKernel CoolApplication )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )
find_package( ROOT COMPONENTS Core Tree MathCore MathMore Hist RIO pthread )
find_package( tdaq-common )

# tag NEEDS_CORAL_BASE was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( TrigConfStorage
                   src/*.cxx
                   PUBLIC_HEADERS TrigConfStorage
                   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${COOL_LIBRARIES} ${CORAL_LIBRARIES} TrigConfBase TrigConfHLTData TrigConfL1Data
                   PRIVATE_LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} TrigConfJobOptData L1TopoConfig )

atlas_add_executable( TrigConf2COOLApp
                      src/test/2COOLApp.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${COOL_LIBRARIES} ${CORAL_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} TrigConfBase TrigConfHLTData TrigConfL1Data TrigConfJobOptData L1TopoConfig TrigConfStorage )

atlas_add_executable( TrigConfConsistencyChecker
                      src/test/ConsistencyChecker.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${COOL_LIBRARIES} ${CORAL_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} TrigConfBase TrigConfHLTData TrigConfL1Data TrigConfJobOptData L1TopoConfig TrigConfStorage )

atlas_add_executable( TrigConfReadWrite
                      src/test/ReadWrite.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${COOL_LIBRARIES} ${CORAL_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} TrigConfBase TrigConfHLTData TrigConfL1Data TrigConfJobOptData L1TopoConfig TrigConfStorage )

atlas_add_executable( TrigConfCoolFix
                      src/test/CoolFix.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${COOL_LIBRARIES} ${CORAL_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} TrigConfBase TrigConfHLTData TrigConfL1Data TrigConfJobOptData L1TopoConfig TrigConfStorage )

atlas_add_executable( TrigConfTestTriggerDBCoolMix
                      src/test/TestTriggerDBCoolMix.cxx
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${COOL_LIBRARIES} ${CORAL_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} TrigConfBase TrigConfHLTData TrigConfL1Data TrigConfJobOptData L1TopoConfig TrigConfStorage )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( scripts/*.py )

