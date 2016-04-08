################################################################################
# Package: MuGirlCandidate
################################################################################

# Declare the package name:
atlas_subdir( MuGirlCandidate )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/IRegionSelector
                          DetectorDescription/Identifier
                          GaudiKernel
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonPrepRawData
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonSegment
                          MuonSpectrometer/MuonReconstruction/MuonRecUtils/MuonLayerHough
                          Reconstruction/MuonIdentification/MuGirlInterfaces
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkExtrapolation/TrkExUtils
                          Trigger/TrigEvent/TrigMuonEvent
                          PRIVATE
                          DetectorDescription/RoiDescriptor
                          Event/EventPrimitives
                          Event/xAOD/xAODTracking
                          MuonSpectrometer/MuonCnv/MuonCnvToolInterfaces
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonDataPrep/CscClusterization
                          MuonSpectrometer/MuonReconstruction/MuonPatternFinders/MuonPatternFinderTools/MuonHoughPatternTools
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          MuonSpectrometer/MuonReconstruction/MuonSegmentMakers/MuonSegmentMakerTools/MuonSegmentMakerToolInterfaces
                          Reconstruction/MuonIdentification/MuidInterfaces
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/Particle
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkRoad
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( MuGirlCandidate
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AthenaBaseComps StoreGateLib SGtests GeoPrimitives IRegionSelector Identifier GaudiKernel MuonPrepRawData MuonRIO_OnTrack MuonSegment MuonLayerHough MuGirlInterfaces TrkRIO_OnTrack TrkExUtils TrigMuonEvent RoiDescriptor EventPrimitives xAODTracking MuonReadoutGeometry MuonIdHelpersLib CscClusterizationLib MuonHoughPatternToolsLib MuonRecHelperToolsLib MuonRecToolInterfaces MuidInterfaces muonEvent Particle TrkDetElementBase TrkSurfaces TrkEventPrimitives TrkParameters TrkPrepRawData TrkRoad TrkTrack TrkExInterfaces )

# Install files from the package:
atlas_install_headers( MuGirlCandidate )
atlas_install_python_modules( python/*.py )

