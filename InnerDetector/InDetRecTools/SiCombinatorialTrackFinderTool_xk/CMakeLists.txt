# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( SiCombinatorialTrackFinderTool_xk )

# Component(s) in the package:
atlas_add_component( SiCombinatorialTrackFinderTool_xk
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetPrepRawData InDetReadoutGeometry InDetRecToolInterfaces MagFieldConditions SiSPSeededTrackFinderData StoreGateLib TrkExInterfaces TrkGeometry TrkMeasurementBase TrkToolInterfaces TrkTrack )
