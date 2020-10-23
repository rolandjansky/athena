# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( BeamSpotConditionsData )

# possible extra dependencies
set ( extra_lib )
if ( NOT SIMULATIONBASE )
   set ( extra_lib VxVertex )
endif()

# Component(s) in the package:
atlas_add_library( BeamSpotConditionsData
		           src/*.cxx
		           PUBLIC_HEADERS BeamSpotConditionsData
		           LINK_LIBRARIES AthenaKernel GeoPrimitives ${extra_lib} )
