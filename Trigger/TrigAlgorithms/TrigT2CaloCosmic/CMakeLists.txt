################################################################################
# Package: TrigT2CaloCosmic
################################################################################

# Declare the package name:
atlas_subdir( TrigT2CaloCosmic )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/IRegionSelector
                          GaudiKernel
                          Trigger/TrigAlgorithms/TrigT2CaloCommon
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          Calorimeter/CaloGeoHelpers
                          Calorimeter/CaloIdentifier
                          Trigger/TrigT1/TrigT1Interfaces )

# Component(s) in the package:
atlas_add_library( TrigT2CaloCosmicLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT2CaloCosmic
                   LINK_LIBRARIES IRegionSelector GaudiKernel TrigCaloEvent TrigSteeringEvent TrigT2CaloCommonLib
                   PRIVATE_LINK_LIBRARIES CaloGeoHelpers CaloIdentifier TrigT1Interfaces )

atlas_add_component( TrigT2CaloCosmic
                     src/components/*.cxx
                     LINK_LIBRARIES IRegionSelector GaudiKernel TrigT2CaloCommonLib TrigCaloEvent TrigSteeringEvent CaloGeoHelpers CaloIdentifier TrigT1Interfaces TrigT2CaloCosmicLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

