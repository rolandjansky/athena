# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ThinningUtils )

# Extra dependencies, based on the build environment:
set( extra_libs )
if( NOT XAOD_ANALYSIS )
   set( extra_libs CaloEvent TrkTrack TrigDecisionToolLib )
endif()

# Component(s) in the package:
atlas_add_component(ThinningUtils
   src/*.h src/*.cxx src/components/*.cxx
   LINK_LIBRARIES AthContainers AthLinks AthenaBaseComps StoreGateLib EventInfo xAODBase
   xAODCaloEvent xAODEgamma xAODJet xAODMuon xAODParticleEvent xAODPFlow xAODTau
   xAODTracking xAODTruth GaudiKernel InDetReadoutGeometry TRT_ReadoutGeometry ExpressionEvaluationLib MCTruthClassifierLib DerivationFrameworkInterfaces ${extra_libs})

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

atlas_add_test(thinTRTStandaloneConfig_test
               SCRIPT python -m ThinningUtils.ThinTRTStandaloneConfig
               POST_EXEC_SCRIPT nopost.sh)