# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODPrimitives )

# Component(s) in the package.
atlas_add_library( xAODPrimitives
   xAODPrimitives/*.h xAODPrimitives/tools/*.h Root/*.cxx
   PUBLIC_HEADERS xAODPrimitives
   LINK_LIBRARIES AthContainers )

atlas_add_dictionary( xAODPrimitivesDict
   xAODPrimitives/xAODPrimitivesDict.h
   xAODPrimitives/selection.xml
   LINK_LIBRARIES xAODPrimitives )

# Test(s) in the package.
atlas_add_test( ut_xAOD_primitives_enums_test
   SOURCES test/ut_xAOD_primitives_enums_test.cxx
   LINK_LIBRARIES xAODPrimitives )

# Install files from the package.
atlas_install_python_modules( python/*.py )
