# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrkSpacePointAnalysis )

# Component(s) in the package:
atlas_add_component( TrkSpacePointAnalysis
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel Identifier InDetIdentifier InDetReadoutGeometry SiSpacePoint StoreGateLib TrkSpacePoint)

atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
