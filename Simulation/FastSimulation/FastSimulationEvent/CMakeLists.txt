################################################################################
# Package: FastSimulationEvent
################################################################################

# Declare the package name:
atlas_subdir( FastSimulationEvent )

atlas_add_library( FastSimulationEventLib
                   src/*.cxx
                   PUBLIC_HEADERS FastSimulationEvent
                   LINK_LIBRARIES AthenaKernel GeoPrimitives TrkMaterialOnTrack )

