/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
@page TRT_ConditionsData_page TRT_ConditionsData package

@author phansen@nbi.dk

@section TRT_ConditionsData_TRT_ConditionsDataIntro Introduction

This package provides TDS objects for storing TRT Conditions data.

@section TRT_ConditionsData_TRT_ConditionsDataOverview Class Overview

  -  ArrayStore: Templated indexed storage of objects
  -  FloatArrayStore : Specialization to a float array object and  an Identifier index
  -  FloatArrayStoreContainer : A CondMultChanCollection (this is how test beam constants are stored)
  -  TRTCond::NestedContainer : A container type with nested granularity levels, each holding a default.
  -  TRTCond::ExpandedIdentifier : Identifier for TRT Conditions data. Specifies "level of granularity".
  -  TRTCond::MultChanContainer : A CondMultChanCollection of NestedContainer at level LAYERWHEEL.
  -  TRTCond::RtRelation : Base class accessing to R(t) constants in the TRTCond namespace
  -  TRTCond::BasicRtRelation : R(t) relation represented by a float par[4] of polynomial coefficients
  -  TRTCond::BinnedRtRelation : R(t) relation represented by a vector<float> of look-up table values.
  -  TRTCond::RtRelationContainer : A NestedContainer of RtRelation instances
  -  TRTCond::RtRelationMultChanContainer : A MultChanContainer
  -  TRTCond::RtRelationFactory : Read/Write from/to text file
  -  TRTCond::StrawDx : A separate Rphi shift for each end of a wire 
  -  TRTCond::StrawDxContainer :  A NestedContainer of StrawDx instances
  -  TRTCond::StrawDxMultChanContainer :  A MultChanContainer
  -  TRTCond::StrawT0 : A class holding T0s
  -  TRTCond::StrawT0Container :  A NestedContainer of StrawT0 instances
  -  TRTCond::StrawT0MultChanContainer :  A MultChanContainer
  -  TRTCond::StrawStatus : A class holding dead/noisy status
  -  TRTCond::StrawStatusContainer :  A NestedContainer of StrawStatus instances
  -  TRTCond::StrawStatusMultChanContainer :  A MultChanContainer
  -  TRTCond::LinearPacker :  Packs a value into a PackedType e.g. a char
  -  TRTAlifloat : DEPRECIATED
  -  TRTRtfloat : DEPRECIATED
  -  TRTWeifloat : DEPRECIATED
  -  TRTStrawStatusData :  A class holding dead/noisy status (to be relaced by TRTCond::StrawStatus)
 


*/
/**


*/
/**

*/
