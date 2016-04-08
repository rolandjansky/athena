################################################################################
# Package: ZDC_SimEvent
################################################################################

# Declare the package name:
atlas_subdir( ZDC_SimEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/SGTools
                          Simulation/HitManagement )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( ZDC_SimEvent
                   src/*.cxx
                   PUBLIC_HEADERS ZDC_SimEvent
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES SGTools HitManagement
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( ZDC_SimEventDict
                      ZDC_SimEvent/ZDC_SimEventDict.h
                      ZDC_SimEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} SGTools HitManagement ZDC_SimEvent )

