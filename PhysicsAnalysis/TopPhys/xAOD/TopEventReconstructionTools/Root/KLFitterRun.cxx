/*
   Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
 */

#include "TopEventReconstructionTools/KLFitterRun.h"

#include "TopEvent/Event.h"
#include "TopEvent/EventTools.h"
#include "TopConfiguration/TopConfig.h"
#include "TopConfiguration/Tokenize.h"

#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>

namespace top {
  KLFitterRun::KLFitterRun(const std::string& kSelectionName, const std::string& kParameters,
                           std::shared_ptr<top::TopConfig> config) :
    m_name(""),
    m_useJetAutoSet(false),
    m_Njcut(-1),
    m_nb(-1),
    m_delta(-1),
    m_myFitter(nullptr) {
    std::string kLeptonType = "";
    std::string kCustomParameters = "";
    if (kParameters.find(" ") != std::string::npos) {
      kLeptonType = kParameters.substr(0, kParameters.find(" "));
      kCustomParameters = kParameters.substr(kParameters.find(" ") + 1);
      m_useJetAutoSet = top::KLFitterRun::hasAutoSetOption(kCustomParameters);
    } else kLeptonType = kParameters;
    m_name = "RECO::KLFitterRun_" + kLeptonType;
    if(m_useJetAutoSet) m_name=m_name+Form("_Nb%dDelta%dNjcut%d",m_nb,m_delta,m_Njcut);
    m_myFitter = std::unique_ptr<top::KLFitterTool> (new top::KLFitterTool(m_name));
    top::check(m_myFitter->setProperty("config", config), "Failed to setProperty of KLFitterTool");
    top::check(m_myFitter->setProperty("LeptonType", kLeptonType), "Failed to setProperty of KLFitterTool");
    top::check(m_myFitter->setProperty("CustomParameters", kCustomParameters), "Failed to setProperty of KLFitterTool");
    top::check(m_myFitter->setProperty("SelectionName", kSelectionName), "Failed to setProperty of KLFitterTool");
    if(m_useJetAutoSet) {
      top::check( m_myFitter->setProperty("Nb",m_nb), "Error when setting Nb" );
      top::check( m_myFitter->setProperty("Delta",m_delta), "Error when setting Delta" );
      top::check( m_myFitter->setProperty("Njcut",m_Njcut), "Error when setting Njcut" );
    }

    top::check(m_myFitter->initialize(), "Failed to initialize KLFitterTool");
  }

  bool KLFitterRun::apply(const top::Event& event) const {

    top::check(m_myFitter->execute(event), "Failed to run KLFitterTool");
    return true;
  }


 
  bool KLFitterRun::hasAutoSetOption(const std::string custom_parameters) {
    const std::string autoSetOpt="KLFitterJetSelectionMode:kAutoSet";
    const TString nbOpt="nb";
    const TString deltaOpt="delta";
    const TString njcutOpt="Njcut";

    if(custom_parameters.find(autoSetOpt)==std::string::npos) return false;
    std::vector<std::string> custom_tokens;
    tokenize(custom_parameters, custom_tokens, " ");
    for(auto token : custom_tokens) {
      if(token.find(autoSetOpt)==std::string::npos) continue;
      std::vector<TString> custom_subtokens;
      tokenize(token, custom_subtokens, ":");
      for(TString subtok : custom_subtokens) {
	if(subtok.BeginsWith(nbOpt)) {
	  subtok.ReplaceAll(nbOpt,"");
	  if(subtok.IsFloat()) m_nb=subtok.Atoi();
	}
	if(subtok.BeginsWith(deltaOpt)) {
	  subtok.ReplaceAll(deltaOpt,"");
	  if(subtok.IsFloat()) m_delta=subtok.Atoi();
	}
	if(subtok.BeginsWith(njcutOpt)) {
	  subtok.ReplaceAll(njcutOpt,"");
	  if(subtok.IsFloat()) m_Njcut=subtok.Atoi();
	}
      }
      break;
    }

    std::string errorMessage = "Bad AutoSet option. It should be: '"+(std::string)autoSetOpt+"."+(std::string)nbOpt+"<X>."+(std::string)deltaOpt+"<Y>."+(std::string)njcutOpt+"<Z>' where <X>, <Y> and <Z> are positive integers. Order of nb, delta and Njcut is irrelevant";

    bool allok=(m_Njcut>=6 &&  m_delta>=0 && m_nb>=2);
    top::check(allok,errorMessage);

    top::check(m_delta+m_nb<=m_Njcut,"Njcut must be >= (nb+delta)");

    return allok;
  }



  std::string KLFitterRun::name() const {
    return m_name;
  }
}
