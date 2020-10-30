# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( JetMonitoring )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

atlas_add_library( JetMonitoringLib
                   JetMonitoring/*.h
                   INTERFACE
                   PUBLIC_HEADERS JetMonitoring
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools AthenaBaseComps AthenaMonitoringKernelLib AthenaMonitoringLib GaudiKernel JetInterface StoreGateLib xAODEventInfo xAODHIEvent xAODJet )

# Component(s) in the package:
atlas_add_component( JetMonitoring
                     src/*.cxx
                     Root/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools AthContainers AthenaBaseComps AthenaMonitoringKernelLib AthenaMonitoringLib FourMomUtils GaudiKernel JetInterface JetMonitoringLib JetUtils StoreGateLib TrigDecisionInterface xAODEventInfo xAODHIEvent xAODJet )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

