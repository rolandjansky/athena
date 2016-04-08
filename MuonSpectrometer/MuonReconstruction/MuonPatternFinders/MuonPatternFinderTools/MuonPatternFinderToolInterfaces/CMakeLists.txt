################################################################################
# Package: MuonPatternFinderToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( MuonPatternFinderToolInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPattern
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          Tracking/TrkEvent/TrkRoad )

# Install files from the package:
atlas_install_headers( MuonPatternFinderToolInterfaces )

