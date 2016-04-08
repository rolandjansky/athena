################################################################################
# Package: IOVDbTestConditions
################################################################################

# Declare the package name:
atlas_subdir( IOVDbTestConditions )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/SGTools
                          Database/AthenaPOOL/AthenaPoolUtilities )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( IOVDbTestConditions
                   src/IOVDbTestAmdbCorrection.cxx
                   src/IOVDbTestMDTEleMap.cxx
                   PUBLIC_HEADERS IOVDbTestConditions
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} SGTools AthenaPoolUtilities
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( IOVDbTestConditionsDict
                      IOVDbTestConditions/IOVDbTestConditionsDict.h
                      IOVDbTestConditions/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} SGTools AthenaPoolUtilities IOVDbTestConditions )

