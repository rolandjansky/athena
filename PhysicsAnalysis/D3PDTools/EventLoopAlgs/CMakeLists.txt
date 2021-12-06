# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Set the name of the package:
atlas_subdir( EventLoopAlgs )

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
   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES EventLoop MultiDraw RootCoreUtils SampleHandler
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AsgMessagingLib xAODEventInfo xAODRootAccess )

# Test(s) in the package:
atlas_add_test( gt_DuplicateChecker
   SOURCES test/gt_DuplicateChecker.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgMessagingLib AsgTestingLib AthContainers
   EventLoop EventLoopAlgs RootCoreUtils SampleHandler xAODEventInfo xAODRootAccess
   POST_EXEC_SCRIPT nopost.sh )
set_tests_properties (EventLoopAlgs_gt_DuplicateChecker_ctest PROPERTIES LABELS "EventLoopAlgs;EventLoop;SampleHandler" )

atlas_add_test( ut_alg_skim
   SOURCES test/ut_alg_skim.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} EventLoop EventLoopAlgs RootCoreUtils SampleHandler
   POST_EXEC_SCRIPT nopost.sh )
set_tests_properties (EventLoopAlgs_ut_alg_skim_ctest PROPERTIES LABELS "EventLoopAlgs;EventLoop;SampleHandler" )
