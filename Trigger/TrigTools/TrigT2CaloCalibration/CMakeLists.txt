################################################################################
# Package: TrigT2CaloCalibration
################################################################################

# Declare the package name:
atlas_subdir( TrigT2CaloCalibration )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloClusterCorrection
                          Calorimeter/CaloConditions
                          Calorimeter/CaloGeoHelpers
                          Calorimeter/CaloRec
                          Control/AthenaBaseComps
                          Control/CLIDSvc
                          Control/SGTools
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODTrigCalo
                          GaudiKernel
                          Trigger/TrigEvent/TrigCaloEvent
                          PRIVATE
                          Calorimeter/CaloDetDescr
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/RegistrationServices
                          Event/EventInfo )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigT2CaloCalibrationLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT2CaloCalibration
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES CaloConditions CaloGeoHelpers AthenaBaseComps SGTools AthenaPoolUtilities xAODTrigCalo GaudiKernel TrigCaloEvent CaloClusterCorrectionLib CaloRecLib StoreGateLib SGtests CaloDetDescrLib AthenaPoolCnvSvcLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaKernel EventInfo )

atlas_add_component( TrigT2CaloCalibration
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} CaloClusterCorrectionLib CaloConditions CaloGeoHelpers CaloRecLib AthenaBaseComps SGTools StoreGateLib SGtests AthenaPoolUtilities xAODTrigCalo GaudiKernel TrigCaloEvent CaloDetDescrLib AthenaKernel AthenaPoolCnvSvcLib EventInfo TrigT2CaloCalibrationLib )

atlas_add_poolcnv_library( TrigT2CaloCalibrationPoolCnv
                           src/*.cxx
                           FILES TrigT2CaloCalibration/T2CaloJetCalib_dBObj.h
                           INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                           LINK_LIBRARIES ${ROOT_LIBRARIES} CaloClusterCorrectionLib CaloConditions CaloGeoHelpers CaloRecLib AthenaBaseComps SGTools StoreGateLib SGtests AthenaPoolUtilities xAODTrigCalo GaudiKernel TrigCaloEvent CaloDetDescrLib AthenaKernel AthenaPoolCnvSvcLib EventInfo TrigT2CaloCalibrationLib )

atlas_add_dictionary( TrigT2CaloCalibrationDict
                      TrigT2CaloCalibration/TrigT2CaloCalibrationDict.h
                      TrigT2CaloCalibration/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} CaloClusterCorrectionLib CaloConditions CaloGeoHelpers CaloRecLib AthenaBaseComps SGTools StoreGateLib SGtests AthenaPoolUtilities xAODTrigCalo GaudiKernel TrigCaloEvent CaloDetDescrLib AthenaKernel AthenaPoolCnvSvcLib EventInfo TrigT2CaloCalibrationLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

