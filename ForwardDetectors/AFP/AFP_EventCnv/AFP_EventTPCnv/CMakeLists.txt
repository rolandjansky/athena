# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( AFP_EventTPCnv )

# Component(s) in the package.
atlas_add_tpcnv_library( AFP_EventTPCnv
   AFP_EventTPCnv/*.h src/*.cxx
   PUBLIC_HEADERS AFP_EventTPCnv
   LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib AFP_DigiEv
   AFP_LocRecoEv AFP_SimEv AthenaPoolCnvSvcLib
   PRIVATE_LINK_LIBRARIES Identifier GaudiKernel )

atlas_add_dictionary( AFP_EventTPCnvDict
   AFP_EventTPCnv/AFP_EventTPCnvDict.h
   AFP_EventTPCnv/selection.xml
   LINK_LIBRARIES AFP_EventTPCnv )

# Test(s) in the package.
foreach( test AFP_SiDigiCnv_p1_test
   AFP_SIDLocRecoEventCnv_p1_test AFP_SIDLocRecoEvCollectionCnv_p1_test
   AFP_SIDSimHitCnv_p1_test AFP_TDDigiCnv_p1_test AFP_TDLocRecoEventCnv_p1_test
   AFP_TDLocRecoEvCollectionCnv_p1_test AFP_TDSimHitCnv_p1_test )

   atlas_add_test( ${test}
      SOURCES test/${test}.cxx
      LINK_LIBRARIES GaudiKernel TestTools AFP_EventTPCnv )

endforeach()


