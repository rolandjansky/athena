################################################################################
# Package: RecoToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( RecoToolInterfaces )

# Extra dependencies based on the build environment:
set( extra_lib )
if( NOT XAOD_STANDALONE )
  set( extra_lib GaudiKernel ParticleCaloExtension TrkCaloExtension TrkTrack)
endif()


# Component(s) in the package:
atlas_add_library( RecoToolInterfaces
                   PUBLIC_HEADERS RecoToolInterfaces
                   LINK_LIBRARIES AsgTools xAODBase xAODCaloEvent 
		   xAODPrimitives xAODTracking ${extra_lib})

