# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigCostMonitorMT )

# External dependencies:
find_package( TBB )

# Component(s) in the package:
atlas_add_library( TrigCostMonitorMTLib
                   TrigCostMonitorMT/*.h
                   src/lib/*.cxx
                   PUBLIC_HEADERS TrigCostMonitorMT
                   LINK_LIBRARIES AthenaBaseComps GaudiKernel StoreGateLib TrigCompositeUtilsLib TrigConfData TrigConfHLTUtilsLib TrigDataAccessMonitoringLib xAODTrigger
                   PRIVATE_LINK_LIBRARIES AthViews AthenaKernel )

atlas_add_component( TrigCostMonitorMT
                     src/*.cxx
                     src/components/*.cxx 
                     INCLUDE_DIRS ${TBB_INCLUDE_DIRS}
                     LINK_LIBRARIES ${TBB_LIBRARIES} AthContainers AthViews GaudiKernel TrigConfHLTUtilsLib TrigCostMonitorMTLib TrigSteeringEvent TrigTimeAlgsLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Test(s) in the packageL
atlas_add_test( AlgorithmIdentifier_test
     SOURCES test/AlgorithmIdentifier_test.cxx
     LINK_LIBRARIES TrigCostMonitorMTLib TestTools StoreGateLib AthenaKernel GaudiKernel SGTools )
