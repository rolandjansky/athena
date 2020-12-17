# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SiClusterOnTrackTool )

# Component(s) in the package:
atlas_add_component( SiClusterOnTrackTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaPoolUtilities EventPrimitives GaudiKernel GeoPrimitives Identifier InDetCondTools InDetIdentifier InDetPrepRawData InDetRIO_OnTrack InDetReadoutGeometry PixelConditionsData PixelGeoModelLib PixelReadoutGeometry SCT_ModuleDistortionsLib SiClusterizationToolLib StoreGateLib TrkEventUtils TrkParameters TrkRIO_OnTrack TrkSurfaces TrkToolInterfaces )
