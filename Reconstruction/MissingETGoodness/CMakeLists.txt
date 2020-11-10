# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( MissingETGoodness )

# External dependencies:
find_package( ROOT COMPONENTS Cint Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_root_dictionary( MissingETGoodnessLib
                           MissingETGoodnessLibDictSource
                           ROOT_HEADERS MissingETGoodness/TRunRange.h MissingETGoodness/Goodies.h MissingETGoodness/TEasyFormula.h MissingETGoodness/TSelection.h MissingETGoodness/TSelectionSetArchive.h MissingETGoodness/TSelectionSet.h MissingETGoodness/EtmissGoodnessQuality.h MissingETGoodness/NtupleGoodiesFiller.h MissingETGoodness/EtmissGoodnessManager.h MissingETGoodness/EtmissGoodness_Loose_20091117.h MissingETGoodness/EtmissGoodness_Medium_20091117.h MissingETGoodness/EtmissGoodness_Tight_20091117.h MissingETGoodness/EtmissGoodness_Loose_20091217.h MissingETGoodness/EtmissGoodness_Loose_20100115.h MissingETGoodness/EtmissGoodness_Loose_20100217.h MissingETGoodness/EtmissGoodness_Loose_20100408.h MissingETGoodness/EtmissGoodness_Loose_20100415.h MissingETGoodness/EtmissGoodness.h MissingETGoodness/EtmissGoodnessConfig.h Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

atlas_add_library( MissingETGoodnessLib
                   Root/*.cxx
                   ${MissingETGoodnessLibDictSource}
                   PUBLIC_HEADERS MissingETGoodness
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} CaloEvent AthenaBaseComps CxxUtils GaudiKernel JetEvent JetUtils MissingETEvent muonEvent Particle StoreGateLib )

atlas_add_component( MissingETGoodness
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} CaloEvent GaudiKernel JetEvent JetUtils MissingETEvent muonEvent Particle AthenaKernel GoodRunsListsLib EventBookkeeperMetaData FourMomUtils xAODEventInfo LArRecEvent PrimaryDPDMakerLib TileEvent MissingETGoodnessLib )

# Install files from the package:
atlas_install_python_modules( python/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( scripts/make* )

