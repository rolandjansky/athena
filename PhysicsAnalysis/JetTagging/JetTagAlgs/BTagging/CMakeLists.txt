################################################################################
# Package: BTagging
################################################################################

# Declare the package name:
atlas_subdir( BTagging )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthToolSupport/AsgTools
                          Control/AthenaBaseComps
                          Event/xAOD/xAODBTagging
                          Event/xAOD/xAODJet
                          GaudiKernel
                          MagneticField/MagFieldInterfaces
                          Reconstruction/Jet/JetInterface
                          Tracking/TrkEvent/VxVertex
                          PRIVATE
                          DetectorDescription/GeoPrimitives
                          Event/xAOD/xAODBase
                          Event/xAOD/xAODCore
                          Event/xAOD/xAODMuon
                          Event/xAOD/xAODTracking
                          InnerDetector/InDetRecTools/InDetRecToolInterfaces
                          PhysicsAnalysis/AnalysisCommon/ParticleJetTools
                          PhysicsAnalysis/JetTagging/JetTagEvent
                          PhysicsAnalysis/JetTagging/JetTagInfo
                          PhysicsAnalysis/JetTagging/JetTagTools
                          Reconstruction/Particle
                          Tracking/TrkEvent/TrkLinks
                          Tracking/TrkEvent/VxJetVertex
                          Tracking/TrkEvent/VxSecVertex )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( BTaggingLib
                   src/BTagTool.cxx
                   src/BTagTrackAssociation.cxx
                   src/BTagSecVertexing.cxx
                   src/JetBTaggerTool.cxx
                   src/BTagLabeling.cxx
                   src/StandAloneJetBTaggerAlg.cxx
                   src/BTagJetPtScaling.cxx
                   src/BTagTrackAugmenterAlg.cxx
                   PUBLIC_HEADERS BTagging
                   PRIVATE_INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES AsgTools AthenaBaseComps xAODBTagging xAODJet GaudiKernel MagFieldInterfaces JetInterface VxVertex ParticleJetToolsLib JetTagToolsLib
                   PRIVATE_LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives xAODBase xAODCore xAODMuon xAODTracking InDetRecToolInterfaces JetTagEvent JetTagInfo Particle TrkLinks VxJetVertex VxSecVertex )

atlas_add_component( BTagging
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AsgTools AthenaBaseComps xAODBTagging xAODJet GaudiKernel MagFieldInterfaces JetInterface VxVertex GeoPrimitives xAODBase xAODCore xAODMuon xAODTracking InDetRecToolInterfaces ParticleJetToolsLib JetTagEvent JetTagInfo JetTagToolsLib Particle TrkLinks VxJetVertex VxSecVertex BTaggingLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

