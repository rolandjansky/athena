# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TRT_DriftCircleOnTrackTool )

# Component(s) in the package:
atlas_add_component( TRT_DriftCircleOnTrackTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetRIO_OnTrack StoreGateLib TrkRIO_OnTrack TrkToolInterfaces TRT_ReadoutGeometry TrkEventPrimitives TrkRIO_OnTrack LumiBlockData TRT_DriftFunctionToolLib )
