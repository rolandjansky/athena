/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: CalcTopPartonHistory.h 800464 2017-03-13 18:06:24Z tpelzer $
#ifndef ANALYSISTOP_TOPPARTONS_CALCTOPPARTONHISTORY_H
#define ANALYSISTOP_TOPPARTONS_CALCTOPPARTONHISTORY_H

/**
  * @author John Morris <john.morris@cern.ch>
  * @author Silvestre Marino Romano <sromanos@cern.ch>
  * @author Samuel Calvet <scalvet@cern.ch>
  * 
  * @brief CalcTopPartonHistory
  *   Determine the history of the top partons
  * 
  * $Revision: 800464 $
  * $Date: 2017-03-13 19:06:24 +0100 (Mon, 13 Mar 2017) $
  * 
  **/ 

// system include(s):
#include <memory>
#include <vector>

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "AthContainers/DataVector.h"
#include "AthContainers/AuxElement.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "TopPartons/PartonHistory.h"

// forward declaration(s):
namespace top{
  class TopConfig;
}

namespace top{
  
  class CalcTopPartonHistory : public asg::AsgTool {
    public:
      explicit CalcTopPartonHistory( const std::string& name );
      virtual ~CalcTopPartonHistory() {}

      CalcTopPartonHistory(const CalcTopPartonHistory& rhs) = delete;
      CalcTopPartonHistory(CalcTopPartonHistory&& rhs) = delete;
      CalcTopPartonHistory& operator=(const CalcTopPartonHistory& rhs) = delete;
      
      ///Store the four-momentum of the post-FSR top or anti-top found using statusCodes
      ///This would only work if there is at most one "true" top of each charge (i.e. won't work for SS tops or 4 tops)
      ///This code was adapted from the 7TeV parton-level differential ttbar routine: https://svnweb.cern.ch/trac/atlasphys-top/browser/Physics/Top/Software/MCvalidation/Rivet/Rivet2.X/trunk/routines/ATLAS_2014_I1304289/ATLAS_2014_I1304289.cc
      bool topAfterFSR_SC(const xAOD::TruthParticleContainer* truthParticles, int start, TLorentzVector& top_afterFSR_SC_p4);
     
      ///Store the four-momentum of several particles in the top decay chain
      bool topWb(const xAOD::TruthParticleContainer* truthParticles, int start, TLorentzVector& t_beforeFSR_p4, TLorentzVector& t_afterFSR_p4, TLorentzVector& W_p4, TLorentzVector& b_p4, TLorentzVector& Wdecay1_p4, int& Wdecay1_pdgId, TLorentzVector& Wdecay2_p4, int& Wdecay2_pdgId);
      
      ///Store the four-momentum of b (not from tops_ before and after FSR
      bool b(const xAOD::TruthParticleContainer* truthParticles, TLorentzVector& b_beforeFSR, TLorentzVector& b_afterFSR);
      
      ///Store the four-momentum of several particles in the W decay chain
      bool Wlv(const xAOD::TruthParticleContainer* truthParticles, TLorentzVector& W_p4, TLorentzVector& Wdecay1_p4, int& Wdecay1_pdgId, TLorentzVector& Wdecay2_p4, int& Wdecay2_pdgId);
      
      ///Return particle after FSR (before the decay vertex)
      const xAOD::TruthParticle* findAfterFSR(const xAOD::TruthParticle* particle);
      
      ///Return true when particle is a top before FSR
      bool hasParticleIdenticalParent(const xAOD::TruthParticle* particle);
            
      virtual StatusCode execute(); 
    protected:
      std::shared_ptr<top::TopConfig> m_config;
      
      void fillEtaBranch(xAOD::PartonHistory* partonHistory,std:: string branchName, TLorentzVector &tlv);
      
  }; 
}

#endif
