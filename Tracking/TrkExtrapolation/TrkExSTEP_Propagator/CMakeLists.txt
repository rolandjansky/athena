# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkExSTEP_Propagator )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( TrkExSTEP_Propagator
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaKernel GeoPrimitives EventPrimitives TrkGeometry TrkEventPrimitives
		             TrkMaterialOnTrack TrkParameters TrkExInterfaces TrkExUtils GaudiKernel TrkDetDescrUtils TrkSurfaces TrkTrack
		             MagFieldElements MagFieldConditions CxxUtils)
