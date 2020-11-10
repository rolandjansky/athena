# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( JetCalibTools )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO Graf Gpad )

# Libraries in the package:
atlas_add_library( JetCalibToolsLib
   JetCalibTools/*.h Root/*.cxx
   PUBLIC_HEADERS JetCalibTools
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODEventShape
   xAODJet xAODTracking PATInterfaces JetInterface AsgDataHandlesLib
   PRIVATE_LINK_LIBRARIES xAODMuon PathResolver )

if( NOT XAOD_STANDALONE )
   atlas_add_component( JetCalibTools
      src/components/*.cxx
      LINK_LIBRARIES JetCalibToolsLib )
endif()

atlas_add_dictionary( JetCalibToolsDict
   JetCalibTools/JetCalibToolsDict.h
   JetCalibTools/selection.xml
   LINK_LIBRARIES JetCalibToolsLib )

# Executable(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_executable( JetCalibTools_Example
      util/JetCalibTools_Example.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODJet
      xAODEventShape xAODCore xAODRootAccess JetCalibToolsLib )
   
   atlas_add_executable( JetCalibTools_PlotJESFactors
      util/JetCalibTools_PlotJESFactors.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODJet
      xAODCore xAODRootAccess JetCalibToolsLib )
   
   atlas_add_executable( JetCalibTools_PlotJMSFactors
      util/JetCalibTools_PlotJMSFactors.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODJet
      xAODCore xAODRootAccess JetCalibToolsLib )

   atlas_add_executable( JetCalibTools_SmearingPlots
      util/JetCalibTools_SmearingPlots.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODJet
      xAODCore xAODRootAccess JetCalibToolsLib )
   
else()
   atlas_add_executable( JetCalibTools_Example
      util/JetCalibTools_Example.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODJet
      xAODEventShape xAODCore POOLRootAccessLib xAODRootAccess JetCalibToolsLib )

   atlas_add_executable( JetCalibTools_PlotJESFactors
      util/JetCalibTools_PlotJESFactors.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODJet
      xAODCore xAODRootAccess POOLRootAccessLib JetCalibToolsLib )

   atlas_add_executable( JetCalibTools_PlotJMSFactors
      util/JetCalibTools_PlotJMSFactors.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODJet
      xAODCore xAODRootAccess POOLRootAccessLib JetCalibToolsLib )

   atlas_add_executable( JetCalibTools_SmearingPlots
      util/JetCalibTools_SmearingPlots.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEventInfo xAODJet
      xAODCore xAODRootAccess POOLRootAccessLib JetCalibToolsLib )

endif()

atlas_install_python_modules( python/*.py )
