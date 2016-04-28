################################################################################
# Package: LArTBEvent
################################################################################

# Declare the package name:
atlas_subdir( LArTBEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          Control/SGTools )

# Component(s) in the package:
atlas_add_library( LArTBEvent
                   src/*.cxx
                   PUBLIC_HEADERS LArTBEvent
                   LINK_LIBRARIES AthContainers SGTools )

