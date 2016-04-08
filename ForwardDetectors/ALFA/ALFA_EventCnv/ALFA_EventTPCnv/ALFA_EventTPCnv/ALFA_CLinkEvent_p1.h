/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_CLinkEvent_p1_h
#define ALFA_CLinkEvent_p1_h

#include "ALFA_CLinkEv/ALFA_CLinkEvent.h"
#include "DataModelAthenaPool/DataLink_p1.h"

class ALFA_CLinkEvent_p1
{
public:
	// needed by athenaRoot
	ALFA_CLinkEvent_p1()
	{
		memset(&m_DCSId,0,sizeof(DCSID));
	};
	friend class ALFA_CLinkEventCnv_p1;
 
private:
	DCSID m_DCSId;
	DataLink_p1 m_RawDataEvColl;
	DataLink_p1 m_DigitEvColl;
	DataLink_p1 m_ODDigitEvColl;
	DataLink_p1 m_LocRecEvColl;
	DataLink_p1 m_LocRecODEvColl;
	DataLink_p1 m_LocRecCorrEvColl;
	DataLink_p1 m_LocRecCorrODEvColl;
// 	DataLink_p1 m_GloRecEvColl;
};

#endif
