################################################################################
# Package: RunDependentSimComps
################################################################################

# Declare the package name:
atlas_subdir( RunDependentSimComps )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( share/RunDepTaskMaker.py share/PrintFirstJobForRun.py )

