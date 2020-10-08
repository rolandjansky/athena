# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack


atlas_subdir( SystematicsHandles )

atlas_add_library( SystematicsHandlesLib
   SystematicsHandles/*.h SystematicsHandles/*.icc Root/*.cxx
   PUBLIC_HEADERS SystematicsHandles
   LINK_LIBRARIES AsgTools PATInterfaces AnaAlgorithmLib xAODBase xAODCore AthContainers xAODEventInfo
   PRIVATE_LINK_LIBRARIES RootCoreUtils
   xAODJet xAODMuon xAODEgamma xAODTau )

if( XAOD_STANDALONE )
   atlas_add_test( ut_CopyHelpers
      SOURCES test/ut_CopyHelpers.cxx
      LINK_LIBRARIES xAODRootAccess AsgTools xAODBase xAODJet
      SystematicsHandlesLib )
endif()
