################################################################################
# Package: MuidTrackBuilder
################################################################################

# Declare the package name:
atlas_subdir( MuidTrackBuilder )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          DetectorDescription/AtlasDetDescr
                          GaudiKernel
                          Reconstruction/MuonIdentification/MuidInterfaces
                          Tracking/TrkDetDescr/TrkDetDescrInterfaces
                          Tracking/TrkDetDescr/TrkGeometry
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          PRIVATE
                          Control/AthenaKernel
			  Event/EventPrimitives
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonCompetingRIOsOnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecHelperTools
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces
                          Reconstruction/MuonIdentification/MuonCombinedToolInterfaces
                          Reconstruction/MuonIdentification/muonEvent
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkExtrapolation/TrkExUtils
                          Tracking/TrkFitter/TrkFitterInterfaces
                          Tracking/TrkFitter/TrkiPatFitterUtils
                          Tracking/TrkTools/TrkToolInterfaces
                          MagneticField/MagFieldElements
                          MagneticField/MagFieldConditions
			  )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( MuidTrackBuilder
                     src/CombinedMuonTrackBuilder.cxx
                     src/MuidMuonRecovery.cxx
                     src/MuonMatchQuality.cxx
                     src/MuonTrackQuery.cxx
                     src/OutwardsCombinedMuonTrackBuilder.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AtlasDetDescr GaudiKernel MuidInterfaces TrkDetDescrInterfaces TrkGeometry TrkParameters TrkTrack InDetRIO_OnTrack MuonReadoutGeometry MuonIdHelpersLib MuonCompetingRIOsOnTrack MuonRIO_OnTrack MuonRecHelperToolsLib MuonRecToolInterfaces MuonCombinedToolInterfaces muonEvent TrkSurfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkEventUtils TrkMaterialOnTrack TrkMeasurementBase TrkPseudoMeasurementOnTrack TrkTrackSummary VxVertex TrkExInterfaces TrkExUtils TrkFitterInterfaces TrkiPatFitterUtils TrkToolInterfaces MagFieldElements MagFieldConditions )

# Install files from the package:
atlas_install_headers( MuidTrackBuilder )

