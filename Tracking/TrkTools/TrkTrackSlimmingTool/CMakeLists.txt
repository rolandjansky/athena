# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkTrackSlimmingTool )

# Component(s) in the package:
atlas_add_component( TrkTrackSlimmingTool
                     src/*.cxx
                     src/components/*.cxx
		             LINK_LIBRARIES AthenaBaseComps TrkToolInterfaces CxxUtils AtlasDetDescr TrkSurfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkMaterialOnTrack TrkMeasurementBase TrkParameters TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkTrack TrkTrackSummary )
