################################################################################
# Package: AnalysisTriggerEvent
################################################################################

# Declare the package name:
atlas_subdir( AnalysisTriggerEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/Navigation
                          Control/SGTools
                          Event/EventKernel
                          Event/FourMom )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( AnalysisTriggerEvent
                   src/*.cxx
                   PUBLIC_HEADERS AnalysisTriggerEvent
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES Navigation SGTools EventKernel FourMom
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( AnalysisTriggerEventDict
                      AnalysisTriggerEvent/AnalysisTriggerEventDict.h
                      AnalysisTriggerEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} Navigation SGTools EventKernel FourMom AnalysisTriggerEvent )

