# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( HIMonitoring )

# External dependencies:
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_component( HIMonitoring
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaMonitoringLib xAODHIEvent xAODTrigMinBias
                     ElectronPhotonSelectorToolsLib LWHists PATCoreAcceptLib xAODEgamma xAODForward xAODMuon )

# Install files from the package:
atlas_install_joboptions( share/*.py )

