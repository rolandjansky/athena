# Auto-generated on: 2017-03-08 14:47:36.489231

# Declare the name of this package:
atlas_subdir( TopEventReconstructionTools None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          xAODCore
                          AthContainers
                          AsgTools
                          TopEvent
                          TopEventSelectionTools
                          TopAnalysis
			  KLFitter )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO Math MathMore MathCore Graf Minuit Minuit2 )

# Custom definitions needed for this package:
add_definitions( -g )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( TopEventReconstructionTools _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( TopEventReconstructionTools Root/*.cxx Root/*.h Root/*.icc
                   TopEventReconstructionTools/*.h TopEventReconstructionTools/*.icc TopEventReconstructionTools/*/*.h
                   TopEventReconstructionTools/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS TopEventReconstructionTools
                   LINK_LIBRARIES xAODCore
                                  AthContainers
                                  AsgTools
                                  TopEvent
                                  TopEventSelectionTools
                                  TopAnalysis
				  KLFitter
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

# Install data files from the package:
atlas_install_data( share/* )

# Build the executables of the package:
atlas_add_executable( topreco_test_mt2
                      util/topreco_test_mt2.cxx
                      LINK_LIBRARIES xAODCore
                                     AthContainers
                                     AsgTools
                                     TopEvent
                                     TopEventSelectionTools
                                     TopAnalysis
				     KLFitter
                                     ${ROOT_LIBRARIES}
                                     TopEventReconstructionTools )

