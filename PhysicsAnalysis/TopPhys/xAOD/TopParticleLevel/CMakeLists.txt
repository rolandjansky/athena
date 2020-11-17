# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the name of this package:
atlas_subdir( TopParticleLevel )

# Need fast jet for the RC jet substructure code
find_package( FastJet COMPONENTS fastjetplugins fastjettools )
find_package( FastJetContrib COMPONENTS EnergyCorrelator Nsubjettiness )

# Build a library that other components can link against:
atlas_add_library( TopParticleLevel Root/*.cxx Root/*.h Root/*.icc
                   TopParticleLevel/*.h TopParticleLevel/*.icc TopParticleLevel/*/*.h
                   TopParticleLevel/*/*.icc 
                   PUBLIC_HEADERS TopParticleLevel
                   PRIVATE_INCLUDE_DIRS ${FASTJET_INCLUDE_DIRS} ${FASTJETCONTRIB_INCLUDE_DIRS}
                   LINK_LIBRARIES AsgMessagingLib
                                  AsgTools
                                  JetReclusteringLib
                                  MCTruthClassifierLib
                                  TopDataPreparation
                                  TopEvent
                                  xAODBase
                                  xAODCore
                                  xAODEventInfo
                                  xAODJet
                                  xAODMissingET
                                  xAODMuon
                                  xAODRootAccess
                                  xAODTruth
                   PRIVATE_LINK_LIBRARIES ${FASTJET_LIBRARIES}
                                          ${FASTJETCONTRIB_LIBRARIES}
                                          AthContainers
                                          FourMomUtils
                                          JetSubStructureUtils
                                          TopConfiguration
                                          TruthUtils )
