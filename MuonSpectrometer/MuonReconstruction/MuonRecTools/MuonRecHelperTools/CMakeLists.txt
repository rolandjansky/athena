################################################################################
# Package: MuonRecHelperTools
################################################################################

# Declare the package name:
atlas_subdir( MuonRecHelperTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonRecHelperToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS MuonRecHelperTools
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaBaseComps Identifier GaudiKernel MuonPattern MuonSegment TrkParameters TrkToolInterfaces MuonIdHelpersLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks xAODMuon MuonCompetingRIOsOnTrack MuonPrepRawData MuonRIO_OnTrack MuonRecToolInterfaces TrkSurfaces TrkEventPrimitives TrkMeasurementBase TrkPrepRawData TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkSegment TrkTrack TrkTrackSummary TrkExInterfaces MuonSegmentMakerToolInterfaces xAODMuonCnvLib )

atlas_add_component( MuonRecHelperTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps Identifier GaudiKernel MuonPattern MuonSegment TrkParameters TrkToolInterfaces AthLinks xAODMuon MuonIdHelpersLib MuonCompetingRIOsOnTrack MuonPrepRawData MuonRIO_OnTrack MuonRecToolInterfaces TrkSurfaces TrkEventPrimitives TrkMeasurementBase TrkPrepRawData TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkSegment TrkTrack TrkTrackSummary TrkExInterfaces MuonRecHelperToolsLib MuonSegmentMakerToolInterfaces xAODMuonCnvLib )

