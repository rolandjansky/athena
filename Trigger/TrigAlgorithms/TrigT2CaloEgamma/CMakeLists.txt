################################################################################
# Package: TrigT2CaloEgamma
################################################################################

# Declare the package name:
atlas_subdir( TrigT2CaloEgamma )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Calorimeter/CaloGeoHelpers
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          Event/xAOD/xAODTrigCalo
                          Event/xAOD/xAODTrigRinger
                          GaudiKernel
                          Reconstruction/egamma/egammaInterfaces
                          Trigger/TrigAlgorithms/TrigT2CaloCommon
                          Trigger/TrigEvent/TrigSteeringEvent
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Calorimeter/CaloIdentifier
                          Control/AthLinks
                          DetectorDescription/IRegionSelector
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigTools/TrigT2CaloCalibration
                          Trigger/TrigTools/TrigTimeAlgs )

# Component(s) in the package:
atlas_add_library( TrigT2CaloEgammaLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT2CaloEgamma
                   LINK_LIBRARIES CaloEvent CaloGeoHelpers DataModel SGTools xAODTrigCalo xAODTrigRinger GaudiKernel TrigSteeringEvent TrigT2CaloCommonLib CaloDetDescrLib TrigT2CaloCalibrationLib TrigTimeAlgsLib
                   PRIVATE_LINK_LIBRARIES CaloIdentifier AthLinks IRegionSelector TrigT1Interfaces )

atlas_add_component( TrigT2CaloEgamma
                     src/components/*.cxx
                     LINK_LIBRARIES CaloEvent CaloGeoHelpers DataModel SGTools xAODTrigCalo xAODTrigRinger GaudiKernel TrigT2CaloCommonLib TrigSteeringEvent CaloDetDescrLib CaloIdentifier AthLinks IRegionSelector TrigT1Interfaces TrigT2CaloCalibrationLib TrigTimeAlgsLib TrigT2CaloEgammaLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

