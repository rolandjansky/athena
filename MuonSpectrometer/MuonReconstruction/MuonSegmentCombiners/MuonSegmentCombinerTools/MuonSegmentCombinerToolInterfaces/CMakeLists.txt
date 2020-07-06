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
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          Tracking/TrkEvent/TrkSegment )

atlas_add_library( MuonSegmentCombinerToolInterfaces
                   MuonSegmentCombinerToolInterfaces/*.h
                   INTERFACE
                   PUBLIC_HEADERS MuonSegmentCombinerToolInterfaces
                   LINK_LIBRARIES GaudiKernel MuonSegment MuonPattern TrkSegment MuonPrepRawData MuonEDM_AssociationObjects  )


