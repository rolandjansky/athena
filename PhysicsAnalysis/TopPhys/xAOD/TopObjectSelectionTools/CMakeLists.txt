# Auto-generated on: 2017-03-08 14:47:37.990954

# Declare the name of this package:
atlas_subdir( TopObjectSelectionTools None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          xAODBase
                          xAODPrimitives
                          xAODEventInfo
                          xAODEgamma
                          xAODMuon
                          xAODTau
                          xAODTracking
			  FourMomUtils
                          ElectronPhotonSelectorTools
                          TopEvent
                          TopSystematicObjectMaker
                          AssociationUtils
                          TrigConfInterfaces
                          TrigDecisionTool
                          TriggerMatchingTool
                          TrigTauMatching
                          TriggerAnalysisInterfaces
                          TrigGlobalEfficiencyCorrection
                          GoodRunsLists
                          JetAnalysisInterfaces
                          JetSubStructureMomentTools
                          JetSubStructureUtils
                          JetReclustering
                          BoostedJetTaggers	
			  MuonAnalysisInterfaces
			  FTagAnalysisInterfaces )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf )

# Custom definitions needed for this package:
add_definitions( -g )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( TopObjectSelectionTools _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( TopObjectSelectionTools Root/*.cxx Root/*.h Root/*.icc
                   TopObjectSelectionTools/*.h TopObjectSelectionTools/*.icc TopObjectSelectionTools/*/*.h
                   TopObjectSelectionTools/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS TopObjectSelectionTools
                   LINK_LIBRARIES xAODBase
                                  xAODPrimitives
                                  xAODEventInfo
                                  xAODEgamma
                                  xAODMuon
                                  xAODTau
                                  xAODTracking
				  FourMomUtils
                                  ElectronPhotonSelectorToolsLib
                                  TopEvent
                                  TopSystematicObjectMaker
                                  AssociationUtilsLib
                                  TrigConfInterfaces
                                  TrigDecisionToolLib
                                  TriggerMatchingToolLib
                                  TrigTauMatchingLib
                                  TrigGlobalEfficiencyCorrectionLib
                                  GoodRunsListsLib
                                  JetAnalysisInterfacesLib
                                  JetSubStructureMomentToolsLib
                                  JetSubStructureUtils
                                  JetReclusteringLib
                                  BoostedJetTaggersLib
				  MuonAnalysisInterfacesLib
				  FTagAnalysisInterfacesLib
                                  ${ROOT_LIBRARIES}		  
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

# Install data files from the package:
atlas_install_data( share/* )

