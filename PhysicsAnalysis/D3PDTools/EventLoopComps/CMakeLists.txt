# The name of the package:
atlas_subdir( EventLoopComps )

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

# Component(s) in the package:
atlas_add_root_dictionary( EventLoopCompsLib
   EventLoopCompsDictSource
   ROOT_HEADERS EventLoopComps/*.h Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

atlas_add_library( EventLoopCompsLib
   EventLoopComps/*.h EventLoopComps/*.ihh Root/*.cxx ${EventLoopCompsDictSource}
   PUBLIC_HEADERS EventLoopComps
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} RootCoreUtils AsgTools xAODRootAccess )

file( GLOB _tests RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/test/gt_[a-z]*.cxx" )
foreach( source ${_tests} )
  string (REGEX REPLACE "test/(.*).cxx" "\\1" test ${source})
   atlas_add_test( ${test}
      SOURCES test/${test}.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} ${GTEST_LIBRARIES} EventLoopCompsLib )
endforeach()
