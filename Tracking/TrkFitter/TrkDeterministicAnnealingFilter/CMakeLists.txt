# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkDeterministicAnnealingFilter )

# Component(s) in the package:
atlas_add_component( TrkDeterministicAnnealingFilter
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrkCompetingRIOsOnTrack TrkEventPrimitives TrkEventUtils TrkParameters TrkFitterInterfaces TrkFitterUtils TrkToolInterfaces AtlasDetDescr TrkSurfaces TrkMeasurementBase TrkPrepRawData TrkRIO_OnTrack TrkTrack TrkExInterfaces TrkValInterfaces )

