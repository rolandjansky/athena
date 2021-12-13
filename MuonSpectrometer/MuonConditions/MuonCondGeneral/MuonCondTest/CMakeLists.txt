################################################################################
# Package: MuonCondTest
################################################################################

# Declare the package name:
atlas_subdir( MuonCondTest )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( MuonCondTest
                     src/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel MuonCondInterface MuonCondData MuonCondSvcLib CoralUtilitiesLib MuonReadoutGeometry Identifier )

# Install files from the package:
atlas_install_headers( MuonCondTest )
atlas_install_joboptions( share/*.py )

