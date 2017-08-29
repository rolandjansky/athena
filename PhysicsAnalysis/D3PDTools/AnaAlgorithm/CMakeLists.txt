# The name of the package:
atlas_subdir( AnaAlgorithm )

# Find the needed external(s):
find_package( ROOT COMPONENTS Core RIO Hist Tree TreePlayer Proof ProofPlayer )
find_package( GTest )
find_package( GMock )

# The package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   PhysicsAnalysis/D3PDTools/RootCoreUtils
   Control/AthToolSupport/AsgTools
   PRIVATE
   Control/xAODRootAccess )

if (XAOD_STANDALONE)
  set (headers AnaAlgorithm/*.h)
  set (inline_headers AnaAlgorithm/*.ihh)
  set (sources Root/*.cxx)
else (XAOD_STANDALONE)
  set (headers AnaAlgorithm/AnaAlgorithm.h AnaAlgorithm/Global.h  AnaAlgorithm/MessageCheck.h)
  set (inline_headers AnaAlgorithm/AnaAlgorithm.ihh)
  set (sources Root/AnaAlgorithm.cxx Root/MessageCheck.h)
endif (XAOD_STANDALONE)


# Component(s) in the package:
atlas_add_root_dictionary( AnaAlgorithmLib
   AnaAlgorithmDictSource
   ROOT_HEADERS ${headers} Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

atlas_add_library( AnaAlgorithmLib
   ${headers} ${inline_headers} ${sources} ${AnaAlgorithmDictSource}
   PUBLIC_HEADERS AnaAlgorithm
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} RootCoreUtils AsgTools xAODRootAccess )

if (XAOD_STANDALONE)
  file( GLOB _tests RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/test/gt_[a-z]*.cxx" )
  foreach( source ${_tests} )
    string (REGEX REPLACE "test/(.*).cxx" "\\1" test ${source})
    atlas_add_test( ${test}
      SOURCES test/${test}.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} ${GTEST_LIBRARIES} AnaAlgorithmLib EventLoopTestLib )
  endforeach()
endif (XAOD_STANDALONE)
