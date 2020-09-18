################################################################################
# Package: MuidTrackBuilder
################################################################################

# Declare the package name:
atlas_subdir( MuidTrackBuilder )

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

