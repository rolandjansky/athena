# Auto-generated on: 2017-03-08 14:47:38.930377

# Declare the name of this package:
atlas_subdir( TopPartons None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          xAODCore
                          AthContainers
                          AthLinks
                          AsgTools
                          xAODTruth
                          TopConfiguration )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore TMVA Graf )

# Custom definitions needed for this package:
add_definitions( -g )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( TopPartons _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( TopPartons Root/*.cxx Root/*.h Root/*.icc
                   TopPartons/*.h TopPartons/*.icc TopPartons/*/*.h
                   TopPartons/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS TopPartons
                   LINK_LIBRARIES xAODCore
                                  AthContainers
                                  AthLinks
                                  AsgTools
                                  xAODTruth
                                  TopConfiguration
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

