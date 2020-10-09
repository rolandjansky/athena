# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( CaloBadChannelTool )

# Component(s) in the package:
atlas_add_component( CaloBadChannelTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES CaloConditions LArRecConditions CaloIdentifier AthenaBaseComps GaudiKernel StoreGateLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Tests:
atlas_add_test( CaloBadChanToolConfig_test
                SCRIPT python -m CaloBadChannelTool.CaloBadChanToolConfig
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh)
