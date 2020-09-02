################################################################################
# Package: AnalysisExamples
################################################################################

# Declare the package name:
atlas_subdir( AnalysisExamples )

# External dependencies:
find_package( CLHEP )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_component( AnalysisExamples
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${HEPPDT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${HEPPDT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib CaloEvent AthenaBaseComps AthenaKernel Navigation SGTools StoreGateLib SGtests NavFourMom xAODEventInfo xAODTracking GaudiKernel TruthHelper GeneratorObjects AthAnalysisToolsLib AnalysisUtilsLib ParticleEvent AnalysisTriggerEvent JetTagEvent JetTagInfo MuonIDEvent McParticleEvent McParticleKernel JetEvent muonEvent Particle ParticleTruth RecoToolInterfaces egammaEvent VxVertex TrkVKalVrtFitterLib TrigDecisionToolLib TrigInDetEvent TrigParticle TrigSteeringEvent CompositeParticleEvent )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

