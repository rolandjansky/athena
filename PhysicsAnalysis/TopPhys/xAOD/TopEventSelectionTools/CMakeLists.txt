# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the name of this package:
atlas_subdir( TopEventSelectionTools )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( TopEventSelectionTools _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( TopEventSelectionTools Root/*.cxx Root/*.h Root/*.icc
                   TopEventSelectionTools/*.h TopEventSelectionTools/*.icc TopEventSelectionTools/*/*.h
                   TopEventSelectionTools/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS TopEventSelectionTools
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES}
                                  AsgMessagingLib
                                  AsgTools
                                  FTagAnalysisInterfacesLib
                                  JetInterface JetSelectorToolsLib
                                  MuonAnalysisInterfacesLib
                                  PMGAnalysisInterfacesLib
                                  PMGToolsLib
                                  TopCorrections
                                  TopEvent
                                  TopParticleLevel
                                  TriggerAnalysisInterfaces
                   PRIVATE_LINK_LIBRARIES EventLoop
                                          PATInterfaces
                                          TopConfiguration
                                          xAODBTagging
                                          xAODBase
                                          xAODEgamma
                                          xAODEventInfo
                                          xAODJet
                                          xAODMuon )

