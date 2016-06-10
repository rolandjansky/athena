################################################################################
# Package: MuonCalibJobTransforms
################################################################################

# Declare the package name:
atlas_subdir( MuonCalibJobTransforms )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Tools/PyJobTransformsCore )

# External dependencies:
find_package( PythonLibs )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_runtime( share/RT_default_comm.dat )

