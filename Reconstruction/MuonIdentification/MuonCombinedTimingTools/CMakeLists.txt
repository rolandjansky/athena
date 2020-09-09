################################################################################
# Package: MuonCombinedTimingTools
################################################################################

# Declare the package name:
atlas_subdir( MuonCombinedTimingTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( MuonCombinedTimingTools
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps xAODMuon GaudiKernel MuonCombinedToolInterfaces TrkSegment TrkSpaceTimePoint TrkToolInterfaces )

# Install files from the package:
atlas_install_python_modules( python/*.py )

