################################################################################
# Package: PyAnalysisUtils
################################################################################

# Declare the package name:
atlas_subdir( PyAnalysisUtils )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

atlas_add_test( root_pickle_test
                SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/test/root_pickle_t.py )

atlas_add_test( pydraw_test
                SCRIPT python ${CMAKE_CURRENT_SOURCE_DIR}/test/pydraw_t.py )

atlas_add_test( interpolate_test
                SCRIPT python -m PyAnalysisUtils.interpolate )

atlas_add_test( combo_test
                SCRIPT python -m PyAnalysisUtils.combo )

