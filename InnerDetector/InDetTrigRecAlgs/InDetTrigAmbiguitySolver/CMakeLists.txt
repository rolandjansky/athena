################################################################################
# Package: InDetTrigAmbiguitySolver
################################################################################

# Declare the package name:
atlas_subdir( InDetTrigAmbiguitySolver )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Tracking/TrkEvent/TrkTrack
                          Trigger/TrigSteer/TrigInterfaces
                          PRIVATE
                          Control/AthenaKernel
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkTools/TrkToolInterfaces
                          Trigger/TrigEvent/TrigParticle )

# Component(s) in the package:
atlas_add_component( InDetTrigAmbiguitySolver
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel TrkTrack TrigInterfacesLib AthenaKernel TrkEventPrimitives TrkParameters TrkToolInterfaces TrigParticle )

# Install files from the package:
atlas_install_headers( InDetTrigAmbiguitySolver )
atlas_install_python_modules( python/*.py )

