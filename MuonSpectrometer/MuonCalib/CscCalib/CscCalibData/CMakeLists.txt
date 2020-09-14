################################################################################
# Package: CscCalibData
################################################################################

# Declare the package name:
atlas_subdir( CscCalibData )

# External dependencies:
find_package( ROOT COMPONENTS Hist Core Tree MathCore RIO pthread )

# Component(s) in the package:
atlas_add_library( CscCalibData
                   src/*.cxx
                   PUBLIC_HEADERS CscCalibData
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers )

