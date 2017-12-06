# $Id: CMakeLists.txt 777300 2016-10-08 21:20:26Z ssnyder $
################################################################################
# Package: AthContainers
################################################################################

# Declare the package name:
atlas_subdir( AthContainers )

# Extra dependencies, based on the environment we are in:
if( NOT XAOD_STANDALONE )
   set( extra_deps Control/AthenaKernel Control/SGTools GaudiKernel )
   set( extra_libs AthenaKernel SGTools GaudiKernel )
   set( extra_sources src/*.cxx )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainersInterfaces
   Control/AthLinks
   Control/CxxUtils
   ${extra_deps}
   PRIVATE
   AtlasTest/TestTools )

# External dependencies:
find_package( Boost COMPONENTS thread )
find_package( ROOT COMPONENTS Core )

# Component(s) in the package:
atlas_add_library( AthContainers
   AthContainers/*.h AthContainers/*.icc
   AthContainers/tools/*.h AthContainers/tools/*.icc
   Root/*.cxx ${extra_sources}
   PUBLIC_HEADERS AthContainers
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} AthLinks CxxUtils ${extra_libs} )

atlas_add_dictionary( AthContainersDict
   AthContainers/AthContainersDict.h
   AthContainers/selection.xml
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers
   EXTRA_FILES Root/dict/*.cxx )

# Helper function setting up the test(s) in the package:
function( _add_test name )
   cmake_parse_arguments( ARG "" "EXTRA_PATTERNS" "" ${ARGN} )
   if( ARG_EXTRA_PATTERNS )
      set( extra_arg EXTRA_PATTERNS ${ARG_EXTRA_PATTERNS} )
   endif()
   atlas_add_test( ${name}
      SOURCES test/${name}.cxx
      INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AthLinks CxxUtils
      TestTools AthContainers ${extra_libs}
      ${extra_arg} )
endfunction( _add_test )

# Test(s) in the package:
_add_test( removeDuplicates_test )
_add_test( DataVector_test )
_add_test( DataVector_a_test )
_add_test( DataVector_b_test )
_add_test( DataVector_c_test )
_add_test( DataVector_d_test )
_add_test( DataVector_e_test )
_add_test( DataVector_f_test )
_add_test( DVLInfo_test )
_add_test( DVLCast_test )
_add_test( IsMostDerivedFlag_test )
_add_test( ElementProxy_test )
_add_test( DVLIterator_test )
_add_test( DVL_iter_swap_test )
_add_test( foreach_test )
_add_test( AuxTypeVector_test )
_add_test( AuxTypeVectorFactory_test )
_add_test( AuxTypeRegistry_test EXTRA_PATTERNS "will use std::" )
_add_test( AuxVectorBase_test EXTRA_PATTERNS "will use std::" )
_add_test( AuxStoreInternal_test )
_add_test( AuxStoreStandalone_test )
_add_test( AuxElement_test )
_add_test( AuxElementComplete_test )
_add_test( error_test )
_add_test( threading_test )
_add_test( threading_nothreads_test )
_add_test( exceptions_test )
_add_test( normalizedTypeinfoName_test )
_add_test( getThinnedFlags_test )
_add_test( copyAuxStoreThinned_test )
_add_test( copyThinned_test )
_add_test( PackedParameters_test )
_add_test( PackedConverter_test )
_add_test( PackedContainer_test )
_add_test( debug_test )

if( NOT XAOD_STANDALONE )
   _add_test( dataVectorAsELV_test )
   _add_test( ViewVector_test )
   _add_test( ViewVectorBase_test )
   _add_test( AuxVectorData_test EXTRA_PATTERNS "will use std::" )
   _add_test( DVLDataBucket_test )
endif()
