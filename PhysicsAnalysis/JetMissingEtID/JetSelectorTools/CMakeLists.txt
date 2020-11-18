# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( JetSelectorTools )

# External dependencies:
find_package( ROOT COMPONENTS Core RIO )

# Component(s) in the package:
atlas_add_library( JetSelectorToolsLib
   JetSelectorTools/*.h Root/*.cxx
   PUBLIC_HEADERS JetSelectorTools
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgMessagingLib AsgTools xAODEventInfo xAODJet JetInterface PATCoreAcceptLib )

if( NOT XAOD_STANDALONE )
   atlas_add_component( JetSelectorTools
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES AsgTools AthenaBaseComps GaudiKernel JetSelectorToolsLib xAODBase xAODEventInfo )
endif()

atlas_add_dictionary( JetSelectorToolsDict
   JetSelectorTools/JetSelectorToolsDict.h
   JetSelectorTools/selection.xml
   LINK_LIBRARIES JetSelectorToolsLib )

# Executable(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_executable( TestJetCleaningTool
      util/TestJetCleaningTool.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODJet JetSelectorToolsLib )
endif()

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_data( share/*.config )
