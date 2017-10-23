# Auto-generated on: 2017-03-08 14:47:33.629640

# Declare the name of this package:
atlas_subdir( TopAnalysis None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          xAODCore
                          xAODRootAccess
                          xAODEgamma
                          xAODMuon
                          xAODJet
                          xAODMissingET
                          xAODCutFlow
			  xAODMetaData
                          AsgTools
                          AthContainers			  
                          PATInterfaces
                          TopCPTools
                          TopEventSelectionTools
                          TopConfiguration
                          TopCorrections
                          TopEvent
                          TopParticleLevel
                          TopPartons
                          TopObjectSelectionTools
                          TopSystematicObjectMaker
                          TopFakes
			  TopDataPreparation
			  TopHLUpgrade 
			  Tools/PathResolver )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf )

# Custom definitions needed for this package:
add_definitions( -g )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( TopAnalysis _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( TopAnalysis Root/*.cxx Root/*.h Root/*.icc
                   TopAnalysis/*.h TopAnalysis/*.icc TopAnalysis/*/*.h
                   TopAnalysis/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS TopAnalysis
                   LINK_LIBRARIES xAODCore
                                  xAODRootAccess
                                  xAODEgamma
                                  xAODMuon
                                  xAODJet
                                  xAODMissingET
                                  xAODCutFlow
				  xAODMetaData
                                  AsgTools
                                  AthContainers
                                  PATInterfaces
                                  TopCPTools
                                  TopEventSelectionTools
                                  TopConfiguration
                                  TopCorrections
                                  TopEvent
                                  TopParticleLevel
                                  TopPartons
                                  TopObjectSelectionTools
                                  TopSystematicObjectMaker
                                  TopFakes
				  TopDataPreparation
				  TopHLUpgrade
				  PathResolver
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

# Install data files from the package:
atlas_install_data( share/* )

# Install user scripts
atlas_install_scripts( scripts/* )

# Build the executables of the package:
atlas_add_executable( top-xaod
                      util/top-xaod.cxx
                      LINK_LIBRARIES xAODCore
                                     xAODRootAccess
                                     xAODEgamma
                                     xAODMuon
                                     xAODJet
                                     xAODMissingET
                                     xAODCutFlow
				     xAODMetaData
                                     AsgTools
                                     AthContainers
                                     PATInterfaces
                                     TopCPTools
                                     TopEventSelectionTools
                                     TopConfiguration
                                     TopCorrections
                                     TopEvent
                                     TopParticleLevel
                                     TopPartons
                                     TopObjectSelectionTools
                                     TopSystematicObjectMaker
                                     TopFakes
				     TopDataPreparation
                                     TopHLUpgrade
				     PathResolver
                                     ${ROOT_LIBRARIES}
                                     TopAnalysis )

atlas_add_executable( top-tool-ftag
                      util/top-tool-ftag.cxx
                      LINK_LIBRARIES xAODCore
                                     xAODRootAccess
                                     xAODEgamma
                                     xAODMuon
                                     xAODJet
                                     xAODMissingET
                                     xAODCutFlow
				     xAODMetaData
                                     AsgTools
                                     AthContainers
                                     PATInterfaces
                                     TopCPTools
                                     TopEventSelectionTools
                                     TopConfiguration
                                     TopCorrections
                                     TopEvent
                                     TopParticleLevel
                                     TopPartons
                                     TopObjectSelectionTools
                                     TopSystematicObjectMaker
                                     TopFakes
                                     TopDataPreparation
                                     TopHLUpgrade
				     PathResolver
                                     ${ROOT_LIBRARIES}
                                     TopAnalysis )
atlas_add_executable( top-xaod-v2
                      util/top-xaod-v2.cxx
                      LINK_LIBRARIES xAODCore
                                     xAODRootAccess
                                     xAODEgamma
                                     xAODMuon
                                     xAODJet
                                     xAODMissingET
                                     xAODCutFlow
                                     xAODMetaData
                                     AsgTools
                                     AthContainers
                                     PATInterfaces
                                     TopCPTools
                                     TopEventSelectionTools
                                     TopConfiguration
                                     TopCorrections
                                     TopEvent
                                     TopParticleLevel
                                     TopPartons
                                     TopObjectSelectionTools
                                     TopSystematicObjectMaker
                                     TopFakes
                                     TopDataPreparation
                                     TopHLUpgrade
				     PathResolver
                                     ${ROOT_LIBRARIES}
                                     TopAnalysis )

atlas_add_test(	top-xaod-runtime-ctest
                SCRIPT test/runtime_ctest.py
                PROPERTIES TIMEOUT 300 )			  
