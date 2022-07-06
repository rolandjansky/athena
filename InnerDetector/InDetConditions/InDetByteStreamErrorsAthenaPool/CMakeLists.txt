# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetByteStreamErrorsAthenaPool )

# Component(s) in the package:
atlas_add_tpcnv_library( InDetByteStreamErrorsAthenaPool
   src/*.cxx
   PUBLIC_HEADERS InDetByteStreamErrorsAthenaPool
   LINK_LIBRARIES AthAllocators AthenaPoolCnvSvcLib Identifier InDetByteStreamErrors )

atlas_add_poolcnv_library( InDetByteStreamErrorsAthenaPoolPoolCnv
   FILES InDetByteStreamErrors/InDetBSErrContainer.h
   InDetByteStreamErrors/TRT_BSIdErrContainer.h
   InDetByteStreamErrors/TRT_BSErrContainer.h
   InDetByteStreamErrors/IDCInDetBSErrContainer.h
   LINK_LIBRARIES InDetByteStreamErrorsAthenaPool )

atlas_add_dictionary( InDetByteStreamErrorsAthenaPoolCnvDict
   InDetByteStreamErrorsAthenaPool/InDetByteStreamErrorsAthenaPoolCnvDict.h
   InDetByteStreamErrorsAthenaPool/selection.xml
   LINK_LIBRARIES InDetByteStreamErrorsAthenaPool )

# Tests in the package:
atlas_add_test( InDetBSErrContainerCnv_p1_test
   SOURCES test/InDetBSErrContainerCnv_p1_test.cxx
   src/InDetBSErrContainerCnv_p1.cxx
   LINK_LIBRARIES GaudiKernel AthenaPoolCnvSvcLib Identifier InDetByteStreamErrors InDetByteStreamErrorsAthenaPool )

atlas_add_test( IDCInDetBSErrContainerCnv_p1_test
   SOURCES test/IDCInDetBSErrContainerCnv_p1_test.cxx
   src/IDCInDetBSErrContainerCnv_p1.cxx
   LINK_LIBRARIES GaudiKernel AthenaPoolCnvSvcLib Identifier InDetByteStreamErrors InDetByteStreamErrorsAthenaPool )

#atlas_add_test( TRT_BSErrContainerCnv_p1_test
#   SOURCES test/TRT_BSErrContainerCnv_p1_test.cxx
#   src/TRT_BSErrContainerCnv_p1.cxx
#   LINK_LIBRARIES GaudiKernel AthenaPoolCnvSvcLib Identifier AthAllocators InDetByteStreamErrors InDetByteStreamErrorsAthenaPool )

atlas_add_test( TRT_BSIdErrContainerCnv_p1_test
   SOURCES test/TRT_BSIdErrContainerCnv_p1_test.cxx
   src/TRT_BSIdErrContainerCnv_p1.cxx
   LINK_LIBRARIES GaudiKernel AthenaPoolCnvSvcLib Identifier AthAllocators InDetByteStreamErrors InDetByteStreamErrorsAthenaPool )
