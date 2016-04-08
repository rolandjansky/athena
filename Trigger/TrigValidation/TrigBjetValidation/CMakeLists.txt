################################################################################
# Package: TrigBjetValidation
################################################################################

# Declare the package name:
atlas_subdir( TrigBjetValidation )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_joboptions( share/TrigBjetValidation_RTT_topOptions.py )
atlas_install_runtime( test/TrigBjetValidation_TestConfiguration.xml share/TrigBjetValidation_RTT_topOptions.py share/TrigBjetValidation_Histos )

