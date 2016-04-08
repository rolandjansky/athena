/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRTCONDITIONSDATA_TRTCONDITIONSDICT_H
#define TRTCONDITIONSDATA_TRTCONDITIONSDICT_H

#include "TRT_ConditionsData/TRTStrawStatusData.h"
#include "TRT_ConditionsData/FloatArrayStore.h" 
#include "TRT_ConditionsData/FloatArrayStoreContainer.h" 
#include "TRT_ConditionsData/NestedContainer.h"
#include "TRT_ConditionsData/StrawT0Container.h"
#include "TRT_ConditionsData/RtRelationContainer.h"
#include "TRT_ConditionsData/StrawT0MultChanContainer.h"
#include "TRT_ConditionsData/RtRelationMultChanContainer.h"
#include "TRT_ConditionsData/BasicRtRelation.h"
#include "TRT_ConditionsData/DinesRtRelation.h"
#include "TRT_ConditionsData/BinnedRtRelation.h"
#include "TRT_ConditionsData/StrawDxMultChanContainer.h"

#include "TRT_ConditionsData/StrawStatusContainer.h"
#include "TRT_ConditionsData/StrawStatusMultChanContainer.h"


struct TRT_ConditionsDataDict_Dummy {
  TRTCond::StrawT0Container x ;
  TRTCond::RtRelationContainer y ;
  TRTCond::StrawT0MultChanContainer xx ;
  TRTCond::RtRelationMultChanContainer yy ;
  TRTCond::StrawDxMultChanContainer aa ;
  TRTCond::BasicRtRelation Brtr ;
  TRTCond::DinesRtRelation Drtr ;
  // I don't understand why I need this one, but without it it will not generate the dictionary
  TRTCond::NestedContainer<1,TRTCond::StrawT0,TRTCond::StrawT0Trait> zz ;
  TRTCond::NestedContainer<1,TRTCond::StrawDx,TRTCond::StrawDxTrait> bb ;
  TRTCond::StrawStatusMultChanContainer t ;
  TRTCond::NestedContainer<1,TRTCond::StrawStatus,TRTCond::DeadStrawTrait> tt1 ;
  TRTCond::NestedContainer<3,TRTCond::StrawStatus,TRTCond::DeadStrawTrait> tt3;
  TRTCond::NestedContainer<4,TRTCond::StrawStatus,TRTCond::DeadStrawTrait> tt4;
  TRTCond::NestedContainerBase<3,TRTCond::NestedContainer<4,TRTCond::StrawStatus,TRTCond::DeadStrawTrait>,TRTCond::StrawStatus,TRTCond::DeadStrawTrait> ttb3;
  TRTCond::NestedContainerBase<4,TRTCond::StrawStatus,TRTCond::StrawStatus,TRTCond::DeadStrawTrait> ttb4;
  std::vector<TRTCond::NestedContainer<4,TRTCond::StrawStatus,TRTCond::DeadStrawTrait> > v1;
  std::vector<TRTCond::StrawStatus> v2;
  TRTCond::NestedContainer<3,TRTCond::StrawT0,TRTCond::StrawT0Trait> nc3;
  TRTCond::NestedContainer<4,TRTCond::StrawT0,TRTCond::StrawT0Trait> nc4;
  TRTCond::NestedContainerBase<3,TRTCond::NestedContainer<4,TRTCond::StrawT0,TRTCond::StrawT0Trait>,TRTCond::StrawT0,TRTCond::StrawT0Trait> ncb3;

  TRTCond::NestedContainerBase<4,TRTCond::StrawT0,TRTCond::StrawT0,TRTCond::StrawT0Trait> tp01;
  std::vector<TRTCond::NestedContainer<4,TRTCond::StrawT0,TRTCond::StrawT0Trait> > tp02;
  std::vector<TRTCond::StrawT0> tp03;
  TRTCond::NestedContainer<3,TRTCond::StrawDx,TRTCond::StrawDxTrait> tp04;
  TRTCond::NestedContainer<4,TRTCond::StrawDx,TRTCond::StrawDxTrait> tp05;
  TRTCond::NestedContainerBase<3,TRTCond::NestedContainer<4,TRTCond::StrawDx,TRTCond::StrawDxTrait>,TRTCond::StrawDx,TRTCond::StrawDxTrait> tp06;
  TRTCond::NestedContainerBase<4,TRTCond::StrawDx,TRTCond::StrawDx,TRTCond::StrawDxTrait> tp07;
  std::vector<TRTCond::NestedContainer<4,TRTCond::StrawDx,TRTCond::StrawDxTrait> > tp08;
  std::vector<TRTCond::StrawDx> tp09;
  TRTCond::NestedContainerPointerTrait<TRTCond::RtRelation*> tp010;
  TRTCond::NestedContainer<1,TRTCond::RtRelation*,TRTCond::NestedContainerPointerTrait<TRTCond::RtRelation*> > tp011;
  TRTCond::NestedContainer<3,TRTCond::RtRelation*,TRTCond::NestedContainerPointerTrait<TRTCond::RtRelation*> > tp012;
  TRTCond::NestedContainer<4,TRTCond::RtRelation*,TRTCond::NestedContainerPointerTrait<TRTCond::RtRelation*> > tp013;
  TRTCond::NestedContainerBase<1,TRTCond::NestedContainer<2,TRTCond::RtRelation*,TRTCond::NestedContainerPointerTrait<TRTCond::RtRelation*> >,TRTCond::RtRelation*,TRTCond::NestedContainerPointerTrait<TRTCond::RtRelation*> > tp014;
  TRTCond::NestedContainerBase<3,TRTCond::NestedContainer<4,TRTCond::RtRelation*,TRTCond::NestedContainerPointerTrait<TRTCond::RtRelation*> >,TRTCond::RtRelation*,TRTCond::NestedContainerPointerTrait<TRTCond::RtRelation*> > tp015;
  TRTCond::NestedContainerBase<4,TRTCond::RtRelation*,TRTCond::RtRelation*,TRTCond::NestedContainerPointerTrait<TRTCond::RtRelation*> > tp016;
  std::vector<TRTCond::NestedContainer<2,TRTCond::RtRelation*,TRTCond::NestedContainerPointerTrait<TRTCond::RtRelation*> > > tp017;
  std::vector<TRTCond::NestedContainer<4,TRTCond::RtRelation*,TRTCond::NestedContainerPointerTrait<TRTCond::RtRelation*> > > tp018;
  std::vector<TRTCond::RtRelation*> tp019;

};

#endif
