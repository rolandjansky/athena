# $Id: CMakeLists.txt 729177 2016-03-11 14:32:18Z krasznaa $
################################################################################
# Package: AthAllocators
################################################################################

# Declare the package name:
atlas_subdir( AthAllocators )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   GaudiKernel
   PRIVATE
   AtlasTest/TestTools
   Control/CxxUtils )

# External dependencies:
find_package( Boost COMPONENTS thread )

# Component(s) in the package:
atlas_add_library( AthAllocators AthAllocators/*.h AthAllocators/*.icc src/*.cxx
   PUBLIC_HEADERS AthAllocators
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} GaudiKernel
   PRIVATE_LINK_LIBRARIES CxxUtils )

atlas_add_dictionary( AthAllocatorsDict
   AthAllocators/AthAllocatorsDict.h AthAllocators/selection.xml
   LINK_LIBRARIES AthAllocators )

# Test(s) in the package:
macro( _simple_test name )
   atlas_add_test( ${name}
      SOURCES test/${name}.cxx
      LINK_LIBRARIES CxxUtils AthAllocators )
endmacro( _simple_test )

_simple_test( ArenaAllocatorBase_test )
_simple_test( ArenaBlockAllocatorBase_test )
_simple_test( ArenaBlock_test )
_simple_test( ArenaPoolAllocator_test )
_simple_test( ArenaHeapAllocator_test )
_simple_test( ArenaHandleBase_test )
_simple_test( ArenaHandleBaseAllocT_test )
_simple_test( ArenaHandleBaseT_test )
_simple_test( ArenaHeader_test )
_simple_test( ArenaCachingHandle_test )
_simple_test( ArenaHandle_test )
_simple_test( ArenaAllocatorCreator_test )
_simple_test( ArenaAllocatorRegistry_test )
_simple_test( Arena_test )
_simple_test( ArenaBase_test )
_simple_test( ArenaPoolSTLAllocator_test )
_simple_test( ArenaHeapSTLAllocator_test )
_simple_test( ArenaSharedHeapSTLAllocator_test )

atlas_add_test( DataPool_test
   SOURCES test/DataPool_test.cxx
   LINK_LIBRARIES GaudiKernel TestTools AthAllocators CxxUtils
   EXTRA_PATTERNS "^IncidentSvc +DEBUG Adding|^HistogramPersis.* (INFO|DEBUG)|^JobOptionsSvc +INFO|DEBUG Property update for OutputLevel"
   ENVIRONMENT "JOBOPTSEARCHPATH=${CMAKE_CURRENT_SOURCE_DIR}/share" )
