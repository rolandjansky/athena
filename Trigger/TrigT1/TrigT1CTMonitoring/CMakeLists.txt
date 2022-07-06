# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1CTMonitoring )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel )
find_package( CORAL COMPONENTS CoralBase )
find_package( ROOT COMPONENTS Core MathCore Hist )

# Component(s) in the package:
atlas_add_component( TrigT1CTMonitoring
   src/BSMonitoringAlg.h src/BSMonitoringAlg.cxx src/components/TrigT1CTMonitoring_entries.cxx
   INCLUDE_DIRS ${COOL_INCLUDE_DIRS}
   LINK_LIBRARIES ${COOL_LIBRARIES} AthenaBaseComps AthenaMonitoringKernelLib AthenaMonitoringLib AthenaPoolUtilities xAODEventInfo GaudiKernel MuonDigitContainer MuonRDO MuonTrigCoinData TrigConfData TrigConfInterfaces TrigConfL1Data TrigT1Interfaces TrigT1Result )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( scripts/*.py )
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
