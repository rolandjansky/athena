/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

// ----------------------------------------------
//
//  xAODBSignalFilter.h
//
//  Author:      Malte Muller, August 2002
//  Modified by: Cristiano Alpigiani 
//               <Cristiano.Alpigiani@cern.ch>, 
//               September 2013
//  
// ----------------------------------------------
#ifndef GENERATORFILTERSXAODBSIGNALFILTER_H
#define GENERATORFILTERSXAODBSIGNALFILTER_H

#include "GeneratorModules/GenFilter.h"
#include "GaudiKernel/NTuple.h"
#include <vector>

#include "AtlasHepMC/GenEvent.h"
#include "AtlasHepMC/GenParticle.h"
#include "AtlasHepMC/GenVertex.h"
#include "TTree.h"
#include "TLorentzVector.h"

#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthParticle.h"

class xAODBSignalFilter : public GenFilter
{
  
 public:
  
  xAODBSignalFilter(const std::string& name, ISvcLocator* pSvcLocator);
  //
  virtual StatusCode filterFinalize();
  virtual StatusCode filterEvent();
 
 private:

  // ** Private data members **
  
  // For cuts on final particle pT and eta
  bool   m_cuts_f_e_on,    m_cuts_f_mu_on,   m_cuts_f_had_on,
         m_cuts_f_gam_on,  m_cuts_f_K0_on,
         m_localLVL1MuonCutOn, m_localLVL2MuonCutOn,
         m_localLVL2ElectronCutOn,
         m_InvMass_switch, m_TotalInvMass_switch;
  double m_cuts_f_e_pT,    m_cuts_f_mu_pT,   m_cuts_f_had_pT,
         m_cuts_f_gam_pT,  m_cuts_f_K0_pT;
  double m_cuts_f_e_eta,   m_cuts_f_mu_eta,  m_cuts_f_had_eta,
         m_cuts_f_gam_eta, m_cuts_f_K0_eta;
  double m_localLVL1MuonCutPT,  m_localLVL1MuonCutEta, m_localLVL2MuonCutPT,
         m_localLVL2MuonCutEta, m_localLVL2ElectronCutPT,
         m_localLVL2ElectronCutEta;
  double m_LVL1Counter;     // Counting variable for events passing LVL1
  double m_LVL2Counter;     // Counting variable for events passing LVL2
  double m_rejectedTrigger; // Failed to pass trigger
  double m_rejectedAll;     // Failed to pass filter (both trigger and signal selection)  
  int    m_B_pdgid;       // pdgID of the mother
  int    m_InvMass_PartId1, m_InvMass_PartId2; // pdgID of the couple used for mass cuts
  double m_InvMass_PartFakeMass1, m_InvMass_PartFakeMass2;
  double m_InvMassMin, m_InvMassMax;           // Mass range for invariant mass cut
  double m_TotalInvMassMin, m_TotalInvMassMax;
  //
  int    m_EventCnt;
		
  // ** Private member functions **

  // Find child
  void FindAllChildren(const xAOD::TruthParticle* mother,std::string treeIDStr,  
		       bool fromFinalB, bool &foundSignal, bool &passedAllCuts,
		       TLorentzVector &p1, TLorentzVector &p2, bool fromSelectedB, 
                       TLorentzVector &total_4mom) const;

  // Check whether child has pass cuts
  bool FinalStatePassedCuts(const xAOD::TruthParticle* child) const; 

  // Test whether final states pass cuts
   bool test_cuts(const double myPT, const double testPT,
		 const double myEta, const double testEta) const;
  
  // LVL1 and LVL2 cuts
  bool LVL1_Mu_Trigger(const xAOD::TruthParticle* child) const;
  bool LVL2_eMu_Trigger(const xAOD::TruthParticle* child) const;
  
  // Print child (for debug)
  void PrintChild(const xAOD::TruthParticle* child, const std::string& treeIDStr, const bool fromFinalB) const; 
  
};


#endif
