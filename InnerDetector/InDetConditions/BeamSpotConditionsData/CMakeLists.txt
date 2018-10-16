#################################
# Package: BeamSpotConditionsData
#################################

# package name
atlas_subdir( BeamSpotConditionsData )

# possible extra dependencies
set ( extra_dep )
set ( extra_lib )
if ( NOT SIMULATIONBASE )
   set ( extra_dep VxVertex )
   set ( extra_lib VxVertex )
endif()

# dependencies
atlas_depends_on_subdirs( PUBLIC
			  Control/AthenaKernel
			  AthenaPoolUtilities
			  GeoPrimitives
			  ${extra_dep}
			)

# components
atlas_add_library( BeamSpotConditionsData
		   src/*.cxx
		   PUBLIC_HEADERS BeamSpotConditionsData
		   LINK_LIBRARIES AthenaPoolUtilities GeoPrimitives ${extra_lib}
		 )
