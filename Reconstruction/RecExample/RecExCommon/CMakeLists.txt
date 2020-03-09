################################################################################
# Package: RecExCommon
################################################################################

# Declare the package name:
atlas_subdir( RecExCommon )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( data/*.ascii.gz )
atlas_install_scripts( share/RecExCommon_links_zc.sh share/RecExCommon_links_cern_zc.sh share/RecExCommon_links.sh share/RecExCommon_links.csh share/RecExCommon_runTimeFiles_zc.sh share/RecExCommon_dump.C Testing/manyrecotests.sh share/recexcommon-links.py share/qtest_run1.sh share/qtest_run2.sh )

