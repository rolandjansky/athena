################################################################################
# Package: LArRawEvent
################################################################################

# Declare the package name:
atlas_subdir( LArRawEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          Control/AthenaKernel
                          Control/AthContainers
                          DetectorDescription/Identifier
                          PRIVATE
                          Control/SGTools )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( LArRawEvent
                   src/*.cxx
                   PUBLIC_HEADERS LArRawEvent
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} CaloIdentifier AthContainers Identifier
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} SGTools )

atlas_add_dictionary( LArRawEventDict
                      LArRawEvent/LArRawEventDict.h
                      LArRawEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloIdentifier AthContainers Identifier SGTools LArRawEvent )

