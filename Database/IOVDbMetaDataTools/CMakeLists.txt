# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( IOVDbMetaDataTools )

atlas_add_library( IOVDbMetaDataToolsLib
                   INTERFACE
                   PUBLIC_HEADERS IOVDbMetaDataTools
                   LINK_LIBRARIES GaudiKernel )

# Component(s) in the package:
atlas_add_component( IOVDbMetaDataTools
                     src/IOVDbMetaDataTool.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel CxxUtils IOVDbDataModel IOVDbMetaDataToolsLib StoreGateLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
