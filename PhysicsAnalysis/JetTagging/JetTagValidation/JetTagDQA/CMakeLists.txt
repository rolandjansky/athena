################################################################################
# Package: JetTagDQA
################################################################################

# Declare the package name:
atlas_subdir( JetTagDQA )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaMonitoring
                          Event/xAOD/xAODBTagging
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODTracking
                          PhysicsAnalysis/AnalysisCommon/ParticleJetTools
                          Tracking/TrkValidation/TrkValHistUtils )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( JetTagDQA
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} GaudiKernel AthenaBaseComps AthenaMonitoringLib xAODBTagging xAODBase xAODJet xAODTracking ParticleJetToolsLib TrkValHistUtils )

