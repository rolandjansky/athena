################################################################################
# Package: TrkValEvent
################################################################################

# Declare the package name:
atlas_subdir( TrkValEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack )

# External dependencies:
find_package( CLHEP )
find_package( HepMC )

# Component(s) in the package:
atlas_add_library( TrkValEvent
                   src/*.cxx
                   PUBLIC_HEADERS TrkValEvent
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${HEPMC_LIBRARIES} TrkEventPrimitives TrkParameters TrkTrack )

