# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkFitterUtils )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( TrkFitterUtils
                   src/*.cxx
                   PUBLIC_HEADERS TrkFitterUtils
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
		           LINK_LIBRARIES ${CLHEP_LIBRARIES} AthContainers GeoPrimitives Identifier GaudiKernel TrkSurfaces TrkEventPrimitives TrkEventUtils TrkMeasurementBase TrkParameters TrkExUtils CxxUtils
		           PRIVATE_LINK_LIBRARIES AtlasDetDescr TrkMaterialOnTrack TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkSegment TrkTrack TrkVertexOnTrack TrkExInterfaces TrkToolInterfaces )
