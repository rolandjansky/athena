################################################################################
# Package: MissingETGoodness
################################################################################

# Declare the package name:
atlas_subdir( MissingETGoodness )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloEvent
                          Control/AthenaBaseComps
                          Control/CxxUtils
                          Control/StoreGate
                          GaudiKernel
                          Reconstruction/Jet/JetEvent
                          Reconstruction/Jet/JetUtils
                          Reconstruction/MissingETEvent
                          Reconstruction/MuonIdentification/muonEvent
                          Reconstruction/Particle
                          PRIVATE
                          Control/AthenaKernel
                          DataQuality/GoodRunsLists
                          Event/EventBookkeeperMetaData
                          Event/FourMomUtils
                          Event/xAOD/xAODEventInfo
                          LArCalorimeter/LArRecEvent
                          PhysicsAnalysis/PrimaryDPDMaker
                          TileCalorimeter/TileEvent )

# External dependencies:
find_package( CLHEP )
find_package( ROOT COMPONENTS Cint Core Tree MathCore Hist RIO pthread )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_root_dictionary( MissingETGoodnessLib
                           MissingETGoodnessLibDictSource
                           ROOT_HEADERS MissingETGoodness/TRunRange.h MissingETGoodness/Goodies.h MissingETGoodness/TEasyFormula.h MissingETGoodness/TSelection.h MissingETGoodness/TSelectionSetArchive.h MissingETGoodness/TSelectionSet.h MissingETGoodness/EtmissGoodnessQuality.h MissingETGoodness/NtupleGoodiesFiller.h MissingETGoodness/EtmissGoodnessManager.h MissingETGoodness/EtmissGoodness_Loose_20091117.h MissingETGoodness/EtmissGoodness_Medium_20091117.h MissingETGoodness/EtmissGoodness_Tight_20091117.h MissingETGoodness/EtmissGoodness_Loose_20091217.h MissingETGoodness/EtmissGoodness_Loose_20100115.h MissingETGoodness/EtmissGoodness_Loose_20100217.h MissingETGoodness/EtmissGoodness_Loose_20100408.h MissingETGoodness/EtmissGoodness_Loose_20100415.h MissingETGoodness/EtmissGoodness.h MissingETGoodness/EtmissGoodnessConfig.h Root/LinkDef.h
                           EXTERNAL_PACKAGES  ROOT CLHEP )

atlas_add_library( MissingETGoodnessLib
                   Root/*.cxx
                   ${MissingETGoodnessLibDictSource}
                   PUBLIC_HEADERS MissingETGoodness
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   PRIVATE_DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} CaloEvent AthenaBaseComps CxxUtils GaudiKernel JetEvent JetUtils MissingETEvent muonEvent Particle StoreGateLib SGtests AthenaKernel GoodRunsListsLib PrimaryDPDMakerLib
                   PRIVATE_LINK_LIBRARIES ${CLHEP_LIBRARIES} EventBookkeeperMetaData FourMomUtils xAODEventInfo LArRecEvent TileEvent )

atlas_add_component( MissingETGoodness
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${CLHEP_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} ${CLHEP_LIBRARIES} CaloEvent AthenaBaseComps CxxUtils StoreGateLib SGtests GaudiKernel JetEvent JetUtils MissingETEvent muonEvent Particle AthenaKernel GoodRunsListsLib EventBookkeeperMetaData FourMomUtils xAODEventInfo LArRecEvent PrimaryDPDMakerLib TileEvent MissingETGoodnessLib )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_scripts( scripts/make* )

