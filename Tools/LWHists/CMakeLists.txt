# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( LWHists )

# External dependencies:
find_package( ROOT COMPONENTS Hist RIO Core MathCore )

# To let valgrind look for problems:
#add_definitions( -DLW_DEBUG_POOLS_USE_NEW )

# To make the pools monitor acquire/release ("mini-valgrind"):
#add_definitions( -DLW_DEBUG_POOLS_DEBUG_USAGE )

# Jump through extra hoops to reproduce obscure ROOT behaviour:
add_definitions( -DLW_STRICT_ROOT_BEHAVIOUR )

# To find users who really maybe shouldn't use LW histograms:
#add_definitions( -DLW_DEBUG_HEAVY_USERS )

# To compile an extra symbol for better ROOTSPY performance:
#add_definitions( -DLW_DEBUG_ROOTSPY_SYMB )

# The package's main library:
atlas_add_library( LWHists src/*.cxx
  PUBLIC_HEADERS LWHists
  LINK_LIBRARIES CxxUtils
  PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

# Executables in the package:
atlas_add_executable( lwhists_validate src/tests/validation/*.cxx
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES} LWHists CxxUtils )

atlas_add_executable( lwhists_benchmark src/tests/benchmarking/*.cxx
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES ${ROOT_LIBRARIES} LWHists )

atlas_add_executable( lwhists_comphistfiles src/tests/comphistfiles/*.cxx
  INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
  LINK_LIBRARIES CxxUtils ${ROOT_LIBRARIES} )


atlas_add_test( LWHists_val
   SCRIPT lwhists_validate --seed=117
   PROPERTIES TIMEOUT 600
   LOG_IGNORE_PATTERN "Systematic tests" )


atlas_add_test( LWHists_val_rootbackend
   SCRIPT lwhists_validate --rootbackend --seed=117
   PROPERTIES TIMEOUT 600
   LOG_IGNORE_PATTERN "Systematic tests" )


atlas_add_test( LWHists_val_triggerrootconv
   SCRIPT lwhists_validate --triggerconv --seed=117
   PROPERTIES TIMEOUT 600
   LOG_IGNORE_PATTERN "Systematic tests" )
