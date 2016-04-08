################################################################################
# Package: TRT_CalibData
################################################################################

# Declare the package name:
atlas_subdir( TRT_CalibData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          InnerDetector/InDetConditions/TRT_ConditionsData
                          InnerDetector/InDetDetDescr/InDetIdentifier )

# External dependencies:
find_package( CLHEP )

# Install files from the package:
atlas_install_headers( TRT_CalibData )

