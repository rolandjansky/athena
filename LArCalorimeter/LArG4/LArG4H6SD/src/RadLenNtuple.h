/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RadLenNtuple_H
#define RadLenNtuple_H

#include "G4AtlasTools/UserActionBase.h"
#include <string>
#include "GaudiKernel/NTuple.h"
#include <vector>

// Event information
#include "StoreGate/ReadHandle.h"
#include "GeneratorObjects/McEventCollection.h"

class RadLenNtuple final: public UserActionBase {

public:
	RadLenNtuple(const std::string& type, const std::string& name, const IInterface* parent):UserActionBase(type,name,parent),m_mcEvtColl("GEN_EVENT") {}
	virtual void BeginOfEvent(const G4Event*) override;
        virtual void EndOfEvent(const G4Event*) override;
        virtual void Step(const G4Step*) override;

	virtual StatusCode queryInterface(const InterfaceID&, void**) override;
	virtual StatusCode initialize() override;


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

    // Data access
    SG::ReadHandle<McEventCollection> m_mcEvtColl;

};

#endif
