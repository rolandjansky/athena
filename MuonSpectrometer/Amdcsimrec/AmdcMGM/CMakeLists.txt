################################################################################
# Package: AmdcMGM
################################################################################

# Declare the package name:
atlas_subdir( AmdcMGM )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( AmdcMGMLib
                   src/*.cxx
                   PUBLIC_HEADERS AmdcMGM
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives GaudiKernel MuonReadoutGeometry AmdcAthLib MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES Identifier )

atlas_add_component( AmdcMGM
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GeoPrimitives GaudiKernel AmdcAthLib MuonReadoutGeometry MuonIdHelpersLib Identifier AmdcMGMLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

