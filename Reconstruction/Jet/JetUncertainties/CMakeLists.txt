# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( JetUncertainties )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO Graf Gpad )

# Libraries in the package:
atlas_add_library( JetUncertaintiesLib
   JetUncertainties/*.h Root/*.cxx
   PUBLIC_HEADERS JetUncertainties
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgMessagingLib AsgTools xAODEventInfo xAODJet PATInterfaces
   JetCPInterfaces
   PRIVATE_LINK_LIBRARIES xAODCore xAODTracking PathResolver )

if( NOT XAOD_STANDALONE )
   atlas_add_component( JetUncertainties
      src/components/*.cxx
      LINK_LIBRARIES JetUncertaintiesLib )
endif()

atlas_add_dictionary( JetUncertaintiesDict
   JetUncertainties/JetUncertaintiesDict.h
   JetUncertainties/selection.xml
   LINK_LIBRARIES JetUncertaintiesLib )

# Executable(s) in the package:
atlas_add_executable( MakeCorrelationMatrixPlots
   util/MakeCorrelationMatrixPlots.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} JetUncertaintiesLib )

if( XAOD_STANDALONE )
   atlas_add_executable( MakeUncertaintyPlots
      util/MakeUncertaintyPlots.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODJet xAODEventInfo
      xAODRootAccess JetUncertaintiesLib )

   atlas_add_executable( JetUncertainties_testTool
      util/testTool.cxx
      LINK_LIBRARIES xAODJet xAODEventInfo PATInterfaces xAODRootAccess
      JetUncertaintiesLib )
endif()

# Install files from the package:
atlas_install_generic( share/*
   DESTINATION share PKGNAME_SUBDIR )
