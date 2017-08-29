# $Id: CMakeLists.txt 804732 2017-05-11 22:02:44Z krumnack $

# The name of the package:
atlas_subdir( EventLoop )

find_package( GTest )
find_package( GMock )

# The package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   PhysicsAnalysis/D3PDTools/RootCoreUtils
   PhysicsAnalysis/D3PDTools/SampleHandler
   PRIVATE
   Control/AthToolSupport/AsgTools
   Control/xAODRootAccess )

# Find the needed external(s):
find_package( ROOT COMPONENTS Core RIO Hist Tree TreePlayer Proof ProofPlayer )
find_package( GTest )

# Component(s) in the package:
atlas_add_root_dictionary( EventLoop
   EventLoopDictSource
   ROOT_HEADERS EventLoop/Algorithm.h EventLoop/AnaAlgorithmWrapper.h EventLoop/BackgroundDriver.h
   EventLoop/BackgroundJob.h EventLoop/BackgroundTSelector.h
   EventLoop/BackgroundWorker.h EventLoop/BatchDriver.h
   EventLoop/BatchJob.h EventLoop/BatchSegment.h EventLoop/BatchSample.h
   EventLoop/BatchWorker.h EventLoop/CondorDriver.h EventLoop/DirectDriver.h
   EventLoop/GEDriver.h EventLoop/Job.h
   EventLoop/LLDriver.h EventLoop/LSFDriver.h EventLoop/LocalDriver.h
   EventLoop/OutputStream.h EventLoop/MetricsSvc.h EventLoop/ProofArgs.h
   EventLoop/ProofDriver.h EventLoop/ProofTSelector.h EventLoop/SoGEDriver.h
   EventLoop/StatusCode.h EventLoop/TEventSvc.h EventLoop/TorqueDriver.h
   EventLoop/VomsProxySvc.h
   EventLoop/Worker.h EventLoop/ProofWorker.h EventLoop/SlurmDriver.h
   Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

atlas_add_library( EventLoop
   EventLoop/*.h EventLoop/*.ihh Root/*.cxx ${EventLoopDictSource}
   PUBLIC_HEADERS EventLoop
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${GTEST_LIBRARIES} RootCoreUtils
   SampleHandler AnaAlgorithmLib
   PRIVATE_LINK_LIBRARIES AsgTools xAODRootAccess )

target_compile_definitions (EventLoop PUBLIC USE_CMAKE)

# Test(s) in the package:
function( _add_gtest cxx_name )
   get_filename_component( test_name ${cxx_name} NAME_WE )
   atlas_add_test( ${test_name}
      SOURCES test/${test_name}.cxx
      INCLUDE_DIRS ${GTEST_INCLUDE_DIRS} ${GMOCK_INCLUDE_DIRS}
      LINK_LIBRARIES ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES} RootCoreUtils AsgTools
      SampleHandler EventLoop EventLoopTestLib )
endfunction( _add_gtest )

function( _add_test cxx_name )
   get_filename_component( test_name ${cxx_name} NAME_WE )
   atlas_add_test( ${test_name}
      SOURCES test/${test_name}.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} RootCoreUtils SampleHandler EventLoop EventLoopTestLib )
endfunction( _add_test )

set (MANUAL_TESTS test/gt_LSFDriver_EOS.cxx test/ut_background_driver.cxx test/ut_driver_condor.cxx test/ut_driver_ll.cxx test/ut_driver_lsf.cxx test/ut_driver_proof_farm.cxx test/ut_driver_torque.cxx test/ut_xrootd_output.cxx)

file( GLOB _tests RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/test/[a-z]*.cxx" )
foreach( source ${_tests} )
  string (REGEX REPLACE "test/(.*).cxx" "\\1" util ${source})
  list (FIND MANUAL_TESTS "${source}" _index)
  if (${_index} GREATER -1)
    message ("EventLoop_${util}" ${source} LINK_LIBRARIES EventLoop)
    add_executable ("EventLoop_${util}" ${source})
    target_link_libraries ("EventLoop_${util}" EventLoop EventLoopTestLib)
  else (${_index} GREATER -1)
    _add_gtest( test/${util}.cxx )
  endif (${_index} GREATER -1)
endforeach()

file (GLOB util_sources RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/util/[a-zA-Z0-9]*.cxx")
foreach (source ${util_sources})
  string (REGEX REPLACE "util/(.*).cxx" "\\1" util ${source})
  atlas_add_executable (${util} ${source} LINK_LIBRARIES EventLoop)
endforeach (source ${util_sources})



# Install files from the package:
atlas_install_scripts( scripts/el_retrieve scripts/el_wait )
atlas_install_data( data/*.root )
