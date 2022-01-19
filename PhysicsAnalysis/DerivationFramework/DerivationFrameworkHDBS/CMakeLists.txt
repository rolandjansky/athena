################################################################################
# Package: DerivationFrameworkHDBS
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkHDBS )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthenaBaseComps
   Control/StoreGate
   Event/xAOD/xAODEgamma
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODJet
   Event/xAOD/xAODMuon
   Event/xAOD/xAODTracking
   Event/xAOD/xAODTruth
   Event/xAOD/FourMomUtils
   InnerDetector/InDetRecTools/InDetConversionFinderTools
   InnerDetector/InDetRecTools/InDetTrackSelectionTool
   PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
   Tracking/TrkVertexFitter/TrkVKalVrtFitter
   Tracking/TrkVertexFitter/TrkVertexFitterInterfaces
   Tracking/TrkVertexFitter/TrkVertexAnalysisUtils
   Trigger/TrigAnalysis/TrigDecisionTool
   PhysicsAnalysis/BPhys/BPhysTools
   PRIVATE
   GaudiKernel
   Tracking/TrkEvent/VxVertex
   Event/xAOD/xAODTrigger
   Event/xAOD/xAODBPhys
   Control/AthContainers
   Generators/GenInterfaces
   Generators/TruthRivetTools
   Generators/TruthUtils
   Tools/PathResolver
   PhysicsAnalysis/ElectronPhotonID/PhotonVertexSelection
   PhysicsAnalysis/Interfaces/EgammaAnalysisInterfaces
   PhysicsAnalysis/AnalysisCommon/PMGOverlapRemovalTools
   Reconstruction/egamma/egammaInterfaces
   PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools)

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Core Physics )

# Component(s) in the package:
atlas_add_component( DerivationFrameworkHDBS
  DerivationFrameworkHDBS/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps
   xAODEgamma xAODJet xAODMuon xAODTracking xAODTruth xAODBPhysLib FourMomUtils TrkVKalVrtFitterLib InDetConversionFinderToolsLib
   TrkVertexFitterInterfaces TrkVertexAnalysisUtilsLib TrigDecisionToolLib xAODEventInfo
   GaudiKernel VxVertex xAODTrigger PhotonVertexSelection GammaORToolsLib AthContainers
   TruthConvertersLib TruthRivetToolsLib TruthUtils PathResolver EgammaAnalysisInterfacesLib ElectronPhotonSelectorToolsLib)

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

