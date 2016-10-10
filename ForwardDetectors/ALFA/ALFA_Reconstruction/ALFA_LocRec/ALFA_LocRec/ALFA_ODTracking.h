/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_ODTRACKING_H
#define ALFA_ODTRACKING_H

#include <iostream>
#include <list>

#include "AthenaKernel/MsgStreamMember.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

//#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "AthenaKernel/getMessageSvc.h"

#include "Rtypes.h"
#include "TMath.h"

#include "ALFA_LocRec/ALFA_UserObjects.h"
#include "ALFA_Geometry/ALFA_constants.h"


class ALFA_ODTracking
{
	public:
		ALFA_ODTracking();
		~ALFA_ODTracking();

	private:
		Int_t m_iMultiplicityCut;
		Float_t m_fDistanceCut;
		Int_t m_iDataType, m_iLayerCut, m_iMulti[ODSIDESCNT][ODPLATESCNT];
		Int_t m_iFibSel[ODSIDESCNT][ODPLATESCNT];

	private:
		std::list<ODRESULT> m_listResults;

	public:
		StatusCode Initialize(Int_t fMultiplicityCut, Float_t fDistanceCut, Int_t iLayerCut, Int_t iDataType);
		StatusCode Execute(Int_t iRPot, const std::list<ODHIT> &ListODHits, Float_t faOD[RPOTSCNT][ODPLATESCNT][ODSIDESCNT][ODLAYERSCNT*ODFIBERSCNT], Float_t fbOD[RPOTSCNT][ODPLATESCNT][ODSIDESCNT][ODLAYERSCNT*ODFIBERSCNT]);
		StatusCode Finalize(std::list<ODRESULT>* pListResults);

		void GetData(Int_t (&iFibSel)[ODSIDESCNT][ODPLATESCNT]);

	private:
		void FiberProjection(Int_t iRPot, std::map<int, FIBERS> &MapLayers, Float_t faOD[RPOTSCNT][ODPLATESCNT][ODSIDESCNT][ODLAYERSCNT*ODFIBERSCNT], Float_t fbOD[RPOTSCNT][ODPLATESCNT][ODSIDESCNT][ODLAYERSCNT*ODFIBERSCNT]);
		void FindingPosition(Int_t iRPot, std::map<int, FIBERS> &MapLayers, Float_t faOD[RPOTSCNT][ODPLATESCNT][ODSIDESCNT][ODLAYERSCNT*ODFIBERSCNT], Float_t fbOD[RPOTSCNT][ODPLATESCNT][ODSIDESCNT][ODLAYERSCNT*ODFIBERSCNT]);

public:
	/// Log a message using the Athena controlled logging system
	MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
	/// Check whether the logging system is active at the provided verbosity level
	bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }
private:
	/// Private message stream member
	mutable Athena::MsgStreamMember m_msg;
};


#endif // ALFA_ODTRACKING_H
