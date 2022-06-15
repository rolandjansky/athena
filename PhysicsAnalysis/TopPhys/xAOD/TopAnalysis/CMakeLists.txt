# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the name of this package:
atlas_subdir( TopAnalysis )

# External dependencies:
find_package( Boost COMPONENTS regex )
find_package( ROOT REQUIRED COMPONENTS Core )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( TopAnalysis _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( TopAnalysis Root/*.cxx Root/*.h Root/*.icc
                   TopAnalysis/*.h TopAnalysis/*.icc TopAnalysis/*/*.h
                   TopAnalysis/*/*.icc ${_cintDictSource}
                   PUBLIC_HEADERS TopAnalysis
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES}
                                  AnaAlgorithmLib
                                  AsgMessagingLib
                                  AsgTools
                                  AthContainers
                                  TopConfiguration
                                  TopCorrections
                                  TopEventSelectionTools
                                  xAODCutFlow
                                  xAODMetaData
                                  xAODRootAccess
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES}
                                          EventLoop
                                          PATInterfaces
                                          TopCPTools
                                          TopEvent
                                          TopObjectSelectionTools
                                          TopParticleLevel
                                          TopPartons
                                          TopDataPreparation
                                          FakeBkgToolsLib
                                          xAODBTagging
                                          xAODCore
                                          xAODEgamma
                                          xAODJet
                                          xAODMissingET
                                          xAODMuon
                                          xAODTau
                                          xAODTracking
                                          xAODTruth )

atlas_add_dictionary( TopAnalysisDict
                      TopAnalysis/TopAnalysisDict.h
                      TopAnalysis/selection.xml
                      LINK_LIBRARIES TopAnalysis )

# Install data files from the package:
atlas_install_data( share/* )

# Install user scripts
atlas_install_scripts( scripts/* )

# Build the executables of the package:
atlas_add_executable( top-xaod
                      util/top-xaod.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES}
                                     PathResolver
                                     TopAnalysis
                                     TopCPTools
                                     TopConfiguration
                                     TopCorrections
                                     TopDataPreparation
                                     TopEvent
                                     TopObjectSelectionTools
                                     TopParticleLevel
                                     TopPartons
                                     TopSystematicObjectMaker
                                     FakeBkgToolsLib
                                     xAODCore
                                     xAODCutFlow
                                     xAODRootAccess )


atlas_add_executable( top-el
                      util/top-el.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES}
                                     EventLoop
                                     PathResolver
                                     TopAnalysis
                                     TopCPTools
                                     TopConfiguration
                                     TopCorrections
                                     TopDataPreparation
                                     TopEvent
                                     TopObjectSelectionTools
                                     TopParticleLevel
                                     TopPartons
                                     TopSystematicObjectMaker
                                     xAODCore
                                     xAODCutFlow
                                     xAODRootAccess )

atlas_add_executable( top-tool-ftag
                      util/top-tool-ftag.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES}
                                     TopAnalysis
                                     TopCPTools
                                     TopConfiguration
                                     TopCorrections
                                     TopEvent
                                     TopObjectSelectionTools
                                     xAODRootAccess )
