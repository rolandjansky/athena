/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ---------------------------------------------------------------------- 

#include <iostream>

#include "TH1.h"
#include "TMath.h"

#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/JetDefinition.hh"
#include "fastjet/SISConePlugin.hh"

#include "../HepMCAnalysis_i/UserAnalysis.h"


// ---------------------------------------------------------------------- 
UserAnalysis::UserAnalysis(): 
  m_h_njets(0), m_Ptjet(0)
{}

// ---------------------------------------------------------------------- 
UserAnalysis::~UserAnalysis()
{}

// ---------------------------------------------------------------------- 
int UserAnalysis::Init(double maxEta, double minPt)
{
  m_max_eta = maxEta;
  m_min_pt = minPt;
  
  m_outputFileName = "UserAnalysis.root";
  m_outputRootDir = "User";
 
  m_h_njets = initHist("njets", "N_jets", "Number of Jets", 15, 0.0, 15.0);
  m_Ptjet = initHist("Ptjet", "jet p_{T}", "jet p_{T}", 50, 0, 200);

  return true; 
}

// ---------------------------------------------------------------------- 
int UserAnalysis::Process(HepMC::GenEvent *event)
{ 
  if (event->event_number() % 1000 == 0) {
    std::cout << "Processing event " << event->event_number() << std::endl;
  }
  
  m_h_njets->Fill(m_inclusive_jets.size());

  for (size_t i = 0; i < m_inclusive_jets.size(); ++i) {
    m_Ptjet->Fill(m_inclusive_jets[i].perp());
  }
  
  return true;
}
 
