/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopEventSelectionTools/JetFtagEffPlots.h"

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

const double JetFtagEffPlots::toGeV = 0.001;


JetFtagEffPlots::JetFtagEffPlots(const std::string& name,
                               TFile* outputFile, const std::string& params, std::shared_ptr<top::TopConfig> config, EL::Worker* wk) :
    m_nominalHashValue(0),
    m_CDIfile("xAODBTaggingEfficiency/13TeV/2017-21-13TeV-MC16-CDI-2018-02-09_v1.root"),
    m_fill_total_hists(false),
    m_apply_jet_isolation(false),
    m_jet_isolation_cut(0.8),
    m_use_event_weight(false),
    m_histogram_suffix(""),
    // default pT and eta binning
    m_ptBins ("15:20:30:45:60:80:110:160:210:260:310:400:500:600:800:1000:1200:1500:1800:2500"),
    m_etaBins ("0.:0.3:0.8:1.2:2.1:2.8:3.6:4.5"),
    m_max_pT(-1),
    m_min_pT(-1),
    m_N_pT_bins(-1),
    m_max_Eta(-1),
    m_min_Eta(-1),
    m_N_Eta_bins(-1),
    m_jetCollection(""),
    m_WP("FixedCutBEff_70"),
    m_tagger("DL1"),
    m_config (config),
    m_selection_tool("BTaggingSelectionTool/selTool"),
    m_sfRetriever(nullptr)
 {

    CP::SystematicSet nominal;
    m_nominalHashValue = nominal.hash();
    // retrieve jet collection 
    m_jetCollection = m_config->sgKeyJets();

    //decoding arguments
    std::istringstream stream(params);
    std::string s;
    while (stream>>s) {
      if (s.substr(0,3)=="WP:")
        m_WP = s.substr(3,s.size()-3);
      else if (s.substr(0,7)=="tagger:")
        m_tagger = s.substr(7,s.size()-7);
      else if (s.substr(0,8)=="CDIfile:")
        m_CDIfile = s.substr(8,s.size()-8);
      else if (s.substr(0,3)=="pt:" || s.substr(0,3)=="pT:" )
        m_ptBins = s.substr(3,s.size()-3);
      else if (s.substr(0,7)=="abseta:" )
        m_etaBins = s.substr(7,s.size()-7);
      else if (s.substr(0,7)=="max_pT:" )
        m_max_pT = std::stof( s.substr(7,s.size()-7) );
      else if (s.substr(0,7)=="min_pT:" )
        m_min_pT = std::stof( s.substr(7,s.size()-7) );
      else if (s.substr(0,5)=="N_pT:" )
        m_N_pT_bins = std::stoi( s.substr(5,s.size()-5) );
      else if (s.substr(0,8)=="max_eta:" )
        m_max_Eta = std::stof( s.substr(8,s.size()-8) );
      else if (s.substr(0,8)=="min_eta:" )
        m_min_Eta = std::stof( s.substr(8,s.size()-8) );
      else if (s.substr(0,6)=="N_eta:" )
        m_N_Eta_bins = std::stoi( s.substr(6,s.size()-6) );
      else if (s.substr(0,7)=="suffix:" )
        m_histogram_suffix = s.substr(7,s.size()-7);
      else if ( s=="fill_total_hist")
        m_fill_total_hists = true;
      else if ( s=="apply_jet_isolation")
        m_apply_jet_isolation = true;
      else if (s.substr(0,14)=="isolation_cut:" )
        m_jet_isolation_cut = std::stof( s.substr(14,s.size()-14) );
      else if ( s=="use_event_weight")
        m_use_event_weight = true;
      else {
        std::cout << "ERROR: Can't understand argument " << s << " for JETFTAGEFFPLOTS." << std::endl;
        exit(1);
      }
    }

    if(m_use_event_weight){
      m_sfRetriever = asg::ToolStore::get<ScaleFactorRetriever>("top::ScaleFactorRetriever");
    }

    m_hists = std::make_shared<PlotManager>(name+"/JetFtagEffPlots", outputFile, wk);

    //handle binning and book histograms
    std::vector<double> ptBins;
    std::vector<double> etaBins;

    if(m_N_pT_bins < 1){

          formatBinning(m_ptBins, ptBins);
          formatBinning(m_etaBins, etaBins);
    }

    for (std::string flavour:{"B","C","L","T"}){
          

          if(m_fill_total_hists){
                  std::string total_hist_name = flavour +"_total"+m_histogram_suffix;
                  if(m_N_pT_bins < 1){
                      m_hists->addHist(total_hist_name,";pT [GeV];|#eta^{jet}|;Events",
                            ptBins.size()-1, ptBins.data(), etaBins.size()-1, etaBins.data());
                  }else{
                      m_hists->addHist(total_hist_name,";pT [GeV];|#eta^{jet}|;Events",
                            m_N_pT_bins, m_min_pT, m_max_pT, m_N_Eta_bins, m_min_Eta, m_max_Eta);  
                  }
            }

          std::string pass_hist_name = m_tagger+"_"+m_WP+"_"+ m_jetCollection+"_"+flavour +"_pass"+m_histogram_suffix;
          if(m_N_pT_bins < 1){
              m_hists->addHist(pass_hist_name,";pT [GeV];|#eta^{jet}|;Events",
                    ptBins.size()-1, ptBins.data(), etaBins.size()-1, etaBins.data());
          }else{
              m_hists->addHist(pass_hist_name,";pT [GeV];|#eta^{jet}|;Events",
                    m_N_pT_bins, m_min_pT, m_max_pT, m_N_Eta_bins, m_min_Eta, m_max_Eta);  
          }
     }



    m_selection_tool = asg::AnaToolHandle<IBTaggingSelectionTool>("BTaggingSelectionTool/BTagSelec_"+m_tagger+"_"+m_WP+"_"+m_jetCollection);
    top::check( m_selection_tool.setProperty( "FlvTagCutDefinitionsFileName",m_CDIfile ) , "failed to initialize BtagSelectionTool "+m_tagger+"_"+m_WP+"_"+m_jetCollection );
    top::check( m_selection_tool.setProperty("TaggerName",    m_tagger  ) , "failed to initialize BtagSelectionTool "+m_tagger+"_"+m_WP+"_"+m_jetCollection );
    top::check( m_selection_tool.setProperty("OperatingPoint", m_WP ) , "failed to initialize BtagSelectionTool "+m_tagger+"_"+m_WP+"_"+m_jetCollection );
    top::check( m_selection_tool.setProperty("JetAuthor",      m_jetCollection  ) , "failed to initialize BtagSelectionTool "+m_tagger+"_"+m_WP+"_"+m_jetCollection );

    top::check( m_selection_tool.initialize(), "failed to initialize BtagSelectionTool "+m_tagger+"_"+m_WP+"_"+m_jetCollection);


}

bool JetFtagEffPlots::apply(const top::Event& event) const {
    //only MC
    if(!top::isSimulation(event)) return true;
    // only nominal
    if(event.m_hashValue != m_nominalHashValue) return true;
    // do loose or tight events only if requested
    if( event.m_isLoose && !m_config->doLooseEvents()) return true;
    if(!event.m_isLoose && !m_config->doTightEvents()) return true;

    
    double eventWeight = 1.0;
 
    if(m_use_event_weight){
        float mc_weight = event.m_info->mcEventWeight();
        float jvt_sf = event.m_jvtSF;
        float pu_weight = 1.0;
        if (top::ScaleFactorRetriever::hasPileupSF(event)){
            pu_weight = top::ScaleFactorRetriever::pileupSF(event);
        }
        float lepton_sf = m_sfRetriever->leptonSF(event,nominal);

        eventWeight = mc_weight*jvt_sf*pu_weight*lepton_sf;
    }

          


    FillHistograms(m_hists, eventWeight, event);
    
    return true;


}

void JetFtagEffPlots::FillHistograms(std::shared_ptr<PlotManager> h_ptr, double w_event, const top::Event& event) const{
      

      std::vector<float> jet_isolation;
      if(m_apply_jet_isolation){
        for (unsigned long jet_i = 0; jet_i < event.m_jets.size(); jet_i++) {
          const xAOD::Jet* jetPtr = event.m_jets.at(jet_i);

            float min_dr = 10;

            TLorentzVector jetvector;
            jetvector.SetPtEtaPhiM(jetPtr->pt(), jetPtr->eta(), jetPtr->phi(), jetPtr->m());

            for (unsigned long jet_j = 0; jet_j < event.m_jets.size(); jet_j++) {
              if(jet_i == jet_j){ continue; }

              const xAOD::Jet* jetPtr_j = event.m_jets.at(jet_j);

              TLorentzVector jetvector_j;
              jetvector_j.SetPtEtaPhiM(jetPtr_j->pt(), jetPtr_j->eta(), jetPtr_j->phi(), jetPtr_j->m());

              float dr = jetvector.DeltaR(jetvector_j);
              if(dr < min_dr){ min_dr = dr; }

            }

            jet_isolation.push_back(min_dr);

        }
      }

   
    for (unsigned long jet_i = 0; jet_i < event.m_jets.size(); jet_i++) {
      const xAOD::Jet* jetPtr = event.m_jets.at(jet_i);

      int jet_flavor = -99;
      bool status = jetPtr->getAttribute<int>("HadronConeExclTruthLabelID", jet_flavor);
      
      if (!status) continue;

      if(m_apply_jet_isolation){

        if(jet_isolation.at(jet_i) < m_jet_isolation_cut){
          
          continue;
        }
      }


      std::string flav_name;
          
      if (jet_flavor == 5) flav_name = "B";
          
      else if (jet_flavor == 4 ) flav_name = "C";

      else if (jet_flavor == 0 ) flav_name = "L";

      else if (jet_flavor == 15 ) flav_name = "T";
         
      if(m_fill_total_hists){
      static_cast<TH2D*>(h_ptr->hist(flav_name+"_total"+m_histogram_suffix))->Fill(jetPtr->pt()*toGeV, fabs(jetPtr->eta() ), w_event);
      }

      bool pass_cut = m_selection_tool->accept(jetPtr);

        if(pass_cut){

            std::string hist_name = m_tagger+"_"+m_WP+"_"+ m_jetCollection+"_"+flav_name +"_pass"+m_histogram_suffix;
            static_cast<TH2D*>(h_ptr->hist(hist_name))->Fill(jetPtr->pt()*toGeV, fabs(jetPtr->eta() ), w_event);
        }


      }

}
// function to translate the binnings into vector of bin edges
void JetFtagEffPlots::formatBinning(const std::string& str, std::vector<double>& binEdges ) {
  std::stringstream ss(str);
  std::string tok;
  char separator = ':';
  while(std::getline(ss, tok, separator)) {
    binEdges.push_back(std::atof(tok.c_str()));
  }
}

std::string JetFtagEffPlots::name() const {
    return "JETFTAGEFFPLOTS";
}

}  // namespace top
