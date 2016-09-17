/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AFP_SIDBASICKALMAN_H
#define AFP_SIDBASICKALMAN_H

#include <iostream>
#include <list>
#include <map>
#include <vector>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "AthenaKernel/getMessageSvc.h"

#include "Ttypes.h"
#include "TH2.h"
#include "TFile.h"
#include "TMath.h"

#include "AFP_LocReco/AFP_UserObjects.h"
#include "AFP_Geometry/AFP_constants.h"
#include "AFP_Geometry/AFP_Geometry.h"

#include <CLHEP/Matrix/Matrix.h>
#include <CLHEP/Matrix/Vector.h>


#define SIDSTATIONID 4
#define MAXCHI2HIT 3.0
#define MAXCHI2TRK 3.0
#define MAXSHAREDHITS 2

using namespace std;

class AFP_SIDBasicKalman
{
	public:
		AFP_SIDBasicKalman();
		~AFP_SIDBasicKalman();

	private:
		float m_AmpThresh;
		int m_iDataType;

		list<SIDRESULT> m_listResults;
		list<SIDHIT>    m_ListSIDHits;
		map<Int_t, SIDHITPOS> m_MapSIDHitPos;

	public:
		StatusCode Initialize(float fAmpThresh, int iDataType, const list<SIDHIT> &ListSIDHits, Float_t fsSID[SIDSTATIONID][SIDCNT], Float_t fxSID[SIDSTATIONID][SIDCNT], Float_t fySID[SIDSTATIONID][SIDCNT], Float_t fzSID[SIDSTATIONID][SIDCNT]);
		StatusCode Execute();
		StatusCode Finalize(list<SIDRESULT>* pListResults);
		
		void GetData();

	private:
		Float_t fxMapSID[SIDSTATIONID][SIDCNT][336][80];
		Float_t fyMapSID[SIDSTATIONID][SIDCNT][336][80];
		Float_t fzMapSID[SIDSTATIONID][SIDCNT][336][80];
		
		void HistInitialize();
		void HistFinalize();		
		
		TFile *MyFile;
		TH2F* histS1_PixMap;
		TH2F* histS2_PixMap;
		TH2F* histS3_PixMap;
		TH2F* histS4_PixMap;
		
	private:
		vector<SIDHITSEED> pTrkSeeds;
		CLHEP::HepMatrix Hk, Qk, Vk, C0;
		
		CLHEP::HepMatrix m0, x0;
		Float_t z0;
		
		vector< CLHEP::HepMatrix > Fk;
		vector< CLHEP::HepMatrix > Ck;
		vector< CLHEP::HepMatrix > CkP;
		vector< CLHEP::HepMatrix > Rk;

		vector< CLHEP::HepVector > xk;
		vector< CLHEP::HepVector > xkP;
		vector< CLHEP::HepVector > rk;
		vector< CLHEP::HepVector > rkP;
		vector< CLHEP::HepVector > mk;
		vector< Float_t > chik;
		vector< Float_t > zk;
		vector< Int_t >   HID;
		
		vector< CLHEP::HepVector > xkS;
		vector< CLHEP::HepMatrix > CkS;
		vector< Float_t > chikS;
		
		void FillSIDHITPOS(const SIDHIT &SIDHit, map<Int_t, SIDHITPOS> &MapSIDHitPos);
		void GetTrkSeeds();
		bool FillTrkPropagators(const SIDHITSEED &SIDHitSeed, Int_t plateF);
		bool FillTrkPropagators(Int_t stID, Int_t plateF);
		void Smooth();
		void FilterTrkCollection();
		Int_t GetSharedHits(const vector<Int_t> &HID1, const vector<Int_t>  &HID2);
		void ClearMatrix();
		
};

#endif // AFP_SIDBASICKALMAN_H
