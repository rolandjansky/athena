# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( IsolationSelection )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist Tree TMVA )

set(extra_libs)
if ( NOT XAOD_STANDALONE AND NOT XAOD_ANALYSIS )
  set(extra_libs TrkExUtils)
endif()

# Libraries in the package:
atlas_add_library( IsolationSelectionLib
  IsolationSelection/*.h Root/*.cxx
  PUBLIC_HEADERS IsolationSelection
  PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES AthContainers AsgTools xAODBase xAODEgamma xAODMuon xAODEventInfo ${extra_libs}
  xAODPrimitives PATCoreLib PATInterfaces InDetTrackSelectionToolLib xAODTracking
  PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} PathResolver FourMomUtils TrackVertexAssociationToolLib InDetTrackSelectionToolLib )

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
if( XAOD_STANDALONE OR XAOD_ANALYSIS )
   atlas_add_executable( testIsolationCloseByCorrectionTool
      util/testIsolationCloseByCorrectionTool.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODBase xAODRootAccess xAODEventInfo
      xAODEgamma xAODMuon xAODCore IsolationSelectionLib )

   atlas_add_executable( testIsolationSelectionTool
      util/testIsolationSelectionTool.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODEgamma xAODMuon
      xAODPrimitives IsolationSelectionLib xAODRootAccess)
endif()

# Test(s) in the package:
if( XAOD_STANDALONE OR XAOD_ANALYSIS )
   atlas_add_test( ut_reflex SCRIPT test/ut_reflex.py )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )
