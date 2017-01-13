################################################################################
# Package: HLTUtils
################################################################################

# Declare the package name:
atlas_subdir( HLTUtils )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( 
  share/hlt_relocate.sh 
  share/hlt_postinstall.sh 
  share/make_hlt_env.py 
  share/make_hlt_rep.py 
  share/make_hlt_patch.py 
  share/make_coralServer_rep.py 
  share/asetup_wrapper 
  )

