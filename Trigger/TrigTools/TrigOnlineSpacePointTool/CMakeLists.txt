# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigOnlineSpacePointTool )

# Component(s) in the package:
atlas_add_component( TrigOnlineSpacePointTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AtlasDetDescr BeamSpotConditionsData GaudiKernel IRegionSelector InDetIdentifier InDetReadoutGeometry PixelReadoutGeometry SCT_ReadoutGeometry StoreGateLib TrigInDetEvent TrigInDetToolInterfacesLib TrkSpacePoint )
