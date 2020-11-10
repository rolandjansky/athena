# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TileRawUtils )

# External dependencies:
find_package( tdaq-common COMPONENTS ers )

# Component(s) in the package:
atlas_add_component( TileRawUtils
                     src/Tile*.cxx
                     src/Reg*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${TDAQ-COMMON_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TDAQ-COMMON_LIBRARIES} CaloIdentifier AthenaBaseComps Identifier GaudiKernel TileEvent TileIdentifier CaloDetDescrLib StoreGateLib IRegionSelector RegionSelectorLib TileCalibBlobObjs TileConditionsLib TileDetDescr TileByteStreamLib PixelConditionsData )
