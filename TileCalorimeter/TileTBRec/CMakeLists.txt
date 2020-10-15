# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TileTBRec )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )
find_package( tdaq-common )

# Component(s) in the package:
atlas_add_component( TileTBRec
                     src/Tile*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} AthenaBaseComps AthContainers GaudiKernel TileConditionsLib TileIdentifier TileByteStreamLib CaloEvent CaloIdentifier AthenaKernel StoreGateLib ByteStreamCnvSvcBaseLib xAODEventInfo TileCalibBlobObjs TileEvent TileSimEvent TileRecUtilsLib PathResolver )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/TileEtaCTB.txt )

