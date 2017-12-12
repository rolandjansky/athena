################################################################################
# Package: MuonCombinedEvent
################################################################################

# Declare the package name:
atlas_subdir( MuonCombinedEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthLinks
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          Reconstruction/MuonIdentification/MuidEvent
                          Reconstruction/MuonIdentification/muonEvent
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkParameters
			  MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonLayerEvent
                          PRIVATE
                          GaudiKernel
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          Tracking/TrkEvent/TrkTrack )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( MuonCombinedEvent
                   src/*.cxx
                   PUBLIC_HEADERS MuonCombinedEvent
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthLinks DataModel SGTools GeoPrimitives Identifier xAODMuon xAODTracking MuidEvent muonEvent TrkEventPrimitives TrkMaterialOnTrack TrkParameters MuonLayerEvent
                   PRIVATE_LINK_LIBRARIES GaudiKernel MuonSegment TrkTrack )

