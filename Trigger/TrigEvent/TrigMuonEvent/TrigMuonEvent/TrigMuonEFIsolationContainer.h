// dear emacs, this is -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUONEVENT_TRIGMUONEFISOLATIONCONTAINER_H__
#define TRIGMUONEVENT_TRIGMUONEFISOLATIONCONTAINER_H__
/**
 * @class TrigMuonEFIsolationContainer
 *
 * Data object to hold a collection of TrigMuonEFIsolation objects.
 *
 * @author Mark Owen
 */

// athena / gaudi includes
#include "AthContainers/DataVector.h"
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/BaseInfo.h"

// local includes
#include "TrigMuonEvent/TrigMuonEFIsolation.h"

class TrigMuonEFIsolationContainer : public DataVector<TrigMuonEFIsolation> {

 public:
  /// standard container constructor
  TrigMuonEFIsolationContainer( SG::OwnershipPolicy ownPolicy = SG::OWN_ELEMENTS) :
    DataVector<TrigMuonEFIsolation>( ownPolicy ) {
  }
    
  /// Destructor
  ~TrigMuonEFIsolationContainer() {}
  
}; //class TrigMuonEFIsolationContainer

CLASS_DEF( TrigMuonEFIsolationContainer , 1175295175 , 1 )

SG_BASE( TrigMuonEFIsolationContainer, DataVector<TrigMuonEFIsolation>);

#endif //TRIGMUONEVENT_TRIGMUONEFISOLATIONCONTAINER_H__
