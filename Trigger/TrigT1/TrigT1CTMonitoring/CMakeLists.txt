# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigT1CTMonitoring )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel )
find_package( CORAL COMPONENTS CoralBase )
find_package( ROOT COMPONENTS Core MathCore Hist )

# Component(s) in the package:
atlas_add_component( TrigT1CTMonitoring
   src/*.h src/*.cxx src/components/*.cxx
   INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${COOL_LIBRARIES} ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} AthenaBaseComps AthenaMonitoringLib AthenaPoolUtilities CoraCool xAODEventInfo GaudiKernel LWHists MuonDigitContainer MuonRDO MuonTrigCoinData TrigConfData TrigConfInterfaces TrigConfL1Data TrigT1CaloEventLib TrigT1Interfaces TrigT1Result )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( scripts/*.py )
