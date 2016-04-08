/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_TDDigiCollectionCnv_h
#define AFP_TDDigiCollectionCnv_h

#include "AFP_DigiEv/AFP_TDDigiCollection.h"
#include "AFP_EventTPCnv/AFP_TDDigiCollection_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "GaudiKernel/MsgStream.h"

typedef AFP_TDDigiCollection_p1  AFP_TDDigiCollection_PERS;

class AFP_TDDigiCollectionCnv : public T_AthenaPoolCustomCnv <AFP_TDDigiCollection, AFP_TDDigiCollection_PERS>
{
	friend class CnvFactory<AFP_TDDigiCollectionCnv>;

protected:
	AFP_TDDigiCollectionCnv(ISvcLocator* svcloc) : T_AthenaPoolCustomCnv <AFP_TDDigiCollection, 
AFP_TDDigiCollection_PERS >(svcloc) {}

	AFP_TDDigiCollection_PERS* createPersistent (AFP_TDDigiCollection *transCont);
	AFP_TDDigiCollection* createTransient ();
 };

#endif //AFP_TDDigiCollectionCnv_h
