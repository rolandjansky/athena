################################################################################
# Package: JetMissingEtTagTools
################################################################################

# Declare the package name:
atlas_subdir( JetMissingEtTagTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODJet
                          GaudiKernel
                          PhysicsAnalysis/EventTag/TagEvent
			  PhysicsAnalysis/JetTagging/JetTagPerformanceCalibration/xAODBTaggingEfficiency
                          PRIVATE
                          Control/AthContainers
                          Control/CxxUtils
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODMissingET
                          Event/xAOD/xAODParticleEvent
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTau
                          PhysicsAnalysis/AnalysisCommon/AnalysisUtils
                          PhysicsAnalysis/JetMissingEtID/JetSelectorTools
			  Reconstruction/MET/METInterface
                          Reconstruction/Jet/JetCalibTools
			  Reconstruction/Jet/JetInterface 
                          Reconstruction/Jet/JetUtils )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( JetMissingEtTagToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS JetMissingEtTagTools
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES AthenaBaseComps AthenaPoolUtilities xAODJet GaudiKernel TagEvent AnalysisUtilsLib JetSelectorToolsLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} AthContainers CxxUtils xAODCore xAODMissingET xAODParticleEvent JetUtils )

atlas_add_component( JetMissingEtTagTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CLHEP_LIBRARIES} AthenaBaseComps AthenaPoolUtilities xAODJet GaudiKernel TagEvent AthContainers CxxUtils xAODCore xAODMissingET xAODParticleEvent AnalysisUtilsLib JetSelectorToolsLib JetUtils JetMissingEtTagToolsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

