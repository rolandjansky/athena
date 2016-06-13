/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 ** 
 **   Author:       Denis Damazio
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/TrigL2CaloLayersHypo.h
 **                   
 **   Description: Hypothesis algorithms to be run at Level 2 after T2Calo
 **
 **   Authour: R.Goncalo - Thu Jan 20 22:02:43 BST 2005
 **
 **   Created:      Sept  7 2010
 **   Modified:     
 **
 **************************************************************************/ 

#ifndef TRIG_TrigL2CaloLayersHypo_H 
#define TRIG_TrigL2CaloLayersHypo_H

#include <cmath> 
#include <vector>

// athena stuff stuff
#include "GaudiKernel/IToolSvc.h"

// core trigger stuff
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "TrigInterfaces/HypoAlgo.h"

/**
 * \class TrigL2CaloLayersHypo
 * \brief TrigL2CaloLayersHypo is a Trigger Hypo Algorithm that retrieves the L2 TrigEmCluster Container
 * created by TrigT2Calo and then applies a subset of calorimeter selection cuts: ET and shower shapes
 * . A TE will be set active if the selection cuts are fullfilled.
 *
 *
 */


class TrigL2CaloLayersHypo: public HLT::HypoAlgo {
  
  public:
    TrigL2CaloLayersHypo(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigL2CaloLayersHypo();

    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

  private:

    //Calorimeter Layer  cuts
    std::vector<float> m_EnergyFracCut;
    std::vector<float> m_EnergyAbsCut;
    
    bool m_acceptAll;
  
    int m_PassedCuts; //!< counter for candidates passing each cut
    float m_monEta; // Monitor cluster eta
    float m_monPhi; // Monitor cluster phi!!
    float m_Energy; // Monitor total RoI energy
    float m_preSamp; // Presampler Energy
    float m_preSampFrac; // Presampler Energy Fraction
};

#endif
