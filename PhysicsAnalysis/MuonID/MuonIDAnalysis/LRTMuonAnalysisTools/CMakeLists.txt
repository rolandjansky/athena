# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LRTMuonAnalysisTools )

# Libraries in the package:
atlas_add_library( LRTMuonAnalysisToolsLib
   LRTMuonAnalysisTools/*.h Root/*.cxx
   PUBLIC_HEADERS LRTMuonAnalysisTools
   LINK_LIBRARIES AsgTools MuonAnalysisInterfacesLib xAODMuon xAODTracking )

if( NOT XAOD_STANDALONE )
   atlas_add_component( LRTMuonAnalysisTools
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES LRTMuonAnalysisToolsLib )
endif()

atlas_add_dictionary( LRTMuonAnalysisToolsDict
   LRTMuonAnalysisTools/LRTMuonAnalysisToolsDict.h
   LRTMuonAnalysisTools/selection.xml
   LINK_LIBRARIES LRTMuonAnalysisToolsLib )
