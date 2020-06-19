################################################################################
# Package: DerivationFrameworkJetEtMiss
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkJetEtMiss )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          Control/AthenaKernel
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODPFlow
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigger
                          Event/xAOD/xAODTruth
                          Event/xAOD/xAODCaloEvent
			  InnerDetector/InDetRecTools/InDetTrackSelectionTool
			  InnerDetector/InDetRecTools/TrackVertexAssociationTool
			  PhysicsAnalysis/AnalysisCommon/ParticleJetTools
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
			  PhysicsAnalysis/Interfaces/JetAnalysisInterfaces
			  PhysicsAnalysis/Interfaces/FTagAnalysisInterfaces
                          Reconstruction/Jet/JetInterface
			  Reconstruction/Jet/JetMomentTools
			  Reconstruction/Jet/JetJvtEfficiency
			  Reconstruction/PFlow/PFlowUtils
                          Tools/PathResolver 
			  Trigger/TrigAnalysis/TrigAnalysisInterfaces )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( DerivationFrameworkJetEtMissLib
                   src/*.cxx
                   NO_PUBLIC_HEADERS
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel
                   PRIVATE_LINK_LIBRARIES AthenaBaseComps xAODCore xAODJet xAODPFlow xAODTracking xAODTrigger xAODTruth JetInterface PathResolver PFlowUtilsLib ParticleJetToolsLib FTagAnalysisInterfacesLib JetAnalysisInterfacesLib JetMomentToolsLib InDetTrackSelectionToolLib TrackVertexAssociationToolLib JetJvtEfficiencyLib)

atlas_add_component( DerivationFrameworkJetEtMiss
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthenaBaseComps xAODCore xAODJet xAODPFlow xAODTracking xAODTrigger xAODTruth JetInterface PathResolver PFlowUtilsLib DerivationFrameworkJetEtMissLib JetJvtEfficiencyLib)

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_runtime( share/*.root )

