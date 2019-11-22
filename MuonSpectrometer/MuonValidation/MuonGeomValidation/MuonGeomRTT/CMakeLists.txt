################################################################################
# Package: MuonGeomRTT
################################################################################

# Declare the package name:
atlas_subdir( MuonGeomRTT )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_runtime( test/MuonGeomRTT_TestConfiguration.xml share/*.C )

