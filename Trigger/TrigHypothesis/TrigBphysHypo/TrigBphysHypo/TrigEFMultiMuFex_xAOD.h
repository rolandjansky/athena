/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBphysHypo/TrigEFMultiMuFex_xAOD.h
 **
 **   Description: EF hypothesis algorithms for B0_s -> mu+ mu-  
 **                Currently a dummy algorithm
 **
 **   Author: J.Kirk
 **
 **   Created:   12.09.07
 **   Modified:     
 **
 **************************************************************************/ 

#ifndef TRIG_TrigEFMultiMuCombo_xAOD_H
#define TRIG_TrigEFMultiMuCombo_xAOD_H

// standard stuff
#include <string>
#include <map>
#include <cmath> 
// general athena stuff
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "StoreGate/StoreGateSvc.h"
// trigger includes
#include "TrigInterfaces/ComboAlgo.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "TrkVKalVrtFitter/TrkVKalVrtFitter.h"
#include "TrigParticle/TrigEFBphys.h"
#include "TrigParticle/TrigEFBphysContainer.h"
//#include "TrigMuonEvent/TrigMuonEF.h"
//Ntuples
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"

//#include "TrigTimeAlgs/TrigTimerSvc.h"
//#include "TrigTimeAlgs/TrigTimer.h"

#include "TrigBphysHypo/Constants.h"
#include "xAODTrigBphys/TrigBphys.h"
#include "xAODTrigBphys/TrigBphysContainer.h"
#include "xAODTrigBphys/TrigBphysAuxContainer.h"

class StoreGateSvc;
class TriggerElement;
class CombinedMuonFeature;


class TrigEFMultiMuFex_xAOD: public HLT::ComboAlgo  {
  
  public:
    TrigEFMultiMuFex_xAOD(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigEFMultiMuFex_xAOD();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();     

    HLT::ErrorCode acceptInputs(HLT::TEConstVec& inputTE, bool& pass );
    HLT::ErrorCode hltExecute(HLT::TEConstVec& inputTE, HLT::TriggerElement* outputTE);
    
  private:

    void processTriMuon(HLT::TEConstVec& inputTE);
    TrigEFBphys* checkInvMass2(const Trk::Track* muon1, const Trk::Track* muon2, double Mass);
    TrigEFBphys* checkInvMass3(const Trk::Track* muon1, const Trk::Track* muon2, const Trk::Track* muon3, double Mass);
    bool checkInvMass2(xAOD::TrigBphys*&, const Trk::Track* muon1, const Trk::Track* muon2, double Mass);
    bool checkInvMass3(xAOD::TrigBphys*&, const Trk::Track* muon1, const Trk::Track* muon2, const Trk::Track* muon3, double Mass);

    
    double getInvMass2(const Trk::Track *muon1,const Trk::Track *muon2);
    double getInvMass3(const Trk::Track *muon1,const Trk::Track *muon2,const Trk::Track *muon3);
    void findTracks(HLT::TEConstVec& inputTE, const Trk::Track* muon1, const Trk::Track* muon2, TrigEFBphys* TrigPart);
    void findTracks(HLT::TEConstVec& inputTE, const Trk::Track* muon1, const Trk::Track* muon2,const Trk::Track* muon3, TrigEFBphys* TrigPart);

    void findTracks(HLT::TEConstVec& inputTE, const Trk::Track* muon1, const Trk::Track* muon2, xAOD::TrigBphys* TrigPart);
    void findTracks(HLT::TEConstVec& inputTE, const Trk::Track* muon1, const Trk::Track* muon2,const Trk::Track* muon3, xAOD::TrigBphys * TrigPart);

    
    double invariantMass(const Trk::Track* , const Trk::Track* );

    ToolHandle < Trk::IVertexFitter  >       m_fitterSvc;
    Trk::TrkVKalVrtFitter*              m_VKVFitter;                                                                                                                
     
//    ToolHandle <Trk::ITrkVKalVrtFitter>       m_fitterSvc;

    // variables to keep pre-results from acceptInput to hltExecute
    const CombinedMuonFeature *m_muon1;
    const CombinedMuonFeature *m_muon2;    


    // Mass window cuts
    bool m_oppositeCharge;
    unsigned int  m_NInputMuon;
    int  m_NMassMuon;
    float m_lowerMassCut;
    float m_upperMassCut;
    bool m_ApplyupperMassCut;
    string m_muonAlgo;
    // to set Accept-All mode: should be done with force-accept when possible
    bool m_acceptAll;

  TrigTimer* m_BmmHypTot;
  TrigTimer* m_BmmHypVtx;
    
  //Counters
  int m_lastEvent;
  int m_lastEventPassed;
  unsigned int m_countTotalEvents;
  unsigned int m_countTotalRoI;
  unsigned int m_countPassedEvents;
  unsigned int m_countPassedRoIs;
  unsigned int m_countPassedmumuPairsEv;
  unsigned int m_countPassedBsMassEv;
  unsigned int m_countPassedVtxFitEv;
  unsigned int m_countPassedmumuPairs2R;
  unsigned int m_countPassedBsMass2R;
  unsigned int m_countPassedVtxFit2R;

  //TrigEFBphysContainer* m_trigBphysColl;
    xAOD::TrigBphysContainer* m_xAODTrigBphysColl;
    xAOD::TrigBphysAuxContainer* m_xAODTrigBphysAuxColl;

    
    
  //Monitored variables 
  std::vector<float> mon_mu1pT;
  std::vector<float> mon_mu2pT;
  std::vector<float> mon_MuMumass;
  std::vector<float> mon_FitMass;
  std::vector<float> mon_Chi2;
    

  void addUnique(std::vector<const Trk::Track*>&, const Trk::Track*);

  
};


#endif
