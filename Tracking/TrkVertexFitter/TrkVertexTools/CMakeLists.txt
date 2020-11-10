# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkVertexTools )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( TrkVertexTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaBaseComps AthenaKernel xAODTracking GaudiKernel TrkVertexFitterInterfaces EventPrimitives 
		             GeneratorObjects TrkEventPrimitives VxVertex BeamSpotConditionsData )
