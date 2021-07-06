################################################################################
# Package: MuonCalibMath
################################################################################

# Declare the package name:
atlas_subdir( MuonCalibMath )

# External dependencies:
find_package( ROOT COMPONENTS MathCore MathMore Core Tree Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonCalibMath
                   src/*.cxx
                   PUBLIC_HEADERS MuonCalibMath
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES EventPrimitives AthenaKernel
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

