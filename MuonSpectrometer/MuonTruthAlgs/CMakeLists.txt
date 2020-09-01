################################################################################
# Package: MuonTruthAlgs
################################################################################

# Declare the package name:
atlas_subdir( MuonTruthAlgs )

atlas_add_library( MuonTruthAlgsLib
                   MuonTruthAlgs/*.h
                   INTERFACE
                   PUBLIC_HEADERS MuonTruthAlgs
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel StoreGateLib TrkToolInterfaces TrkTrack TrkTruthData MuonRecToolInterfaces MuonIdHelpersLib MuonPrepRawData MuonSimData TrackRecordLib )

# Component(s) in the package:
atlas_add_component( MuonTruthAlgs
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS 
                     LINK_LIBRARIES MuonTruthAlgsLib AtlasHepMCLib SGtests Identifier AthLinks AthContainers AtlasDetDescr EventPrimitives xAODMuon xAODTracking xAODTruth GeneratorObjects MuonReadoutGeometry MuonGeoModelLib MuonCompetingRIOsOnTrack MuonPattern MuonRIO_OnTrack MuonSegment MuonRecHelperToolsLib MCTruthClassifierLib Particle ParticleTruth TrkDetElementBase TrkGeometry TrkSurfaces TrkCompetingRIOsOnTrack TrkEventUtils TrkMeasurementBase TrkParameters TrkPrepRawData TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkSegment TrkExInterfaces TrkFitterInterfaces MuonTrackMakerUtils )

