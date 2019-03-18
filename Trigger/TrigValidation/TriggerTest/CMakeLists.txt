################################################################################
# Package: TriggerTest
################################################################################

# Declare the package name:
atlas_subdir( TriggerTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          TestPolicy )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.cxx Testing/*.conf )
atlas_install_scripts( test/exec*.sh test/test*.sh )

