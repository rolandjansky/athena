/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// 
// NAME:     TrigL2BgammaXHypo.h
// PACKAGE:  Trigger/TrigHypothesis/TrigBphysHypo
// 
// AUTHOR:   Julie Kirk
// copied from Jpsiee, currently dummy algo
// ********************************************************************

#ifndef TRIG_TrigL2BgammaXFex_H 
#define TRIG_TrigL2BgammaXFex_H

#include <string>
#include "GaudiKernel/ToolHandle.h"
#include "TrigInterfaces/FexAlgo.h"

#include "TrigInDetEvent/TrigInDetTrackFitPar.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
#include "TrigInDetToolInterfaces/ITrigVertexFitter.h"
#include "TrigInDetToolInterfaces/ITrigL2VertexFitter.h"
#include "TrigParticle/TrigL2Bphys.h"
#include "TrigParticle/TrigL2BphysContainer.h"

#include "TrigBphysHypo/Constants.h"

class StoreGateSvc;
class TriggerElement;

class ITrigVertexFitter;
class ITrigL2VertexFitter;
class ITrigVertexingTool;

class TrigL2BgammaXFex: public HLT::FexAlgo {
  
  public:
    TrigL2BgammaXFex(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigL2BgammaXFex();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();     
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* inputTE, HLT::TriggerElement* outputTE );
    HLT::ErrorCode acceptInput(const HLT::TriggerElement* inputTE, bool& pass );
    bool PassTrackPair(const TrigInDetTrackFitPar* param_1, const TrigInDetTrackFitPar* param_2);
    double Mass(std::vector<const TrigInDetTrackFitPar*> tracks, std::vector<double> massHypo);
  private:

    // Properties:
    
    StoreGateSvc* m_storeGate;

    //    ToolHandle<ITrigVertexFitter>  m_vertFitter;
    // ToolHandle<ITrigL2VertexFitter> m_L2vertFitter;
    // ToolHandle<ITrigVertexingTool> m_vertexingTool;
      
    unsigned int m_trackalgoID;
    float        m_trackPtthr;
    float        m_dEta_cut;
    float        m_dPhi_cut;
    float        m_dz0_cut;
    float        m_pTsum_cut;
    float        m_JpsiMasslow_cut;
    float        m_JpsiMasshigh_cut;
    bool         m_doVertexFit;
    bool         m_doT2Calo;
    bool         m_acceptAll;

  unsigned int m_lastEvent;
  unsigned int m_lastEventPassed;
  unsigned int m_countTotalEvents;
  unsigned int m_countTotalRoI;
  unsigned int m_countPassedEvents;
  unsigned int m_countPassedRoIs;
  unsigned int m_countPassedTrackPair;
  unsigned int m_countPassedJpsiMass;

  TrigL2BphysContainer* m_trigBphysColl;
  TrigVertexCollection* m_VertexColl;

  /* monitored variables*/
  int m_Ntrack;
  std::vector<float> m_trackPt;
  std::vector<float> m_dEta;
  std::vector<float> m_dPhi;
  std::vector<float> m_dz0;
  std::vector<float> m_Ptsum;
  std::vector<float> m_JpsiMass;
  std::vector<float> m_JpsiFitMass;
};

#endif
