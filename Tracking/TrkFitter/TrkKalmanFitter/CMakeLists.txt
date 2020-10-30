# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkKalmanFitter )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( TrkKalmanFitter
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps GeoPrimitives GaudiKernel TrkEventPrimitives TrkEventUtils TrkParameters TrkExInterfaces TrkFitterInterfaces TrkFitterUtils AtlasDetDescr TrkDetDescrInterfaces TrkDetElementBase TrkGeometry TrkSurfaces TrkCompetingRIOsOnTrack TrkMeasurementBase TrkPrepRawData TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkTrack TrkToolInterfaces TrkValInterfaces )
