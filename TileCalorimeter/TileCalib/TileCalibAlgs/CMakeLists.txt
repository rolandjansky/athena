# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TileCalibAlgs )

# External dependencies:
find_package( CLHEP )
find_package( CORAL COMPONENTS CoralBase )
find_package( ROOT COMPONENTS Core Tree MathCore Matrix Hist RIO )

# Component(s) in the package:
atlas_add_library( TileCalibAlgsLib
                   TileCalibAlgs/*.h
                   INTERFACE
                   PUBLIC_HEADERS TileCalibAlgs
                   LINK_LIBRARIES GaudiKernel )

atlas_add_component( TileCalibAlgs
                     src/Tile*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CORAL_LIBRARIES} ${CLHEP_LIBRARIES} CaloDetDescrLib CaloIdentifier AthenaBaseComps Identifier GaudiKernel TileConditionsLib TileIdentifier TrigT1CaloCalibToolInterfaces CaloEvent AthenaKernel StoreGateLib AthenaPoolUtilities ByteStreamCnvSvcBaseLib xAODEventInfo xAODTrigL1Calo TileCalibBlobObjs TileEvent TileRecUtilsLib TileByteStreamLib TileMonitoringLib RegistrationServicesLib CaloConditions TileCalibAlgsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

atlas_add_test( TileDigiNoiseCalibAlgConfig_test
                SCRIPT python -m TileCalibAlgs.TileDigiNoiseCalibAlgConfig
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh)

atlas_add_test( TileRawChNoiseCalibAlgConfig_test
                SCRIPT python -m TileCalibAlgs.TileRawChNoiseCalibAlgConfig
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh)
