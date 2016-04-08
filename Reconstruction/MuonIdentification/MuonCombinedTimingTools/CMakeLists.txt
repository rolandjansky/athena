################################################################################
# Package: MuonCombinedTimingTools
################################################################################

# Declare the package name:
atlas_subdir( MuonCombinedTimingTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Event/xAOD/xAODMuon
                          GaudiKernel
                          Reconstruction/MuonIdentification/MuonCombinedToolInterfaces
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/TrkSpaceTimePoint
                          Tracking/TrkTools/TrkToolInterfaces )

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

