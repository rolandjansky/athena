################################################################################
# Package: OnlineRecoTests
################################################################################

# Declare the package name:
atlas_subdir( OnlineRecoTests )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( scripts/ort_*.py scripts/ort_*.sh )

atlas_add_test( flake8_test_dir
                SCRIPT flake8 --select=ATL,F,E7,E9,W6 ${CMAKE_CURRENT_SOURCE_DIR}/python
                POST_EXEC_SCRIPT nopost.sh )
