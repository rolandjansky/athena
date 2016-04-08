################################################################################
# Package: TrkMeasurementUpdator_xk
################################################################################

# Declare the package name:
atlas_subdir( TrkMeasurementUpdator_xk )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          GaudiKernel
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkPatternParameters )

# Component(s) in the package:
atlas_add_component( TrkMeasurementUpdator_xk
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps TrkToolInterfaces GaudiKernel TrkEventPrimitives TrkParameters TrkPatternParameters )

# Install files from the package:
atlas_install_headers( TrkMeasurementUpdator_xk )

