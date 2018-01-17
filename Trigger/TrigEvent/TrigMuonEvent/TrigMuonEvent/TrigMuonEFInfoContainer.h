// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENT_TRIGMUONEFINFOCONTAINER_H
#define TRIGMUONEVENT_TRIGMUONEFINFOCONTAINER_H
/********************************************************************
 
 NAME:     TrigMuonEFInfoContainer.h
 Package : offline/Trigger/TrigEvent/TrigMuonEvent 
 
 AUTHORS:  Sergio Grancagnolo
 CREATED:  April 2008
 
 PURPOSE:  This is a data object, containing a collection of TrigMuonEFInfo Objects.
********************************************************************/
 
// INCLUDE HEADER FILES:
#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "TrigMuonEvent/TrigMuonEFInfo.h"
#include "AthenaKernel/BaseInfo.h"

class TrigMuonEFInfoContainer : public DataVector<TrigMuonEFInfo> 
{
public:
  TrigMuonEFInfoContainer( SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS )
    : DataVector<TrigMuonEFInfo>( ownPolicy ) {}
  ~TrigMuonEFInfoContainer() {}
  void print();
};

CLASS_DEF( TrigMuonEFInfoContainer, 1173853172, 1)

SG_BASE(TrigMuonEFInfoContainer, DataVector<TrigMuonEFInfo>);
 
#endif // TRIGMUONEVENT_TRIGMUONEFINFOCONTAINER_H
