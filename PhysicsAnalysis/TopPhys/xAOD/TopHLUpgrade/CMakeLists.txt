# Auto-generated on: 2017-03-08 14:47:37.415174

# Declare the name of this package:
atlas_subdir( TopHLUpgrade None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          TopConfiguration
                          TopEvent
                          xAODTruth
                          xAODJet
                          xAODMissingET
                          TopParticleLevel
			  UpgradePerformanceFunctions )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( TopHLUpgrade _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( TopHLUpgrade Root/*.cxx Root/*.h Root/*.icc
                   TopHLUpgrade/*.h TopHLUpgrade/*.icc TopHLUpgrade/*/*.h
                   TopHLUpgrade/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS TopHLUpgrade
                   LINK_LIBRARIES TopConfiguration
                                  TopEvent
                                  xAODTruth
                                  xAODJet
                                  xAODMissingET
                                  TopParticleLevel
				  UpgradePerformanceFunctions
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

