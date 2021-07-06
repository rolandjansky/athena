################################################################################
# Package: TrigEgammaValidation
################################################################################

# Declare the package name:
atlas_subdir( TrigEgammaValidation )

# Install files from the package:
atlas_install_joboptions(  share/testNavZeeTPExample.py  )
atlas_install_runtime( share/*.C share/*.py )

