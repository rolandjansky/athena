################################################################################
# Package: TrigT2CaloTileMon
################################################################################

# Declare the package name:
atlas_subdir( TrigT2CaloTileMon )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Trigger/TrigAlgorithms/TrigT2CaloCommon
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          Calorimeter/CaloGeoHelpers
                          Trigger/TrigT1/TrigT1Interfaces )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigT2CaloTileMonLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT2CaloTileMon
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel TrigCaloEvent TrigSteeringEvent TrigT2CaloCommonLib
                   PRIVATE_LINK_LIBRARIES CaloGeoHelpers TrigT1Interfaces )

atlas_add_component( TrigT2CaloTileMon
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel TrigT2CaloCommonLib TrigCaloEvent TrigSteeringEvent CaloGeoHelpers TrigT1Interfaces TrigT2CaloTileMonLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

