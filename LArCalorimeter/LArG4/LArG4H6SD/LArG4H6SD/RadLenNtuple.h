/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RadLenNtuple_H
#define RadLenNtuple_H

#include "FadsActions/UserAction.h"
#include <string>
#include "GaudiKernel/NTuple.h"
#include <vector>

class RadLenNtuple: public FADS::UserAction {

public:
	RadLenNtuple(std::string s): FADS::UserAction(s) {}
	void BeginOfEventAction(const G4Event*);
        void EndOfEventAction(const G4Event*);
        void BeginOfRunAction(const G4Run*);
        void EndOfRunAction(const G4Run*);
        void SteppingAction(const G4Step*);

private:

	NTuple::Item<float>   m_tot_x;
	NTuple::Item<float>   m_tot_ni;
	NTuple::Item<float>   m_cryo_x;
	NTuple::Item<float>   m_cryo_ni;
	NTuple::Item<float>   m_em_x;
	NTuple::Item<float>   m_em_ni;
	NTuple::Item<float>   m_hec_x;
	NTuple::Item<float>   m_hec_ni;
	NTuple::Item<float>   m_fcal_x;
	NTuple::Item<float>   m_fcal_ni;
	NTuple::Item<float>   m_fcal_y;
	NTuple::Item<float>   m_em_y;
	NTuple::Item<float>   m_hec_y;
	NTuple::Item<float>   m_cryo_y;
	NTuple::Item<float>   m_xcoord;
};

#endif
