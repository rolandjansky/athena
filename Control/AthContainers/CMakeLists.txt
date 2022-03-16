################################################################################
# Package: AthContainers
################################################################################

# Declare the package name:
atlas_subdir( AthContainers )

# Extra dependencies, based on the environment we are in:
if( NOT XAOD_STANDALONE )
   set( extra_libs AthenaKernel AthAllocators SGTools GaudiKernel )
   set( extra_sources src/*.cxx )
endif()

# External dependencies:
find_package( Boost COMPONENTS thread )
find_package( ROOT COMPONENTS Core )
find_package( TBB )

# Component(s) in the package:
atlas_add_library( AthContainers
   AthContainers/*.h AthContainers/*.icc
   AthContainers/tools/*.h AthContainers/tools/*.icc
   Root/*.cxx ${extra_sources}
   PUBLIC_HEADERS AthContainers
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${TBB_LIBRARIES}
                  AthContainersInterfaces AthLinks CxxUtils ${extra_libs} )

atlas_add_dictionary( AthContainersDict
   AthContainers/AthContainersDict.h
   AthContainers/selection.xml
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers
   EXTRA_FILES Root/dict/*.cxx )

# Helper function setting up the test(s) in the package:
function( _add_test name )
   cmake_parse_arguments( ARG "" "LOG_IGNORE_PATTERN" "" ${ARGN} )
   if( ARG_LOG_IGNORE_PATTERN )
      set( extra_arg LOG_IGNORE_PATTERN ${ARG_LOG_IGNORE_PATTERN} )
   endif()
   atlas_add_test( ${name}
      SOURCES test/${name}.cxx
      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthLinks CxxUtils
      TestTools AthContainers ${extra_libs}
      ${extra_arg} )
endfunction( _add_test )

# Test(s) in the package:
_add_test( removeDuplicates_test LOG_IGNORE_PATTERN "0x[0-9a-f\]{4,}" )
_add_test( DVLInfo_test )
_add_test( DVLCast_test )
_add_test( IsMostDerivedFlag_test )
_add_test( ElementProxy_test )
_add_test( DVLIterator_test )
_add_test( DVL_iter_swap_test )
_add_test( AuxTypeVector_test )
_add_test( AuxTypeVectorFactory_test )
_add_test( AuxTypeRegistry_test LOG_IGNORE_PATTERN "will use std::" )
_add_test( AuxVectorBase_test LOG_IGNORE_PATTERN "will use std::" )
_add_test( AuxStoreInternal_test )
_add_test( AuxStoreStandalone_test LOG_IGNORE_PATTERN "will use std::" )
_add_test( AuxElement_test )
_add_test( AuxElementComplete_test )
_add_test( error_test LOG_IGNORE_PATTERN "will use std::" )
_add_test( threading_test )
_add_test( threading_nothreads_test )
_add_test( exceptions_test )
_add_test( normalizedTypeinfoName_test )
_add_test( PackedParameters_test )
_add_test( PackedConverter_test )
_add_test( PackedContainer_test )
_add_test( debug_test )
_add_test( AtomicDecorator_test )
_add_test( AtomicConstAccessor_test )
_add_test( supportsThinning_test )
_add_test( AuxStoreConstMem_test )


if( NOT XAOD_STANDALONE )
   _add_test( ViewVector_test )
   _add_test( ViewVectorBase_test )
   _add_test( AuxVectorData_test LOG_IGNORE_PATTERN "will use std::" )
   _add_test( DVLDataBucket_test )
   _add_test( ThinningDecision_test )
   _add_test( DataVector_test LOG_IGNORE_PATTERN "0x[0-9a-f\]{4,}" )
   _add_test( DataVector_a_test )
   _add_test( DataVector_b_test )
   _add_test( DataVector_c_test )
   _add_test( DataVector_d_test )
   _add_test( DataVector_e_test )
   _add_test( DataVector_f_test )
   _add_test( DataVector_rule5_test )
   _add_test( copyAuxStoreThinned_test )
   _add_test( copyThinned_test )
   _add_test( dataVectorAsELV_test )
   _add_test( DataVectorWithAlloc_test )
endif()
