################################################################################
# Package: MuidCombinedTools
################################################################################

# Declare the package name:
atlas_subdir( MuidCombinedTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          MagneticField/MagFieldInterfaces
                          Reconstruction/MuonIdentification/MuidInterfaces
                          Tracking/TrkEvent/TrkParameters
                          PRIVATE
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          Reconstruction/MuonIdentification/MuidEvent
                          Reconstruction/MuonIdentification/MuonCombinedToolInterfaces
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/iPat/iPatTrackParameters
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkExtrapolation/TrkExUtils
                          Tracking/TrkFitter/TrkiPatFitterUtils
                          Tracking/TrkTools/TrkToolInterfaces )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( MuidCombinedTools
                     src/MuidBackTracker.cxx
                     src/MuidMatchMaker.cxx
                     src/MuidVertexCombiner.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps GaudiKernel MagFieldInterfaces MuidInterfaces TrkParameters InDetRIO_OnTrack MuonRIO_OnTrack MuidEvent MuonCombinedToolInterfaces muonEvent iPatTrackParameters TrkSurfaces TrkEventPrimitives TrkMeasurementBase TrkPseudoMeasurementOnTrack TrkTrack TrkTrackSummary VxVertex TrkExInterfaces TrkExUtils TrkiPatFitterUtils TrkToolInterfaces )

# Install files from the package:
atlas_install_headers( MuidCombinedTools )

