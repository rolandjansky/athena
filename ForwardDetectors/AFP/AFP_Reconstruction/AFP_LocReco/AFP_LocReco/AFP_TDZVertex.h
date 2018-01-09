/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_TDZVERTEX_H
#define AFP_TDZVERTEX_H

#include <iostream>
#include <list>
#include <vector>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "AthenaKernel/getMessageSvc.h"

#include "Rtypes.h"
#include "TMath.h"

#include "AFP_LocReco/AFP_UserObjects.h"
//#include "AFP_Geometry/AFP_constants.h"


class AFP_TDZVertex
{
	public:
		AFP_TDZVertex();
		~AFP_TDZVertex();

	private:
		float m_AmpThresh;
		int m_iDataType;

	private:
                std::list<TDRESULT> m_listResults;

	public:
		StatusCode Initialize(float fAmpThresh, int iDataType);
		StatusCode Execute(const std::list<TDHIT> &ListTDHits);
		StatusCode Finalize(std::list<TDRESULT>* pListResults);

		void GetData();

	private:

};

#endif // AFP_TDZVERTEX_H
