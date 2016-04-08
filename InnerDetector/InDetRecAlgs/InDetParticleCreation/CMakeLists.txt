################################################################################
# Package: InDetParticleCreation
################################################################################

# Declare the package name:
atlas_subdir( InDetParticleCreation )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          PRIVATE
                          Event/NavFourMom
                          Reconstruction/Particle
                          Tracking/TrkEvent/TrkParticleBase
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackLink
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkTools/TrkToolInterfaces )

# Component(s) in the package:
atlas_add_component( InDetParticleCreation
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel NavFourMom Particle TrkParticleBase TrkTrack TrkTrackSummary VxVertex TrkToolInterfaces )

# Install files from the package:
atlas_install_headers( InDetParticleCreation )

