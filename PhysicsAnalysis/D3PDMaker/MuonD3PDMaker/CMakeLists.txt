# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MuonD3PDMaker )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( MuonD3PDMaker
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AtlasHepMCLib AthenaBaseComps AthenaKernel AthContainers StoreGateLib AtlasDetDescr EventKernel FourMomUtils xAODMuon xAODTracking xAODTruth GaudiKernel MuonCalibITools MuonIdHelpersLib MuonPattern MuonPrepRawData MuonSegment MuonRecHelperToolsLib MuonRecToolInterfaces MuonSimEvent AnalysisTriggerEvent D3PDMakerUtils MCTruthClassifierLib McParticleEvent muonEvent TrkGeometry TrkEventPrimitives TrkPrepRawData TrkSegment TrkTrackSummary TrkTruthData TrkExInterfaces TrkToolInterfaces TrigMuonEvent TrackRecordLib MuonTruthAlgsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

