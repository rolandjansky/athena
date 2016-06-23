################################################################################
# Package: JetTagTools
################################################################################

# Declare the package name:
atlas_subdir( JetTagTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Control/AthenaBaseComps
                          Control/SGTools
                          DetectorDescription/GeoPrimitives
                          Event/xAOD/xAODBTagging
                          Event/xAOD/xAODJet
                          Event/xAOD/xAODTracking
                          GaudiKernel
                          PhysicsAnalysis/JetTagging/JetTagInfo
                          Reconstruction/Jet/JetRec
                          Reconstruction/Jet/JetSubStructureMomentTools
                          Reconstruction/Jet/JetSubStructureUtils
                          Reconstruction/MVAUtils
                          Reconstruction/egamma/egammaMVACalib
                          Tracking/TrkEvent/TrkParameters
                          PRIVATE
                          Control/AthenaKernel
                          Control/Navigation
                          Event/xAOD/xAODMuon
                          InnerDetector/InDetRecTools/InDetTrackSelectionTool
                          InnerDetector/InDetRecTools/TrackVertexAssociationTool
                          PhysicsAnalysis/AnalysisCommon/ParticleJetTools
                          PhysicsAnalysis/JetTagging/JetTagCalibration
                          PhysicsAnalysis/MuonID/MuonSelectorTools
                          PhysicsAnalysis/MuonID/MuonIDAnalysis/MuonMomentumCorrections
                          Reconstruction/Jet/JetEDM
                          Reconstruction/Particle
                          Reconstruction/RecoTools/ITrackToVertex
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkLinks
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackLink
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkEvent/VxJetVertex
                          Tracking/TrkEvent/VxSecVertex
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkUtilityPackages/TrkNeuralNetworkUtils
                          Tracking/TrkVertexFitter/TrkVertexFitterInterfaces )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )
find_package( FastJet )
find_package( ROOT COMPONENTS TMVA Core Tree MathCore Hist RIO pthread MathMore Minuit Minuit2 Matrix Physics HistPainter Rint RooFitCore RooFit )

# Component(s) in the package:
atlas_add_library( JetTagToolsLib
                   src/DummyTag.cxx
                   src/TrackSelector.cxx
                   src/HistoHelperRoot.cxx
                   src/NewLikelihoodTool.cxx
                   src/BasicTrackGradeFactory.cxx
                   src/DetailedTrackGradeFactory.cxx
                   src/GradedTrack.cxx
                   src/IPTag.cxx
                   src/JetTagUtils.cxx
                   src/SVForIPTool.cxx
                   src/SVTag.cxx
                   src/JetFitterTag.cxx
                   src/JetFitterNtupleWriter.cxx
                   src/JetFitterNNTool.cxx
                   src/MV1Tag.cxx
                   src/MV1cTag.cxx
                   src/MV2Tag.cxx
                   src/MVbTag.cxx
                   src/JetFitterVariablesFactory.cxx
                   src/MSVVariablesFactory.cxx
                   src/MultiSVTag.cxx
                   src/JetVertexCharge.cxx
                   src/SoftMuonTag.cxx
                   src/ExKtbbTag.cxx
                   src/ExKtbbTagTool.cxx
                   PUBLIC_HEADERS JetTagTools
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${FASTJET_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AsgTools AthenaBaseComps SGTools GeoPrimitives xAODBTagging xAODJet xAODTracking GaudiKernel JetTagInfo JetSubStructureUtils TrkParameters JetRecLib JetSubStructureMomentToolsLib egammaMVACalibLib MVAUtilsLib TrackVertexAssociationToolLib ParticleJetToolsLib JetTagCalibrationLib MuonSelectorToolsLib TrkNeuralNetworkUtilsLib
                   PRIVATE_LINK_LIBRARIES ${FASTJET_LIBRARIES} AthenaKernel Navigation xAODMuon JetEDM Particle ITrackToVertex TrkEventPrimitives TrkLinks TrkTrack TrkTrackSummary VxJetVertex VxSecVertex VxVertex TrkVertexFitterInterfaces )

atlas_add_component( JetTagTools
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${FASTJET_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${FASTJET_LIBRARIES} ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} AsgTools AthenaBaseComps SGTools GeoPrimitives xAODBTagging xAODJet xAODTracking GaudiKernel JetTagInfo JetRecLib JetSubStructureMomentToolsLib JetSubStructureUtils egammaMVACalibLib MVAUtilsLib TrkParameters AthenaKernel Navigation xAODMuon TrackVertexAssociationToolLib ParticleJetToolsLib JetTagCalibrationLib MuonSelectorToolsLib JetEDM Particle ITrackToVertex TrkEventPrimitives TrkLinks TrkTrack TrkTrackSummary VxJetVertex VxSecVertex VxVertex TrkNeuralNetworkUtilsLib TrkVertexFitterInterfaces JetTagToolsLib )

# Install files from the package:
atlas_install_runtime( share/*.root share/*.txt )

