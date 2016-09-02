################################################################################
# Package: PanTauUtils
################################################################################

# Declare the package name:
atlas_subdir( PanTauUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Reconstruction/PanTau/PanTauEvent
                          PRIVATE
                          GaudiKernel
                          PhysicsAnalysis/TauID/TauTrackEvent
                          Reconstruction/tauEvent )

# Component(s) in the package:
atlas_add_library( PanTauUtils
                   src/*.cxx
                   PUBLIC_HEADERS PanTauUtils
                   LINK_LIBRARIES PanTauEvent
                   PRIVATE_LINK_LIBRARIES GaudiKernel TauTrackEvent tauEvent )

