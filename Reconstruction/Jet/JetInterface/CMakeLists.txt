################################################################################
# Package: JetInterface
################################################################################

# Declare the package name:
atlas_subdir( JetInterface )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODBase )

# Component(s) in the package:
atlas_add_library( JetInterface
                   Root/*.cxx
                   PUBLIC_HEADERS JetInterface
                   LINK_LIBRARIES AsgTools xAODEventInfo xAODJet xAODTracking xAODBase)

