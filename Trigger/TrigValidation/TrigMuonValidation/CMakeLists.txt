################################################################################
# Package: TrigMuonValidation
################################################################################

# Declare the package name:
atlas_subdir( TrigMuonValidation )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          TestPolicy )

# Install files from the package:
atlas_install_joboptions( share/TrigMuonValidation_RTT_*.py share/TrigMuonValidation_CreateEfficiencies.py )

