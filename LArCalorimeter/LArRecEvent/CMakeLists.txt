################################################################################
# Package: LArRecEvent
################################################################################

# Declare the package name:
atlas_subdir( LArRecEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Calorimeter/CaloIdentifier
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          DetectorDescription/Identifier
                          PRIVATE
                          Event/FourMomUtils )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( LArRecEvent
                   src/*.cxx
                   PUBLIC_HEADERS LArRecEvent
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES CaloEvent CaloIdentifier DataModel SGTools Identifier
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} FourMomUtils )

atlas_add_dictionary( LArRecEventDict
                      LArRecEvent/LArRecEventDict.h
                      LArRecEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} CaloEvent CaloIdentifier DataModel SGTools Identifier FourMomUtils LArRecEvent )

