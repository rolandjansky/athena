################################################################################
# Package: MuonRecRTT
################################################################################

# Declare the package name:
atlas_subdir( MuonRecRTT )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/MuonRecRTT_TestConfiguration.xml share/*.C share/*.py scripts/*.py ExtraFiles/*.html test/FileCheck.py )

