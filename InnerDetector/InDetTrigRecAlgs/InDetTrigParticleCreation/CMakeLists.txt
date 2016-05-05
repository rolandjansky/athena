################################################################################
# Package: InDetTrigParticleCreation
################################################################################

# Declare the package name:
atlas_subdir( InDetTrigParticleCreation )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Tracking/TrkEvent/TrkTrack
                          Trigger/TrigSteer/TrigInterfaces
                          PRIVATE
                          Control/AthenaKernel
                          DetectorDescription/AtlasDetDescr
                          DetectorDescription/IRegionSelector
                          DetectorDescription/IdDictDetDescr
                          DetectorDescription/Identifier
                          Event/xAOD/xAODTracking
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          Reconstruction/Particle
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrackLink
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkTools/TrkToolInterfaces
                          Trigger/TrigEvent/TrigParticle
                          Trigger/TrigEvent/TrigSteeringEvent )

# Component(s) in the package:
atlas_add_component( InDetTrigParticleCreation
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES GaudiKernel TrkTrack TrigInterfacesLib AthenaKernel AtlasDetDescr IRegionSelector IdDictDetDescr Identifier xAODTracking InDetIdentifier Particle TrkParameters TrkParticleBase TrkTrackSummary VxVertex TrkToolInterfaces TrigParticle TrigSteeringEvent )

# Install files from the package:
atlas_install_headers( InDetTrigParticleCreation )
atlas_install_python_modules( python/*.py )

