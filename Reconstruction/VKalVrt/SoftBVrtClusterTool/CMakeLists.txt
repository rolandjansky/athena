################################################################################
# Package: SoftBVrtClusterTool
################################################################################

# Declare the package name:
atlas_subdir( SoftBVrtClusterTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          PRIVATE
                          Control/AthenaBaseComps
                          DataQuality/GoodRunsLists
                          Event/xAOD/xAODBTagging
                          Event/xAOD/xAODEventInfo
                          Event/xAOD/xAODEventShape
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          Event/xAOD/xAODTrigge
                          Event/xAOD/xAODTruth
                          Event/xAOD/xAODMissingET
                          DetectorDescription/GeoPrimitives
                          InnerDetector/InDetRecTools/InDetTrackSelectionTool
                          InnerDetector/InDetRecTools/TrackVertexAssociationTool
			  JetParticleLabelTool	  
                          PhysicsAnalysis/AnalysisCommon/ParticleJetTools
                          PhysicsAnalysis/AnalysisCommon/PileupReweighting
                          Reconstruction/Jet/JetCalibTools
                          Reconstruction/Jet/JetInterface
                          Reconstruction/RecoTools/ITrackToVertex
			  InnerDetector/InDetRecTools/InDetRecToolInterfaces
			  Tracking/TrkEvent/VxSecVertex
			  Tracking/TrkVertexFitter/TrkVertexSeedFinderUtils
                          PhysicsAnalysis/DerivationFramework/DerivationFrameworkFlavourTag
                          PhysicsAnalysis/JetTagging/JetTagPerformanceCalibration/xAODBTaggingEfficiency )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

atlas_add_library( SoftBVrtClusterToolLib
		   src/*.cxx
		   PUBLIC_HEADERS SoftBVrtClusterTool
		   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthenaBaseComps GoodRunsListsLib xAODBTagging xAODEventInfo xAODEventShape xAODJet xAODMuon xAODTracking xAODTrigger xAODTruth xAODMissingET TrackVertexAssociationToolLib ParticleJetToolsLib JetSubStructureUtils PileupReweightingLib JetInterface ITrackToVertex TrkVertexFitterInterfaces VxSecVertex TrigDecisionToolLib DerivationFrameworkFlavourTagLib )


# Component(s) in the package:
atlas_add_component( SoftBVrtClusterTool
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel AthenaBaseComps GoodRunsListsLib xAODBTagging xAODEventInfo xAODEventShape xAODJet xAODMuon xAODTracking xAODTrigger xAODTruth xAODMissingET TrackVertexAssociationToolLib ParticleJetToolsLib JetSubStructureUtils PileupReweightingLib JetInterface ITrackToVertex TrkVertexFitterInterfaces VxSecVertex TrigDecisionToolLib DerivationFrameworkFlavourTagLib SoftBVrtClusterToolLib )

#install 
atlas_install_python_modules( python/*.py )