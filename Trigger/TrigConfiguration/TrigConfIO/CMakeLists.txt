################################################################################
# Package: TrigConfIO
################################################################################

# Declare the package name:
atlas_subdir( TrigConfIO )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Trigger/TrigConfiguration/TrigConfData
                          Trigger/TrigConfiguration/TrigConfBase
                          PRIVATE
                          Control/StoreGate
                          Control/AthenaBaseComps
                          Tools/PathResolver
                          )

# External dependencies:
find_package( Boost )
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )


# Component(s) in the package:
atlas_add_library( TrigConfIOLib
  TrigConfIO/*.h src/JsonFileLoader.cxx src/TrigDBLoader.cxx src/TrigDBMenuLoader.cxx src/TrigDBJobOptionsLoader.cxx src/TrigDBL1PrescalesSetLoader.cxx src/TrigDBHLTPrescalesSetLoader.cxx
  PUBLIC_HEADERS TrigConfIO
  INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
  LINK_LIBRARIES ${Boost_LIBRARIES} TrigConfBase TrigConfData GaudiKernel
  PRIVATE_LINK_LIBRARIES PathResolver
  )

atlas_add_component( TrigConfIO
  src/HLTMenuCondAlg.* src/L1MenuCondAlg.* src/components/*.cxx
  LINK_LIBRARIES GaudiKernel AthenaBaseComps StoreGateLib TrigConfData
  TrigConfIOLib
  )

atlas_add_executable( TestTriggerMenuAccess utils/TestTriggerMenuAccess.cxx 
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} TrigConfData TrigConfIOLib
   )

atlas_add_executable( TriggerMenuRW utils/TriggerMenuRW.cxx 
  INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
  LINK_LIBRARIES ${Boost_LIBRARIES} TrigConfIOLib
  )

# Install files from the package.
atlas_install_python_modules( python/*.py )
atlas_install_scripts( scripts/*.py )
atlas_install_data( data/*.json data/*.xml )
atlas_install_joboptions( share/*.py )

# Test(s) in the package.
atlas_add_test( ReadTriggerConfig
   SOURCES test/read_config_info.cxx
   LINK_LIBRARIES TrigConfData TrigConfIOLib
   ENVIRONMENT "TESTFILEPATH=${CMAKE_CURRENT_SOURCE_DIR}/test/data"
   POST_EXEC_SCRIPT nopost.sh
   )

