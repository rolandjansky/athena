################################################################################
# Package: LArConditionsCommon
################################################################################

# Declare the package name:
atlas_subdir( LArConditionsCommon )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( share/LArCellConditions.py share/DumpLArRunFormat.py python/getCurrentFolderTag.py )

