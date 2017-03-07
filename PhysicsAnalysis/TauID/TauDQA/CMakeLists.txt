################################################################################
# Package: TauDQA
################################################################################

# Declare the package name:
atlas_subdir( TauDQA )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaMonitoring
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODTau
                          Event/xAOD/xAODTruth
                          PhysicsAnalysis/TauID/TauAnalysisTools
                          PhysicsAnalysis/TauID/TauHistUtils
                          PhysicsAnalysis/TruthParticleID/McParticleEvent
                          Tracking/TrkValidation/TrkValHistUtils )

# External dependencies:
find_package( HepMC )
find_package( HepPDT )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( TauDQA
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${HEPPDT_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${HEPMC_INCLUDE_DIRS}
                     LINK_LIBRARIES ${HEPPDT_LIBRARIES} ${ROOT_LIBRARIES} ${HEPMC_LIBRARIES} GaudiKernel AthenaBaseComps AthenaMonitoringLib xAODEgamma xAODJet xAODTau xAODTruth TauHistUtils McParticleEvent TrkValHistUtils )

