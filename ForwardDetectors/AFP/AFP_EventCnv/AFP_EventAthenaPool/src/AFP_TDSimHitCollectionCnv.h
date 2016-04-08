/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_TDSimHitCollectionCnv_h
#define AFP_TDSimHitCollectionCnv_h

#include "AFP_SimEv/AFP_TDSimHitCollection.h"
#include "AFP_EventTPCnv/AFP_TDSimHitCollection_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "GaudiKernel/MsgStream.h"

typedef AFP_TDSimHitCollection_p1  AFP_TDSimHitCollection_PERS;

class AFP_TDSimHitCollectionCnv : public T_AthenaPoolCustomCnv <AFP_TDSimHitCollection, AFP_TDSimHitCollection_PERS>
{
	friend class CnvFactory<AFP_TDSimHitCollectionCnv>;

protected:
	AFP_TDSimHitCollectionCnv(ISvcLocator* svcloc) : T_AthenaPoolCustomCnv <AFP_TDSimHitCollection, AFP_TDSimHitCollection_PERS >(svcloc) {}

	AFP_TDSimHitCollection_PERS* createPersistent (AFP_TDSimHitCollection *transCont);
	AFP_TDSimHitCollection* createTransient ();
 };

#endif //AFP_TDSimHitCollectionCnv_h
