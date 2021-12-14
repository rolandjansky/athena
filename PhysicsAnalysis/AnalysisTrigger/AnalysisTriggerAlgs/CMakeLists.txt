# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AnalysisTriggerAlgs )

# Component(s) in the package:
atlas_add_component( AnalysisTriggerAlgs
   src/*.h src/*.cxx src/components/*.cxx
   LINK_LIBRARIES AthenaBaseComps AthenaKernel StoreGateLib
   xAODTrigL1Calo xAODTrigger GaudiKernel
   TrigConfData TrigT1CaloEventLib TrigT1CaloToolInterfaces TrigT1Interfaces
   TrigT1Result TrigT1CaloUtilsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

atlas_add_test( RoIBResultToxAODCATest
   SCRIPT python -m AnalysisTriggerAlgs.AnalysisTriggerAlgsCAConfig
   POST_EXEC_SCRIPT nopost.sh )


