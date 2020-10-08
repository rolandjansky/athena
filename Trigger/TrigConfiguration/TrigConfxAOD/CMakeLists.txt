# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigConfxAOD )

# Libraries in the package:
atlas_add_library( TrigConfxAODLib
   TrigConfxAOD/*.h Root/*.cxx
   PUBLIC_HEADERS TrigConfxAOD
   LINK_LIBRARIES AsgTools xAODTrigger TrigConfL1Data TrigConfHLTData
   TrigConfInterfaces )

if( NOT XAOD_STANDALONE )
   atlas_add_component( TrigConfxAOD
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES AthenaBaseComps AthenaKernel StoreGateLib GaudiKernel TrigConfData
      TrigConfxAODLib )
endif()

atlas_add_dictionary( TrigConfxAODDict
   TrigConfxAOD/TrigConfxAODDict.h
   TrigConfxAOD/selection.xml
   LINK_LIBRARIES TrigConfxAODLib )

# Executable(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_executable( xAODTrigConfReadTest
      util/xAODTrigConfReadTest.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess AsgTools TrigConfxAODLib )
endif()

# Test(s) in the package:
if( XAOD_STANDALONE )
   # FIX ME: temporarily disabled as it is failing
   # atlas_add_test( xaodconfigtool_test
   #    SCRIPT test/ut_trigconfxaod_xaodconfigtool_test.py )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( scripts/*.py )
