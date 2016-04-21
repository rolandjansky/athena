################################################################################
# Package: AtlasSealCLHEP
################################################################################

# Declare the package name:
atlas_subdir( AtlasSealCLHEP )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolServices
                          Control/CxxUtils )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( AtlasSealCLHEP
                   src/OldCLHEPStreamers.cxx
                   PUBLIC_HEADERS AtlasSealCLHEP
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES}
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( AtlasSealCLHEPDict
                      AtlasSealCLHEP/AtlasSealCLHEPDict.h
                      AtlasSealCLHEP/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasSealCLHEP )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

atlas_add_test( OldCLHEPStreamers_test
                SOURCES
                test/OldCLHEPStreamers_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasSealCLHEP )

