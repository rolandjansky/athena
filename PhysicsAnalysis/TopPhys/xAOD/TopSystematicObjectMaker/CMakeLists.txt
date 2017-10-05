# Auto-generated on: 2017-03-08 14:47:39.125284

# Declare the name of this package:
atlas_subdir( TopSystematicObjectMaker None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          xAODCore
                          xAODRootAccess
                          xAODEventInfo
                          PATInterfaces
                          AsgTools
                          xAODEgamma
                          xAODMuon
                          xAODJet
                          ElectronPhotonFourMomentumCorrection
                          ElectronPhotonSelectorTools
                          ElectronPhotonShowerShapeFudgeTool
                          TauAnalysisTools
                          JetCalibTools
                          JetCPInterfaces
                          JetResolution
                          JetInterface
                          METInterface
                          METUtilities
                          IsolationSelection
                          IsolationCorrections
                          TopConfiguration
                          TopEvent
                          TopJetSubstructure
                          xAODTracking
                          InDetTrackSystematicsTools 
			  MuonAnalysisInterfaces )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf )

# Custom definitions needed for this package:
add_definitions( -g )

# Build a library that other components can link against:
atlas_add_library( TopSystematicObjectMaker Root/*.cxx Root/*.h Root/*.icc
                   TopSystematicObjectMaker/*.h TopSystematicObjectMaker/*.icc TopSystematicObjectMaker/*/*.h
                   TopSystematicObjectMaker/*/*.icc 
                   PUBLIC_HEADERS TopSystematicObjectMaker
                   LINK_LIBRARIES xAODCore
                                  xAODRootAccess
                                  xAODEventInfo
                                  PATInterfaces
                                  AsgTools
                                  xAODEgamma
                                  xAODMuon
                                  xAODJet
                                  ElectronPhotonFourMomentumCorrectionLib
                                  ElectronPhotonSelectorToolsLib
                                  ElectronPhotonShowerShapeFudgeToolLib
                                  TauAnalysisToolsLib
                                  JetCalibToolsLib
                                  JetCPInterfaces
                                  JetResolutionLib
                                  JetInterface
                                  METInterface
                                  METUtilitiesLib
                                  IsolationSelectionLib
                                  IsolationCorrectionsLib
                                  TopConfiguration
                                  TopEvent
                                  TopJetSubstructure
                                  xAODTracking
                                  InDetTrackSystematicsToolsLib
				  MuonAnalysisInterfacesLib
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

# Install data files from the package:
atlas_install_data( share/* )

