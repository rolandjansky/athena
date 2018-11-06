################################################################################
# Package: RecBackgroundEvent
################################################################################

# Declare the package name:
atlas_subdir( RecBackgroundEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/AthLinks
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODJet
                          Tracking/TrkEvent/TrkSegment )

# Component(s) in the package:
atlas_add_library( RecBackgroundEvent
                   src/*.cxx
                   PUBLIC_HEADERS RecBackgroundEvent
                   LINK_LIBRARIES AthLinks AthenaKernel xAODCaloEvent xAODJet TrkSegment )

