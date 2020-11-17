# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( IsolationSelection )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist Tree )

# Libraries in the package:
atlas_add_library( IsolationSelectionLib
  IsolationSelection/*.h Root/*.cxx
  PUBLIC_HEADERS IsolationSelection
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AsgTools xAODBase xAODEgamma xAODMuon
  xAODPrimitives xAODTracking PATCoreLib PATInterfaces
  PRIVATE_LINK_LIBRARIES InDetTrackSelectionToolLib FourMomUtils PathResolver )

if( NOT XAOD_STANDALONE )
   atlas_add_component( IsolationSelection
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES AsgTools AthenaBaseComps xAODBase xAODCore xAODEgamma xAODMuon
      GaudiKernel IsolationSelectionLib )
endif()

atlas_add_dictionary( IsolationSelectionDict
   IsolationSelection/IsolationSelectionDict.h
   IsolationSelection/selection.xml
   LINK_LIBRARIES IsolationSelectionLib )

# Executable(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_executable( testIsolationCloseByCorrectionTool
      util/testIsolationCloseByCorrectionTool.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODBase xAODRootAccess xAODEventInfo
      xAODEgamma xAODMuon xAODCore IsolationSelectionLib )

   atlas_add_executable( testIsolationSelectionTool
      util/testIsolationSelectionTool.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODEgamma xAODMuon
      xAODPrimitives IsolationSelectionLib )
endif()

# Test(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_test( ut_reflex SCRIPT test/ut_reflex.py )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )
