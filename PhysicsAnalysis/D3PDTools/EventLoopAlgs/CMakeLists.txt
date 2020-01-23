# $Id: CMakeLists.txt 777930 2016-10-11 22:30:28Z krasznaa $

# Set the name of the package:
atlas_subdir( EventLoopAlgs )

# Set up the dependencies of the package:
atlas_depends_on_subdirs(
   PUBLIC
   PhysicsAnalysis/D3PDTools/EventLoop
   PhysicsAnalysis/D3PDTools/MultiDraw
   PhysicsAnalysis/D3PDTools/SampleHandler
   PRIVATE
   Control/AthContainers
   Control/AthToolSupport/AsgTools
   Control/AthToolSupport/AsgTesting
   Event/xAOD/xAODEventInfo
   PhysicsAnalysis/D3PDTools/RootCoreUtils )

# External(s):
find_package( ROOT COMPONENTS Core Tree RIO Hist )

# Libraries in the package:
atlas_add_root_dictionary( EventLoopAlgs EventLoopAlgsDictSource
   ROOT_HEADERS EventLoopAlgs/AlgSelect.h EventLoopAlgs/DuplicateChecker.h
   EventLoopAlgs/NTupleSvc.h EventLoopAlgs/UnitTestSkim.h
   EventLoopAlgs/WhiteBoardSvc.h SampleHandler/MetaData.h
   SampleHandler/MetaVector.h Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

atlas_add_library( EventLoopAlgs
   EventLoopAlgs/*.h Root/*.cxx ${EventLoopAlgsDictSource}
   PUBLIC_HEADERS EventLoopAlgs
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} EventLoop MultiDraw SampleHandler
   PRIVATE_LINK_LIBRARIES xAODEventInfo )

# Test(s) in the package:
atlas_add_test( gt_DuplicateChecker
   SOURCES test/gt_DuplicateChecker.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AsgTools RootCoreUtils AsgTestingLib
   SampleHandler EventLoop xAODEventInfo xAODRootAccess EventLoopAlgs )
set_tests_properties (EventLoopAlgs_gt_DuplicateChecker_ctest PROPERTIES LABELS "EventLoopAlgs;EventLoop;SampleHandler" )

atlas_add_test( ut_alg_skim
   SOURCES test/ut_alg_skim.cxx
   LINK_LIBRARIES EventLoop RootCoreUtils SampleHandler EventLoopAlgs )
set_tests_properties (EventLoopAlgs_ut_alg_skim_ctest PROPERTIES LABELS "EventLoopAlgs;EventLoop;SampleHandler" )
