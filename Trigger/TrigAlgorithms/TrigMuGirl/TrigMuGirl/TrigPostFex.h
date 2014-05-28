/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
    Authors: E. Reinherz-Aronis, A. Kreisel 
             This Fex is able to run after both TrigMuGirl and TrigMooHLT
             for now the code assum it's coming in the same sequqance 
             as TrigMuGirl or TrigMooHLT 
***************************************************************************/
#ifndef TRIGPOSTFEX_H
#define TRIGPOSTFEX_H

#include "TrigInterfaces/FexAlgo.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

//class TriggerElement;
class TrigPostFexNtuple;

class TrigPostFex: public HLT::FexAlgo{

public:
    TrigPostFex(const std::string& name, ISvcLocator* pSvcLocator);
    ~TrigPostFex();

    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE);
    HLT::ErrorCode doMuTruth();
	
private:
    
    DoubleProperty m_MassMin;
    DoubleProperty m_MassMax;

    StoreGateSvc* m_pStoreGate;

    const EventInfo* pEventInfo;
    int eRunNumber;
    int eEventNumber;

    float muon_pT;

    std::vector<float> m_pt;
    std::vector<float> m_cotTh;
    std::vector<float> m_phi;
    std::vector<float> m_m;
    std::vector<float> m_charge;

    int NumberOfMuons;

    StringProperty                      m_ntupleName;       /**< The name of the output NTuple */
    StringProperty                      m_ntupleTitle;      /**< The label of the output NTuple */
    StringProperty			m_fileName;
    BooleanProperty                     m_doTruth;
    BooleanProperty                     m_doNTuple;
    TrigPostFexNtuple*                     m_pTrigPostFexNtuple;
    NTuple::Tuple*                      m_ntuple;
    int nMuons;
    int whichOutput;

};

#endif 
