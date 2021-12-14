# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# @author Nils Krumnack


atlas_subdir( SystematicsHandles )

atlas_add_library( SystematicsHandlesLib
   SystematicsHandles/*.h SystematicsHandles/*.icc Root/*.cxx
   PUBLIC_HEADERS SystematicsHandles
   LINK_LIBRARIES AsgTools PATInterfaces AnaAlgorithmLib xAODBase xAODCore AthContainers xAODEventInfo
   AsgServicesLib AsgDataHandlesLib
   PRIVATE_LINK_LIBRARIES RootCoreUtils
   xAODJet xAODMuon xAODEgamma xAODTau )

atlas_add_dictionary( SystematicsHandlesDict
   SystematicsHandles/SystematicsHandlesDict.h
   SystematicsHandles/selection.xml
   LINK_LIBRARIES SystematicsHandlesLib )

if( NOT XAOD_STANDALONE )
   atlas_add_component( SystematicsHandles
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES GaudiKernel SystematicsHandlesLib )
endif()

if( XAOD_STANDALONE )
   atlas_add_test( ut_CopyHelpers
      SOURCES test/ut_CopyHelpers.cxx
      LINK_LIBRARIES xAODRootAccess AsgTools xAODBase xAODJet
      SystematicsHandlesLib
      POST_EXEC_SCRIPT nopost.sh )
endif()
