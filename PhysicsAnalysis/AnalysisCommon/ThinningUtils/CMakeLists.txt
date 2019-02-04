# $Id: CMakeLists.txt 795961 2017-02-07 12:53:47Z jcatmore $
################################################################################
# Package: ThinningUtils
################################################################################

# Declare the package name:
atlas_subdir( ThinningUtils )

# Extra dependencies, based on the build environment:
set( extra_src )
set( extra_deps )
set( extra_libs )
if( NOT XAOD_ANALYSIS )
  set( extra_src  src/ThinCaloCellsTool.h src/ThinCaloCellsTool.cxx
                  src/ThinCaloCellsAlg.h  src/ThinCaloCellsAlg.cxx
                  src/ThinTrkTrackAlg.h   src/ThinTrkTrackAlg.cxx )
  set( extra_deps Calorimeter/CaloEvent Trigger/TrigAnalysis/TrigDecisionTool )
  set( extra_libs CaloEvent TrigDecisionToolLib )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PRIVATE
   Control/AthLinks
   Control/AthenaBaseComps
   Control/AthenaKernel
   Event/EventInfo
   Event/xAOD/xAODBase
   Event/xAOD/xAODCaloEvent
   Event/xAOD/xAODEgamma
   Event/xAOD/xAODJet
   Event/xAOD/xAODMuon
   Event/xAOD/xAODParticleEvent
   Event/xAOD/xAODTau
   Event/xAOD/xAODTracking
   Event/xAOD/xAODTruth
   GaudiKernel
   PhysicsAnalysis/CommonTools/ExpressionEvaluation
   PhysicsAnalysis/DerivationFramework/DerivationFrameworkInterfaces
   PhysicsAnalysis/MCTruthClassifier
   ${extra_deps} )

# Component(s) in the package:
atlas_add_component(ThinningUtils
  src/ReducePileUpEventInfoAlg.h           src/ReducePileUpEventInfoAlg.cxx
  src/ThinCaloClustersAlg.h                src/ThinCaloClustersAlg.cxx
  src/ThinCaloClustersTool.h               src/ThinCaloClustersTool.cxx
  src/ThinGeantTruthAlg.h                  src/ThinGeantTruthAlg.cxx
  src/ThinIParticlesAlg.h                  src/ThinIParticlesAlg.cxx
  src/ThinIParticlesTool.h                 src/ThinIParticlesTool.cxx
  src/ThinInDetForwardTrackParticlesAlg.h  src/ThinInDetForwardTrackParticlesAlg.cxx
  src/ThinNegativeEnergyCaloClustersAlg.h  src/ThinNegativeEnergyCaloClustersAlg.cxx
  src/ThinNegativeEnergyNeutralPFOsAlg.h   src/ThinNegativeEnergyNeutralPFOsAlg.cxx
  src/ThinTrackParticlesAlg.h              src/ThinTrackParticlesAlg.cxx
  src/ThinTrackParticlesTool.cxx           src/ThinTrackParticlesTool.h
  src/ThinAssociatedObjectsTool.cxx        src/ThinAssociatedObjectsTool.h
  src/EleLinkThinningTool.cxx              src/EleLinkThinningTool.h
  src/DeltaRThinningTool.cxx               src/DeltaRThinningTool.h
  src/components/*.cxx ${extra_src}
  LINK_LIBRARIES AthLinks AthenaBaseComps AthenaKernel EventInfo xAODBase
  xAODCaloEvent xAODEgamma xAODJet xAODMuon xAODParticleEvent xAODTau
  xAODTracking xAODTruth xAODBTagging GaudiKernel ExpressionEvaluationLib MCTruthClassifierLib ${extra_libs})


# Install files from the package:
atlas_install_python_modules( python/*.py )
