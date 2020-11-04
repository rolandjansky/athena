# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_TrackSegmentsTool_xk )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore Graf )

# Component(s) in the package:
atlas_add_component( TRT_TrackSegmentsTool_xk
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps AthenaKernel AthenaPoolUtilities CxxUtils GaudiKernel GeoModelInterfaces InDetIdentifier InDetPrepRawData InDetRIO_OnTrack InDetRecToolInterfaces StoreGateLib TRT_ReadoutGeometry TrkEventPrimitives TrkEventUtils TrkExInterfaces TrkGeometry TrkParameters TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkSurfaces TrkToolInterfaces )
