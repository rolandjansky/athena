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
  m_decorate(true)
{
  declareProperty( "ConfigFile",    m_configFile = "" );
  declareProperty( "CalibArea",     m_calibArea = "" );
  declareProperty( "CalcSF",        m_calcSF = false );
  declareProperty( "WorkingPoint",  m_wkpt = "" );
  declareProperty( "TaggerType",    m_tagType = "XXX" );
  declareProperty( "DecorateJet",   m_decorate = true );
  declareProperty( "Decoration",    m_decorationName = "XX" );
  declareProperty( "IsMC",          m_IsMC = true );

  // Truth labeling properties
  declareProperty( "UseTRUTH3",                     m_truthLabelUseTRUTH3 = true );
  declareProperty( "TruthParticleContainerName",    m_truthParticleContainerName = "TruthParticles" );
  declareProperty( "TruthBosonContainerName",       m_truthBosonContainerName = "TruthBosonsWithDecayParticles" );
  declareProperty( "TruthTopQuarkContainerName",    m_truthTopQuarkContainerName = "TruthTopQuarkWithDecayParticles" );

  // Keras properties
  declareProperty( "CalibAreaKeras",  m_kerasCalibArea = "BoostedJetTaggers/TopoclusterTopTagger/Boost2017/" );
  declareProperty( "KerasConfigFile", m_kerasConfigFileName = "XXX" );
  declareProperty( "KerasOutput",     m_kerasConfigOutputName = "XXX" );

  // TMVA properties
  declareProperty( "CalibAreaTMVA",  m_tmvaCalibArea = "BoostedJetTaggers/JSSWTopTaggerBDT/Boost2017/" );
  declareProperty( "TMVAConfigFile", m_tmvaConfigFileName = "XXX" );
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
