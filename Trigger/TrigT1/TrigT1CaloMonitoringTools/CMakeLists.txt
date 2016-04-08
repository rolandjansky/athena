################################################################################
# Package: TrigT1CaloMonitoringTools
################################################################################

# Declare the package name:
atlas_subdir( TrigT1CaloMonitoringTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Control/AthenaMonitoring
                          GaudiKernel
                          PRIVATE
                          Control/DataModel
                          Control/SGTools
                          Event/EventInfo
                          Event/xAOD/xAODTrigL1Calo
                          Tools/LWHists
                          Trigger/TrigConfiguration/TrigConfInterfaces
                          Trigger/TrigConfiguration/TrigConfL1Data
                          Trigger/TrigT1/TrigT1CaloEvent
                          Trigger/TrigT1/TrigT1CaloUtils
                          Trigger/TrigT1/TrigT1Interfaces )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigT1CaloMonitoringToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT1CaloMonitoringTools
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AsgTools GaudiKernel AthenaMonitoringLib TrigT1CaloEventLib TrigT1CaloUtilsLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools EventInfo xAODTrigL1Calo LWHists TrigConfL1Data TrigT1Interfaces )

atlas_add_component( TrigT1CaloMonitoringTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools AthenaMonitoringLib GaudiKernel DataModel SGTools EventInfo xAODTrigL1Calo LWHists TrigConfL1Data TrigT1CaloEventLib TrigT1CaloUtilsLib TrigT1Interfaces TrigT1CaloMonitoringToolsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

