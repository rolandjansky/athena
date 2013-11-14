/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef MuonRecDigiStoreTDS_H
#define MuonRecDigiStoreTDS_H

#include "GaudiKernel/DataObject.h"

/////////////////////////////////////////////////////////////////////////////
#include "MboyAthEvt/MboyAthEvtMisc.h"

#include "MboyAthEvt/MuonRecDigiStoreAth.h"

static const CLID CLID_MuonRecDigiStoreTDS = 4025;

  /**
   @class MuonRecDigiStoreTDS

   This class allows to store a MuonRecDigiStoreAth in TDS
   
  @author samusog@cern.ch
  
  */

class MuonRecDigiStoreTDS:public DataObject {
public:
   MuonRecDigiStoreTDS();
   virtual ~MuonRecDigiStoreTDS();

public:
///////////////////////////////////

   static const CLID& classID() { return CLID_MuonRecDigiStoreTDS; }
   virtual const CLID& clID() const { return CLID_MuonRecDigiStoreTDS; }

public:
///////////////////////////////////
//Data

   MuonRecDigiStoreAth m_MuonRecDigiStore ; //!< the MuonRecDigiStoreAth

};
#endif

