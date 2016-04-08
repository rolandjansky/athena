################################################################################
# Package: InDetTrigExtensProcessor
################################################################################

# Declare the package name:
atlas_subdir( InDetTrigExtensProcessor )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkFitter/TrkFitterUtils
                          Trigger/TrigSteer/TrigInterfaces
                          PRIVATE
                          Control/StoreGate
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkFitter/TrkFitterInterfaces
                          Tracking/TrkTools/TrkToolInterfaces
                          Trigger/TrigEvent/TrigParticle )

# Component(s) in the package:
atlas_add_component( InDetTrigExtensProcessor
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel TrkEventPrimitives TrkTrack TrkFitterUtils TrigInterfacesLib StoreGateLib SGtests TrkMeasurementBase TrkParameters TrkPrepRawData TrkRIO_OnTrack TrkFitterInterfaces TrkToolInterfaces TrigParticle )

# Install files from the package:
atlas_install_headers( InDetTrigExtensProcessor )
atlas_install_python_modules( python/*.py )

