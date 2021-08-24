# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SiDetElementsRoadTool_xk )

# Component(s) in the package:
atlas_add_component( SiDetElementsRoadTool_xk
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel AthenaPoolUtilities GaudiKernel InDetReadoutGeometry InDetRecToolInterfaces MagFieldConditions MagFieldElements PixelReadoutGeometryLib SCT_ReadoutGeometry SiSPSeededTrackFinderData StoreGateLib TrkExInterfaces TrkGeometry TrkParameters TrkPrepRawData TrkSurfaces )
