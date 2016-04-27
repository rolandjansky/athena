################################################################################
# Package: TriggerJobOpts
################################################################################

# Declare the package name:
atlas_subdir( TriggerJobOpts )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Trigger/TriggerCommon/TrigEDMConfig
                          Trigger/TriggerCommon/TrigTier0 )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

