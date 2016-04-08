################################################################################
# Package: TrigT1CaloEvent
################################################################################

# Declare the package name:
atlas_subdir( TrigT1CaloEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          GaudiKernel
                          Trigger/TrigT1/TrigT1Interfaces )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigT1CaloEventLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1CaloEvent
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES DataModel SGTools GaudiKernel TrigT1Interfaces
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_component( TrigT1CaloEvent
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools GaudiKernel TrigT1Interfaces TrigT1CaloEventLib )

atlas_add_dictionary( TrigT1CaloEventDict
                      TrigT1CaloEvent/TrigT1CaloEventDict.h
                      TrigT1CaloEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools GaudiKernel TrigT1Interfaces TrigT1CaloEventLib )

