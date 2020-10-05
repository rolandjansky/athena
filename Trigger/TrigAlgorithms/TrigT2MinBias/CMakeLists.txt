# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT2MinBias )

# External dependencies:
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_component( TrigT2MinBias
                     src/*.cxx src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} AthViews AthenaBaseComps AthenaMonitoringKernelLib CaloEvent CaloIdentifier DecisionHandlingLib GaudiKernel IRegionSelector InDetIdentifier InDetPrepRawData InDetRawData StoreGateLib TileByteStreamLib TileConditionsLib TileEvent TileIdentifier TrigCaloEvent TrigCompositeUtilsLib TrigInDetEvent TrigInDetToolInterfacesLib TrigInterfacesLib TrigSteeringEvent TrigT2CaloCommonLib TrigTimeAlgsLib TrkSpacePoint ZdcConditions ZdcEvent ZdcIdentifier xAODEventInfo xAODTrigMinBias xAODTrigger )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} --extend-extensions=ATL900,ATL901 )
