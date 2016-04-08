################################################################################
# Package: CscCalibData
################################################################################

# Declare the package name:
atlas_subdir( CscCalibData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools )

# External dependencies:
find_package( ROOT COMPONENTS Hist Core Tree MathCore RIO pthread )

# Component(s) in the package:
atlas_add_library( CscCalibData
                   src/*.cxx
                   PUBLIC_HEADERS CscCalibData
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools )

