################################################################################
# Package: UserAnalysisEvent
################################################################################

# Declare the package name:
atlas_subdir( UserAnalysisEvent )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Install files from the package:
atlas_install_headers( UserAnalysisEvent )

