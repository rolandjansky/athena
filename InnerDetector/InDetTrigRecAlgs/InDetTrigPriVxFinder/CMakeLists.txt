################################################################################
# Package: InDetTrigPriVxFinder
################################################################################

# Declare the package name:
atlas_subdir( InDetTrigPriVxFinder )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Trigger/TrigSteer/TrigInterfaces
                          PRIVATE
                          Control/StoreGate
                          Event/EventPrimitives
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          MagneticField/MagFieldInterfaces
                          InnerDetector/InDetConditions/InDetBeamSpotService
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackLink
                          Tracking/TrkEvent/VxVertex
                          Event/xAOD/xAODTracking
                          Trigger/TrigEvent/TrigParticle )

# Component(s) in the package:
atlas_add_component( InDetTrigPriVxFinder
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel TrigInterfacesLib StoreGateLib SGtests EventPrimitives InDetRecToolInterfaces MagFieldInterfaces TrkEventPrimitives TrkParameters TrkParticleBase TrkTrack VxVertex TrigParticle xAODTracking )

# Install files from the package:
atlas_install_headers( InDetTrigPriVxFinder )
atlas_install_python_modules( python/*.py )

