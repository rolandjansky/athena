################################################################################
# Package: DerivationFrameworkBPhys
################################################################################

# Declare the package name:
atlas_subdir( DerivationFrameworkBPhys )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Event/xAOD/xAODMuon
   Control/DataModel
   Control/AthenaBaseComps
   PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
   PhysicsAnalysis/JpsiUpsilonTools
   PhysicsAnalysis/MuonID/MuonSelectorTools
   Event/xAOD/xAODTracking
   Event/xAOD/xAODBPhys
   Control/AthenaKernel
   Reconstruction/RecoTools/RecoToolInterfaces
   Event/EventPrimitives
   PhysicsAnalysis/BPhys/BPhysTools
   PRIVATE
   Event/xAOD/xAODBase
   Event/xAOD/xAODMetaData
   Tracking/TrkVertexFitter/TrkVertexAnalysisUtils
   Event/xAOD/xAODEventInfo
   Database/AthenaPOOL/AthenaPoolUtilities
   Event/xAOD/xAODPrimitives
   Calorimeter/CaloEvent
   Reconstruction/RecoTools/TrackToCalo
   Reconstruction/RecoTools/TrackToVertex
   Tracking/TrkEvent/TrkCaloExtension
   InnerDetector/InDetConditions/InDetBeamSpotService
   InnerDetector/InDetRecTools/InDetTrackSelectionTool
   Trigger/TrigAnalysis/TrigDecisionTool
   InnerDetector/InDetRecAlgs/InDetV0Finder
   Tracking/TrkVertexFitter/TrkVertexSeedFinderUtils
   Event/xAOD/xAODParticleEvent)

# Component(s) in the package:
atlas_add_library( DerivationFrameworkBPhysLib
   DerivationFrameworkBPhys/*.h src/*.cxx
   PUBLIC_HEADERS DerivationFrameworkBPhys
   LINK_LIBRARIES xAODMuon DataModel AthenaBaseComps JpsiUpsilonTools
   MuonSelectorTools
   xAODTracking xAODBPhysLib AthenaKernel RecoToolInterfaces EventPrimitives
   BPhysToolsLib
   PRIVATE_LINK_LIBRARIES xAODBase xAODMetaData TrkVertexAnalysisUtils
   xAODEventInfo AthenaPoolUtilities xAODPrimitives CaloEvent TrackToCalo
   TrackToVertex TrkCaloExtension InDetBeamSpotServiceLib
   InDetTrackSelectionToolLib TrigDecisionTool InDetV0FinderLib)

atlas_add_component( DerivationFrameworkBPhys
   src/components/*.cxx
   LINK_LIBRARIES DerivationFrameworkBPhysLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
