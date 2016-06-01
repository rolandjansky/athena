################################################################################
# Package: TrigT1CaloUtils
################################################################################

# Declare the package name:
atlas_subdir( TrigT1CaloUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          Event/xAOD/xAODTrigL1Calo
                          GaudiKernel
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigT1/TrigT1CaloEvent
                          Trigger/TrigT1/TrigT1Interfaces
                          PRIVATE
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigT1/TrigT1Result )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigT1CaloUtilsLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1CaloUtils
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES CaloEvent DataModel SGTools xAODTrigL1Calo GaudiKernel TrigT1Interfaces TrigT1CaloEventLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} TrigConfL1Data TrigT1Result )

atlas_add_component( TrigT1CaloUtils
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} CaloEvent DataModel SGTools xAODTrigL1Calo GaudiKernel TrigT1CaloEventLib TrigT1Interfaces TrigConfL1Data TrigT1Result TrigT1CaloUtilsLib )

