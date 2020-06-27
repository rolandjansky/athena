################################################################################
# Package: RecExCommon
################################################################################

# Declare the package name:
atlas_subdir( RecExCommon )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( data/*.ascii.gz )
atlas_install_scripts( share/RecExCommon_links.sh share/recexcommon-links.py share/qtest_run1.sh share/qtest_run2.sh )
