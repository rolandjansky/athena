################################################################################
# Package: OnlineRecoTests
################################################################################

# Declare the package name:
atlas_subdir( OnlineRecoTests )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( scripts/ort_*.py scripts/ort_*.sh )
