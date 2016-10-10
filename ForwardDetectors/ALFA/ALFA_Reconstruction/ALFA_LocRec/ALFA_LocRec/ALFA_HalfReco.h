/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_HALFRECO_H
#define ALFA_HALFRECO_H

#include <iostream>
#include <vector>

#include "AthenaKernel/MsgStreamMember.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

//#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "AthenaKernel/getMessageSvc.h"

#include "Rtypes.h"
#include "TMath.h"
#include "TH1.h"
#include "TH2.h"

#include "ALFA_LocRec/ALFA_UserObjects.h"
#include "ALFA_Geometry/ALFA_constants.h"

class ALFA_HalfReco
{
	public:
		ALFA_HalfReco();
		~ALFA_HalfReco();

	private:
		Int_t m_iUVCut;
		Int_t m_iHalf;
		Int_t m_iRPot;
//		Int_t m_iNumHitsLayer[ALFAFIBERSCNT];		//number of hits in the layer
		Int_t m_iMultiplicityCut;
		Float_t m_fOverlapCut;
		Float_t m_fRecXPos;
		Float_t m_fRecYPos;
		Float_t m_faMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT];	//slope, offset and Z-pos for MD fibers [2*10][64]
		Float_t m_fbMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT];

		Int_t m_iNumU, m_iNumV;
		double	m_fOvU, m_fOvV;
		Int_t m_fhits[ALFALAYERSCNT*ALFAPLATESCNT];


	private:
		std::map<int, FIBERS> m_MapLayers;

	public:
		StatusCode Initialize(Float_t faMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT], Float_t fbMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT], Int_t iHalf, Int_t fMultiplicityCut, Int_t iUVCut, Float_t fOverlapCut);
		StatusCode Execute(Int_t iRPot, const std::list<MDHIT> &ListMDHits);
		StatusCode Finalize(Float_t &fRecXPos, Float_t &fRecYPos);

		void GetData(Int_t &iNumU, Int_t &iNumV, Float_t &fOvU, Float_t &fOvV, Int_t (&iFibSel)[ALFALAYERSCNT*ALFAPLATESCNT]);

	private:
		void HistFill(Float_t &b_p, Float_t &b_n, Float_t &Ov_p, Float_t &Ov_n, Int_t &NumU, Int_t &NumV, Int_t iFlag);
		void OverLap();
		void SetData(Int_t iNumU, Int_t iNumV, Float_t fOvU, Float_t fOvV);

public:
	/// Log a message using the Athena controlled logging system
	MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
	/// Check whether the logging system is active at the provided verbosity level
	bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }
private:
	/// Private message stream member
	mutable Athena::MsgStreamMember m_msg;
};

#endif // ALFA_HALFRECO_H
