# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_DetElementsRoadTool_xk )

# Component(s) in the package:
atlas_add_component( TRT_DetElementsRoadTool_xk
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel GeoModelInterfaces InDetRecToolInterfaces MagFieldConditions MagFieldElements StoreGateLib TRT_ReadoutGeometry TrkExInterfaces TrkGeometry TrkParameters TrkSurfaces )
