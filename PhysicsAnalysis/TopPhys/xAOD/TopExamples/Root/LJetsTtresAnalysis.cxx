/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TopExamples/LJetsTtresAnalysis.h"
#include "TopExamples/DefaultPlots.h"
#include "TopExamples/AnalysisTools.h"

#include "TopEvent/Event.h"
#include "TopEvent/EventTools.h"

#include "TFile.h"
#include "TH1.h"

#include <sstream>
#include <iostream>

#include "TopEventReconstructionTools/TtresNeutrinoBuilder.h"
#include "TopEventReconstructionTools/TtresChi2.h"


namespace top {

LJetsTtresAnalysis::LJetsTtresAnalysis(TFile* outputFile) :
        m_mcChannelNumber(0),
        m_histsREl("rejets", outputFile),
        m_histsRMu("rmujets", outputFile),
        m_histsBEl("bejets", outputFile),
        m_histsBMu("bmujets", outputFile),
        m_cutflowEventsREl(nullptr),
        m_cutflowMCWeightsREl(nullptr),
        m_cutflowEventsRMu(nullptr),
        m_cutflowMCWeightsRMu(nullptr),
        m_cutflowEventsBEl(nullptr),
        m_cutflowMCWeightsBEl(nullptr),
        m_cutflowEventsBMu(nullptr),
        m_cutflowMCWeightsBMu(nullptr),
        m_counterREl(0),
        m_counterRMu(0),
        m_counterBEl(0),
        m_counterBMu(0),
        m_neutrinoBuilder("MeV"),
        m_chi2("MeV")
    {

    //tell it to load AKT10
    SetContainerNames()->largeJetCollectionName = "CorrectedSelectedAntiKt10LCTopoTrimmedPtFrac30SmallR5Jets"; // AntiKt10LCTopoTrimmedPtFrac30SmallR5Jets
    //SetContainerNames()->largeJetCollectionName = "CorrectedSelectedAntiKt10LCTopoJets";
    m_chi2.Init(TtresChi2::DATA2012SUMMER2013);
 
    //add some default histograms for each channel
    top::addPlots(m_histsREl);
    top::addPlots(m_histsRMu);

    top::addPlots(m_histsBEl);
    top::addPlots(m_histsBMu);

    //some extra histograms
    m_histsREl.addHist("mwt", ";MWT [GeV];Events", 20, 0, 200);
    m_histsRMu.addHist("mwt", ";MWT [GeV];Events", 20, 0, 200);

    m_histsREl.addHist("met", ";MET [GeV];Events", 30, 0, 300);
    m_histsRMu.addHist("met", ";MET [GeV];Events", 30, 0, 300);

    m_histsREl.addHist("lep_pt", ";lepton p_{T} [GeV];Events", 50, 0, 500);
    m_histsRMu.addHist("lep_pt", ";lepton p_{T} [GeV];Events", 50, 0, 500);
    m_histsREl.addHist("lep_eta", ";lepton #eta ;Events", 40, -2.0, 2.0);
    m_histsRMu.addHist("lep_eta", ";lepton #eta ;Events", 40, -2.0, 2.0);
    m_histsREl.addHist("lep_phi", ";lepton #phi ;Events", 64, -3.2, 3.2);
    m_histsRMu.addHist("lep_phi", ";lepton #phi ;Events", 64, -3.2, 3.2);

    m_histsREl.addHist("overlap", ";Overlap boosted resolved ;Events", 3, -0.5, 2.5);
    m_histsRMu.addHist("overlap", ";Overlap boosted resolved ;Events", 3, -0.5, 2.5);

    m_histsBEl.addHist("mwt", ";MWT [GeV];Events", 20, 0, 200);
    m_histsBMu.addHist("mwt", ";MWT [GeV];Events", 20, 0, 200);

    m_histsBEl.addHist("met", ";MET [GeV];Events", 30, 0, 300);
    m_histsBMu.addHist("met", ";MET [GeV];Events", 30, 0, 300);

    m_histsBEl.addHist("lep_pt", ";lepton p_{T} [GeV];Events", 50, 0, 500);
    m_histsBMu.addHist("lep_pt", ";lepton p_{T} [GeV];Events", 50, 0, 500);
    m_histsBEl.addHist("lep_eta", ";lepton #eta ;Events", 40, -2.0, 2.0);
    m_histsBMu.addHist("lep_eta", ";lepton #eta ;Events", 40, -2.0, 2.0);
    m_histsBEl.addHist("lep_phi", ";lepton #phi ;Events", 64, -3.2, 3.2);
    m_histsBMu.addHist("lep_phi", ";lepton #phi ;Events", 64, -3.2, 3.2);

    m_histsBEl.addHist("overlap", ";Overlap boosted resolved ;Events", 3, -0.5, 2.5);
    m_histsBMu.addHist("overlap", ";Overlap boosted resolved ;Events", 3, -0.5, 2.5);

    m_histsBEl.addHist("ljet_pt", ";large-R jet p_{T} [GeV];Events", 50, 0, 500);
    m_histsBMu.addHist("ljet_pt", ";large-R jet p_{T} [GeV];Events", 50, 0, 500);
    m_histsBEl.addHist("ljet_m", ";large-R jet mass [GeV];Events", 30, 0, 300);
    m_histsBMu.addHist("ljet_m", ";large-R jet mass [GeV];Events", 30, 0, 300);
    m_histsBEl.addHist("ljet_eta", ";large-R jet #eta ;Events", 40, -2.0, 2.0);
    m_histsBMu.addHist("ljet_eta", ";large-R jet #eta ;Events", 40, -2.0, 2.0);
    m_histsBEl.addHist("ljet_phi", ";large-R jet #phi ;Events", 64, -3.2, 3.2);
    m_histsBMu.addHist("ljet_phi", ";large-R jet #phi ;Events", 64, -3.2, 3.2);
    m_histsBEl.addHist("ljet_sd12", ";large-R jet #sqrt{d_{12}} [GeV];Events", 10, 0, 100);
    m_histsBMu.addHist("ljet_sd12", ";large-R jet #sqrt{d_{12}} [GeV];Events", 10, 0, 100);

    m_histsBEl.addHist("ljetSel_pt", ";large-R jet p_{T} [GeV];Events", 50, 0, 500);
    m_histsBMu.addHist("ljetSel_pt", ";large-R jet p_{T} [GeV];Events", 50, 0, 500);
    m_histsBEl.addHist("ljetSel_m", ";large-R jet mass [GeV];Events", 30, 0, 300);
    m_histsBMu.addHist("ljetSel_m", ";large-R jet mass [GeV];Events", 30, 0, 300);
    m_histsBEl.addHist("ljetSel_eta", ";large-R jet #eta ;Events", 40, -2.0, 2.0);
    m_histsBMu.addHist("ljetSel_eta", ";large-R jet #eta ;Events", 40, -2.0, 2.0);
    m_histsBEl.addHist("ljetSel_phi", ";large-R jet #phi ;Events", 64, -3.2, 3.2);
    m_histsBMu.addHist("ljetSel_phi", ";large-R jet #phi ;Events", 64, -3.2, 3.2);
    m_histsBEl.addHist("ljetSel_sd12", ";large-R jet #sqrt{d_{12}} [GeV];Events", 10, 0, 100);
    m_histsBMu.addHist("ljetSel_sd12", ";large-R jet #sqrt{d_{12}} [GeV];Events", 10, 0, 100);

    m_histsREl.addHist("mtt", ";boosted m_{tt} [GeV];Events", 30, 0, 3000);
    m_histsRMu.addHist("mtt", ";boosted m_{tt} [GeV];Events", 30, 0, 3000);

    m_histsBEl.addHist("mtt", ";boosted m_{tt} [GeV];Events", 30, 0, 3000);
    m_histsBMu.addHist("mtt", ";boosted m_{tt} [GeV];Events", 30, 0, 3000);
}

LJetsTtresAnalysis::~LJetsTtresAnalysis() {
}

void LJetsTtresAnalysis::newFile(TFile* inputFile) {
    updateCutflow("rejets/cutflow", m_cutflowEventsREl, inputFile);
    updateCutflow("rejets/cutflow_mc", m_cutflowMCWeightsREl, inputFile);
    updateCutflow("rmujets/cutflow", m_cutflowEventsRMu, inputFile);
    updateCutflow("rmujets/cutflow_mc", m_cutflowMCWeightsRMu, inputFile);
    updateCutflow("bejets/cutflow", m_cutflowEventsBEl, inputFile);
    updateCutflow("bejets/cutflow_mc", m_cutflowMCWeightsBEl, inputFile);
    updateCutflow("bmujets/cutflow", m_cutflowEventsBMu, inputFile);
    updateCutflow("bmujets/cutflow_mc", m_cutflowMCWeightsBMu, inputFile);
}

void LJetsTtresAnalysis::fillIt(top::PlotManager &hists, const top::Event& topEvent, bool isBoosted) {
    double eventWeight = 1.;
    if (top::isSimulation(topEvent)) {
        m_mcChannelNumber = topEvent.m_info->mcChannelNumber();
//         eventWeight = topEvent.m_info->mcEventWeight();
        eventWeight = topEvent.m_truthEvent->at(0)->weights()[0];// FIXME temporary bugfix
    }
    top::fillPlots(topEvent, hists, eventWeight);

    double mwt = -1;
    if (topEvent.m_electrons.size() == 1)
      mwt = top::mwt(*topEvent.m_electrons[0], *topEvent.m_met);
    if (topEvent.m_muons.size() == 1)
      mwt = top::mwt(*topEvent.m_muons[0], *topEvent.m_met);

    hists.hist("mwt")->Fill(mwt * top::toGeV, eventWeight);
    hists.hist("met")->Fill(topEvent.m_met->met() * top::toGeV, eventWeight);

    TLorentzVector lepton;
    if (topEvent.m_electrons.size() == 1)
      lepton.SetPtEtaPhiE(topEvent.m_electrons[0]->pt(), topEvent.m_electrons[0]->eta(), topEvent.m_electrons[0]->phi(), topEvent.m_electrons[0]->e());
    if (topEvent.m_muons.size() == 1)
      lepton.SetPtEtaPhiE(topEvent.m_muons[0]->pt(), topEvent.m_muons[0]->eta(), topEvent.m_muons[0]->phi(), topEvent.m_muons[0]->e());

    hists.hist("lep_pt")->Fill(lepton.Pt() * top::toGeV, eventWeight);
    hists.hist("lep_eta")->Fill(lepton.Eta(), eventWeight);
    hists.hist("lep_phi")->Fill(lepton.Phi(), eventWeight);

    if (isBoosted) {
      const xAOD::Jet *selJet = nullptr;
      for (const auto* const jetPtr : topEvent.m_jets) {
        if (jetPtr->auxdata<int>("closeToLepton") == 1) {
          selJet = jetPtr;
          break;
        }
      }

      const xAOD::Jet *largeJet = nullptr;
      for (const auto* const jetPtr : topEvent.m_largeJets) {
        if (jetPtr->auxdata<int>("good") == 1) {
          largeJet = jetPtr;
          break;
        }
      }
      if (largeJet != nullptr && selJet != nullptr) {
        hists.hist("ljetSel_m")->Fill(largeJet->m() * top::toGeV, eventWeight);
        double d12 = 0;
        largeJet->getAttribute("Split12", d12);
        hists.hist("ljetSel_sd12")->Fill(d12 * top::toGeV, eventWeight);
        hists.hist("ljetSel_pt")->Fill(largeJet->pt() * top::toGeV, eventWeight);
        hists.hist("ljetSel_eta")->Fill(largeJet->eta(), eventWeight);
        hists.hist("ljetSel_phi")->Fill(largeJet->phi(), eventWeight);

        TLorentzVector tt(0,0,0,0);
        tt += largeJet->p4();
        tt += lepton;
        tt += selJet->p4();
        std::vector<TLorentzVector*> vec_nu = m_neutrinoBuilder.candidatesFromWMass_Rotation(&lepton, topEvent.m_met->met(), topEvent.m_met->phi(), true);
        TLorentzVector nu(0,0,0,0);
        if (vec_nu.size() > 0) {
          nu = *(vec_nu[0]);
          for (size_t z = 0; z < vec_nu.size(); ++z) delete vec_nu[z];
          vec_nu.clear();
        }
        tt += nu;

        hists.hist("mtt")->Fill(tt.M() * top::toGeV, eventWeight);
      }

      if (topEvent.m_largeJets.size() > 0) {
        hists.hist("ljet_m")->Fill(topEvent.m_largeJets[0]->m() * top::toGeV, eventWeight);
        double d12 = 0;
        topEvent.m_largeJets[0]->getAttribute("Split12", d12);
        hists.hist("ljet_sd12")->Fill(d12 * top::toGeV, eventWeight);
        hists.hist("ljet_pt")->Fill(topEvent.m_largeJets[0]->pt() * top::toGeV, eventWeight);
        hists.hist("ljet_eta")->Fill(topEvent.m_largeJets[0]->eta(), eventWeight);
        hists.hist("ljet_phi")->Fill(topEvent.m_largeJets[0]->phi(), eventWeight);
      }
    } else { // is resolved
      // inputs 
      // LEPTON --> TLorentzVector for your lepton
      // vjets -->  std::vector<TLorentzVector*> for the jets
      // vjets_btagged --> std::vector<bool> to say if the jets are btagged or not
      // met --> TLorentzVector for your MET

      // outputs, they will be filled by the TTBarLeptonJetsBuilder_chi2
      int  igj3, igj4; // index for the Whad
      int igb3, igb4; // index for the b's
      int  ign1;  // index for the neutrino (because chi2 can test both pz solution)
      double chi2ming1, chi2ming1H, chi2ming1L;
      std::vector<TLorentzVector *> vjets;
      std::vector<bool> vjets_btagged;
      for (size_t z = 0; z < topEvent.m_jets.size(); ++z) {
        vjets.push_back(new TLorentzVector(0,0,0,0));
        vjets[z]->SetPtEtaPhiE(topEvent.m_jets[z]->pt(), topEvent.m_jets[z]->eta(), topEvent.m_jets[z]->phi(), topEvent.m_jets[z]->e());
        const xAOD::BTagging *myBTag = topEvent.m_jets[z]->btagging();
        // https://twiki.cern.ch/twiki/bin/viewauth/AtlasProtected/BTagingxAODEDM
        // https://twiki.cern.ch/twiki/bin/view/AtlasProtected/BTaggingBenchmarks
        vjets_btagged.push_back(myBTag->SV1plusIP3D_discriminant() > 1.85); // best discriminant available for 8 TeV (cut at 70%)
      }
      TLorentzVector met(0,0,0,0);
      met.SetPtEtaPhiM(topEvent.m_met->met(), 0, topEvent.m_met->phi(), 0);
      bool status = m_chi2.findMinChiSquare(&lepton, &vjets, &vjets_btagged, &met, igj3, igj4, igb3, igb4, ign1, chi2ming1, chi2ming1H, chi2ming1L); // status has to be true

      double mtt = -1;
      if (status) mtt = m_chi2.getResult_Mtt();
      hists.hist("mtt")->Fill(mtt * top::toGeV, eventWeight);
      for (size_t z = 0; z < vjets.size(); ++z) {
        delete vjets[z];
      }
      vjets.clear();
      vjets_btagged.clear();
    }
}

void LJetsTtresAnalysis::event(const top::Event& topEvent) {
    double eventWeight = 1.;
    if (top::isSimulation(topEvent)) {
        m_mcChannelNumber = topEvent.m_info->mcChannelNumber();
//         eventWeight = topEvent.m_info->mcEventWeight();
        eventWeight = topEvent.m_truthEvent->at(0)->weights()[0];// FIXME temporary bugfix
    }

    int pass_rejets = top::passesPreSelection(topEvent, "rejets");
    int pass_rmujets = top::passesPreSelection(topEvent, "rmujets");

    int pass_bejets  = top::passesPreSelection(topEvent, "bejets");
    int pass_bmujets = top::passesPreSelection(topEvent, "bmujets");

    if (pass_rejets) fillIt(m_histsREl, topEvent, false);
    if (pass_rmujets) fillIt(m_histsRMu, topEvent, false);

    if (pass_bejets) fillIt(m_histsBEl, topEvent, true);
    if (pass_bmujets) fillIt(m_histsBMu, topEvent, true);

    // fill overlap histogram with 1 if there is an overlap
    // fill it with 0 for resolved only or 2 if it is boosted only
    if (pass_rejets && pass_bejets)
        m_histsREl.hist("overlap")->Fill(1.0, eventWeight);

    if (pass_rejets && !pass_bejets)
        m_histsREl.hist("overlap")->Fill(0.0, eventWeight);

    if (!pass_rejets && pass_bejets)
        m_histsREl.hist("overlap")->Fill(2.0, eventWeight);

    if (pass_rmujets && pass_bmujets)
        m_histsRMu.hist("overlap")->Fill(1.0, eventWeight);

    if (pass_rmujets && !pass_bmujets)
        m_histsRMu.hist("overlap")->Fill(0.0, eventWeight);

    if (!pass_rmujets && pass_bmujets)
        m_histsRMu.hist("overlap")->Fill(2.0, eventWeight);

    if (pass_rejets && pass_bejets)
        m_histsBEl.hist("overlap")->Fill(1.0, eventWeight);

    if (pass_rejets && !pass_bejets)
        m_histsBEl.hist("overlap")->Fill(0.0, eventWeight);

    if (!pass_rejets && pass_bejets)
        m_histsBEl.hist("overlap")->Fill(2.0, eventWeight);

    if (pass_rmujets && pass_bmujets)
        m_histsBMu.hist("overlap")->Fill(1.0, eventWeight);

    if (pass_rmujets && !pass_bmujets)
        m_histsBMu.hist("overlap")->Fill(0.0, eventWeight);

    if (!pass_rmujets && pass_bmujets)
        m_histsBMu.hist("overlap")->Fill(2.0, eventWeight);

    if (pass_rejets)
      m_counterREl++;
    if (pass_rmujets)
      m_counterRMu++;
    if (pass_bejets)
      m_counterBEl++;
    if (pass_bmujets)
      m_counterBMu++;

}

void LJetsTtresAnalysis::finalise(TFile* outputFile) {
    std::cout << "Cutflows directly from the input files:\n";
    printCutflow(m_cutflowEventsREl, m_cutflowMCWeightsREl, "rejets", m_counterREl);
    printCutflow(m_cutflowEventsRMu, m_cutflowMCWeightsRMu, "rmujets", m_counterRMu);

    printCutflow(m_cutflowEventsBEl, m_cutflowMCWeightsBEl, "bejets", m_counterBEl);
    printCutflow(m_cutflowEventsBMu, m_cutflowMCWeightsBMu, "bmujets", m_counterBMu);

    //normalise to 1 fb-1 for MC simulation
    double sf = 1.;
    if (m_mcChannelNumber != 0) {
        const double mcWeightsInSample = m_cutflowMCWeightsREl->GetBinContent(1);
        sf = sfToOneInversefb(m_mcChannelNumber, mcWeightsInSample);
    }

    m_histsREl.scaleHistograms(sf);
    m_histsRMu.scaleHistograms(sf);

    m_histsBEl.scaleHistograms(sf);
    m_histsBMu.scaleHistograms(sf);
    outputFile->Write();
}


void LJetsTtresAnalysis::printCutflow(TH1D* eventHist, TH1D* mcWeightHist, const std::string& name, unsigned int localYield) {
    std::cout << name << " cutflow:\n";
    std::cout << "    " << 
            std::setw(4) << "Num" << 
            std::setw(20) << "Name" <<
            std::setw(20) << "Grid (Events)" <<
            std::setw(20) << "Grid (MC Weights)" <<
            std::setw(20) << "Local (Events)" << "\n";

    for (int i = 1; i <= eventHist->GetNbinsX(); ++i) {
        std::cout << "    " << std::setw(4) << i
                << std::setw(20) << eventHist->GetXaxis()->GetBinLabel(i)
                << std::setw(20) << eventHist->GetBinContent(i)
                << std::setw(20) << mcWeightHist->GetBinContent(i);

        if (i == eventHist->GetNbinsX())
            std::cout << std::setw(20) << localYield;

        std::cout << "\n";
    }

    std::cout << "\n";
}

}
