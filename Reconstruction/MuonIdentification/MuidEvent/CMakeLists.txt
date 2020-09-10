################################################################################
# Package: MuidEvent
################################################################################

# Declare the package name:
atlas_subdir( MuidEvent )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( MuidEvent
                   src/CaloLayer.cxx
                   src/CaloMeas.cxx
                   src/IndetMatchCandidate.cxx
                   src/MuidTrack.cxx
                   src/MuonMatch.cxx
                   PUBLIC_HEADERS MuidEvent
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthContainers TrkParameters
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} Particle TrkSurfaces TrkTrack VxVertex )

