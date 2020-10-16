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
    m_myFitter = std::unique_ptr<top::KLFitterTool> (new top::KLFitterTool(m_name));
    top::check(m_myFitter->setProperty("config", config), "Failed to setProperty of KLFitterTool");
    top::check(m_myFitter->setProperty("LeptonType", kLeptonType), "Failed to setProperty of KLFitterTool");
    top::check(m_myFitter->setProperty("CustomParameters", kCustomParameters), "Failed to setProperty of KLFitterTool");
    top::check(m_myFitter->setProperty("SelectionName", kSelectionName), "Failed to setProperty of KLFitterTool");
    top::check(m_myFitter->initialize(), "Failed to initialize KLFitterTool");
  }

  bool KLFitterRun::apply(const top::Event& event) const {

    if(m_useJetAutoSet) {
      std::vector<std::vector<unsigned int> > jetsToRun = top::KLFitterRun::setJetsToRun(event);
      top::check( jetsToRun[CANBEB].size()>=2,"less than 2 b-jets" );
      top::check( jetsToRun[CANBELF].size()>=4,"less than 4 LF-jets" );

      top::check( m_myFitter->setProperty("CanBeBJets",jetsToRun[CANBEB]), "Error when passing the B jets" );
      top::check( m_myFitter->setProperty("CanBeLFJets",jetsToRun[CANBELF]), "Error when passing the LF jets" );
    }
    top::check(m_myFitter->execute(event), "Failed to run KLFitterTool");
    return true;
  }

  std::vector<std::vector<unsigned int> > KLFitterRun::setJetsToRun(const top::Event& event) const {

    //first order in b-weight
    unsigned int index(0);
    std::vector<std::pair<unsigned int,std::pair<double,double> > > bwOrd_jets(0);
    for (const auto& jet : event.m_jets) {
      double weight(-99.);
      m_myFitter->HasTag(*jet, weight);
      std::cout << ":) " << weight << std::endl;
      bwOrd_jets.push_back(std::make_pair(index,std::make_pair(weight,jet->pt())));
      index++;
    }
    std::sort(bwOrd_jets.begin(), bwOrd_jets.end(), [](auto &left, auto &right) { return left.second.first > right.second.first; });

    //then separate which jet can be what
    std::vector<std::pair<unsigned int,std::pair<double,double> > > onlyBjets(bwOrd_jets.begin(),bwOrd_jets.begin()+m_nb);
    std::vector<std::pair<unsigned int,std::pair<double,double> > > canbeBOTHjets(bwOrd_jets.begin()+m_nb,bwOrd_jets.begin()+m_nb+m_delta);
    std::vector<std::pair<unsigned int,std::pair<double,double> > > onlyQjets(bwOrd_jets.begin()+m_nb+m_delta,bwOrd_jets.end()); 

    //now cut away excess jets (if any)
    //always re-order the LF in pt, before cutting
    std::sort(onlyQjets.begin(), onlyQjets.end(), [](auto &left, auto &right) { return left.second.second > right.second.second; }); 

    //remove the extra LF jets: 
    int NLFjcut=m_Njcut-(m_nb+m_delta);
    if((int)onlyQjets.size()>NLFjcut) onlyQjets.erase(onlyQjets.begin()+NLFjcut,onlyQjets.end());

    //now merge everything to prepare the lists
    std::vector<std::pair<unsigned int,std::pair<double,double> > > Bjets = onlyBjets;
    Bjets.insert(Bjets.end(),canbeBOTHjets.begin(),canbeBOTHjets.end());
    std::vector<std::pair<unsigned int,std::pair<double,double> > > LFjets = onlyQjets;
    LFjets.insert(LFjets.end(),canbeBOTHjets.begin(),canbeBOTHjets.end());

    //re-sort in pt because we like it more (won't change a thing on how KLFitter loops)
    std::sort(Bjets.begin(), Bjets.end(), [](auto &left, auto &right) { return left.second.second > right.second.second; }); 
    std::sort(LFjets.begin(), LFjets.end(), [](auto &left, auto &right) { return left.second.second > right.second.second; }); 

    //now fill the output lists
    std::vector<std::vector<unsigned int> > jetSets(JETTYPESIZE, std::vector<unsigned int>(0));
    for(uint jj=0;jj<Bjets.size();jj++) jetSets[CANBEB].push_back(Bjets.at(jj).first);
    for(uint jj=0;jj<LFjets.size();jj++) jetSets[CANBELF].push_back(LFjets.at(jj).first);

    return jetSets;
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
