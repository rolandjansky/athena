/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


/**************************************************************************
 ** 
 **   Author:       R.Goncalo    
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/TrigL2PhotonHypo.h
 **                   
 **   Description: Hypothesis algorithms to be run for Photon 
 **                signatures at Level 2 after T2Calo
 ** 
 **   Authour: R.Goncalo 
 **
 **   Created:      Thu Jun 06 16:01:12 BST 2006
 **   Modified:  V. Perez Reale adapted to New Steering Nov 06
 **              V. Perez Reale added doxygen comments 26/6/07
 **              V. Dao cleaned up accesor for monitoring 13/10/2009
 **************************************************************************/ 

#ifndef TRIG_PHOTON_HYPO_H 
#define TRIG_PHOTON_HYPO_H

#include <vector>

// athena stuff stuff
#include "GaudiKernel/IToolSvc.h"
#include "CLHEP/Units/SystemOfUnits.h"

// core trigger stuff
#include "TrigT1Interfaces/RecEmTauRoI.h"
#include "TrigInterfaces/HypoAlgo.h"

// EDM
#include "xAODTrigCalo/TrigEMCluster.h"
#include "xAODTrigEgamma/TrigPhoton.h"
#include "xAODTrigEgamma/TrigPhotonContainer.h"

#include "TrigTimeAlgs/TrigTimerSvc.h"


/**
 * \class TrigL2PhotonHypo
 * \brief TrigL2PhotonHypo is a Trigger Hypo Algorithm that retrieves the L2 TrigPhotonContainer
 * created by TrigL2PhotonFex and then apply a subset of calorimeter selection cuts. A TE will be
 * set active if the selection cuts are fullfilled.
 *
 *
 */

class TrigL2PhotonHypo : public HLT::HypoAlgo {
  
  public:
    TrigL2PhotonHypo(const std::string & name, ISvcLocator* pSvcLocator);
    ~TrigL2PhotonHypo();

   HLT::ErrorCode hltInitialize();
   HLT::ErrorCode hltFinalize();
   HLT::ErrorCode hltExecute(const HLT::TriggerElement* outputTE, bool& pass);
    
  private:

    //Calorimeter electron ID  cuts
    std::vector<float> m_etabin; //!<  selection variable for L2 calo selection:eta bins
    std::vector<float> m_eTthr; //!<  selection variable for L2 calo selection
    std::vector<float> m_eT2thr; //!<  selection variable for L2 calo selection: ET
    std::vector<float> m_hadeTthr; //!<  selection variable for L2 calo selection
    std::vector<float> m_hadeT2thr; //!<  selection variable for L2 calo selection: ET (Had)
    std::vector<float> m_carcorethr; //!<  selection variable for L2 calo selection:Rcore
    std::vector<float> m_caeratiothr; //!<  selection variable for L2 calo selection: Eratio
    std::vector<float> m_F1thr; //!< selection variable for L2 calo: fraction of energy deposited in 1st sampling    

    std::vector<float> m_calib;
    
    bool m_acceptAll; //!<  accept all events bool property if true
    bool m_etCutOnly;

    //L1-L2 matching variables
    float  m_detacluster; //!<  deta between track and L1 roI
    float  m_dphicluster; //!<  dphi between track and L1 roI
    
    int  m_PassedCuts; //!< counter for candidates passing each cut

    // functions to retrieve monitored quantities

    const xAOD::TrigPhotonContainer* m_PhotonContainer; //!<  TrigPhotonConainer pointer
    
    /**
     * \brief implements histograms for monitoring
     */
    static inline double PhEt    (const xAOD::TrigPhoton* ph)	{if(ph && (ph->emCluster()->et()))  return ph->emCluster()->et();   else return -99.;}
    static inline double PhEta   (const xAOD::TrigPhoton* ph)	{if(ph && (ph->eta())) return ph->eta();  else return -99.;}
    
    static inline double PhPhi   (const xAOD::TrigPhoton* ph)	{if(ph && (ph->phi())) return ph->phi();  else return -99.;}
    static inline double PhRcore (const xAOD::TrigPhoton* ph)	{if(ph && (ph->rcore()))   return ph->rcore();    else return -99.;}
    static inline double PhEratio(const xAOD::TrigPhoton* ph)	{if(ph && (ph->eratio())) return ph->eratio();  else return -99.;}
    
    static inline double F1      (const xAOD::TrigPhoton* ph) {
      if( ph && (ph->f1()) )  
	return ph->f1();   
      else return -99.;
    }
     
};

#endif
