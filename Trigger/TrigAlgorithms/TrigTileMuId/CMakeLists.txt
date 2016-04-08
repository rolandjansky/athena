################################################################################
# Package: TrigTileMuId
################################################################################

# Declare the package name:
atlas_subdir( TrigTileMuId )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          MagneticField/MagFieldInterfaces
                          TileCalorimeter/TileEvent
                          TileCalorimeter/TileSvc/TileByteStream
                          Trigger/TrigAlgorithms/TrigT2CaloCommon
                          Trigger/TrigEvent/TrigInDetEvent
                          Trigger/TrigEvent/TrigMuonEvent
                          Trigger/TrigSteer/TrigInterfaces
                          Trigger/TrigTools/TrigTimeAlgs
                          PRIVATE
                          Calorimeter/CaloIdentifier
                          Control/AthenaKernel
                          Control/AthenaMonitoring
                          DetectorDescription/RegionSelector
                          Event/ByteStreamCnvSvcBase
                          Event/ByteStreamData
                          Generators/GeneratorObjects
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigTools/TrigInDetToolInterfaces )

# Component(s) in the package:
atlas_add_library( TrigTileMuIdLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigTileMuId
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel MagFieldInterfaces TileEvent TrigInDetEvent TrigMuonEvent TileByteStreamLib TrigT2CaloCommonLib TrigInterfacesLib TrigTimeAlgsLib AthenaMonitoringLib RegionSelectorLib ByteStreamCnvSvcBaseLib ByteStreamData_test
                   PRIVATE_LINK_LIBRARIES CaloIdentifier AthenaKernel ByteStreamData GeneratorObjects TrigSteeringEvent TrigT1Interfaces )

atlas_add_component( TrigTileMuId
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MagFieldInterfaces TileEvent TileByteStreamLib TrigT2CaloCommonLib TrigInDetEvent TrigMuonEvent TrigInterfacesLib TrigTimeAlgsLib CaloIdentifier AthenaKernel AthenaMonitoringLib RegionSelectorLib ByteStreamCnvSvcBaseLib ByteStreamData ByteStreamData_test GeneratorObjects TrigSteeringEvent TrigT1Interfaces TrigTileMuIdLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

