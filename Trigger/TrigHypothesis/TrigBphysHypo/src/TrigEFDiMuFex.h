/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
    Authors: E. Reinherz-Aronis, A. Kreisel 
             This Fex is able to run after both TrigMuGirl and TrigMounEF
***************************************************************************/
#ifndef TRIGEFDIMUFEX_XAOD_H
#define TRIGEFDIMUFEX_XAOD_H

#include "TrigInterfaces/FexAlgo.h"
#include "TrigParticle/TrigEFBphys.h"
#include "TrigParticle/TrigEFBphysContainer.h"

// for vertex fitting
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"

#include "xAODTrigBphys/TrigBphys.h"
#include "xAODTrigBphys/TrigBphysContainer.h"
#include "xAODTrigBphys/TrigBphysAuxContainer.h"


class TriggerElement;
//class TrigEFDiMuNtuple;

class TrigEFDiMuFex: public HLT::FexAlgo{

public:
    TrigEFDiMuFex(const std::string& name, ISvcLocator* pSvcLocator);
    ~TrigEFDiMuFex();

    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE);
    HLT::ErrorCode doMuTruth();
	
private:
    
    DoubleProperty m_MassMin;
    DoubleProperty m_MassMax;
    BooleanProperty m_ApplyMassMax;
    BooleanProperty m_ApplyOppCharge;
    //    BooleanProperty m_acceptAll;

    StoreGateSvc* m_pStoreGate;

    int eRunNumber;
    int eEventNumber;

    float jpsiMass;
    //    float jpsiMassCut;
    float jpsiE;
    //    float jpsiECut;

    std::vector<float> m_pt;
    std::vector<float> m_cotTh;
    std::vector<float> m_eta;
    std::vector<float> m_phi;
    std::vector<float> m_m;
    std::vector<float> m_charge;
    std::vector<float> m_type;

    float p1[4], p2[4];

    int NumberOfMuons;

    //TrigEFBphys* pMuPair;
    //TrigEFBphysContainer* pOut;
    xAOD::TrigBphysContainer * mTrigBphysColl;
    //xAOD::TrigBphysAuxContainer * mTrigBphysAuxColl;

    int                                 m_max_pair;
    StringProperty                      m_ntupleName;       /**< The name of the output NTuple */
    StringProperty                      m_ntupleTitle;      /**< The label of the output NTuple */
    StringProperty			m_fileName;
    BooleanProperty                     m_doTruth;
    BooleanProperty                     m_doNTuple;
    // TrigEFDiMuNtuple*                   m_pTrigEFDiMuNtuple;
    int *m_pTrigEFDiMuNtuple;
    NTuple::Tuple*                      m_ntuple;

    BooleanProperty                     m_GotFitTool;
    ToolHandle<Trk::IVertexFitter>      m_iVKVVertexFitter; 
    Trk::TrkVKalVrtFitter*              m_eVKalVrtFitter;
    
    /* monitored variables */
    std::vector<float> mon_muonpT;
    std::vector<float> mon_muonEta;
    std::vector<float> mon_muonPhi;
    std::vector<float> mon_jpsiMass;
    std::vector<float> mon_VtxPt;
    
};

#endif 
