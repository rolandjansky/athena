# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack


atlas_subdir( SystematicsHandles )

atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/AthToolSupport/AsgTools
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   PhysicsAnalysis/D3PDTools/AnaAlgorithm
   Event/xAOD/xAODBase
   Event/xAOD/xAODCore
   Event/xAOD/xAODEventInfo
   PRIVATE
   Control/xAODRootAccess
   Event/xAOD/xAODJet
   Event/xAOD/xAODMuon
   Event/xAOD/xAODEgamma
   Event/xAOD/xAODTau
   PhysicsAnalysis/D3PDTools/RootCoreUtils )

atlas_add_library( SystematicsHandlesLib
   SystematicsHandles/*.h SystematicsHandles/*.icc Root/*.cxx
   PUBLIC_HEADERS SystematicsHandles
   LINK_LIBRARIES AsgTools PATInterfaces AnaAlgorithmLib xAODBase xAODCore AthContainers xAODEventInfo
   PRIVATE_LINK_LIBRARIES RootCoreUtils
   xAODJet xAODMuon xAODEgamma xAODTau )

atlas_add_test( cc_SysReadHandle
   SOURCES test/cc_SysReadHandle.cxx
   LINK_LIBRARIES AsgTools AnaAlgorithmLib SystematicsHandlesLib )

atlas_add_test( cc_SysCopyHandle
   SOURCES test/cc_SysCopyHandle.cxx
   LINK_LIBRARIES AsgTools AnaAlgorithmLib xAODEventInfo xAODJet
   SystematicsHandlesLib )

if( XAOD_STANDALONE )
   atlas_add_test( ut_CopyHelpers
      SOURCES test/ut_CopyHelpers.cxx
      LINK_LIBRARIES xAODRootAccess AsgTools xAODBase xAODJet
      SystematicsHandlesLib )
endif()

atlas_add_test( cc_SysWriteHandle
   SOURCES test/cc_SysWriteHandle.cxx
   LINK_LIBRARIES AsgTools AnaAlgorithmLib SystematicsHandlesLib )

atlas_add_test( cc_SysListHandle
   SOURCES test/cc_SysListHandle.cxx
   LINK_LIBRARIES AsgTools AnaAlgorithmLib SystematicsHandlesLib )

atlas_add_test( cc_SysDecorationHandle
   SOURCES test/cc_SysDecorationHandle.cxx
   LINK_LIBRARIES AsgTools AnaAlgorithmLib SystematicsHandlesLib )
