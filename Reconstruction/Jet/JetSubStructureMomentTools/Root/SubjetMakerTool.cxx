/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/SubjetMakerTool.h"

#include <vector>
#include <sstream>
#include <string>
#include <fastjet/PseudoJet.hh>
#include <fastjet/ClusterSequence.hh>

#include "xAODJet/Jet.h"

#include "JetEDM/JetConstituentFiller.h"

using namespace std;
using fastjet::PseudoJet;

SubjetMakerTool::SubjetMakerTool(std::string name) : 
  JetSubStructureMomentToolsBase(name)
{
  declareProperty("type", m_type = "AntiKt");
  declareProperty("R", m_R = 0.2);
  declareProperty("PtCut", m_minPt = 20e3);
  ATH_MSG_DEBUG("Initializing SubjetMaker tool.");
}

int SubjetMakerTool::modifyJet(xAOD::Jet &jet) const {
  if(checkForConstituents(jet) == false) return 1;

  std::vector<float> s_e;
  std::vector<float> s_px;
  std::vector<float> s_py;
  std::vector<float> s_pz;

  std::vector<fastjet::PseudoJet> constit_pseudojets = jet::JetConstituentFiller::constituentPseudoJets(jet);
  fastjet::PseudoJet pjet = fastjet::join(constit_pseudojets);

  fastjet::JetAlgorithm jetalg = fastjet::antikt_algorithm;
  if (m_type == "AntiKt") {
    jetalg = fastjet::antikt_algorithm;
  } else if (m_type == "Kt") {
    jetalg = fastjet::kt_algorithm;
  } else if (m_type == "CamKt") {
    jetalg = fastjet::cambridge_algorithm;
  }

  fastjet::JetDefinition microjet_def(jetalg, m_R);
  fastjet::ClusterSequence microjet_cs(pjet.constituents(), microjet_def);
  vector<fastjet::PseudoJet> microjets = fastjet::sorted_by_pt(microjet_cs.inclusive_jets(m_minPt));
  for (size_t z = 0; z < microjets.size(); ++z) {
    s_e.push_back(microjets[z].e());
    s_px.push_back(microjets[z].px());
    s_py.push_back(microjets[z].py());
    s_pz.push_back(microjets[z].pz());
  }

  std::stringstream ss;
  ss.str("");
  ss << "Subjet" << m_type << (int) (m_R*100) << "_e";
  jet.setAttribute(ss.str().c_str(), s_e);
  ss.str("");
  ss << "Subjet" << m_type << (int) (m_R*100) << "_px";
  jet.setAttribute(ss.str().c_str(), s_px);
  ss.str("");
  ss << "Subjet" << m_type << (int) (m_R*100) << "_py";
  jet.setAttribute(ss.str().c_str(), s_py);
  ss.str("");
  ss << "Subjet" << m_type << (int) (m_R*100) << "_pz";
  jet.setAttribute(ss.str().c_str(), s_pz);

  return 0;
}
