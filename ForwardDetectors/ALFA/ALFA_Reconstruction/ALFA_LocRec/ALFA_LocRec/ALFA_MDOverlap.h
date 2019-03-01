/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_MDOVERLAP_H
#define ALFA_MDOVERLAP_H

#include <iostream>
#include <math.h>
#include <list>

#include "AthenaKernel/MsgStreamMember.h"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

#include "GaudiKernel/StatusCode.h"

#include "AthenaKernel/getMessageSvc.h"

#include "TH1.h"
#include "TH2.h"


#include "ALFA_Geometry/ALFA_constants.h"
#include "ALFA_Geometry/ALFA_GeometryReader.h"
#include "ALFA_LocRec/ALFA_UserObjects.h"


class ALFA_MDOverlap
{
	public:
		ALFA_MDOverlap();
		~ALFA_MDOverlap();

	private:
		//number of RP, from 0 to 7 (B7L1U == 0, B7L1L == 1, ...)
		Int_t m_iRPot;

		Int_t m_iTBins;	// Theta Bins
		Int_t m_iRBins;	// R Bins

		//min-value of U & V fiber offset
		Float_t m_fbMinU;
		Float_t m_fbMinV;
		Float_t m_fFiberD;
		Float_t m_fzStep;

		Float_t m_fPrivPI;
		Float_t m_fTLow;
		Float_t m_fTHigh;
		Float_t m_fRLow;
		Float_t m_fRHigh;

		Float_t m_fHitBU;
		Float_t m_fHitBV;

		Float_t m_fRecXPos;
		Float_t m_fRecYPos;

	private:
		Int_t m_iFHits[ALFALAYERSCNT*ALFAPLATESCNT];

		//slope, offset and Z-pos for MD fibers [2*10][64]
		Float_t m_faMD[ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT];
		Float_t m_fbMD[ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT];
		Float_t m_fzMD[ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT];

	private:
		TH2D* m_histU_PT;
		TH2D* m_histV_PT;

	private:
		std::list<MDHIT> m_ListMDHits;

	public:



	private:
		void HistInitialize();
		void HistFinalize();

		StatusCode SelectHitInLayer();
		StatusCode Overlap();


	public:
		StatusCode Initialize(const eRPotName &eRPName, const std::list<MDHIT> &ListMDHits, Float_t faMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT], Float_t fbMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT], Float_t fzMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT]);
		StatusCode Execute();
		StatusCode Finalize(Float_t &fRecXPos, Float_t &fRecYPos);

		void GetData(Int_t (&iFibSel)[ALFALAYERSCNT*ALFAPLATESCNT]);

public:
	/// Log a message using the Athena controlled logging system
	MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
	/// Check whether the logging system is active at the provided verbosity level
	bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }
private:
	/// Private message stream member
	mutable Athena::MsgStreamMember m_msg;


};
#endif // ALFA_MDOVERLAP_H
