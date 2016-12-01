/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * Tau score flattening tool
 *
 * Author: Lorenz Hauswald
 */

#include "TauDiscriminant/TauScoreFlatteningTool.h"
#include "TFile.h"
#include "TKey.h"
#include <algorithm>

TauScoreFlatteningTool::TauScoreFlatteningTool(const std::string& name):
  TauRecToolBase(name),
  m_inputFile(""),
  m_effVeryLoose1P(0.95),
  m_effLoose1P(0.65),
  m_effMedium1P(0.55),
  m_effTight1P(0.45),
  m_effVeryLoose3P(0.65),
  m_effLoose3P(0.65),
  m_effMedium3P(0.55),
  m_effTight3P(0.45),
  m_defineWP(false)
{
  declareProperty("FlatteningFile", m_inputFile);

  declareProperty("DefineWPs", m_defineWP);

  declareProperty("SigEffWPVeryLoose1P", m_effVeryLoose1P);
  declareProperty("SigEffWPLoose1P", m_effLoose1P);
  declareProperty("SigEffWPMedium1P", m_effMedium1P);
  declareProperty("SigEffWPTight1P", m_effTight1P);

  declareProperty("SigEffWPVeryLoose3P", m_effVeryLoose3P);
  declareProperty("SigEffWPLoose3P", m_effLoose3P);
  declareProperty("SigEffWPMedium3P", m_effMedium3P);
  declareProperty("SigEffWPTight3P", m_effTight3P);

}

StatusCode TauScoreFlatteningTool::initialize()
{
  m_inputFile = find_file(m_inputFile);
  TFile tmpFile(m_inputFile.c_str());

  fetchInput(static_cast<TDirectory*>(tmpFile.Get("1prong")), m_graphs1p);
  fetchInput(static_cast<TDirectory*>(tmpFile.Get("3prong")), m_graphs3p);
  
  return StatusCode::SUCCESS;
}

void TauScoreFlatteningTool::fetchInput(TDirectory* directory, std::vector<TauCutGraph>& graphvector)
{
  auto keys = directory->GetListOfKeys();
  TIter next(keys);
  TKey* k;
  while((k = static_cast<TKey*>(next()))){
    if(static_cast<std::string>(k->GetClassName()) == "TGraph2D"){
      std::unique_ptr<TGraph2D> graph = std::unique_ptr<TGraph2D>{static_cast<TGraph2D*>(k->ReadObj())};
      graph->SetDirectory(nullptr);
      graphvector.push_back(std::move(graph)); //a TauCutGraph object is created and added to the vector
    }
  }
  
  std::sort(graphvector.begin(), graphvector.end()); //sort by signal efficiency
}

StatusCode TauScoreFlatteningTool::execute(xAOD::TauJet& tau)
{
  static SG::AuxElement::ConstAccessor<int> acc_nVertex("NUMVERTICES");
  static SG::AuxElement::ConstAccessor<float> acc_score("BDTJetScore");
  //  static SG::AuxElement::ConstAccessor<float> acc_pt("ptMvaTES");
  static SG::AuxElement::ConstAccessor<float> acc_pt("ptFinalCalib");
  static SG::AuxElement::ConstAccessor<int> acc_numTrack("NUMTRACK");
  static SG::AuxElement::Accessor<float> acc_newScore("BDTJetScoreSigTrans");

  bool is1P = acc_numTrack(tau) == 1;
  std::vector<TauCutGraph>& graphs = is1P ? m_graphs1p : m_graphs3p;

  
  auto itFirstPass = std::lower_bound(graphs.begin(), graphs.end(), tau, &TauScoreFlatteningTool::passesCut);
  double lowerCut = 0.;
  double upperEff = 1.;
  if(itFirstPass != graphs.end()){
    lowerCut = (*itFirstPass).GetCut(acc_pt(tau), acc_nVertex(tau));
    upperEff = (*itFirstPass).GetEfficiency();
  }
  double upperCut = 1.;
  double lowerEff = 0.;
  if(itFirstPass != graphs.begin()){
    upperCut = (*(itFirstPass-1)).GetCut(acc_pt(tau), acc_nVertex(tau));
    lowerEff = (*(itFirstPass-1)).GetEfficiency();
  }
  
  double newscore = 1 - (acc_score(tau)-lowerCut)*(upperEff-lowerEff)/(upperCut-lowerCut) - lowerEff;
  acc_newScore(tau) = newscore;

  if(m_defineWP)
    {
      if(is1P)
	{
	  tau.setIsTau(xAOD::TauJetParameters::JetBDTSigLoose, newscore > (1-m_effLoose1P));
	  tau.setIsTau(xAOD::TauJetParameters::JetBDTSigMedium, newscore > (1-m_effMedium1P));
	  tau.setIsTau(xAOD::TauJetParameters::JetBDTSigTight, newscore > (1-m_effTight1P));
	  tau.auxdecor<bool>("IsVeryLoose")  = newscore > (1-m_effVeryLoose1P);
	} else 
	{
	  tau.setIsTau(xAOD::TauJetParameters::JetBDTSigLoose, newscore > (1-m_effLoose3P));
	  tau.setIsTau(xAOD::TauJetParameters::JetBDTSigMedium, newscore > (1-m_effMedium3P));
	  tau.setIsTau(xAOD::TauJetParameters::JetBDTSigTight,  newscore > (1-m_effTight3P));
	  tau.auxdecor<bool>("IsVeryLoose")  =  newscore > (1-m_effVeryLoose3P);
	}
    }
  return StatusCode::SUCCESS;
}

StatusCode TauScoreFlatteningTool::finalize()
{
  return StatusCode::SUCCESS;
}

bool TauScoreFlatteningTool::passesCut(const TauCutGraph& graph, const xAOD::TauJet& tau){
  static SG::AuxElement::ConstAccessor<int> acc_nVertex("NUMVERTICES");
  static SG::AuxElement::ConstAccessor<float> acc_score("BDTJetScore");
  //  static SG::AuxElement::ConstAccessor<float> acc_pt("ptMvaTES");
  static SG::AuxElement::ConstAccessor<float> acc_pt("ptFinalCalib");
  return acc_score(tau) < graph.GetCut(acc_pt(tau), acc_nVertex(tau));
}
