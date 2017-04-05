################################################################################
# Package: RunTimeTester
################################################################################

# Declare the package name:
atlas_subdir( RunTimeTester )

# Install files from the package:
atlas_install_runtime( share/RunTimeTester_TestConfiguration.xml share/RttLibraryTools.py )

atlas_install_python_modules( src/*.py )
