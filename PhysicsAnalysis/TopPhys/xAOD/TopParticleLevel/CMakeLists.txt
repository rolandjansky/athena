# Auto-generated on: 2017-03-08 14:47:38.688787

# Declare the name of this package:
atlas_subdir( TopParticleLevel None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          xAODTruth
                          xAODJet
                          xAODMissingET
                          xAODCore
                          xAODRootAccess
                          TopEvent
                          TopConfiguration
			  TruthUtils
                          MCTruthClassifier )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf )

# Custom definitions needed for this package:
add_definitions( -g )

# Build a library that other components can link against:
atlas_add_library( TopParticleLevel Root/*.cxx Root/*.h Root/*.icc
                   TopParticleLevel/*.h TopParticleLevel/*.icc TopParticleLevel/*/*.h
                   TopParticleLevel/*/*.icc 
                   PUBLIC_HEADERS TopParticleLevel
                   LINK_LIBRARIES xAODTruth
                                  xAODJet
                                  xAODMissingET
                                  xAODCore
                                  xAODRootAccess
                                  TopEvent
                                  TopConfiguration
                                  MCTruthClassifierLib
				  TruthUtils
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

