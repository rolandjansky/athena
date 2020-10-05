################################################################################
# Package: G4AncillarySimEvent
################################################################################

# Declare the package name:
atlas_subdir( G4AncillarySimEvent )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( G4AncillarySimEvent
                   src/*.cxx
                   PUBLIC_HEADERS G4AncillarySimEvent
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel GeneratorObjects HitManagement
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( G4AncillarySimEventDict
                      G4AncillarySimEvent/G4AncillarySimEventDict.h
                      G4AncillarySimEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AthenaKernel GeneratorObjects HitManagement G4AncillarySimEvent )

