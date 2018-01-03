################################################################################
# Package: EventTagAlgs
################################################################################

# Declare the package name:
atlas_subdir( EventTagAlgs )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Database/AthenaPOOL/AthenaPoolUtilities
                          GaudiKernel
                          PhysicsAnalysis/EventTag/TagEvent
                          PRIVATE
                          Control/StoreGate
                          Database/APR/POOLCore
                          Event/xAOD/xAODEventInfo
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent
                          PhysicsAnalysis/ElectronPhotonID/ElectronPhotonTagTools
                          PhysicsAnalysis/EventTag/EventTagUtils
                          PhysicsAnalysis/JetMissingEtID/JetMissingEtTagTools
                          PhysicsAnalysis/MuonID/MuonTagTools
                          PhysicsAnalysis/TauID/TauTagTools )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( EventTagAlgs
                     src/AnalysisTagBuilder.cxx
                     src/ElectronTagBuilder.cxx
                     src/GlobalEventTagBuilder.cxx
                     src/GlobalTriggerTagBuilder.cxx
                     src/MissingETTagBuilder.cxx
                     src/MuonTagBuilder.cxx
                     src/ParticleJetTagBuilder.cxx
                     src/PhotonTagBuilder.cxx
                     src/RawInfoSummaryTagBuilder.cxx
                     src/TagBuilderBase.cxx
                     src/TauJetTagBuilder.cxx
                     src/EventSplitter.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaPoolUtilities GaudiKernel TagEvent StoreGateLib SGtests POOLCore xAODEventInfo AnalysisTriggerEvent ElectronPhotonTagToolsLib EventTagUtilsLib JetMissingEtTagToolsLib MuonTagToolsLib TauTagToolsLib )

# Install files from the package:
atlas_install_headers( EventTagAlgs )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

