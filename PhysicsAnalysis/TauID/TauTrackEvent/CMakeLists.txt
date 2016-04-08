################################################################################
# Package: TauTrackEvent
################################################################################

# Declare the package name:
atlas_subdir( TauTrackEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          PhysicsAnalysis/TruthParticleID/McParticleEvent )

# Component(s) in the package:
atlas_add_library( TauTrackEvent
                   src/*.cxx
                   PUBLIC_HEADERS TauTrackEvent
                   LINK_LIBRARIES McParticleEvent )

