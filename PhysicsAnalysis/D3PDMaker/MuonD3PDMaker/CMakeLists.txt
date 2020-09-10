################################################################################
# Package: MuonD3PDMaker
################################################################################

# Declare the package name:
atlas_subdir( MuonD3PDMaker )

# External dependencies:
find_package( Boost COMPONENTS filesystem thread system )
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( MuonD3PDMaker
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${Boost_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaBaseComps AthenaKernel AthContainers StoreGateLib SGtests AtlasDetDescr EventInfo EventKernel FourMomUtils xAODMuon xAODTracking xAODTruth GaudiKernel MuonCalibITools MuonIdHelpersLib MuonPattern MuonPrepRawData MuonSegment MuonRecHelperToolsLib MuonRecToolInterfaces MuonSimEvent AnalysisTriggerEvent D3PDMakerUtils TriggerD3PDMakerLib MCTruthClassifierLib McParticleEvent muonEvent TrkGeometry TrkEventPrimitives TrkPrepRawData TrkSegment TrkTrackSummary TrkTruthData TrkExInterfaces TrkToolInterfaces TrigObjectMatchingLib TrigInDetEvent TrigMuonEvent TrackRecordLib MuonTruthAlgsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

