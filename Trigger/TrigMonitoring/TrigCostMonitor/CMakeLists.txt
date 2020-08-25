# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigCostMonitor )

# External dependencies:
find_package( COOL COMPONENTS CoolKernel CoolApplication )
find_package( CORAL COMPONENTS CoralBase CoralKernel )
find_package( ROOT COMPONENTS Core Tree XMLIO XMLParser )
find_package( tdaq-common COMPONENTS CTPfragment )

# Component(s) in the package:
atlas_add_library( TrigCostMonitorLib
                   src/*.cxx
                   PUBLIC_HEADERS TrigCostMonitor
                   INCLUDE_DIRS ${COOL_INCLUDE_DIRS} ${TDAQ-COMMON_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES ${COOL_LIBRARIES} AthenaBaseComps AthenaMonitoringLib CoolLumiUtilitiesLib GaudiKernel StoreGateLib TrigConfInterfaces TrigConfJobOptData TrigDecisionToolLib TrigMonitoringEvent TrigSteeringLib TrigTimeAlgsLib xAODEventInfo
                   PRIVATE_LINK_LIBRARIES ${CORAL_LIBRARIES} ${ROOT_LIBRARIES} ${TDAQ-COMMON_LIBRARIES} AthenaKernel GeneratorObjects PathResolver TrigConfHLTData TrigConfL1Data TrigConfStorage TrigDataAccessMonitoringLib TrigInterfacesLib TrigNavigationLib TrigSerializeResultLib TrigSteeringEvent TrigT1Interfaces TrigT1Result TruthHelper )

atlas_add_component( TrigCostMonitor
                     src/components/*.cxx
                     LINK_LIBRARIES TrigCostMonitorLib )

atlas_add_dictionary( TrigCostMonitorDict
                      TrigCostMonitor/TrigCostMonitorDict.h
                      TrigCostMonitor/selection.xml
                      LINK_LIBRARIES TrigCostMonitorLib )

# Install files from the package:
atlas_install_python_modules( python/TrigCost*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

