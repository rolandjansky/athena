################################################################################
# Package: TrigT2CaloEgamma
################################################################################

# Declare the package name:
atlas_subdir( TrigT2CaloEgamma )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Calorimeter/CaloGeoHelpers
                          Control/AthenaBaseComps
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
atlas_add_component( TrigT2CaloEgamma
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES CaloEvent CaloGeoHelpers xAODTrigCalo xAODTrigRinger GaudiKernel TrigT2CaloCommonLib TrigSteeringEvent CaloDetDescrLib CaloIdentifier AthLinks IRegionSelector TrigT1Interfaces TrigT2CaloCalibrationLib TrigTimeAlgsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

# Python code checking:
atlas_add_test( flake8
   SCRIPT flake8 --select=ATL,F,E7,E9,W6 ${CMAKE_CURRENT_SOURCE_DIR}/python
   POST_EXEC_SCRIPT nopost.sh )
