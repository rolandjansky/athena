################################################################################
# Package: InDetByteStreamErrorsAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( InDetByteStreamErrorsAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Database/AthenaPOOL/AthenaPoolUtilities
   DetectorDescription/Identifier
   PRIVATE
   GaudiKernel
   AtlasTest/TestTools
   Control/AthAllocators
   Database/AthenaPOOL/AthenaPoolCnvSvc
   InnerDetector/InDetConditions/InDetByteStreamErrors )

# Component(s) in the package:
atlas_add_poolcnv_library( InDetByteStreamErrorsAthenaPoolPoolCnv src/*.cxx
   FILES InDetByteStreamErrors/InDetBSErrContainer.h
   InDetByteStreamErrors/TRT_BSIdErrContainer.h
   InDetByteStreamErrors/TRT_BSErrContainer.h
   LINK_LIBRARIES AthenaPoolUtilities Identifier TestTools AthAllocators
   AthenaPoolCnvSvcLib )

atlas_add_dictionary( InDetByteStreamErrorsAthenaPoolCnvDict
   InDetByteStreamErrorsAthenaPool/InDetByteStreamErrorsAthenaPoolCnvDict.h
   InDetByteStreamErrorsAthenaPool/selection.xml
   LINK_LIBRARIES AthenaPoolUtilities Identifier TestTools AthAllocators
   AthenaPoolCnvSvcLib )

# Tests in the package:
atlas_add_test( InDetBSErrContainerCnv_p1_test
   SOURCES test/InDetBSErrContainerCnv_p1_test.cxx
   src/InDetBSErrContainerCnv_p1.cxx
   LINK_LIBRARIES GaudiKernel AthenaPoolCnvSvcLib Identifier AthAllocators )

atlas_add_test( TRT_BSErrContainerCnv_p1_test
   SOURCES test/TRT_BSErrContainerCnv_p1_test.cxx
   src/TRT_BSErrContainerCnv_p1.cxx
   LINK_LIBRARIES GaudiKernel AthenaPoolCnvSvcLib Identifier AthAllocators )

atlas_add_test( TRT_BSIdErrContainerCnv_p1_test
   SOURCES test/TRT_BSIdErrContainerCnv_p1_test.cxx
   src/TRT_BSIdErrContainerCnv_p1.cxx
   LINK_LIBRARIES GaudiKernel AthenaPoolCnvSvcLib Identifier AthAllocators )

# Install files from the package:
atlas_install_headers( InDetByteStreamErrorsAthenaPool )
