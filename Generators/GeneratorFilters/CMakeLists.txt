################################################################################
# Package: GeneratorFilters
################################################################################

# Declare the package name:
atlas_subdir( GeneratorFilters )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/StoreGate
                          GaudiKernel
                          Generators/GeneratorModules
                          Generators/AtlasHepMC
                          PRIVATE
                          Control/AthenaKernel
                          Control/CxxUtils
                          Event/EventInfo
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODTruth
                          Generators/TruthUtils
                          PhysicsAnalysis/AnalysisCommon/ParticleEvent
                          PhysicsAnalysis/TruthParticleID/McParticleEvent )

# External dependencies:
find_package( CLHEP )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint )

# Component(s) in the package:
atlas_add_library( GeneratorFiltersLib
                   src/*.cxx
                   PUBLIC_HEADERS GeneratorFilters
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                   PRIVATE_INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib GaudiKernel StoreGateLib SGtests GeneratorModulesLib
                   PRIVATE_LINK_LIBRARIES ${HEPPDT_LIBRARIES} AthenaKernel CxxUtils EventInfo xAODJet xAODTruth TruthUtils ParticleEvent McParticleEvent )

atlas_add_component( GeneratorFilters
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} 
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} AtlasHepMCLib StoreGateLib SGtests GaudiKernel GeneratorModulesLib AthenaKernel CxxUtils EventInfo xAODJet xAODTruth TruthUtils ParticleEvent McParticleEvent GeneratorFiltersLib )

# Install job option fragments from the package:
atlas_install_joboptions( share/common/*.py )


