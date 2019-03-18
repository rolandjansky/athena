################################################################################
# Package: TrigT2CaloTau
################################################################################

# Declare the package name:
atlas_subdir( TrigT2CaloTau )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloInterface
                          GaudiKernel
                          Trigger/TrigAlgorithms/TrigT2CaloCommon
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigT2CaloCalibration
                          PRIVATE
                          Calorimeter/CaloEvent
                          Calorimeter/CaloGeoHelpers
                          Calorimeter/CaloIdentifier
                          Control/AthenaKernel
                          Trigger/TrigT1/TrigT1Interfaces )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigT2CaloTauLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT2CaloTau
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel TrigCaloEvent TrigSteeringEvent TrigT2CaloCommonLib TrigInterfacesLib TrigT2CaloCalibrationLib
                   PRIVATE_LINK_LIBRARIES CaloEvent CaloGeoHelpers CaloIdentifier AthenaKernel TrigT1Interfaces )

atlas_add_component( TrigT2CaloTau
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel TrigT2CaloCommonLib TrigCaloEvent TrigSteeringEvent TrigInterfacesLib TrigT2CaloCalibrationLib CaloEvent CaloGeoHelpers CaloIdentifier AthenaKernel TrigT1Interfaces TrigT2CaloTauLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

