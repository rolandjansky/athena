################################################################################
# Package: OjfJet
################################################################################

# Declare the package name:
atlas_subdir( OjfJet )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( OjfJet
                   src/*.cxx
                   PUBLIC_HEADERS OjfJet
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaKernel )

