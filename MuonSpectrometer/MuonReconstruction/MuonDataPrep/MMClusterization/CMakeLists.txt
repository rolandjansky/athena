################################################################################
# Package: MMClusterization
################################################################################

# Declare the package name:
atlas_subdir( MMClusterization )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread Matrix Minuit Minuit2)

# Component(s) in the package:
atlas_add_library( MMClusterizationLib
                   src/*.cxx
                   PUBLIC_HEADERS MMClusterization
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES GaudiKernel MuonPrepRawData MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps EventPrimitives MuonReadoutGeometry )

atlas_add_component( MMClusterization
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel MuonPrepRawData AthenaBaseComps EventPrimitives MuonIdHelpersLib MMClusterizationLib )

