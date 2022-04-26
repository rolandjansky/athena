# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigInDetAnalysisUtils )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore )

# Component(s) in the package:
atlas_add_library( TrigInDetAnalysisUtils
   TrigInDetAnalysisUtils/*.h src/*.cxx
   PUBLIC_HEADERS TrigInDetAnalysisUtils
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel McParticleEvent Particle
   muonEvent egammaEvent tauEvent xAODMuon xAODEgamma xAODTau xAODTruth xAODTracking
   TrkParameters TrkTrack TrkToolInterfaces TrkTrackSummary TrigDecisionToolLib
   TrigInDetAnalysis TrigInDetEvent TrigSteeringEvent TrigInDetTruthEvent
   AtlasHepMCLib )
