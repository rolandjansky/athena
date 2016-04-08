/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SIDSimHitCollectionCnv_h
#define AFP_SIDSimHitCollectionCnv_h

#include "AFP_SimEv/AFP_SIDSimHitCollection.h"
#include "AFP_EventTPCnv/AFP_SIDSimHitCollection_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "GaudiKernel/MsgStream.h"

typedef AFP_SIDSimHitCollection_p1  AFP_SIDSimHitCollection_PERS;

class AFP_SIDSimHitCollectionCnv : public T_AthenaPoolCustomCnv <AFP_SIDSimHitCollection, AFP_SIDSimHitCollection_PERS>
{
	friend class CnvFactory<AFP_SIDSimHitCollectionCnv>;

protected:
	AFP_SIDSimHitCollectionCnv(ISvcLocator* svcloc) : T_AthenaPoolCustomCnv <AFP_SIDSimHitCollection, AFP_SIDSimHitCollection_PERS >(svcloc) {}

	AFP_SIDSimHitCollection_PERS* createPersistent (AFP_SIDSimHitCollection *transCont);
	AFP_SIDSimHitCollection* createTransient ();
 };

#endif //AFP_SIDSimHitCollectionCnv_h
