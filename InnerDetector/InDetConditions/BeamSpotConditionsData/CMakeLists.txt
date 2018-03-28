#################################
# Package: BeamSpotConditionsData
#################################

# package name
atlas_subdir( BeamSpotConditionsData )

# dependencies
atlas_depends_on_subdirs( PUBLIC
			  Control/AthenaKernel
			  AthenaPoolUtilities
			  GeoPrimitives
			)

# components
atlas_add_library( BeamSpotConditionsData
		   src/*.cxx
		   PUBLIC_HEADERS BeamSpotConditionsData
		   LINK_LIBRARIES AthenaPoolUtilities GeoPrimitives 
		 )
