################################################################################
# Package: CscValidationUtil
################################################################################

# Declare the package name:
atlas_subdir( CscValidationUtil )

# Component(s) in the package:
atlas_add_library( CscValidationUtil
                   src/*.cxx
                   PUBLIC_HEADERS CscValidationUtil )

# Install files from the package:
atlas_install_joboptions( share/MuonGeometry.py share/CscDataBuilder.py )

