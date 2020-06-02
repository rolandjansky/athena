################################################################################
# Package: KitValidation
################################################################################

# Declare the package name:
atlas_subdir( KitValidation )

# Install files from the package:
atlas_install_joboptions( share/kv_perfmon.py share/kv_reflex.py )
atlas_install_scripts( share/kvpost.py )

