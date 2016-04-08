################################################################################
# Package: ZdcEvent
################################################################################

# Declare the package name:
atlas_subdir( ZdcEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          DetectorDescription/Identifier
                          PRIVATE
                          ForwardDetectors/ZDC/ZdcConditions
                          ForwardDetectors/ZDC/ZdcIdentifier )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( ZdcEvent
                   src/*.cxx
                   PUBLIC_HEADERS ZdcEvent
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES DataModel SGTools Identifier
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ZdcConditions ZdcIdentifier )

atlas_add_dictionary( ZdcEventDict
                      ZdcEvent/ZdcEventDict.h
                      ZdcEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools Identifier ZdcConditions ZdcIdentifier ZdcEvent )

