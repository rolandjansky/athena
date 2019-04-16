
# Declare the name of this package:
atlas_subdir( TopEventSelectionTools )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          EventLoop
                          AsgTools
                          JetInterface
                          TopEvent
                          TopCorrections
                          TopParticleLevel
                          MuonSelectorTools )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( TopEventSelectionTools _cintDictSource
   ROOT_HEADERS TopEventSelectionTools/ToolLoaderBase.h
   TopEventSelectionTools/TopEventSelectionToolsLoader.h Root/LinkDef.h
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
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

# Install data files from the package:
atlas_install_data( share/* )

