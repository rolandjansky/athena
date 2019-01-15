
# The name of the package:
atlas_subdir( EventLoopTest )

# External(s) needed for the build:
find_package( GTest )
find_package( GMock )

# The package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   PhysicsAnalysis/D3PDTools/RootCoreUtils
   PhysicsAnalysis/D3PDTools/SampleHandler
   PhysicsAnalysis/D3PDTools/EventLoop
   PRIVATE
   Control/AthToolSupport/AsgTools
   Control/xAODRootAccess )

# Find the needed external(s):
find_package( ROOT COMPONENTS Core RIO Hist Tree TreePlayer )
find_package( GTest )

# Component(s) in the package:
atlas_add_root_dictionary( EventLoopTestLib
   EventLoopTestDictSource
   ROOT_HEADERS EventLoopTest/UnitTestAlg.h EventLoopTest/UnitTestAlg1.h
   EventLoopTest/UnitTestAlg2.h EventLoopTest/UnitTestAlg3.h
   EventLoopTest/UnitTestTool.h Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

atlas_add_library( EventLoopTestLib
   EventLoopTest/*.h EventLoopTest/*.ihh Root/*.cxx ${EventLoopTestDictSource}
   PUBLIC_HEADERS EventLoopTest
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${GMOCK_LIBRARIES} RootCoreUtils
   SampleHandler EventLoop
   PRIVATE_LINK_LIBRARIES AsgTools xAODRootAccess )

target_compile_definitions (EventLoopTestLib PUBLIC USE_CMAKE)

# Test(s) in the package:
function( _add_gtest cxx_name )
   set (EXTRA ARGS)
   list (FIND SLOW_TESTS "${cxx_name}" slow_index)
   if (${slow_index} GREATER -1)
     set (EXTRA_ARGS ${EXTRA_ARGS} PROPERTIES TIMEOUT 900)
   endif ()
   get_filename_component( test_name ${cxx_name} NAME_WE )
   atlas_add_test (${test_name}
      SOURCES test/${test_name}.cxx
      INCLUDE_DIRS ${GMOCK_INCLUDE_DIRS}
      LINK_LIBRARIES ${GMOCK_LIBRARIES} RootCoreUtils AsgTools
      SampleHandler EventLoopTestLib ${EXTRA_ARGS})
endfunction( _add_gtest )

set (MANUAL_TESTS test/gt_LSFDriver_EOS.cxx test/make_test_file.cxx test/ut_driver_condor.cxx test/ut_driver_ll.cxx test/ut_driver_lsf.cxx test/ut_driver_torque.cxx test/ut_xrootd_output.cxx test/ut_xrootd_input.cxx)

set (SLOW_TESTS test/gt_DirectDriver.cxx test/gt_LocalDriver.cxx
   test/gt_LeakChecks.cxx)


file( GLOB _tests RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/test/[a-z]*.cxx" )
foreach( source ${_tests} )
  string (REGEX REPLACE "test/(.*).cxx" "\\1" util ${source})
  list (FIND MANUAL_TESTS "${source}" _index)
  if (${_index} GREATER -1)
    add_executable ("EventLoopTestLib_${util}" ${source})
    target_link_libraries ("EventLoopTestLib_${util}" EventLoopTestLib)
  else (${_index} GREATER -1)
    _add_gtest( test/${util}.cxx)
  endif (${_index} GREATER -1)
endforeach()

file (GLOB util_sources RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/util/[a-zA-Z0-9]*.cxx")
foreach (source ${util_sources})
  string (REGEX REPLACE "util/(.*).cxx" "\\1" util ${source})
  atlas_add_executable (${util} ${source} LINK_LIBRARIES EventLoopTestLib)
endforeach (source ${util_sources})



# Install files from the package:
atlas_install_scripts( scripts/el_retrieve scripts/el_wait )
atlas_install_data( data/*.root )
