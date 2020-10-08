################################################################################
# Package: MuonSegmentTrackMaker
################################################################################

# Declare the package name:
atlas_subdir( MuonSegmentTrackMaker )

# Component(s) in the package:
atlas_add_component( MuonSegmentTrackMaker
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaMonitoringKernelLib GaudiKernel StoreGateLib SGtests MuonSegment MuonRecHelperToolsLib MuonRecToolInterfaces Particle TrkSegment TrkTrack )

# Install files from the package:

atlas_install_python_modules( python/MuonTrackMakerAlgsMonitoring.py)
