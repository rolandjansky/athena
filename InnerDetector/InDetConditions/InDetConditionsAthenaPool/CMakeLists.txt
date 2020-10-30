# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( InDetConditionsAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( InDetConditionsAthenaPoolPoolCnv src/*.cxx
   FILES TRT_ConditionsData/TRTStrawStatusData.h
   TRT_ConditionsData/FloatArrayStore.h
   TRT_ConditionsData/StrawT0Container.h
   TRT_ConditionsData/RtRelationContainer.h
   TRT_ConditionsData/StrawDxContainer.h
   TRT_ConditionsData/StrawT0MultChanContainer.h
   TRT_ConditionsData/RtRelationMultChanContainer.h
   TRT_ConditionsData/StrawDxMultChanContainer.h
   TRT_ConditionsData/StrawStatusMultChanContainer.h
   TRT_ConditionsData/StrawStatusContainer.h
   TYPES_WITH_NAMESPACE TRTCond::StrawT0Container
   TRTCond::RtRelationContainer TRTCond::StrawDxContainer
   TRTCond::StrawT0MultChanContainer TRTCond::RtRelationMultChanContainer
   TRTCond::StrawDxMultChanContainer TRTCond::StrawStatusMultChanContainer
   TRTCond::StrawStatusContainer
   MULT_CHAN_TYPES TRTCond::StrawT0MultChanContainer
   TRTCond::RtRelationMultChanContainer
   TRTCond::StrawDxMultChanContainer
   TRTCond::StrawStatusMultChanContainer
   LINK_LIBRARIES AthenaPoolUtilities
   AthenaPoolCnvSvcLib TRT_ConditionsData )

atlas_add_dictionary( InDetConditionsAthenaPoolCnvDict
   InDetConditionsAthenaPool/InDetConditionsAthenaPoolCnvDict.h
   InDetConditionsAthenaPool/selection.xml
   LINK_LIBRARIES AthenaPoolUtilities
   AthenaPoolCnvSvcLib TRT_ConditionsData )
