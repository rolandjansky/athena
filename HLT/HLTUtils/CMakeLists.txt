################################################################################
# Package: HLTUtils
################################################################################

# Declare the package name:
atlas_subdir( HLTUtils )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_scripts( 
  share/make_hlt_rep.py
  share/make_coralServer_rep.py
  share/asetup_wrapper 
  )

# Tests:
atlas_add_test( test_make_hlt_rep
   SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/share/make_hlt_rep.py
   POST_EXEC_SCRIPT nopost.sh
)
