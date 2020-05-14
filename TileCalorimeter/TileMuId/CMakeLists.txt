# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( TileMuId )

# Component(s) in the package.
atlas_add_component( TileMuId
   TileMuId/*.h src/*.cxx src/components/*.cxx
   LINK_LIBRARIES AthenaBaseComps GaudiKernel CaloEvent CaloIdentifier
   AthenaKernel TileEvent StoreGateLib )

# Install files from the package.
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*jobOptions*.py )

# Test(s) in the package.
atlas_add_test( TileMuIdConfig_test
   SCRIPT python -m TileMuId.TileMuIdConfig
   PROPERTIES TIMEOUT 300
   POST_EXEC_SCRIPT nopost.sh)
