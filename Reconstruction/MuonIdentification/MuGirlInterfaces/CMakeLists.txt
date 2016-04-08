################################################################################
# Package: MuGirlInterfaces
################################################################################

# Declare the package name:
atlas_subdir( MuGirlInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecUtils/MuonLayerHough
                          Reconstruction/MuonIdentification/MuonCombinedEvent )

# Component(s) in the package:
atlas_add_library( MuGirlInterfaces
                   PUBLIC_HEADERS MuGirlInterfaces
                   LINK_LIBRARIES xAODTracking GaudiKernel MuonSegment MuonLayerHough MuonCombinedEvent )

