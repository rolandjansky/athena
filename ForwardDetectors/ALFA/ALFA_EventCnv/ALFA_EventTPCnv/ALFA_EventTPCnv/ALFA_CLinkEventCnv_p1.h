/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_CLinkEventCnv_p1_h
#define ALFA_CLinkEventCnv_p1_h

#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
#include "DataModelAthenaPool/DataLinkCnv_p1.h"

#include "ALFA_EventTPCnv/ALFA_CLinkEvent_p1.h"

class MsgStream;

class ALFA_CLinkEventCnv_p1 : public T_AthenaPoolTPCnvBase< ALFA_CLinkEvent, ALFA_CLinkEvent_p1>
{

public:
	ALFA_CLinkEventCnv_p1() {}

	virtual void persToTrans( const ALFA_CLinkEvent_p1* persObj, ALFA_CLinkEvent* transObj, MsgStream &log);
	virtual void transToPers( const ALFA_CLinkEvent* transObj, ALFA_CLinkEvent_p1* persObj, MsgStream &log);

private:
	DataLinkCnv_p1<DataLink<ALFA_RawDataContainer> >  m_DLRawDataEvCollCnv;
	DataLinkCnv_p1<DataLink<ALFA_DigitCollection> >  m_DLDigitEvCollCnv;
	DataLinkCnv_p1<DataLink<ALFA_ODDigitCollection> >  m_DLODDigitEvCollCnv;
	DataLinkCnv_p1<DataLink<ALFA_LocRecEvCollection> >  m_DLLocRecEvCollCnv;
	DataLinkCnv_p1<DataLink<ALFA_LocRecODEvCollection> >  m_DLLocRecODEvCollCnv;
	DataLinkCnv_p1<DataLink<ALFA_LocRecCorrEvCollection> >  m_DLLocRecCorrEvCollCnv;
	DataLinkCnv_p1<DataLink<ALFA_LocRecCorrODEvCollection> >  m_DLLocRecCorrODEvCollCnv;
// 	DataLinkCnv_p1<DataLink<ALFA_GloRecEvCollection> >  m_DLGloRecEvCollCnv;
};

#endif
