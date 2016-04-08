################################################################################
# Package: TrkMultiComponentStateOnSurface
################################################################################

# Declare the package name:
atlas_subdir( TrkMultiComponentStateOnSurface )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          PRIVATE
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkMeasurementBase )

# Component(s) in the package:
atlas_add_library( TrkMultiComponentStateOnSurface
                   src/*.cxx
                   PUBLIC_HEADERS TrkMultiComponentStateOnSurface
                   LINK_LIBRARIES TrkEventPrimitives TrkParameters TrkTrack
                   PRIVATE_LINK_LIBRARIES GaudiKernel TrkSurfaces TrkMaterialOnTrack TrkMeasurementBase )

