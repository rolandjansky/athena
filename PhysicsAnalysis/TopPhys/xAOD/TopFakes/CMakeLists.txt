# Auto-generated on: 2017-03-08 14:47:37.318937

# Declare the name of this package:
atlas_subdir( TopFakes None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          TopEvent
                          TopConfiguration )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO Math MathCore MathMore Graf Matrix )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( TopFakes _cintDictSource
                           ROOT_HEADERS Root/LinkDef.h
                           EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( TopFakes Root/*.cxx Root/*.h Root/*.icc
                   TopFakes/*.h TopFakes/*.icc TopFakes/*/*.h
                   TopFakes/*/*.icc ${_cintDictSource} 
                   PUBLIC_HEADERS TopFakes
                   LINK_LIBRARIES TopEvent	  
                                  TopConfiguration
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

# Install data files from the package:
atlas_install_data( data/* )

