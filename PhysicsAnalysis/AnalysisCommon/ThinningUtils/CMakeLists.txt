# $Id: CMakeLists.txt 795961 2017-02-07 12:53:47Z jcatmore $
################################################################################
# Package: ThinningUtils
################################################################################

# Declare the package name:
atlas_subdir( ThinningUtils )

# Extra dependencies, based on the build environment:
set( extra_libs )
if( NOT XAOD_ANALYSIS )
   set( extra_libs CaloEvent TrigDecisionToolLib )
endif()

# Component(s) in the package:
atlas_add_component(ThinningUtils
   src/*.h src/*.cxx src/components/*.cxx
   LINK_LIBRARIES AthLinks AthenaBaseComps AthenaKernel StoreGateLib SGtests EventInfo xAODBase
   xAODCaloEvent xAODEgamma xAODJet xAODMuon xAODParticleEvent xAODTau
   xAODTracking xAODTruth GaudiKernel InDetReadoutGeometry TRT_ReadoutGeometry ExpressionEvaluationLib TrkToolInterfaces MCTruthClassifierLib DerivationFrameworkInterfaces ${extra_libs})

# Install files from the package:
atlas_install_python_modules( python/*.py )
