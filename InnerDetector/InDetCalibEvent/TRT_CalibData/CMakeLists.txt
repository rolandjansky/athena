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

atlas_add_library( TRT_CalibData
                   TRT_CalibData/*.h
                   INTERFACE
                   PUBLIC_HEADERS TRT_CalibData
                   LINK_LIBRARIES TRT_ConditionsData InDetIdentifier )


