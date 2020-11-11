# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the name of this package:
atlas_subdir( TopExamples )

find_package( ROOT REQUIRED COMPONENTS Core Hist RIO Tree )

# Build a library that other components can link against:
atlas_add_library( TopExamples Root/*.cxx Root/*.h Root/*.icc
                   TopExamples/*.h TopExamples/*.icc TopExamples/*/*.h
                   TopExamples/*/*.icc 
                   PUBLIC_HEADERS TopExamples
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AsgTools
                                  TopAnalysis
                                  TopCPTools
                                  TopCorrections
                                  TopEvent
                                  TopEventReconstructionTools
                                  TopEventSelectionTools
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES}
                                          PathResolver
                                          TopConfiguration
                                          TopDataPreparation
                                          xAODBTagging
                                          xAODEgamma
                                          xAODEventInfo
                                          xAODJet
                                          xAODMissingET
                                          xAODMuon
                                          xAODRootAccess
                                          xAODTau
                                          xAODTruth )

# Install the Python modules of the package:
atlas_install_python_modules( python/*.py )

# Install data files from the package:
atlas_install_data( share/* )

# Build the executables of the package:
atlas_add_executable( mini-to-flat
                      util/mini-to-flat.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES}
                                     TopAnalysis
                                     TopConfiguration
                                     TopEvent
                                     TopExamples
                                     xAODCore
                                     xAODRootAccess )

atlas_add_executable( mini-xaod
                      util/mini-xaod.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES}
                                     TopAnalysis
                                     TopEvent
                                     TopExamples
                                     xAODCore
                                     xAODRootAccess )

atlas_add_executable( mini-to-plots
                      util/mini-to-plots.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES}
                                     TopAnalysis
                                     TopEvent
                                     TopExamples
                                     xAODRootAccess )

atlas_add_executable( combineATOutputs
                      util/combineATOutputs.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_install_scripts( scripts/* )
