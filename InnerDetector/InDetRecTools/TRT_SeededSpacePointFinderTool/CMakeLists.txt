# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_SeededSpacePointFinderTool )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( TRT_SeededSpacePointFinderTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel IRegionSelector InDetIdentifier InDetPrepRawData InDetRecToolInterfaces MagFieldConditions MagFieldElements RoiDescriptor SiSpacePointsSeed StoreGateLib TrkEventUtils TrkGeometry TrkSpacePoint TrkSurfaces )
