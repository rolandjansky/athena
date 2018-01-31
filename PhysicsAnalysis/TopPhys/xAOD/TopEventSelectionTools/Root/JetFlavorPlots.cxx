/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/JetFlavorPlots.h"

#include <cmath>
#include <array>
#include <string>

#include "TopEvent/EventTools.h"
#include "TopConfiguration/TopConfig.h"
#include "TopCorrections/ScaleFactorRetriever.h"

#include "TH1.h"
#include "TH2D.h"
#include "TFile.h"

// Systematic include(s):
#include "PATInterfaces/SystematicSet.h"

namespace top {

const double JetFlavorPlots::toGeV = 0.001;

JetFlavorPlots::JetFlavorPlots(const std::string& name,
                               TFile* outputFile, const std::string& params, std::shared_ptr<top::TopConfig> config, EL::Worker* wk) :
    m_nominalHashValue(0),
    m_detailed(false),
    // default pT and eta binning, and default max number of Jets
    m_ptBins ("15:20:30:45:60:80:110:160:210:260:310:400:500:600:800:1000:1200:1500:1800:2500"),
    m_etaBins ("0.:0.3:0.8:1.2:2.1:2.8:3.6:4.5"),
    m_nJetsMax(15),
    m_jetCollection(""),
    m_config (config) {

    // create the JetFlavorPlots and JetFlavorPlots_Loose directories only if needed
    if (m_config->doTightEvents()) m_hists = std::make_shared<PlotManager>(name+"/JetFlavorPlots", outputFile, wk);
    if (m_config->doLooseEvents()) m_hists_Loose = std::make_shared<PlotManager>(name+"/JetFlavorPlots_Loose", outputFile, wk);

    CP::SystematicSet nominal;
    m_nominalHashValue = nominal.hash();

    // retrieve jet collection and remove the "Jets" at the end of it
    m_jetCollection = m_config->sgKeyJets();
    m_jetCollection.erase(m_jetCollection.length() - 4);

    //decoding arguments
    std::istringstream stream(params);
    std::string s;
    while (stream>>s) {
      if (s=="detailed")
        m_detailed = true;
      else if (s.substr(0,3)=="pt:" || s.substr(0,3)=="pT:" )
        m_ptBins = s.substr(3,s.size()-3);
      else if (s.substr(0,7)=="abseta:" )
        m_etaBins = s.substr(7,s.size()-7);
      else if (s.substr(0,9)=="nJetsMax:" )
        m_nJetsMax = std::atoi(s.substr(9,s.size()-9).c_str());
      else {
        std::cout << "ERROR: Can't understand argument " << s << " for JETFLAVORPLOTS." << std::endl;
        exit(1);
      }
    }

    //handle binning
    std::vector<double> ptBinning;
    std::vector<double> etaBinning;
    formatBinning(m_ptBins, ptBinning);
    formatBinning(m_etaBins, etaBinning);
    std::cout << "INFO: Here is the binning used for JETFLAVORPLOTS in selection "<<name<<":" << std::endl;
    std::cout<<"pt: ";
    for (auto pt:ptBinning) std::cout<<pt<<" ";
    std::cout<<std::endl;
    std::cout<<"abseta: ";
    for (auto eta:etaBinning) std::cout<<eta<<" ";
    std::cout<<std::endl;

     if(m_detailed){
      for (std::string flavour:{"gluon","lightquark","cquark","bquark","other"}){
        if (m_config->doTightEvents()) {
          // vs. nJets and flavour
          for (int i = 0; i <= m_nJetsMax; ++i){
            m_hists->addHist(flavour + "_jets_njet" + std::to_string(i) + "_" + m_jetCollection,";pT [GeV];|#eta^{jet}|;Events",
                            ptBinning.size()-1, ptBinning.data(), etaBinning.size()-1, etaBinning.data());
          }
          // vs. flavour only
          m_hists->addHist(flavour + "_jets_" + m_jetCollection,";pT [GeV];|#eta^{jet}|;Events",
                            ptBinning.size()-1, ptBinning.data(), etaBinning.size()-1, etaBinning.data());
        }
        if (m_config->doLooseEvents()) {
          // vs. nJets and flavour
          for (int i = 0; i <= m_nJetsMax; ++i){
            m_hists_Loose->addHist(flavour + "_jets_njet" + std::to_string(i) + "_" + m_jetCollection,";pT [GeV];|#eta^{jet}|;Events",
                            ptBinning.size()-1, ptBinning.data(), etaBinning.size()-1, etaBinning.data());
          }
          // vs. flavour only
          m_hists_Loose->addHist(flavour + "_jets_" + m_jetCollection,";pT [GeV];|#eta^{jet}|;Events",
                            ptBinning.size()-1, ptBinning.data(), etaBinning.size()-1, etaBinning.data());
        }
      }
      // if "detailed", also making the quark_jets one
      if (m_config->doTightEvents()) {
        m_hists->addHist("quark_jets_" + m_jetCollection, ";pT [GeV];|#eta^{jet}|;Events",
                        ptBinning.size()-1, ptBinning.data(), etaBinning.size()-1, etaBinning.data());
      }
      if (m_config->doLooseEvents()) {
        m_hists_Loose->addHist("quark_jets_" + m_jetCollection, ";pT [GeV];|#eta^{jet}|;Events",
                        ptBinning.size()-1, ptBinning.data(), etaBinning.size()-1, etaBinning.data());
      }
    }
    else {
      // the simplest case, one for gluon, one for quarks
      if (m_config->doTightEvents()) {
        m_hists->addHist("quark_jets_" + m_jetCollection, ";pT [GeV];|#eta^{jet}|;Events",
                        ptBinning.size()-1, ptBinning.data(), etaBinning.size()-1, etaBinning.data());
        m_hists->addHist("gluon_jets_" + m_jetCollection, ";pT [GeV];|#eta^{jet}|;Events",
                        ptBinning.size()-1, ptBinning.data(), etaBinning.size()-1, etaBinning.data());
      }
      if (m_config->doLooseEvents()) {
        m_hists_Loose->addHist("quark_jets_" + m_jetCollection, ";pT [GeV];|#eta^{jet}|;Events",
                        ptBinning.size()-1, ptBinning.data(), etaBinning.size()-1, etaBinning.data());
        m_hists_Loose->addHist("gluon_jets_" + m_jetCollection, ";pT [GeV];|#eta^{jet}|;Events",
                        ptBinning.size()-1, ptBinning.data(), etaBinning.size()-1, etaBinning.data());
      }
    }
}

bool JetFlavorPlots::apply(const top::Event& event) const {
    bool throwWarning=false;
    // only nominal
    if (event.m_hashValue != m_nominalHashValue)
        return true;

    // do loose or tight events only if requested
    if (event.m_isLoose && !m_config->doLooseEvents())
        return true;
    if (!event.m_isLoose && !m_config->doTightEvents())
        return true;

    // chosing the plotmanager depending if it's loose or tight
    std::shared_ptr<PlotManager> hists = event.m_isLoose?m_hists_Loose:m_hists;

    double eventWeight = 1.;
    if (top::isSimulation(event))
//       eventWeight = event.m_info->mcEventWeight();
      eventWeight = event.m_truthEvent->at(0)->weights()[0];// FIXME temporary bugfix
    else  // Only do this for MC
      return true;

    for (const auto* const jetPtr : event.m_jets) {
      int jet_flavor = -99;
      bool status = jetPtr->getAttribute<int>("PartonTruthLabelID", jet_flavor);
      if (!status) continue;

      if(m_detailed){
        if((unsigned int) m_nJetsMax>=event.m_jets.size()){
          // vs. nJets and flavour
          char name[200];
          if (jet_flavor == 21) {  // PDG ID for gluon is 21(9)
            sprintf(name,"gluon_jets_njet%lu_%s", event.m_jets.size(), m_jetCollection.c_str());
            static_cast<TH2D*>(hists->hist(name))->Fill(jetPtr->pt()*toGeV,
                                                        jetPtr->eta(),
                                                        eventWeight);
          }
          else if (jet_flavor >= 1 && jet_flavor <=3 ) {// PDG ID for d,u,s is 1,2,3
            sprintf(name,"lightquark_jets_njet%lu_%s", event.m_jets.size(), m_jetCollection.c_str());
            static_cast<TH2D*>(hists->hist(name))->Fill(jetPtr->pt()*toGeV,
                                                        jetPtr->eta(),
                                                        eventWeight);
          }
          else if (jet_flavor == 4) { // PDG ID for c is 4
            sprintf(name,"cquark_jets_njet%lu_%s", event.m_jets.size(), m_jetCollection.c_str());
            static_cast<TH2D*>(hists->hist(name))->Fill(jetPtr->pt()*toGeV,
                                                        jetPtr->eta(),
                                                        eventWeight);
          }
          else if (jet_flavor == 5) { // PDG ID for c is 5
            sprintf(name,"bquark_jets_njet%lu_%s",  event.m_jets.size(), m_jetCollection.c_str());
            static_cast<TH2D*>(hists->hist(name))->Fill(jetPtr->pt()*toGeV,
                                                        jetPtr->eta(),
                                                        eventWeight);
          }
          else {
            sprintf(name,"other_jets_njet%lu_%s",  event.m_jets.size(), m_jetCollection.c_str());
            static_cast<TH2D*>(hists->hist(name))->Fill(jetPtr->pt()*toGeV,
                                                        jetPtr->eta(),
                                                        eventWeight);
          }
          // if "detailed", also making the quark_jets one
          if (jet_flavor >= 1 && jet_flavor <=4 ) {
            sprintf(name,"quark_jets_%s", m_jetCollection.c_str());
            static_cast<TH2D*>(hists->hist(name))->Fill(jetPtr->pt()*toGeV,
                                                                jetPtr->eta(),
                                                                eventWeight);
          }
        }
        else
          throwWarning=true;
        // vs. flavour only
        char shortname[200];
        if (jet_flavor == 21) {  // PDG ID for gluon is 21(9)
          sprintf(shortname,"gluon_jets_%s", m_jetCollection.c_str());
          static_cast<TH2D*>(hists->hist(shortname))->Fill(jetPtr->pt()*toGeV,
                                                      jetPtr->eta(),
                                                      eventWeight);
        }
        else if (jet_flavor >= 1 && jet_flavor <=3 ) {// PDG ID for d,u,s is 1,2,3
          sprintf(shortname,"lightquark_jets_%s", m_jetCollection.c_str());
          static_cast<TH2D*>(hists->hist(shortname))->Fill(jetPtr->pt()*toGeV,
                                                      jetPtr->eta(),
                                                      eventWeight);
        }
        else if (jet_flavor == 4) { // PDG ID for c is 4
          sprintf(shortname,"cquark_jets_%s", m_jetCollection.c_str());
          static_cast<TH2D*>(hists->hist(shortname))->Fill(jetPtr->pt()*toGeV,
                                                      jetPtr->eta(),
                                                      eventWeight);
        }
        else if (jet_flavor == 5) { // PDG ID for c is 5
          sprintf(shortname,"bquark_jets_%s", m_jetCollection.c_str());
          static_cast<TH2D*>(hists->hist(shortname))->Fill(jetPtr->pt()*toGeV,
                                                      jetPtr->eta(),
                                                      eventWeight);
        }
        else {
          sprintf(shortname,"other_jets_%s", m_jetCollection.c_str());
          static_cast<TH2D*>(hists->hist(shortname))->Fill(jetPtr->pt()*toGeV,
                                                      jetPtr->eta(),
                                                      eventWeight);
        }

      }
      else {
        // the simplest case, one for gluon, one for quarks
        char name[200];
        if (jet_flavor == 21) {  // PDG ID for gluon is 21
          sprintf(name,"gluon_jets_%s", m_jetCollection.c_str());
          static_cast<TH2D*>(hists->hist(name))->Fill(jetPtr->pt()*toGeV,
                                                              jetPtr->eta(),
                                                              eventWeight);
        }
        if (jet_flavor >= 1 && jet_flavor <=4 ) {
          sprintf(name,"quark_jets_%s", m_jetCollection.c_str());
          static_cast<TH2D*>(hists->hist(name))->Fill(jetPtr->pt()*toGeV,
                                                              jetPtr->eta(),
                                                              eventWeight);
        }
      }
    }

    if (throwWarning)
       std::cout<<"Warning! Number of jets in the event is "<<event.m_jets.size()<<", but histograms have been booked up to "<<m_nJetsMax<<". Exclusive histograms in number of jets have not been filled.\n";
    return true;
}

// function to translate the binnings into vector of bin edges
void JetFlavorPlots::formatBinning(const std::string& str, std::vector<double>& binEdges ) {
  std::stringstream ss(str);
  std::string tok;
  char separator = ':';
  while(std::getline(ss, tok, separator)) {
    binEdges.push_back(std::atof(tok.c_str()));
  }
}

std::string JetFlavorPlots::name() const {
    return "JETFLAVORPLOTS";
}

}  // namespace top
