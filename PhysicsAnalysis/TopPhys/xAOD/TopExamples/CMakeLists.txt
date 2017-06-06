# Auto-generated on: 2017-03-08 14:47:36.923782

# Declare the name of this package:
atlas_subdir( TopExamples None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          AsgTools
                          xAODCore
                          xAODRootAccess
                          TopCPTools
                          TopCorrections
                          TopAnalysis
                          TopEvent
                          TopEventSelectionTools
                          TopEventReconstructionTools
                          TopConfiguration
                          PathResolver )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO Math MathMore MathCore Graf Physics )


# Custom definitions needed for this package:
add_definitions( -g )

# Build a library that other components can link against:
atlas_add_library( TopExamples Root/*.cxx Root/*.h Root/*.icc
                   TopExamples/*.h TopExamples/*.icc TopExamples/*/*.h
                   TopExamples/*/*.icc 
                   PUBLIC_HEADERS TopExamples
                   LINK_LIBRARIES AsgTools
                                  xAODCore
                                  xAODRootAccess
                                  TopCPTools
                                  TopCorrections
                                  TopAnalysis
                                  TopEvent
                                  TopEventSelectionTools
                                  TopEventReconstructionTools
                                  TopConfiguration
                                  PathResolver
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

# Install the Python modules of the package:
atlas_install_python_modules( python/*.py )

# Install data files from the package:
atlas_install_data( share/* )

# Build the executables of the package:
atlas_add_executable( mini-to-flat
                      util/mini-to-flat.cxx
                      LINK_LIBRARIES AsgTools
                                     xAODCore
                                     xAODRootAccess
                                     TopCPTools
                                     TopCorrections
                                     TopAnalysis
                                     TopEvent
                                     TopEventSelectionTools
                                     TopEventReconstructionTools
                                     TopConfiguration
                                     PathResolver
                                     ${ROOT_LIBRARIES}
                                     TopExamples )
atlas_add_executable( mini-xaod
                      util/mini-xaod.cxx
                      LINK_LIBRARIES AsgTools
                                     xAODCore
                                     xAODRootAccess
                                     TopCPTools
                                     TopCorrections
                                     TopAnalysis
                                     TopEvent
                                     TopEventSelectionTools
                                     TopEventReconstructionTools
                                     TopConfiguration
                                     PathResolver
                                     ${ROOT_LIBRARIES}
                                     TopExamples )
atlas_add_executable( mini-to-plots
                      util/mini-to-plots.cxx
                      LINK_LIBRARIES AsgTools
                                     xAODCore
                                     xAODRootAccess
                                     TopCPTools
                                     TopCorrections
                                     TopAnalysis
                                     TopEvent
                                     TopEventSelectionTools
                                     TopEventReconstructionTools
                                     TopConfiguration
                                     PathResolver
                                     ${ROOT_LIBRARIES}
                                     TopExamples )

atlas_install_scripts( scripts/* )
