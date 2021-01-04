# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( ElectronPhotonShowerShapeFudgeTool )

# External dependencies:
find_package( ROOT COMPONENTS Core MathCore Hist Graf RIO )

# Component(s) in the package:
atlas_add_library( ElectronPhotonShowerShapeFudgeToolLib
   ElectronPhotonShowerShapeFudgeTool/*.h
   ElectronPhotonShowerShapeFudgeTool/*.icc Root/*.cxx
   PUBLIC_HEADERS ElectronPhotonShowerShapeFudgeTool
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools EgammaAnalysisInterfacesLib
   PRIVATE_LINK_LIBRARIES PathResolver xAODEgamma )

if( NOT XAOD_STANDALONE )
   atlas_add_component( ElectronPhotonShowerShapeFudgeTool
      src/components/*.cxx
      LINK_LIBRARIES ElectronPhotonShowerShapeFudgeToolLib )
endif()

atlas_add_dictionary( ElectronPhotonShowerShapeFudgeToolDict
   ElectronPhotonShowerShapeFudgeTool/ElectronPhotonShowerShapeFudgeToolDict.h
   ElectronPhotonShowerShapeFudgeTool/selection.xml
   LINK_LIBRARIES ElectronPhotonShowerShapeFudgeToolLib )

# Install files from the package:
atlas_install_data( data/*.conf data/*.root )
