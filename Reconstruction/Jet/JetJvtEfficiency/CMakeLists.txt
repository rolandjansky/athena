# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# The name of the package:
atlas_subdir( JetJvtEfficiency )

# External(s):
find_package( ROOT COMPONENTS Core Hist RIO )

# Libraries of the package:
atlas_add_library( JetJvtEfficiencyLib
   JetJvtEfficiency/*.h Root/*.cxx
   PUBLIC_HEADERS JetJvtEfficiency
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODJet xAODEventInfo JetAnalysisInterfacesLib PATInterfaces
   PRIVATE_LINK_LIBRARIES PathResolver )

if( NOT XAOD_STANDALONE )
   atlas_add_component( JetJvtEfficiency
      src/components/*.cxx
      LINK_LIBRARIES JetJvtEfficiencyLib )
endif()

atlas_add_dictionary( JetJvtEfficiencyDict
   JetJvtEfficiency/JetJvtEfficiencyDict.h
   JetJvtEfficiency/selection.xml
   LINK_LIBRARIES JetJvtEfficiencyLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
