/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_CLinkEventCnv_h
#define ALFA_CLinkEventCnv_h 

#include "ALFA_CLinkEv/ALFA_CLinkEvent.h"
#include "ALFA_EventTPCnv/ALFA_CLinkEvent_p1.h"

#include "AthenaPoolCnvSvc/T_AthenaPoolCustomCnv.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"

typedef ALFA_CLinkEvent_p1 ALFA_CLinkEvent_PERS;

//typedef T_AthenaPoolCustomCnv < LUCID_DigitContainer, LUCID_DigitContainer_PERS > LUCID_DigitContainerCnvBase;

class ALFA_CLinkEventCnv : public T_AthenaPoolCustomCnv <ALFA_CLinkEvent, ALFA_CLinkEvent_PERS>
{
	friend class CnvFactory<ALFA_CLinkEventCnv>;

protected:
	ALFA_CLinkEventCnv(ISvcLocator* svcloc) : T_AthenaPoolCustomCnv <ALFA_CLinkEvent, ALFA_CLinkEvent_PERS >(svcloc) {}
	ALFA_CLinkEvent_PERS* createPersistent (ALFA_CLinkEvent *transCont);
	ALFA_CLinkEvent* createTransient ();
};

#endif // ALFA_CLinkEventCnv_h
