# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetTrackSelectionTool )

# External(s):
find_package( ROOT COMPONENTS Core RIO Hist MathCore )

# Libraries in the package:
set( extra_libs )
if( NOT XAOD_ANALYSIS )
   set( extra_libs TrkParameters GaudiKernel TrkToolInterfaces TrkExInterfaces
      PRIVATE_LINK_LIBRARIES TrkTrack TrkEventPrimitives TrkRIO_OnTrack
      InDetRIO_OnTrack TrkTrackSummary VxVertex )
endif()

atlas_add_library( InDetTrackSelectionToolLib
   InDetTrackSelectionTool/*.h Root/*.h Root/*.cxx
   PUBLIC_HEADERS InDetTrackSelectionTool
   LINK_LIBRARIES PATCoreLib AsgAnalysisInterfaces xAODTracking ${extra_libs} )

if( NOT XAOD_STANDALONE )
   set( extra_libs )
   if( NOT XAOD_ANALYSIS )
      set( extra_libs TrkTrack TrkTrackSummary )
   endif()
   atlas_add_component( InDetTrackSelectionTool
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES GaudiKernel AthenaBaseComps xAODTracking
      InDetTrackSelectionToolLib ${extra_libs} )
endif()

atlas_add_dictionary( InDetTrackSelectionToolDict
   InDetTrackSelectionTool/InDetTrackSelectionToolDict.h
   InDetTrackSelectionTool/selection.xml
   LINK_LIBRARIES InDetTrackSelectionToolLib )

# Executable(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_executable( InDetTrackSelectionToolTester
      util/InDetTrackSelectionToolTester.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess
      InDetTrackSelectionToolLib )
endif()

# Test(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_test( ut_InDetTrackSelectionTool
      SOURCES test/ut_InDetTrackSelectionTool.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess
      InDetTrackSelectionToolLib
      POST_EXEC_SCRIPT nopost.sh
      PROPERTIES TIMEOUT 600 )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )
