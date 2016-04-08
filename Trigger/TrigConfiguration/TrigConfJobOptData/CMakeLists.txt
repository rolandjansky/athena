################################################################################
# Package: TrigConfJobOptData
################################################################################

# Declare the package name:
atlas_subdir( TrigConfJobOptData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Trigger/TrigConfiguration/TrigConfL1Data )

# Component(s) in the package:
atlas_add_library( TrigConfJobOptData
                   src/*.cxx
                   PUBLIC_HEADERS TrigConfJobOptData
                   LINK_LIBRARIES TrigConfL1Data )

