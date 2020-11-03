# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HIMonitoring )

# External dependencies:
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_library( HIMonitoringLib
                   src/*.cxx
                   PUBLIC_HEADERS HIMonitoring
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaMonitoringLib xAODHIEvent xAODTrigMinBias
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ElectronPhotonSelectorToolsLib LWHists PATCoreAcceptLib xAODEgamma xAODForward xAODMuon )

atlas_add_component( HIMonitoring
                     src/components/*.cxx
		             LINK_LIBRARIES HIMonitoringLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )

