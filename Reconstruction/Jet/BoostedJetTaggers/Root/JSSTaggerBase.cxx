/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "BoostedJetTaggers/JSSTaggerBase.h"

JSSTaggerBase::JSSTaggerBase(const std::string &name) :
  asg::AsgTool(name),
  m_calibArea(""),
  m_jetPtMin(200000.),
  m_jetPtMax(3000000.),
  m_jetEtaMax(2.0),
  m_decorate(true),
  m_dec_weight("weightdec"),
  m_dec_efficiency("efficiencydec"),
  m_dec_effSF("effSFdec")
{
  declareProperty( "ConfigFile",    m_configFile = "",        "Name of config file" );
  declareProperty( "CalibArea",     m_calibArea = "",         "Path to config file" );
  declareProperty( "CalcSF",        m_calcSF = false,         "Flag to calculate the efficiency SF"  );
  declareProperty( "WorkingPoint",  m_wkpt = "",              "If specified, name of working point is added to the variable names" );
  declareProperty( "TaggerType",    m_tagType = "XXX",        "Tagger type (e.g. SmoothedWZTagger, JSSWTopTaggerDNN, etc.)" );
  declareProperty( "DecorateJet",   m_decorate = true,        "Flag to decorate tagging results to xAOD::Jet" );
  declareProperty( "Decoration",    m_decorationName = "XX",  "Prefix for the variables decorated to xAOD::Jet" );
  declareProperty( "IsMC",          m_IsMC = true,            "Flag to identify data or MC" );

  // Truth labeling properties
  declareProperty( "UseTRUTH3",                     m_truthLabelUseTRUTH3 = true,
		   "Flag to use TRUTH3 containers. If false, TRUTH1 format is used." );
  declareProperty( "TruthParticleContainerName",    m_truthParticleContainerName = "TruthParticles",
		   "Name of truth-particle container (with UseTRUTH3=false). TruthParticles by default" );
  declareProperty( "TruthBosonContainerName",       m_truthBosonContainerName = "TruthBosonsWithDecayParticles",
		   "Name of truth-boson container (with UseTRUTH3=true). TruthBosonWithDecayParticles by default" );
  declareProperty( "TruthTopQuarkContainerName",    m_truthTopQuarkContainerName = "TruthTopQuarkWithDecayParticles",
		   "Name of truth-top container (with UseTRUTH3=true). TruthTopQuarkWithDecayParticles by default" );

  // Keras properties
  declareProperty( "CalibAreaKeras",  m_kerasCalibArea = "BoostedJetTaggers/TopoclusterTopTagger/Boost2017/",
		   "Path to json file to configure ML-taggers (Keras)" );
  declareProperty( "KerasConfigFile", m_kerasConfigFileName = "XXX",
		   "Name of json file to configure ML-taggers (Keras)" );
  declareProperty( "KerasOutput",     m_kerasConfigOutputName = "XXX",
		   "Name of output variable by the ML-tagger (Keras)" );

  // TMVA properties
  declareProperty( "CalibAreaTMVA",  m_tmvaCalibArea = "BoostedJetTaggers/JSSWTopTaggerBDT/Boost2017/",
		   "Path to xml file to configure ML-taggers (TMVA)" );
  declareProperty( "TMVAConfigFile", m_tmvaConfigFileName = "XXX",
		   "Name of xml file to configure ML-taggers (TMVA)" );

  // tagging scale factors
  declareProperty( "WeightDecorationName",      m_weightdecorationName = "SF",
		   "Name of SF variable decorated to xAOD::Jet" );
  declareProperty( "WeightFile",                m_weightFileName = "",
		   "Name of config ROOT file for SF calculation" );
  declareProperty( "WeightHistogramName",       m_weightHistogramName = "",
		   "Name of SF histograms in the ROOT file" );
  declareProperty( "EfficiencyHistogramName",   m_efficiencyHistogramName = "",
		   "Name of efficiency histograms in the ROOT file" );
  declareProperty( "WeightFlavors",             m_weightFlavors = "",
		   "List of jet flavours for which the SF is available. Divided by comma" );
}

void JSSTaggerBase::showCuts() const{
  int nCuts = m_accept.getNCuts();
  for(int iCut=0; iCut<nCuts; iCut++){
    std::string cut_string = "";
    cut_string += "  (";
    cut_string += std::to_string(iCut);
    cut_string += ")  ";
    cut_string += m_accept.getCutName(iCut).Data();
    cut_string += " : ";
    cut_string += m_accept.getCutDescription(iCut).Data();
    ATH_MSG_INFO( cut_string );
  }
}
