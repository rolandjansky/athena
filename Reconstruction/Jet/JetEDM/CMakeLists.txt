################################################################################
# Package: JetEDM
################################################################################

# Declare the package name:
atlas_subdir( JetEDM )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODTracking
                          PRIVATE
                          Event/xAOD/xAODMuon )

# External dependencies:
find_package( FastJet )

# Component(s) in the package:
atlas_add_library( JetEDM
                   Root/*.cxx
                   PUBLIC_HEADERS JetEDM
                   INCLUDE_DIRS ${FASTJET_INCLUDE_DIRS}
                   LINK_LIBRARIES AsgTools xAODBase xAODJet xAODTracking
                   PRIVATE_LINK_LIBRARIES ${FASTJET_LIBRARIES} xAODMuon )

