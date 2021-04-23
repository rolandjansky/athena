################################################################################
# Package: MuonTruthTools
################################################################################

# Declare the package name:
atlas_subdir( MuonTruthTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree )

# Component(s) in the package:
atlas_add_component( MuonTruthTools
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} 
   LINK_LIBRARIES ${ROOT_LIBRARIES} AtlasHepMCLib AthenaBaseComps
   GeoPrimitives Identifier EventPrimitives GaudiKernel MuonIdHelpersLib
   MuonPrepRawData MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib
   MuonRecToolInterfaces MuonTrackMakerUtils MuonSimData TrkDetElementBase
   TrkSurfaces TrkMeasurementBase TrkParameters TrkPrepRawData
   TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkSegment TrkTrack TrkTruthData
   TrkExInterfaces TrkFitterInterfaces TrackRecordLib TrkTruthTrackInterfaces )
