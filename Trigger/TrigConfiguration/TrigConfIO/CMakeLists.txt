################################################################################
# Package: TrigConfIO
################################################################################

# Declare the package name:
atlas_subdir( TrigConfIO )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Trigger/TrigConfiguration/TrigConfBase
                          Trigger/TrigConfiguration/TrigConfData
                          PRIVATE
                          )

# External dependencies:
find_package( Boost )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( TrigConfIO
  TrigConfIO/*.h src/*.cxx
  PUBLIC_HEADERS TrigConfIO
  INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
  PRIVATE_INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
  LINK_LIBRARIES TrigConfData TrigConfBase
  PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} -lstdc++fs
  )

atlas_add_library( TrigConfIOSA
  TrigConfIO/*.h src/*.cxx
  PUBLIC_HEADERS TrigConfIO
  INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
  PRIVATE_INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
  LINK_LIBRARIES TrigConfDataSA TrigConfBase
  DEFINITIONS -DTRIGCONF_STANDALONE
  PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${CORAL_LIBRARIES} -lstdc++fs
  )

atlas_add_executable( TestTriggerMenuAccess utils/TestTriggerMenuAccess.cxx 
  INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
  LINK_LIBRARIES ${Boost_LIBRARIES} TrigConfData TrigConfIO
  )

atlas_add_executable( TriggerMenuRW utils/TriggerMenuRW.cxx 
  INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
  LINK_LIBRARIES ${Boost_LIBRARIES} TrigConfIO
  )

# Install files from the package.
atlas_install_python_modules( python/*.py )
atlas_install_scripts( scripts/*.py )
atlas_install_data( data/*.json data/*.xml )
atlas_install_joboptions( share/*.py )

# => disabling this test for now until I find a better way to store the test menus
# Test(s) in the package.
# atlas_add_test( ReadTriggerConfig
#   SOURCES test/read_config_info.cxx
#   LINK_LIBRARIES TrigConfData TrigConfIO
#   ENVIRONMENT "TESTFILEPATH=${CMAKE_CURRENT_SOURCE_DIR}/test/data"
#   POST_EXEC_SCRIPT nopost.sh
#   )

