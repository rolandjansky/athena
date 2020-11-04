# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetSecVtxTrackSelectionTool )

# Libraries in the package:
set( extra_libs )
if( NOT XAOD_ANALYSIS )
   set( extra_libs InDetTrackSelectionToolLib TrkParameters GaudiKernel TrkToolInterfaces
      PRIVATE_LINK_LIBRARIES TrkTrack TrkEventPrimitives TrkTrackSummary VxVertex )
endif()

atlas_add_library( InDetSecVtxTrackSelectionToolLib
   InDetSecVtxTrackSelectionTool/*.h Root/*.h Root/*.cxx
   PUBLIC_HEADERS InDetSecVtxTrackSelectionTool
   LINK_LIBRARIES AsgMessagingLib AsgTools xAODTracking ${extra_libs} )

if( NOT XAOD_STANDALONE )
   set( extra_libs )
   if( NOT XAOD_ANALYSIS )
      set( extra_libs TrkTrack TrkTrackSummary )
   endif()
   atlas_add_component( InDetSecVtxTrackSelectionTool
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES GaudiKernel AthenaBaseComps xAODTracking
      InDetSecVtxTrackSelectionToolLib ${extra_libs} )
endif()

atlas_add_dictionary( InDetSecVtxTrackSelectionToolDict
   InDetSecVtxTrackSelectionTool/InDetSecVtxTrackSelectionToolDict.h
   InDetSecVtxTrackSelectionTool/selection.xml
   LINK_LIBRARIES InDetSecVtxTrackSelectionToolLib )

# Install files from the package:
atlas_install_joboptions( share/*.py )
