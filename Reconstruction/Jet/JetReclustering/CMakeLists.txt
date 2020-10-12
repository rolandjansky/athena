# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# The name of the package:
atlas_subdir( JetReclustering )

# Libraries in the package:
if( XAOD_STANDALONE )
   atlas_add_root_dictionary( JetReclusteringLib JetReclusteringCintDict
      ROOT_HEADERS JetReclustering/JetReclusteringAlgo.h Root/LinkDef.h
      EXTERNAL_PACKAGES ROOT )

   atlas_add_library( JetReclusteringLib
      JetReclustering/*.h Root/*.cxx ${JetReclusteringCintDict}
      PUBLIC_HEADERS JetReclustering
      LINK_LIBRARIES AsgTools JetInterface JetRecLib xAODRootAccess EventLoop
      PRIVATE_LINK_LIBRARIES xAODJet xAODEventInfo
      JetSubStructureMomentToolsLib )
else()
   atlas_add_library( JetReclusteringLib
      JetReclustering/*.h Root/*.cxx
      PUBLIC_HEADERS JetReclustering
      LINK_LIBRARIES AsgTools JetInterface JetRecLib
      PRIVATE_LINK_LIBRARIES xAODJet xAODRootAccess )

   atlas_add_component( JetReclustering
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES AthenaBaseComps GaudiKernel JetInterface
      JetReclusteringLib )
endif()

atlas_add_dictionary( JetReclusteringDict
   JetReclustering/JetReclusteringDict.h
   JetReclustering/selection.xml
   LINK_LIBRARIES JetReclusteringLib )

# Test(s) in the package:
if( NOT XAOD_STANDALONE )
   atlas_add_test( ut_JetReclusteringTool_test
      SOURCES test/ut_JetReclusteringTool_test.cxx
      LINK_LIBRARIES xAODRootAccess AsgTools JetInterface xAODJet
      POOLRootAccessLib )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )
