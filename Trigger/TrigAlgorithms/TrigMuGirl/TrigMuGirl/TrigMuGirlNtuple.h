/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGMUGIRLNTUPLE_H
#define TRIGMUGIRLNTUPLE_H

#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
#include "DataModel/DataLink.h"

class INTupleSvc;
class TrigMuGirl;

class TrigMuGirlNtuple
{
public:

  TrigMuGirlNtuple(HLT::FexAlgo* pTrigMuGirl, INTupleSvc* pNTupleSvc);

  StatusCode book( const std::string& ntupleName, const std::string& ntupleTitle);
  StatusCode writeRecord();
  StatusCode fillTruth(int nMu, double phi, double cot, double eta, double pt, int pdg, int parentPdg);
  StatusCode fillReco(int nMu, double phi, double cot,  double eta, double pt, int charge, float NN0, float NN1, float NN2, float NN3, float NN4, float NN5, float LHR, float iso , float dR);

  StatusCode fillEvent(int event, int run);
    
private:

    // Truth muon parameters
    NTuple::Item<long>    m_NumTruthMuon;
    NTuple::Array<float>  m_TruthMuonPt;
    NTuple::Array<float>  m_TruthMuonPhi;
    NTuple::Array<float>  m_TruthMuonCot;
    NTuple::Array<float>  m_TruthMuonEta;
    NTuple::Array<long>   m_TruthMuonPdg;
    NTuple::Array<long>   m_TruthMuonParentPdg;
  
    // Reco muon parameters
    NTuple::Item<long>    m_NumRecoMuon;
    NTuple::Array<float>  m_RecoMuonPt;
    NTuple::Array<float>  m_RecoMuonPhi;
    NTuple::Array<float>  m_RecoMuonCot;
    NTuple::Array<float>  m_RecoMuonEta;
    NTuple::Array<long>   m_RecoMuonCharge;
    NTuple::Array<float>  m_RecoMuonNN0;
    NTuple::Array<float>  m_RecoMuonNN1;
    NTuple::Array<float>  m_RecoMuonNN2;
    NTuple::Array<float>  m_RecoMuonNN3;
    NTuple::Array<float>  m_RecoMuonNN4;
    NTuple::Array<float>  m_RecoMuonNN5;
    NTuple::Array<float>  m_RecoMuonLHR;
    NTuple::Array<float>  m_RecoMuonIso;
    NTuple::Array<float>  m_RecoMuondR;

    // event info
    NTuple::Item<long>   m_Event;
    NTuple::Item<long>   m_Run;

    int m_max_pair;
    int m_max_dig;
    int m_max_idtrack;
    int m_maxTruthMuons;
    //    TrigMuGirlFex* m_pTrigMuGirlFex; /**< The TrigMuGirlFex algorithm */
    HLT::FexAlgo* m_pTrigMuGirl; /**< The TrigMuGirlFex algorithm */
    NTuple::Tuple*  m_pNtuple;         /**< The NTuple object */
    INTupleSvc*     m_pNTupleSvc;      /**< The Athena NTuple service */
    std::string     m_ntupleName;      /**< The NTuple name */

};

#endif // TRIGMUGIRLNTUPLE_H 

