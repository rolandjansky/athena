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
                          Event/xAOD/xAODEventInfo
                          PhysicsAnalysis/AnalysisTrigger/AnalysisTriggerEvent
                          PhysicsAnalysis/EventTag/EventTagUtils
                          PhysicsAnalysis/JetMissingEtID/JetMissingEtTagTools )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_component( EventTagAlgs
                     src/AnalysisTagBuilder.cxx
                     src/GlobalEventTagBuilder.cxx
                     src/GlobalTriggerTagBuilder.cxx
                     src/MissingETTagBuilder.cxx
                     src/ParticleJetTagBuilder.cxx
                     src/RawInfoSummaryTagBuilder.cxx
                     src/TagBuilderBase.cxx
                     src/EventSplitter.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaPoolUtilities GaudiKernel TagEvent StoreGateLib SGtests xAODEventInfo AnalysisTriggerEvent EventTagUtilsLib JetMissingEtTagToolsLib )

# Install files from the package:
atlas_install_headers( EventTagAlgs )
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

