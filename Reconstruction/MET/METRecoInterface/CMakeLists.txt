################################################################################
# Package: METRecoInterface
################################################################################

# Declare the package name:
atlas_subdir( METRecoInterface )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Event/xAOD/xAODMissingET )

# Install files from the package:
atlas_install_headers( METRecoInterface )

