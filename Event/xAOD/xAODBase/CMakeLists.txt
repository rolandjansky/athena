# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODBase )

# Extra dependencies based on what environment we are in.
set( extra_libs )
if( NOT XAOD_STANDALONE )
   set( extra_libs AthenaKernel )
endif()

# External dependencies.
find_package( ROOT COMPONENTS Core Physics )
find_package( xAODUtilities )

# Component(s) in the package.
atlas_add_library( xAODBase
   xAODBase/*.h Root/*.cxx
   PUBLIC_HEADERS xAODBase
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers ${extra_libs}
   PRIVATE_LINK_LIBRARIES AthLinks )

atlas_add_xaod_smart_pointer_dicts(
   INPUT xAODBase/selection.xml
   OUTPUT _selectionFile
   CONTAINERS "xAOD::IParticleContainer" )

atlas_add_dictionary( xAODBaseDict
   xAODBase/xAODBaseDict.h
   ${_selectionFile}
   LINK_LIBRARIES AthContainers AthLinks xAODBase )

atlas_add_dictionary( xAODBaseObjectTypeDict
   xAODBase/xAODBaseObjectTypeDict.h
   xAODBase/selection-ObjectType.xml )

# Install files from the package.
atlas_install_python_modules( python/*.py )

# Test(s) in the package.
atlas_add_test( ut_xAODObjectType_test
   SOURCES test/ut_xAODObjectType_test.cxx
   LINK_LIBRARIES xAODBase )

atlas_add_test( ut_xAODObjectType_pytest
   SCRIPT test/ut_xAODObjectType_pytest.py )
