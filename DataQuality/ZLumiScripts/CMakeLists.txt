################################################################################
# Package: ZLumiScripts
################################################################################

# Declare the package name:
atlas_subdir( ZLumiScripts )

# Declare the package's dependencies:
#atlas_depends_on_subdirs( )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Install files from the package:
atlas_install_scripts( scripts/*.py )
