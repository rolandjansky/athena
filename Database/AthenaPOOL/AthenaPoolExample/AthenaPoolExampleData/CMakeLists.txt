################################################################################
# Package: AthenaPoolExampleData
################################################################################

# Declare the package name:
atlas_subdir( AthenaPoolExampleData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/AthLinks
                          Control/CLIDSvc
                          Control/Navigation
                          Control/SGTools
                          GaudiKernel )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( AthenaPoolExampleData
                   src/*.cxx
                   PUBLIC_HEADERS AthenaPoolExampleData
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthContainers AthLinks Navigation SGTools GaudiKernel
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( AthenaPoolExampleDataDict
                      AthenaPoolExampleData/AthenaPoolExampleDataDict.h
                      AthenaPoolExampleData/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthContainers AthLinks Navigation SGTools GaudiKernel AthenaPoolExampleData
                      NAVIGABLES ExampleHitContainer )

