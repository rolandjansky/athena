/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ALFA_MDGap_H
#define ALFA_MDGap_H

#include <iostream>
#include <string>
#include <vector>
#include <map>

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

#define MAXTRACKNUM 10

class ALFA_MDGap
{
	public:
		ALFA_MDGap();
		~ALFA_MDGap();

	private:
		Int_t m_iRPot;
		Int_t m_iUVCut;
		Float_t m_fOverlapCut;

		//slope, offset and Z-pos for MD fibers [2*10][64]
		Float_t m_faMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT];
		Float_t m_fbMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT];

		//number of hits in the layer
		Int_t m_iNumHitsLayer[ALFALAYERSCNT*ALFAPLATESCNT];

		Int_t m_iNU[MAXTRACKNUM];
		Int_t m_iNV[MAXTRACKNUM];
		Int_t m_iFibSel[ALFALAYERSCNT*ALFAPLATESCNT];

		Float_t m_fRecXPos[MAXTRACKNUM];
		Float_t m_fRecYPos[MAXTRACKNUM];
		Float_t m_fOvU[MAXTRACKNUM];
		Float_t m_fOvV[MAXTRACKNUM];

	private:
		std::map<Int_t, FIBERS> m_MapLayers;

	public:
		StatusCode Initialize(Int_t iRPot, Float_t faMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT], Float_t fbMD[RPOTSCNT][ALFALAYERSCNT*ALFAPLATESCNT][ALFAFIBERSCNT], Int_t iUVCut, Float_t fOverlapCut);
		StatusCode Execute(const std::list<MDHIT> &ListMDHits);
		StatusCode Finalize(Float_t (&fRecXPos)[MAXTRACKNUM], Float_t (&fRecYPos)[MAXTRACKNUM]);

		void GetData(Int_t (&iNumU)[MAXTRACKNUM], Int_t (&iNumV)[MAXTRACKNUM], Float_t (&fOvU)[MAXTRACKNUM], Float_t (&fOvV)[MAXTRACKNUM], Int_t (&iFibSel)[ALFALAYERSCNT*ALFAPLATESCNT]);

	private:
		void Proj_Store(Int_t iFiberSide, Int_t (&iOver)[72000], Float_t fbRef, Int_t iSideFlag);
		void Find_Proj(Int_t iOver[72000], Float_t fbRef, Float_t &fb, Float_t &fOv, Int_t &fNum);
		void Finding_Fib(Int_t iFiberSide, Float_t fbRef, Float_t fbRec, Int_t (&iFSel)[ALFAPLATESCNT], Int_t iSideFlag);
		void Reco_Track(std::vector<Float_t> &b_p, std::vector<Float_t> &b_n,
						std::vector<Float_t> &Ov_p, std::vector<Float_t> &Ov_n,
						std::vector<int> &Num_p, std::vector<int> &Num_n,
						Int_t (&FSel_n)[ALFAPLATESCNT], Int_t (&FSel_p)[ALFAPLATESCNT],
						Int_t (&Gaps)[2][2], std::vector<Int_t> &GapsID, Bool_t (&FGaps_p)[ALFAPLATESCNT], Bool_t (&FGaps_n)[ALFAPLATESCNT],
						Int_t (&Gap_Fib_p)[ALFAPLATESCNT][2], Int_t (&Gap_Fib_n)[ALFAPLATESCNT][2]);
		Int_t Silent_Gap(Int_t (&Over)[72000], Float_t b_ref, Float_t b_rec, Int_t FSel[ALFAPLATESCNT], Bool_t (&FGap)[ALFAPLATESCNT], Int_t (&Gap_Fib)[ALFAPLATESCNT][2], Float_t OverLap, Int_t iSideFlag);
		Int_t Active_Gap(Int_t iFiberSide, Int_t (&Over)[72000], Float_t b_ref, Float_t b_rec, Int_t FSel[ALFAPLATESCNT], Float_t OverLap, Int_t iSideFlag);

public:
	/// Log a message using the Athena controlled logging system
	MsgStream& msg( MSG::Level lvl ) const { return m_msg << lvl; }
	/// Check whether the logging system is active at the provided verbosity level
	bool msgLvl( MSG::Level lvl ) const { return m_msg.get().level() <= lvl; }
private:
	/// Private message stream member
	mutable Athena::MsgStreamMember m_msg;

};
#endif // ALFA_MDGap_H
