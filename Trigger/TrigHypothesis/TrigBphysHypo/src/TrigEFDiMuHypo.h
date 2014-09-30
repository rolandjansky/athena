/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
    Authors: E. Reinherz-Aronis, A. Kreisel 
             This hypo is set to work after TrigEFDiMuFex algo
***************************************************************************/
#ifndef TRIGEFDIMUHYPO_XAOD_H
#define TRIGEFDIMUHYPO_XAOD_H

#include "TrigInterfaces/HypoAlgo.h"
#include "StoreGate/StoreGateSvc.h"
#include "TrigParticle/TrigEFBphys.h"
#include "TrigParticle/TrigEFBphysContainer.h"

#include "TrigBphysHypo/Constants.h"

class TriggerElement;

class TrigEFDiMuHypo: public HLT::HypoAlgo
{

public:
    TrigEFDiMuHypo(const std::string& name, ISvcLocator* pSvcLocator);
    ~TrigEFDiMuHypo();

    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
	
private:
    
    DoubleProperty m_MassMin;
    DoubleProperty m_MassMax;
    BooleanProperty m_ApplyMassMax;
    BooleanProperty m_ApplyOppCharge;
    BooleanProperty m_acceptAll;

    BooleanProperty m_useFitMass;

    StoreGateSvc* m_pStoreGate;
    //const EventInfo* pEventInfo;

    int eEventNumber;
    int eRunNumber;
    float jpsiMassCutAlg;
    
    /* monitored variables */
    std::vector<float> mon_jpsiMass;
    std::vector<float> mon_jpsiFitMass;
      // the jpsi mass if it passed the mass cut 
    std::vector<float> mon_jpsiMassCut;
      // possible other monitor
    //std::vector<float> mon_jpsiEta;
    //std::vector<float> mon_jpsiPhi;
};

#endif 
