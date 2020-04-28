################################################################################
# Package: PyAnalysisUtils
################################################################################

# Declare the package name:
atlas_subdir( PyAnalysisUtils )

# Install files from the package:
atlas_install_python_modules( python/*.py )

atlas_add_test( flake8
                SCRIPT flake8 --select=ATL,F,E7,E9,W6 ${CMAKE_CURRENT_SOURCE_DIR}/python
                POST_EXEC_SCRIPT nopost.sh )

atlas_add_test( root_pickle_test
                SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/test/root_pickle_t.py )

atlas_add_test( pydraw_test
                SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/test/pydraw_t.py )

atlas_add_test( interpolate_test
                SCRIPT python -m PyAnalysisUtils.interpolate )

atlas_add_test( combo_test
                SCRIPT python -m PyAnalysisUtils.combo )

