# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the name of this package:
atlas_subdir( TopCorrections )

# External dependencies:
find_package( Lhapdf )
find_package( ROOT REQUIRED COMPONENTS Core )

# Add a ROOT dictionary
atlas_add_root_dictionary( TopCorrections _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( TopCorrections Root/*.cxx Root/*.h Root/*.icc
                   TopCorrections/*.h TopCorrections/*.icc TopCorrections/*/*.h
                   TopCorrections/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS TopCorrections
                   INCLUDE_DIRS ${LHAPDF_INCLUDE_DIRS}
                   LINK_LIBRARIES ${LHAPDF_LIBRARIES}
                                  AsgAnalysisInterfaces
                                  AsgMessagingLib
                                  AsgTools
                                  AthContainers
                                  EgammaAnalysisInterfacesLib
                                  ElectronEfficiencyCorrectionLib
                                  FTagAnalysisInterfacesLib
                                  JetAnalysisInterfacesLib
                                  MuonAnalysisInterfacesLib
                                  PATInterfaces
                                  PMGAnalysisInterfacesLib
                                  PMGToolsLib
                                  TauAnalysisToolsLib
                                  TopConfiguration
                                  TopEvent
                                  TriggerAnalysisInterfaces
                                  xAODEgamma
                                  xAODMuon
                                  xAODTau
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES}
                                          CalibrationDataInterfaceLib
                                          PathResolver
                                          xAODEventInfo
                                          xAODJet
                                          xAODRootAccess
                                          xAODTruth )

# Install data files from the package:
atlas_install_data( share/* )

