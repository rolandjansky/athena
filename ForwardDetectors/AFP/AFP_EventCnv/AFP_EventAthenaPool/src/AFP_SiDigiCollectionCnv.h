/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SiDigiCollectionCnv_h
#define AFP_SiDigiCollectionCnv_h

#include "AFP_DigiEv/AFP_SiDigiCollection.h"
#include "AFP_EventTPCnv/AFP_SiDigiCollection_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"
#include "GaudiKernel/MsgStream.h"

typedef AFP_SiDigiCollection_p1  AFP_SiDigiCollection_PERS;

class AFP_SiDigiCollectionCnv : public T_AthenaPoolCustomCnv <AFP_SiDigiCollection, AFP_SiDigiCollection_PERS>
{
	friend class CnvFactory<AFP_SiDigiCollectionCnv>;

protected:
	AFP_SiDigiCollectionCnv(ISvcLocator* svcloc) : T_AthenaPoolCustomCnv <AFP_SiDigiCollection, AFP_SiDigiCollection_PERS >(svcloc) {}

	AFP_SiDigiCollection_PERS* createPersistent (AFP_SiDigiCollection *transCont);
	AFP_SiDigiCollection* createTransient ();
 };

#endif //AFP_SiDigiCollectionCnv_h
