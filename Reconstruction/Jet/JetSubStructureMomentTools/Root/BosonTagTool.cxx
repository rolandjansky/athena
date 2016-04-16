/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetSubStructureMomentTools/BosonTagTool.h"
#include "JetSubStructureUtils/BosonTag.h"

using namespace std;

BosonTagTool::BosonTagTool(std::string name) :
  JetSubStructureMomentToolsBase(name)
{
  declareProperty("WorkingPoint", m_working_point               = "medium");
  declareProperty("TaggerAlgorithm", m_tagger_alg               = "smooth");
#ifdef ROOTCORE
  declareProperty("RecommendationsFile", m_recommendations_file = "$ROOTCOREBIN/data/JetSubStructureUtils/config_13TeV_Wtagging_MC15_Prerecommendations_20150809.dat");
#else
  declareProperty("RecommendationsFile", m_recommendations_file =  "JetSubStructureUtils/data/config_13TeV_Wtagging_MC15_Prerecommendations_20150809.dat");
#endif
  declareProperty("Debug", m_debug                              = false);
  declareProperty("Verbose", m_verbose                          = false);
}

int BosonTagTool::modifyJet(xAOD::Jet &jet) const
{
  static JetSubStructureUtils::BosonTag tagger(m_working_point, m_tagger_alg, m_recommendations_file, m_debug, m_verbose);
  jet.setAttribute("BosonTag", static_cast<int>(tagger.result(jet)));

  return 0;
}
