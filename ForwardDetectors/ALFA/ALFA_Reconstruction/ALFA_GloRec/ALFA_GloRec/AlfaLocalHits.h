/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef AlfaLocalHits_h
#define AlfaLocalHits_h


#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"

#include "ALFA_Geometry/ALFA_GeometryReader.h"
#include "ALFA_LocRecEv/ALFA_LocRecEvent.h"
#include "ALFA_LocRecCorrEv/ALFA_LocRecCorrEvent.h"


// ROOT headers
#include <TROOT.h>
#include "TString.h"
#include "TObjArray.h"
#include "TObjString.h"

using namespace std;



////////////////////////////////////////////
//
//   user class to store ONE local hit
//
////////////////////////////////////////////
class AlfaLocalHit{
	private:
		int m_pot;
    	TString m_potname;
    	int m_ishit;
    	Float_t m_x, m_y, m_z;
        const ALFA_LocRecCorrEvent * m_LocRecCorr;
  	public:
    	AlfaLocalHit();
    	AlfaLocalHit(const ALFA_LocRecCorrEvent * LocRecCorrHit);
    	AlfaLocalHit(const AlfaLocalHit * h);
        void    SetHit(const ALFA_LocRecCorrEvent * LocRecCorrHit); 
    	void    AssignZ();
    	int     GetPotNum() const {return m_pot;};
    	float   GetX() const {return m_x;};
    	float   GetY() const {return m_y;};
    	float   GetZ() const {return m_z;};
    	int     IsHi() const {return m_ishit;};
    	int     GetMDindex(const char * name);
    	TString GetPotName() const {return m_potname;};
        TString GetMDname (int i);
        const ALFA_LocRecCorrEvent * GetLocRecCorr() const {return m_LocRecCorr;};
        void    Print();
};

///////////////////////////////////////////////////
//
//    user class to store MULTIPLE local hits
//    inherits from AlfaLocalHit
//
//    from such a collection of hits one can 
//    select global track candidates by means
//    of applying "paths", i.e. B7L1U_A7L1U_X_X etc.
//
///////////////////////////////////////////////////
class AlfaLocalHits : public AlfaLocalHit {
	private:
		vector<AlfaLocalHit> m_hits;
		int  m_nhits;
		vector<AlfaLocalHits> m_paths;
		int  m_npaths;
		TString m_pathpattern;
	public:
		AlfaLocalHits(); // .. default ctor.
		void update();
		int  AddHit(AlfaLocalHit h);
		void Reset();
		void ResetPaths();
		int  size();
		int  GetNhits() {return m_nhits;}
		int  GetNpaths(){return m_npaths;}
		void Dump();
		void DumpPaths();
		void ApplyPathPattern(const char * pattern);
		AlfaLocalHits * GetPathHits(int ipath);
		AlfaLocalHit  * GetHit(int ihit);
		void    SetPathPattern(const char * pattern);
		TString GetPathPattern(){return m_pathpattern;}
};



/////////////////////////////////////////////////
//
//    user class to store one track candidate
//
///////////////////////////////////////////////////
class AlfaTrackCand {
	private:
		int  m_arm;
		float m_x, m_y, m_z;
		float m_xslope, m_yslope;
		AlfaLocalHits * m_trackcandhits;
		AlfaLocalHit  * m_nearlocalhit;
		AlfaLocalHit  * m_farlocalhit;
		HepGeom::Point3D<double> m_nearimpactpoint;
		HepGeom::Point3D<double> m_farimpactpoint;
	public:
		AlfaTrackCand(AlfaLocalHits * hits); // .. explicit ctor.
		int IsLeftTrack();
		int IsRightTrack();
		int IsLowerArmTrack();
		int IsUpperArmTrack();
		int IsValidTrack() {return (m_nearlocalhit && m_farlocalhit);};
		int IsLeftOrRightTrack();
		int IsLeftAndRightTrack();
		void CalcTrack();
		int   GetArmNum(){return m_arm;};
		float GetX(){return m_x;};
		float GetY(){return m_y;};
		float GetZ(){return m_z;};
		float GetXslope(){return m_xslope;};
		float GetYslope(){return m_yslope;};
		AlfaLocalHit * GetNearLocalHit(){return m_nearlocalhit;};
		AlfaLocalHit * GetFarLocalHit() {return m_farlocalhit; };
		ALFA_LocRecCorrEvent * GetNearLocRecCorr(){return (ALFA_LocRecCorrEvent*)(m_nearlocalhit->GetLocRecCorr());};
		ALFA_LocRecCorrEvent * GetFarLocRecCorr() {return (ALFA_LocRecCorrEvent*)(m_farlocalhit->GetLocRecCorr());};
		void CalcImpactPoints();
		HepGeom::Point3D<double> * GetNearImpactPoint(){return &m_nearimpactpoint;};
		HepGeom::Point3D<double> * GetFarImpactPoint(){return &m_farimpactpoint;};
		void Dump();
};



#endif // AlfaLocalHits_h
