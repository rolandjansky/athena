################################################################################
# Package: HLTUtils
################################################################################

# Declare the package name:
atlas_subdir( HLTUtils )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( 
  share/make_hlt_rep.py
  share/make_coralServer_rep.py
  share/asetup_wrapper 
  )
