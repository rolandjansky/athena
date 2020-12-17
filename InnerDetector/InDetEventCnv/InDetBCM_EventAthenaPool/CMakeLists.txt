# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetBCM_EventAthenaPool )

# Component(s) in the package:
atlas_add_library( InDetBCM_EventAthenaPool
   InDetBCM_EventAthenaPool/*.h src/*.h src/*.cxx
   PUBLIC_HEADERS InDetBCM_EventAthenaPool
   LINK_LIBRARIES AthenaPoolCnvSvcLib GaudiKernel InDetBCM_RawData )

atlas_add_poolcnv_library( InDetBCM_EventAthenaPoolPoolCnv
   FILES InDetBCM_RawData/BCM_RDO_Container.h
   LINK_LIBRARIES InDetBCM_EventAthenaPool )

atlas_add_dictionary( InDetBCM_EventAthenaPoolCnvDict
   InDetBCM_EventAthenaPool/InDetBCM_EventAthenaPoolCnvDict.h
   InDetBCM_EventAthenaPool/selection.xml
   LINK_LIBRARIES InDetBCM_EventAthenaPool )

# Unit tests in the package.
atlas_add_test( BCM_RawDataCnv_p0_test
   SOURCES test/BCM_RawDataCnv_p0_test.cxx
   LINK_LIBRARIES InDetBCM_EventAthenaPool )

atlas_add_test( BCM_RDO_CollectionCnv_p0_test
   SOURCES test/BCM_RDO_CollectionCnv_p0_test.cxx
   LINK_LIBRARIES InDetBCM_EventAthenaPool )

atlas_add_test( BCM_RDO_ContainerCnv_p0_test
   SOURCES test/BCM_RDO_ContainerCnv_p0_test.cxx
   LINK_LIBRARIES InDetBCM_EventAthenaPool )

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( test/*.py POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Set up (a) test(s) for the converter(s):
find_package( AthenaPoolUtilitiesTest )

if( ATHENAPOOLUTILITIESTEST_FOUND )
  run_tpcnv_test( InDetBCM_EventAthenaPool_20.1.7.2   ESD-20.1.7.2 )
else()
   message( WARNING
      "Couldn't find AthenaPoolUtilitiesTest. No test(s) set up." )
endif()
