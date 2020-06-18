# Copyright (C) 20022020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthExStoreGateExample )

# Component(s) in the package:
atlas_add_library( SGTutorialLib
                   Tutorial/LinkObj.cxx
                   Tutorial/SGRead.cxx
                   Tutorial/SGWrite.cxx
                   PUBLIC_HEADERS AthExStoreGateExample
                   LINK_LIBRARIES AthenaBaseComps AthenaKernel GaudiKernel StoreGateLib
                   PRIVATE_LINK_LIBRARIES AthLinks  )

atlas_add_component( SGTutorial
                     Tutorial/SGTutorial_entries.cxx
                     LINK_LIBRARIES SGTutorialLib )

atlas_add_component( AthExStoreGateExample
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthContainers AthLinks AthenaBaseComps EventInfo PileUpToolsLib SGTools SGTutorialLib TestTools )

atlas_add_component( AthExDFlow
                     src_dflow/*.cxx
                     src_dflow/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel SGTools StoreGateLib xAODEventInfo )

atlas_add_dictionary( AthExStoreGateExampleDict
                      AthExStoreGateExample/AthExStoreGateExampleDict.h
                      AthExStoreGateExample/selection.xml
                      LINK_LIBRARIES SGTutorialLib )

# Install files from the package:
atlas_install_joboptions( share/StoreGateExample_Gen_jobOptions.txt share/StoreGateExample_Gen_jobOptions.py share/StoreGateExample_Del_jobOptions.py share/StoreGateExample_Reentrant_jobOptions.py share/dflow_jobo.py share/StoreGateHiveExample.py share/HandleTest_jobOptions.py )

