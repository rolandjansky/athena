
# Declare the name of this package:
atlas_subdir( TopFakes )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          TopEvent
                          TopConfiguration )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO Math MathCore MathMore Graf Matrix )

# Generate a CINT dictionary source file:
set( _extraHeader )
if( XAOD_STANDALONE )
   set( _extraHeader TopFakes/TopFakesMMWeightCalculator.h )
endif()
atlas_add_root_dictionary( TopFakes _cintDictSource
   ROOT_HEADERS ${_extraHeader} TopFakes/FakesWeights.h TopFakes/MMEffSet.h
   TopFakes/MMEfficiency.h TopFakes/MatrixUtils.h Root/LinkDef.h
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

