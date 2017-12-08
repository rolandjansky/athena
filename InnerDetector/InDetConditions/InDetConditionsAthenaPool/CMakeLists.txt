# $Id: CMakeLists.txt 726439 2016-02-26 14:46:58Z krasznaa $
################################################################################
# Package: InDetConditionsAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( InDetConditionsAthenaPool )

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
   InnerDetector/InDetConditions/InDetByteStreamErrors
   InnerDetector/InDetConditions/TRT_ConditionsData )

# Component(s) in the package:
atlas_add_poolcnv_library( InDetConditionsAthenaPoolPoolCnv src/*.cxx
   FILES TRT_ConditionsData/TRTRtFloat.h
   TRT_ConditionsData/TRTWeiFloat.h
   TRT_ConditionsData/TRTAliFloat.h
   TRT_ConditionsData/TRTStrawStatusData.h
   TRT_ConditionsData/FloatArrayStore.h
   TRT_ConditionsData/StrawT0Container.h
   TRT_ConditionsData/RtRelationContainer.h
   TRT_ConditionsData/StrawDxContainer.h
   TRT_ConditionsData/StrawT0MultChanContainer.h
   TRT_ConditionsData/RtRelationMultChanContainer.h
   TRT_ConditionsData/StrawDxMultChanContainer.h
   TRT_ConditionsData/StrawStatusMultChanContainer.h
   TRT_ConditionsData/StrawStatusContainer.h
   InDetByteStreamErrors/InDetBSErrContainer.h
   InDetByteStreamErrors/TRT_BSIdErrContainer.h
   InDetByteStreamErrors/TRT_BSErrContainer.h
   TYPES_WITH_NAMESPACE TRTCond::StrawT0Container
   TRTCond::RtRelationContainer TRTCond::StrawDxContainer
   TRTCond::StrawT0MultChanContainer TRTCond::RtRelationMultChanContainer
   TRTCond::StrawDxMultChanContainer TRTCond::StrawStatusMultChanContainer
   TRTCond::StrawStatusContainer
   MULT_CHAN_TYPES TRTCond::StrawT0MultChanContainer
   TRTCond::RtRelationMultChanContainer TRTCond::StrawDxMultChanContainer
   TRTCond::StrawStatusMultChanContainer
   LINK_LIBRARIES AthenaPoolUtilities Identifier TestTools AthAllocators
   AthenaPoolCnvSvcLib TRT_ConditionsData )

atlas_add_dictionary( InDetConditionsAthenaPoolCnvDict
   InDetConditionsAthenaPool/InDetConditionsAthenaPoolCnvDict.h
   InDetConditionsAthenaPool/selection.xml
   LINK_LIBRARIES AthenaPoolUtilities Identifier TestTools AthAllocators
   AthenaPoolCnvSvcLib TRT_ConditionsData )

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
atlas_install_headers( InDetConditionsAthenaPool )
