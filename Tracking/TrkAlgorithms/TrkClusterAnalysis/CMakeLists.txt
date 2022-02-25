# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkClusterAnalysis )

# Component(s) in the package:
atlas_add_component( TrkClusterAnalysis
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel Identifier InDetIdentifier InDetPrepRawData InDetReadoutGeometry StoreGateLib)

atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
