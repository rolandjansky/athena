################################################################################
# Package: MuonCombinedAlgs
################################################################################

# Declare the package name:
atlas_subdir( MuonCombinedAlgs )

# Component(s) in the package:
atlas_add_component( MuonCombinedAlgs
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaMonitoringKernelLib xAODCaloEvent xAODMuon xAODTruth xAODTracking GaudiKernel MuonSegment MuonRecToolInterfaces MuonLayerEvent MuonCombinedEvent MuonCombinedToolInterfaces TrkSegment TrkTrack TrkToolInterfaces)

atlas_install_python_modules( python/MuonCombinedAlgsMonitoring.py)
