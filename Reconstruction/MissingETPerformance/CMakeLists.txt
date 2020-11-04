# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MissingETPerformance )

# External dependencies:
find_package( Boost )
find_package( CLHEP )
find_package( ROOT COMPONENTS Graf Gpad Core Tree MathCore Hist RIO Physics )

# Component(s) in the package:
atlas_add_library( MissingETPerformanceLib
                   src/*.cxx
                   PUBLIC_HEADERS MissingETPerformance
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} 
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloConditions CaloEvent AthenaBaseComps SGTools GaudiKernel GeneratorObjects LArRecConditions AnalysisTriggerEvent McParticleEvent JetEvent JetUtils MissingETEvent muonEvent Particle egammaEvent TrigMissingEtEvent StoreGateLib AthAnalysisToolsLib MissingETGoodnessLib TrigDecisionToolLib CaloInterfaceLib
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} AthContainers AtlasHepMCLib AthenaKernel CaloGeoHelpers FourMom FourMomUtils xAODEventInfo TruthHelper tauEvent VxVertex TrigConfL1Data )

atlas_add_component( MissingETPerformance
                     src/components/*.cxx
                     LINK_LIBRARIES MissingETPerformanceLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

