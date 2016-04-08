################################################################################
# Package: TrigTier0
################################################################################

# Declare the package name:
atlas_subdir( TrigTier0 )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

