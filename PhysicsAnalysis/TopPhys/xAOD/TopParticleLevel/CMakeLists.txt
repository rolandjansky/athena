# Declare the name of this package:
atlas_subdir( TopParticleLevel )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          xAODBase
                          xAODTruth
                          xAODJet
                          xAODMissingET
                          xAODCore
                          xAODRootAccess
                          FourMomUtils
                          TopEvent
                          TopConfiguration
                          TopDataPreparation
                          TruthUtils
                          MCTruthClassifier
                          JetReclustering
                          JetSubStructureUtils
                          AsgTools

			  )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf )

# Need fast jet for the RC jet substructure code
find_package( FastJet COMPONENTS fastjetplugins fastjettools )
find_package( FastJetContrib COMPONENTS EnergyCorrelator Nsubjettiness )

# Build a library that other components can link against:
atlas_add_library( TopParticleLevel Root/*.cxx Root/*.h Root/*.icc
                   TopParticleLevel/*.h TopParticleLevel/*.icc TopParticleLevel/*/*.h
                   TopParticleLevel/*/*.icc 
                   PUBLIC_HEADERS TopParticleLevel
                   LINK_LIBRARIES 
                                  xAODBase
                                  xAODTruth
                                  xAODJet
                                  xAODMissingET
                                  xAODCore
                                  xAODRootAccess
                                  FourMomUtils
                                  TopEvent
                                  TopConfiguration
                                  TopDataPreparation
                                  MCTruthClassifierLib
                                  TruthUtils
                                  JetReclusteringLib
                                  JetSubStructureUtils
                                  AsgTools
                                  ${ROOT_LIBRARIES}		  
                                  ${FASTJET_LIBRARIES}
                                  ${FASTJETCONTRIB_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

