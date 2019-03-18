################################################################################
# Package: TrigT2CaloJet
################################################################################

# Declare the package name:
atlas_subdir( TrigT2CaloJet )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloInterface
                          Calorimeter/CaloRec
                          Control/AthContainers
                          DetectorDescription/IRegionSelector
                          GaudiKernel
                          Trigger/TrigAlgorithms/TrigT2CaloCommon
                          Trigger/TrigEvent/TrigCaloEvent
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigT2CaloCalibration
                          Trigger/TrigTools/TrigTimeAlgs
                          PRIVATE
                          Calorimeter/CaloGeoHelpers
                          Control/AthenaKernel
                          Trigger/TrigT1/TrigT1CaloByteStream
                          Trigger/TrigT1/TrigT1CaloEvent
                          Trigger/TrigT1/TrigT1CaloToolInterfaces
                          Trigger/TrigT1/TrigT1Interfaces )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( FastJet )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrigT2CaloJetLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigT2CaloJet
                   INCLUDE_DIRS ${FASTJET_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${FASTJET_LIBRARIES} AthContainers IRegionSelector GaudiKernel TrigCaloEvent TrigParticle TrigSteeringEvent CaloRecLib TrigT2CaloCommonLib TrigInterfacesLib TrigT2CaloCalibrationLib TrigTimeAlgsLib TrigT1CaloEventLib
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} CaloGeoHelpers AthenaKernel TrigT1CaloToolInterfaces TrigT1Interfaces )

atlas_add_component( TrigT2CaloJet
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${FASTJET_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${FASTJET_LIBRARIES} ${ROOT_LIBRARIES} CaloRecLib AthContainers IRegionSelector GaudiKernel TrigT2CaloCommonLib TrigCaloEvent TrigParticle TrigSteeringEvent TrigInterfacesLib TrigT2CaloCalibrationLib TrigTimeAlgsLib CaloGeoHelpers AthenaKernel TrigT1CaloEventLib TrigT1CaloToolInterfaces TrigT1Interfaces TrigT2CaloJetLib )

atlas_add_dictionary( T2L1ToolsDict
                      TrigT2CaloJet/T2L1ToolsDict.h
                      TrigT2CaloJet/selection.xml
                      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${FASTJET_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${Boost_LIBRARIES} ${FASTJET_LIBRARIES} ${ROOT_LIBRARIES} CaloRecLib AthContainers IRegionSelector GaudiKernel TrigT2CaloCommonLib TrigCaloEvent TrigParticle TrigSteeringEvent TrigInterfacesLib TrigT2CaloCalibrationLib TrigTimeAlgsLib CaloGeoHelpers AthenaKernel TrigT1CaloEventLib TrigT1CaloToolInterfaces TrigT1Interfaces TrigT2CaloJetLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )

