/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/BoostedXbbTagTool.h"
#include "JetSubStructureUtils/BoostedXbbTag.h"
#include "xAODMuon/Muon.h"
#include "AsgTools/Check.h"

using namespace std;

BoostedXbbTagTool::BoostedXbbTagTool(std::string name) :
  JetSubStructureMomentToolsBase(name)
{
  declareProperty("WorkingPoint", m_working_point               = "medium");
#ifdef ROOTCORE
  declareProperty("RecommendationsFile", m_recommendations_file = "$ROOTCOREBIN/data/JetSubStructureUtils/config_13TeV_Htagging_MC15_Prerecommendations_20150812.dat");
#else
  declareProperty("RecommendationsFile", m_recommendations_file =  "JetSubStructureUtils/data/config_13TeV_Htagging_MC15_Prerecommendations_20150812.dat");
#endif
  declareProperty("BosonType", m_boson_type                     = "Higgs");
  declareProperty("JetAlgorithmName", m_algorithm_name          = "AK10LCTRIMF5R20");
  declareProperty("NumBTags", m_num_bTags                       = 2);
  declareProperty("DecorPrefix", m_decor_prefix                 = "");
  declareProperty("Debug", m_debug                              = false);
  declareProperty("Verbose", m_verbose                          = false);
  declareProperty("MuonContainerName", m_muon_container_name    = "Muons");
}

int BoostedXbbTagTool::modifyJet(xAOD::Jet &jet) const
{
  static JetSubStructureUtils::BoostedXbbTag tagger(m_working_point, m_recommendations_file, m_boson_type, m_algorithm_name, m_num_bTags, m_decor_prefix, m_debug, m_verbose);

  const xAOD::MuonContainer *muons = 0;
  ASG_CHECK(evtStore()->retrieve(muons, m_muon_container_name));

  jet.setAttribute("BoostedXbbTag", static_cast<int>(tagger.result(jet, muons)));

  return 0;
}
