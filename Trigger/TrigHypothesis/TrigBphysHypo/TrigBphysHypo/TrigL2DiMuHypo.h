/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#ifndef TRIGL2DIMUHYPO_H
#define TRIGL2DIMUHYPO_H

#include "TrigInterfaces/HypoAlgo.h"
#include "TrigT1Interfaces/RecMuonRoI.h" 
#include "GaudiKernel/IIncidentListener.h"

#include "TrigBphysHypo/Constants.h"

class TriggerElement;
//class TrigJpsi;

struct Encoding
{
    unsigned unused:18;
    unsigned mdt:5;
    unsigned tgc:5;
    unsigned rpc:4;
};

class TrigL2DiMuHypo: public HLT::HypoAlgo
{

public:
    TrigL2DiMuHypo(const std::string& name, ISvcLocator* pSvcLocator);
    ~TrigL2DiMuHypo();

    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
	
    bool execHLTAlgorithm(TriggerElement* );

private:
    
    DoubleProperty m_MassMin;
    DoubleProperty m_MassMax;
    BooleanProperty m_ApplyMassMax;
    BooleanProperty m_ApplyOppCharge;
    BooleanProperty m_acceptAll;
    IntegerProperty m_NHits;
    BooleanProperty m_ApplyChi2Cut;
    DoubleProperty  m_Chi2VtxCut;

    int m_roiNum;
    int m_jpsiNum;
    StoreGateSvc * m_pStoreGate;
    void handle(const Incident &); 

    /* monitored variables */
    int mon_nbphys;
    std::vector<float> mon_dimumass;
    std::vector<float> mon_muEta;
    std::vector<float> mon_muPhi;

};

#endif 
