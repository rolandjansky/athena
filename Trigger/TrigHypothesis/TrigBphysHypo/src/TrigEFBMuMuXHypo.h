/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// vim: tabstop=2:shiftwidth=2:expandtab
/**************************************************************************
**
**   File: Trigger/TrigHypothesis/TrigBphysHypo/TrigEFBMuMuXFex.h
**
**   Description: EF hypothesis algorithms for 
**                B^0_{d,s},\Lambda_b \to X \mu^+ \mu^-
**                
**
**   Author: J.Kirk
**   Author: Semen Turchikhin <Semen.Turchikhin@cern.ch>
**
**   Created:   12.09.2007
**   Modified:  08.04.2012
**
***************************************************************************/
            
#ifndef TRIG_TrigEFBMuMuXHypo_H 
#define TRIG_TrigEFBMuMuXHypo_H

// standard stuff
#include <string>
// general athena stuff
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "TrigInterfaces/HypoAlgo.h"
#include "TrigInDetEvent/TrigVertexCollection.h"
//#include "TrigParticle/TrigEFBphys.h"
//#include "TrigParticle/TrigEFBphysContainer.h"

#include "TrigBphysHypo/Constants.h"

#include "xAODTrigBphys/TrigBphys.h"
#include "xAODTrigBphys/TrigBphysContainer.h"
#include "xAODTrigBphys/TrigBphysAuxContainer.h"


class TrigEFBMuMuXHypo: public HLT::HypoAlgo  {
  
  public:
    TrigEFBMuMuXHypo(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigEFBMuMuXHypo();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();     
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
    
  private:

    std::string m_bphysCollectionKey;
   // Cuts and properties
    bool m_acceptBplus;
    bool m_acceptBd;
    bool m_acceptBs;
    bool m_acceptLb;
    bool m_acceptBc;
    bool m_acceptBcDplus;
    bool m_acceptBcDstar;
    bool m_acceptBcD0;
    
    float m_lowerBplusMassCut;
    float m_upperBplusMassCut;
    float m_BplusChi2Cut;
    float m_lowerBdMassCut;
    float m_upperBdMassCut;
    float m_BdChi2Cut;
    float m_lowerKstarMassCut;
    float m_upperKstarMassCut;
    float m_KstarChi2Cut;
    float m_lowerBsMassCut;
    float m_upperBsMassCut;        
    float m_BsChi2Cut;        
    float m_lowerPhi1020MassCut;
    float m_upperPhi1020MassCut;        
    float m_Phi1020Chi2Cut;        
    float m_lowerLbMassCut;
    float m_upperLbMassCut;     
    float m_LbChi2Cut;     
    float m_lowerLambdaMassCut;
    float m_upperLambdaMassCut;     
    float m_LambdaChi2Cut;     
    
    float m_lowerBcMassCut;
    float m_upperBcMassCut;    
    float m_BcChi2Cut;        
//     float m_lowerPhiDsMassCut;
//     float m_upperPhiDsMassCut;        
    float m_lowerDsMassCut;
    float m_upperDsMassCut;        
    float m_DsChi2Cut;        
    
    float m_lowerBcDplusMassCut;
    float m_upperBcDplusMassCut; 
    float m_BcDplusChi2Cut;        
    float m_lowerDplusMassCut;
    float m_upperDplusMassCut;        
    float m_DplusChi2Cut;       
    
    float m_lowerBcDstarMassCut;
    float m_upperBcDstarMassCut; 
    float m_BcDstarChi2Cut;        
    float m_lowerDstarMassCut;
    float m_upperDstarMassCut;        
    float m_DstarChi2Cut;       
    
    float m_lowerBcD0MassCut;
    float m_upperBcD0MassCut; 
    float m_BcD0Chi2Cut;        
    float m_lowerD0MassCut;
    float m_upperD0MassCut;        
    float m_D0Chi2Cut;      

    // to set Accept-All mode: should be done with force-accept when possible
    bool m_acceptAll;

    //Counters
    int m_lastEvent;
    int m_lastEventPassed;
    unsigned int m_countTotalEvents;
    unsigned int m_countTotalRoI;
    unsigned int m_countPassedEvents;
    unsigned int m_countPassedRoIs;
    unsigned int m_countPassedBplus;
    unsigned int m_countPassedBd;
    unsigned int m_countPassedBs;
    unsigned int m_countPassedLb;
    unsigned int m_countPassedBc;
    unsigned int m_countPassedBcDplus;
    unsigned int m_countPassedBcDstar;
    unsigned int m_countPassedBcD0;
 
};

#endif
