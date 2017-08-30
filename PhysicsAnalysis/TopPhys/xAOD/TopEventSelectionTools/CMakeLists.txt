# Auto-generated on: 2017-03-08 14:47:36.708684

# Declare the name of this package:
atlas_subdir( TopEventSelectionTools None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          EventLoop
                          AsgTools
                          JetInterface
                          TopEvent
                          TopCorrections
                          TopParticleLevel
                          MuonSelectorTools 
			  JetSelectorTools )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf )

# Custom definitions needed for this package:
add_definitions( -g )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( TopEventSelectionTools _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( TopEventSelectionTools Root/*.cxx Root/*.h Root/*.icc
                   TopEventSelectionTools/*.h TopEventSelectionTools/*.icc TopEventSelectionTools/*/*.h
                   TopEventSelectionTools/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS TopEventSelectionTools
                   LINK_LIBRARIES EventLoop
                                  AsgTools
                                  JetInterface
                                  TopEvent
                                  TopCorrections
                                  TopParticleLevel
                                  MuonSelectorToolsLib
				  JetSelectorToolsLib
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

# Install data files from the package:
atlas_install_data( share/* )

