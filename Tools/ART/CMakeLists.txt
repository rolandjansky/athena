################################################################################
# Package: ART
################################################################################

# Declare the package name:
atlas_subdir( ART )

# Declare the package's dependencies: 
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_python_modules( python/ART/*.py )
atlas_install_scripts( scripts/*.py scripts/*.sh )


