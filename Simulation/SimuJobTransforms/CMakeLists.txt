################################################################################
# Package: SimuJobTransforms
################################################################################

# Declare the package name:
atlas_subdir( SimuJobTransforms )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Tools/PyJobTransforms )

# External dependencies:
find_package( PythonLibs )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( scripts/*.py )
atlas_install_runtime( scripts/*.sh )
atlas_install_runtime( test/SimuJobTransforms_TestConfiguration.xml )

