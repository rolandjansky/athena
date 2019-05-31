################################################################################
# Package: TrigTileMuId
################################################################################

# Declare the package name:
atlas_subdir( TrigTileMuId )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
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
                          Calorimeter/CaloIdentifier
                          Control/AthenaKernel
                          Control/AthenaMonitoring
                          Control/CxxUtils
                          DetectorDescription/RegionSelector
                          Event/ByteStreamCnvSvcBase
                          Event/ByteStreamData
                          Generators/GeneratorObjects
                          Trigger/TrigEvent/TrigSteeringEvent
                          Trigger/TrigT1/TrigT1Interfaces
                          Trigger/TrigTools/TrigInDetToolInterfaces )

# Component(s) in the package:
atlas_add_component( TrigTileMuId
                     src/*.cxx src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MagFieldInterfaces TileEvent TileByteStreamLib TrigT2CaloCommonLib TrigInDetEvent TrigMuonEvent TrigInterfacesLib TrigTimeAlgsLib CaloIdentifier AthenaKernel AthenaMonitoringLib RegionSelectorLib ByteStreamCnvSvcBaseLib ByteStreamData GeneratorObjects TrigSteeringEvent TrigT1Interfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py )
