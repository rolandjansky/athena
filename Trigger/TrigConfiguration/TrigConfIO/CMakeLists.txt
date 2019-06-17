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

# Component(s) in the package:
atlas_add_library( TrigConfIOLib
                   TrigConfIO/*.h src/*.cxx
                   PUBLIC_HEADERS TrigConfIO
                   LINK_LIBRARIES PathResolver TrigConfBase TrigConfData GaudiKernel )

atlas_add_executable( TestTriggerMenuAccess utils/TestTriggerMenuAccess.cxx 
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} TrigConfIO )

atlas_install_python_modules( python/*.py )

atlas_install_scripts( scripts/*.py )

atlas_install_data( data/*.json data/*.xml )

atlas_install_joboptions( share/*.py )

atlas_add_test( ReadTriggerConfig SOURCES test/read_config_info.cxx
                INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                LINK_LIBRARIES ${Boost_LIBRARIES} TrigConfData TrigConfIOLib
                ENVIRONMENT "TESTFILEPATH=${CMAKE_CURRENT_SOURCE_DIR}/test/data"
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_component( TrigConfIO
                     src/components/*.cxx
                     LINK_LIBRARIES TrigConfIOLib )
