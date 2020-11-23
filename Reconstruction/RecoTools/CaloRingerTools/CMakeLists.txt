# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloRingerTools )

atlas_add_library( CaloRingerToolsLib
                   CaloRingerTools/*.h
                   INTERFACE
                   PUBLIC_HEADERS CaloRingerTools
                   LINK_LIBRARIES GaudiKernel AthLinks StoreGateLib xAODBase xAODCaloRings xAODCaloEvent )

# Component(s) in the package:
atlas_add_component( CaloRingerTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthContainers AthLinks AthenaBaseComps AthenaKernel CaloEvent CaloGeoHelpers CaloRingerToolsLib CaloUtilsLib GaudiKernel PATCoreLib RingerSelectorToolsEnumsLib StoreGateLib xAODBase xAODCaloEvent xAODCaloRings xAODEgamma )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
