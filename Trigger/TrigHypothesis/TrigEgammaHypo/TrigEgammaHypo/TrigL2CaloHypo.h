/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 ** 
 **   Author:       R.Goncalo    
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/TrigL2CaloHypo.h
 **                   
 **   Description: Hypothesis algorithms to be run at Level 2 after T2Calo
 **
 **   Authour: R.Goncalo - Thu Jan 20 22:02:43 BST 2005
 **
 **   Created:      Sat Mar  1 19:55:56 GMT 2005
 **   Modified:     
 *              V. Perez Reale added doxygen comments 26/6/07
 *              Eduardo J. Ortega <ortegaue@uregina.ca> Mon Mar  3 14:27:57 CST 2008
 *                                Added counter for candidates passing each cut
 **
 **************************************************************************/ 

#ifndef TRIG_TrigL2CaloHypo_H 
#define TRIG_TrigL2CaloHypo_H

#include <cmath> 
#include <vector>

// athena stuff stuff
#include "GaudiKernel/IToolSvc.h"

// core trigger stuff
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "TrigInterfaces/HypoAlgo.h"

/**
 * \class TrigL2CaloHypo
 * \brief TrigL2CaloHypo is a Trigger Hypo Algorithm that retrieves the L2 TrigEmCluster Container
 * created by TrigT2Calo and then applies a subset of calorimeter selection cuts: ET and shower shapes
 * . A TE will be set active if the selection cuts are fullfilled.
 *
 *
 */


class TrigL2CaloHypo: public HLT::HypoAlgo {
  
  public:
    TrigL2CaloHypo(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigL2CaloHypo();

    HLT::ErrorCode hltInitialize();
    HLT::ErrorCode hltFinalize();
    HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);

  private:

    //Calorimeter electron ID  cuts
    std::vector<float> m_etabin; //!<  selection variable for L2 calo selection:eta bins
    std::vector<float> m_eTthr;
    std::vector<float> m_eT2thr;
    std::vector<float> m_hadeTthr;
    std::vector<float> m_hadeT2thr;
    std::vector<float> m_carcorethr;
    std::vector<float> m_caeratiothr;
    std::vector<float> m_F1thr;
    std::vector<float> m_WETA2thr;
    std::vector<float> m_WSTOTthr;
    std::vector<float> m_F3thr;
    float m_detacluster;
    float m_dphicluster;
    
    bool m_acceptAll;
  
    // cut variables
    float m_dPhi; //!<  selection variable dphi between phi of L2 cluster and L1 RoI
    float m_dEta; //!<  selection variable dphi between eta of L2  cluster and L1 RoI
    float m_eT_T2Calo; //!<  selection variable for L2 calo selection: ET (EM)
    float m_hadET_T2Calo; //!<  selection variable for L2 calo selection: ET (Had)
    float m_rCore; //!<  selection variable for L2 calo selection: RCore
    float m_energyRatio; //!<  selection variable for L2 calo selection: Eratio
    float m_Weta2; //!<  selection variable for L2 calo selection: Weta2
    float m_Wstot; //!<  selection variable for L2 calo selection: Wstot
    float m_F3; //!<  selection variable for L2 calo selection: F3
    int   m_etaBin; //!< selection cuts are eta-dependent
    int m_PassedCuts; //!< counter for candidates passing each cut
    float m_F1;// cut on fraction of energy deposited in 1st sampling
    int GetCounterValue(int nCand[7]); //<!< Method to use preexisting variable nCand for determining PassCut
    float m_monEta; // Monitor cluster eta
    float m_monPhi; // Monitor cluster phi!!
};

#endif
