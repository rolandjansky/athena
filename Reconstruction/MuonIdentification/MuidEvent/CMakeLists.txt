################################################################################
# Package: MuidEvent
################################################################################

# Declare the package name:
atlas_subdir( MuidEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          Tracking/TrkEvent/TrkParameters
                          PRIVATE
                          Reconstruction/Particle
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/VxVertex )

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
                   LINK_LIBRARIES DataModel SGTools TrkParameters
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} Particle TrkSurfaces TrkTrack VxVertex )

