################################################################################
# Package: MuonSegmentTaggerToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( MuonSegmentTaggerToolInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          MuonSpectrometer/MuonReconstruction/MuonSegmentMakers/MuonSegmentMakerTools/MuonSegmentMakerToolInterfaces
                          Reconstruction/MuonIdentification/MuonCombinedEvent
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters )

# Install files from the package:
atlas_install_headers( MuonSegmentTaggerToolInterfaces )

