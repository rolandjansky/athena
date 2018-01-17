// -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENT_TRIGMUONEFCONTAINER_H
#define TRIGMUONEVENT_TRIGMUONEFCONTAINER_H
/********************************************************************
 
 NAME:     TrigMuonEFContainer.h
 Package : offline/Trigger/TrigEvent/TrigMuonEvent 
 
 AUTHORS:  Gabriella Cataldi
 CREATED:  November 2006
 
 PURPOSE:  This is a data object, containing a collection of TrigMuonEF Objects.
********************************************************************/
 
// INCLUDE HEADER FILES:
#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "TrigMuonEvent/TrigMuonEF.h"
#include "AthenaKernel/BaseInfo.h"

class TrigMuonEFContainer : public DataVector<TrigMuonEF> 
{ 
public:
  TrigMuonEFContainer( SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS )
    : DataVector<TrigMuonEF>( ownPolicy ) {}
  ~TrigMuonEFContainer() {}
  void print();
};

CLASS_DEF(TrigMuonEFContainer, 1173853171, 1)

SG_BASE(TrigMuonEFContainer, DataVector<TrigMuonEF>);
  
#endif // TRIGMUONEVENT_TRIGMUONEFCONTAINER_H
