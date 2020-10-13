# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TileRec )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core Tree )

# Component(s) in the package:
atlas_add_component( TileRec
                     src/Tile*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} AthenaBaseComps AthenaKernel EventContainers GaudiKernel TileConditionsLib TileEvent TileIdentifier CaloDetDescrLib CaloEvent CaloIdentifier Identifier xAODEventInfo StoreGateLib TileCalibBlobObjs TileDetDescr TileL2AlgsLib TileSimEvent TileByteStreamLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
