# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AFP_DBTools )

# External dependencies:
find_package( nlohmann_json )

# Component(s) in the package:
atlas_add_library( AFP_DBToolsLib
                   src/*.cxx
                   PUBLIC_HEADERS AFP_DBTools
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel AthenaPoolUtilities CaloConditions GaudiKernel StoreGateLib nlohmann_json::nlohmann_json)
                   
atlas_add_component( AFP_DBTools
                     src/components/*.cxx AFP_DBTools/*.h
                     LINK_LIBRARIES AFP_DBToolsLib)

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Test(s) in the package:
atlas_add_test( flake8_scripts
                SCRIPT ${ATLAS_FLAKE8} ${CMAKE_CURRENT_SOURCE_DIR}/scripts
                PROPERTIES TIMEOUT 300
                POST_EXEC_SCRIPT nopost.sh )
