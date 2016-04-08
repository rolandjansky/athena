################################################################################
# Package: MuonDQAMonFlags
################################################################################

# Declare the package name:
atlas_subdir( MuonDQAMonFlags )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          TestPolicy )

# Install files from the package:
atlas_install_python_modules( python/*.py )

