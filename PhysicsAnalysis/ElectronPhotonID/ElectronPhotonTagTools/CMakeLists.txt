################################################################################
# Package: ElectronPhotonTagTools
################################################################################

# Declare the package name:
atlas_subdir( ElectronPhotonTagTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/EventInfo
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODMissingET
                          GaudiKernel
			  PhysicsAnalysis/ElectronPhotonID/IsolationCorrections
                          PhysicsAnalysis/AnalysisCommon/IsolationSelection
                          PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools
                          PhysicsAnalysis/ElectronPhotonID/ElectronPhotonShowerShapeFudgeTool
			  PhysicsAnalysis/ElectronPhotonID/ElectronPhotonFourMomentumCorrection
                          PhysicsAnalysis/EventTag/TagEvent
                          PRIVATE
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODParticleEvent
                          Event/xAOD/xAODTracking
                          PhysicsAnalysis/AnalysisCommon/AnalysisUtils )

# tag ROOTGraphicsLibs was not recognized in automatic conversion in cmt2cmake

# tag ROOTMathLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( ElectronPhotonTagToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS ElectronPhotonTagTools
                   LINK_LIBRARIES AthenaBaseComps AthenaPoolUtilities EventInfo xAODEgamma xAODEventInfo xAODMissingET GaudiKernel TagEvent StoreGateLib SGtests IsolationSelectionLib ElectronPhotonSelectorToolsLib ElectronPhotonShowerShapeFudgeToolLib AnalysisUtilsLib
                   PRIVATE_LINK_LIBRARIES xAODCore xAODParticleEvent xAODTracking )

atlas_add_component( ElectronPhotonTagTools
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps StoreGateLib SGtests AthenaPoolUtilities EventInfo xAODEgamma xAODEventInfo xAODMissingET GaudiKernel IsolationSelectionLib ElectronPhotonFourMomentumCorrectionLib ElectronPhotonSelectorToolsLib ElectronPhotonShowerShapeFudgeToolLib TagEvent xAODCore xAODParticleEvent xAODTracking AnalysisUtilsLib ElectronPhotonTagToolsLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

