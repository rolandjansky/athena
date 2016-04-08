################################################################################
# Package: MuonSegmentCombinerToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( MuonSegmentCombinerToolInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPattern
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          Tracking/TrkEvent/TrkSegment )

# Install files from the package:
atlas_install_headers( MuonSegmentCombinerToolInterfaces )

