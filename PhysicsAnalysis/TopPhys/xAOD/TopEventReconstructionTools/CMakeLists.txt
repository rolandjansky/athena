# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the name of this package:
atlas_subdir( TopEventReconstructionTools )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO Math MathMore MathCore Graf Minuit Minuit2 )

# Find KLFitter package. As of May '18, included via atlasexternals,
# source code pulled from https://www.github.com/KLFitter/KLFitter.
find_package( KLFitter )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( TopEventReconstructionTools _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( TopEventReconstructionTools Root/*.cxx Root/*.h Root/*.icc
                   TopEventReconstructionTools/*.h TopEventReconstructionTools/*.icc TopEventReconstructionTools/*/*.h
                   TopEventReconstructionTools/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS TopEventReconstructionTools
                   INCLUDE_DIRS  ${KLFITTER_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${KLFITTER_LIBRARIES}
                                  ${ROOT_LIBRARIES}
                                  AsgTools
                                  FTagAnalysisInterfacesLib
                                  TopConfiguration
                                  TopEventSelectionTools
                                  TopParticleLevel
                                  xAODJet
                   PRIVATE_LINK_LIBRARIES PathResolver
                                          TopAnalysis
                                          TopEvent
                                          xAODCore
                                          xAODTruth )

# Install data files from the package:
atlas_install_data( share/* )
