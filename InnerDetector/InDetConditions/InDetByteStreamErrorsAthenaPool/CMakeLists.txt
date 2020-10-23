# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetByteStreamErrorsAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( InDetByteStreamErrorsAthenaPoolPoolCnv src/*.cxx
   FILES InDetByteStreamErrors/InDetBSErrContainer.h
   InDetByteStreamErrors/TRT_BSIdErrContainer.h
   InDetByteStreamErrors/TRT_BSErrContainer.h
   InDetByteStreamErrors/IDCInDetBSErrContainer.h
   LINK_LIBRARIES Identifier InDetByteStreamErrors AthAllocators AthenaPoolCnvSvcLib )

atlas_add_dictionary( InDetByteStreamErrorsAthenaPoolCnvDict
   InDetByteStreamErrorsAthenaPool/InDetByteStreamErrorsAthenaPoolCnvDict.h
   InDetByteStreamErrorsAthenaPool/selection.xml
   LINK_LIBRARIES Identifier )

# Tests in the package:
atlas_add_test( InDetBSErrContainerCnv_p1_test
   SOURCES test/InDetBSErrContainerCnv_p1_test.cxx
   src/InDetBSErrContainerCnv_p1.cxx
   LINK_LIBRARIES GaudiKernel AthenaPoolCnvSvcLib Identifier InDetByteStreamErrors )

atlas_add_test( IDCInDetBSErrContainerCnv_p1_test
   SOURCES test/IDCInDetBSErrContainerCnv_p1_test.cxx
   src/IDCInDetBSErrContainerCnv_p1.cxx
   LINK_LIBRARIES GaudiKernel AthenaPoolCnvSvcLib Identifier InDetByteStreamErrors )

#atlas_add_test( TRT_BSErrContainerCnv_p1_test
#   SOURCES test/TRT_BSErrContainerCnv_p1_test.cxx
#   src/TRT_BSErrContainerCnv_p1.cxx
#   LINK_LIBRARIES GaudiKernel AthenaPoolCnvSvcLib Identifier AthAllocators InDetByteStreamErrors )

atlas_add_test( TRT_BSIdErrContainerCnv_p1_test
   SOURCES test/TRT_BSIdErrContainerCnv_p1_test.cxx
   src/TRT_BSIdErrContainerCnv_p1.cxx
   LINK_LIBRARIES GaudiKernel AthenaPoolCnvSvcLib Identifier AthAllocators InDetByteStreamErrors )
