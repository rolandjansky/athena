# $Id: CMakeLists.txt 787195 2016-11-29 19:49:18Z krasznaa $

# The name of the package:
atlas_subdir( EventLoop )

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
   ROOT_HEADERS EventLoop/Algorithm.h EventLoop/BackgroundDriver.h
   EventLoop/BackgroundJob.h EventLoop/BackgroundTSelector.h
   EventLoop/BackgroundWorker.h EventLoop/BatchDriver.h
   EventLoop/BatchJob.h EventLoop/BatchSegment.h EventLoop/BatchSample.h
   EventLoop/BatchWorker.h EventLoop/CondorDriver.h EventLoop/DirectDriver.h
   EventLoop/D3PDReaderSvc.h EventLoop/GEDriver.h EventLoop/Job.h
   EventLoop/LLDriver.h EventLoop/LSFDriver.h EventLoop/LocalDriver.h
   EventLoop/OutputStream.h EventLoop/MetricsSvc.h EventLoop/ProofArgs.h
   EventLoop/ProofDriver.h EventLoop/ProofTSelector.h EventLoop/SoGEDriver.h
   EventLoop/StatusCode.h EventLoop/TEventSvc.h EventLoop/TorqueDriver.h
   EventLoop/UnitTestAlg.h EventLoop/UnitTestAlg1.h EventLoop/VomsProxySvc.h
   EventLoop/Worker.h EventLoop/ProofWorker.h EventLoop/SlurmDriver.h
   Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

atlas_add_library( EventLoop
   EventLoop/*.h EventLoop/*.ihh Root/*.cxx ${EventLoopDictSource}
   PUBLIC_HEADERS EventLoop
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} ${GTEST_LIBRARIES} RootCoreUtils
   SampleHandler
   PRIVATE_LINK_LIBRARIES AsgTools xAODRootAccess )

# Test(s) in the package:
function( _add_gtest cxx_name )
   get_filename_component( test_name ${cxx_name} NAME_WE )
   atlas_add_test( ${test_name}
      SOURCES test/${test_name}.cxx
      INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
      LINK_LIBRARIES ${GTEST_LIBRARIES} RootCoreUtils AsgTools
      SampleHandler EventLoop )
endfunction( _add_gtest )

file( GLOB _gtests test/gt_*.cxx )
foreach( gtest ${_gtests} )
   _add_gtest( ${gtest} )
endforeach()

function( _add_test cxx_name )
   get_filename_component( test_name ${cxx_name} NAME_WE )
   atlas_add_test( ${test_name}
      SOURCES test/${test_name}.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} RootCoreUtils SampleHandler EventLoop )
endfunction( _add_test )

file( GLOB _tests test/ut_*.cxx )
foreach( test ${_tests} )
   _add_gtest( ${test} )
endforeach()

# Install files from the package:
atlas_install_scripts( scripts/el_retrieve scripts/el_wait )
atlas_install_data( data/*.root )
