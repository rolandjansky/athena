/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigBphysHypo/TrigL2BMuMuXHypo.h
 **
 **   Description: L2 hypothesis algorithms for B+/- -> K+/- mu+ mu-  
 **                Create TrigL2Bphys
 **
 **   Author: Pavel.Strachota@cern.ch
 **
 **   Created: 10.01.2010 based on C. Adorisio's code TrigL2DiMuHypo.h & D. Scheirich's code TrigL2LbMuMuLHypo.h
 **   Modified:  
 **
 **************************************************************************/ 

#ifndef TrigL2BMuMuXHypo_h
#define TrigL2BMuMuXHypo_h

#include "TrigInterfaces/HypoAlgo.h"

#include "Constants.h"

class TrigBphysHelperUtilsTool;


class TrigL2BMuMuXHypo: public HLT::HypoAlgo  {
  
  public:
    TrigL2BMuMuXHypo(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigL2BMuMuXHypo();
    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();     
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
    
    // private:

    bool m_acceptAll; 
    float m_lowerBMassCut;
    float m_upperBMassCut;
    float m_lowerBdMassCut;
    float m_upperBdMassCut;
    float m_lowerKstarMassCut;
    float m_upperKstarMassCut;
    float m_lowerBsMassCut;
    float m_upperBsMassCut;        
    float m_lowerPhi1020MassCut;
    float m_upperPhi1020MassCut;        
    float m_lowerLbMassCut;
    float m_upperLbMassCut;        
    float m_lowerLambdaMassCut;
    float m_upperLambdaMassCut;        
    float m_lowerBcMassCut;
    float m_upperBcMassCut;        
    float m_lowerDsMassCut;
    float m_upperDsMassCut;        


    unsigned int m_lastEvent;
    unsigned int m_lastEventPassed;
    unsigned int m_countTotalEvents;
    unsigned int m_countTotalRoI;
    unsigned int m_countPassedEvents;
    unsigned int m_countPassedRoIs;
    unsigned int m_countPassedBMass;
    unsigned int m_countPassedBdMass;
    unsigned int m_countPassedBsMass;        
    unsigned int m_countPassedLbMass;        
    unsigned int m_countPassedBcMass;        

private:
    ToolHandle <TrigBphysHelperUtilsTool> m_bphysHelperTool;

    
};

#endif
