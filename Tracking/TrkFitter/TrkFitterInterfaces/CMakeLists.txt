################################################################################
# Package: TrkFitterInterfaces
################################################################################

# Declare the package name:
atlas_subdir( TrkFitterInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/DataModel
                          Event/EventPrimitives
                          GaudiKernel
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkFitter/TrkFitterUtils )

# Component(s) in the package:
atlas_add_library( TrkFitterInterfaces
                   PUBLIC_HEADERS TrkFitterInterfaces
                   LINK_LIBRARIES DataModel EventPrimitives GaudiKernel TrkEventPrimitives TrkParameters TrkFitterUtils )

