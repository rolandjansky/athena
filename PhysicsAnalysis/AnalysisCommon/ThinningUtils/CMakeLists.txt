# $Id: CMakeLists.txt 791506 2017-01-09 00:33:35Z jcatmore $
################################################################################
# Package: ThinningUtils
################################################################################

# Declare the package name:
atlas_subdir( ThinningUtils )

# Extra dependencies, based on the build environment:
set( extra_deps )
set( extra_libs )
if( NOT XAOD_ANALYSIS )
   set( extra_deps Calorimeter/CaloEvent
      Trigger/TrigAnalysis/TrigDecisionTool )
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
   ${extra_deps} )

# Component(s) in the package:
atlas_add_component( ThinningUtils
   src/*.h src/*.cxx src/components/*.cxx
   LINK_LIBRARIES AthLinks AthenaBaseComps AthenaKernel EventInfo xAODBase
   xAODCaloEvent xAODEgamma xAODJet xAODMuon xAODParticleEvent xAODTau
   xAODTracking xAODTruth GaudiKernel ExpressionEvaluationLib ${extra_libs} )

# Install files from the package:
atlas_install_python_modules( python/*.py )
