################################################################################
# Package: InDetCosmicsEventPhase
################################################################################

# Declare the package name:
atlas_subdir( InDetCosmicsEventPhase )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          InnerDetector/InDetConditions/TRT_ConditionsServices
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          Reconstruction/Particle
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/TrkSpaceTimePoint
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkFitter/TrkFitterInterfaces
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          Commission/CommissionEvent
                          Event/xAOD/xAODEventInfo
                          InnerDetector/InDetConditions/TRT_ConditionsData
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          Tracking/TrkEvent/TrkTrackSummary )

# Component(s) in the package:
atlas_add_component( InDetCosmicsEventPhase
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TRT_ConditionsServicesLib InDetRIO_OnTrack Particle TrkParticleBase TrkSegment TrkSpaceTimePoint TrkTrack TrkFitterInterfaces TrkToolInterfaces CommissionEvent xAODEventInfo TRT_ConditionsData InDetIdentifier TrkTrackSummary )

# Install files from the package:
atlas_install_headers( InDetCosmicsEventPhase )

