# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# The name of the package:
atlas_subdir( EventLoopTest )

# External(s) needed for the build:
find_package( GTest )
find_package( GMock )
find_package( ROOT COMPONENTS Core RIO Hist Tree TreePlayer )

# Component(s) in the package:
atlas_add_root_dictionary( EventLoopTestLib
   EventLoopTestDictSource
   ROOT_HEADERS EventLoopTest/UnitTestAlg.h EventLoopTest/UnitTestAlgXAOD.h EventLoopTest/UnitTestAlg1.h
   EventLoopTest/UnitTestAlg2.h EventLoopTest/UnitTestAlg3.h
   EventLoopTest/UnitTestAlg4.h EventLoopTest/UnitTestTool.h Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

atlas_add_library( EventLoopTestLib
   EventLoopTest/*.h EventLoopTest/*.ihh Root/*.cxx ${EventLoopTestDictSource}
   PUBLIC_HEADERS EventLoopTest
   DEFINITIONS USE_CMAKE
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${GMOCK_LIBRARIES} RootCoreUtils
   SampleHandler AnaAlgorithmLib EventLoop EventLoopGrid AsgTestingLib xAODEventInfo
   PRIVATE_LINK_LIBRARIES AsgTools xAODRootAccess xAODEventInfo xAODMetaData )

atlas_add_dictionary( EventLoopTestDict
   EventLoopTest/EventLoopTestDict.h
   EventLoopTest/selection.xml
   LINK_LIBRARIES EventLoopTestLib )

# Test(s) in the package:
function( _add_gtest cxx_name )
   set (EXTRA_ARGS PROPERTIES )
   list (FIND SLOW_TESTS "${cxx_name}" slow_index)
   if (${slow_index} GREATER -1)
     set (EXTRA_ARGS ${EXTRA_ARGS} TIMEOUT 900)
   endif ()
   get_filename_component( test_name ${cxx_name} NAME_WE )
   atlas_add_test (${test_name}
      SOURCES test/${test_name}.cxx
      INCLUDE_DIRS ${GMOCK_INCLUDE_DIRS}
      LINK_LIBRARIES ${GMOCK_LIBRARIES} RootCoreUtils AsgTools AsgTestingLib
      SampleHandler EventLoopTestLib xAODRootAccess ${EXTRA_ARGS})
   set_tests_properties (EventLoopTest_${test_name}_ctest PROPERTIES LABELS "EventLoopTest;EventLoop;SampleHandler;AnaAlgorithm" )
endfunction( _add_gtest )

set (MANUAL_TESTS test/gt_LSFDriver_EOS.cxx test/make_test_file.cxx test/ut_driver_direct_gridinput.cxx test/ut_driver_kubernetes.cxx test/ut_driver_local_docker.cxx test/ut_driver_grid.cxx test/ut_driver_condor.cxx test/ut_driver_ll.cxx test/ut_driver_lsf.cxx test/ut_driver_torque.cxx test/ut_xrootd_output.cxx test/ut_xrootd_input.cxx)

set (SLOW_TESTS test/gt_DirectDriver.cxx test/gt_LocalDriver.cxx
   test/gt_LeakChecks.cxx test/gt_UnitTestAlgXAOD.cxx)


file( GLOB _tests RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/test/[a-z]*.cxx" )
foreach( source ${_tests} )
  string (REGEX REPLACE "test/(.*).cxx" "\\1" util ${source})
  list (FIND MANUAL_TESTS "${source}" _index)
  if (${_index} GREATER -1)
    add_executable ("EventLoopTestLib_${util}" ${source})
    target_link_libraries ("EventLoopTestLib_${util}" EventLoopTestLib EventLoopGrid)
    add_dependencies( Package_EventLoopTest EventLoopTestLib_${util} )
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
atlas_install_data( data/*.yml )
