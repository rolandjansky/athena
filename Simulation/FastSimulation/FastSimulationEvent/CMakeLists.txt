################################################################################
# Package: FastSimulationEvent
################################################################################

# Declare the package name:
atlas_subdir( FastSimulationEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          DetectorDescription/GeoPrimitives
                          Tracking/TrkEvent/TrkMaterialOnTrack )

atlas_add_library( FastSimulationEventLib
                   src/*.cxx
                   PUBLIC_HEADERS FastSimulationEvent
                   LINK_LIBRARIES AthenaKernel GeoPrimitives TrkMaterialOnTrack )

