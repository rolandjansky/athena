# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( PixelCabling )

# Component(s) in the package:
atlas_add_library( PixelCablingLib
   PixelCabling/*.h
   INTERFACE
   PUBLIC_HEADERS PixelCabling
   LINK_LIBRARIES GaudiKernel Identifier InDetIdentifier )

atlas_add_component( PixelCabling
   src/*.h src/*.cxx src/components/*.cxx
   LINK_LIBRARIES AthenaBaseComps GaudiKernel InDetReadoutGeometry PixelCablingLib PixelReadoutGeometry StoreGateLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )

# Test(s) in the package:
atlas_add_test( PixelCablingConfigNew_test
                SCRIPT test/PixelCablingConfigNew_test.py
                PROPERTIES TIMEOUT 300 )
