################################################################################
# Package: RunDependentSimComps
################################################################################

# Declare the package name:
atlas_subdir( RunDependentSimComps )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( share/RunDepTaskMaker.py share/PrintFirstJobForRun.py )

