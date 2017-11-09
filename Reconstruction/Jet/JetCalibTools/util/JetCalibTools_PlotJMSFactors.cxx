/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetCalibTools/JetCalibrationTool.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"

#include "AsgTools/AnaToolHandle.h"

#include <vector>
#include "TString.h"
#include "TH2D.h"
#include "TCanvas.h"


namespace jet
{
  /// JetFourMomAccessor is an extension of JetAttributeAccessor::AccessorWrapper<xAOD::JetFourMom_t>
  /// AccessorWrapper<xAOD::JetFourMom_t> purpose is to provide a direct and simple access to JetFourMom_t attributes (which are 
  ///  internally saved as 4 floats inside jets).
  /// JetFourMomAccessor is here to workaround 2 limitations of AccessorWrapper
  ///  - it does not provide an operator() method 
  ///  - it does not provide const methods as needed in this package.
  /// AccessorWrapper should be updated to remove these limitations.
  ///  when this happens, JetFourMomAccessor can just be replaced by a typedef to the AccessorWrapper<xAOD::JetFourMom_t>
  class JetFourMomAccessor: public xAOD::JetAttributeAccessor::AccessorWrapper<xAOD::JetFourMom_t> {
  public:
    using xAOD::JetAttributeAccessor::AccessorWrapper<xAOD::JetFourMom_t>::AccessorWrapper;
    xAOD::JetFourMom_t operator()(const xAOD::Jet & jet) const {return const_cast<JetFourMomAccessor*>(this)->getAttribute(jet);}
  };
}



int main (int argc, char* argv[])
{
    // Check argument usage
    if (argc != 5 && argc != 6)
    {
        std::cout << "USAGE: " << argv[0] << " <JetCollection> <ConfigFile> <OutputFile> <mass type> (dev mode switch)" << std::endl;
        std::cout << "\tMass types: \"calo\", \"TA\", \"comb\" (capitalization is ignored)" << std::endl;
        return 1;
    }

    // Store the arguments
    const TString jetAlgo = argv[1];
    const TString config  = argv[2];
    const TString outFile = argv[3];
    const TString massType = argv[4];
    const bool isDevMode  = ( argc > 5 && (TString(argv[5]) == "true" || TString(argv[5]) == "dev") ) ? true : false;
    
    // Derived information
    const bool outFileIsExtensible = outFile.EndsWith(".pdf") || outFile.EndsWith(".ps") || outFile.EndsWith(".root");
    const bool doCombMass = !massType.CompareTo("comb",TString::kIgnoreCase);
    const bool doCaloMass = doCombMass || !massType.CompareTo("calo",TString::kIgnoreCase);
    const bool doTAMass   = doCombMass || !massType.CompareTo("ta",TString::kIgnoreCase);

    // Assumed constants
    const TString calibSeq = "JMS"; // only want to apply the JMS here
    const bool isData = false; // doesn't actually matter for JMS, which is always active
    const float massForScan = 80.385e3; // W-boson
    const float etaForScan = 0; // central jets
    const float trackMassFactor = 2./3.; // Recall: mTA = ptCalo * (mTrack/ptTrack), this multiplies mTrack
    const float trackPtFactor = 2./3.;   // Recall: mTA = ptCalo * (mTrack/ptTrack), this multiplies ptTrack

    // Accessor strings
    const TString startingScaleString = "JetEtaJESScaleMomentum";
    const TString caloMassScaleString = "JetJMSScaleMomentumCalo";
    const TString taMassScaleString   = "JetJMSScaleMomentumTA";
    const TString detectorEtaString   = "DetectorEta";
    const TString trackMassString     = "TrackSumMass";
    const TString trackPtString       = "TrackSumPt";
    
    // Accessors
    jet::JetFourMomAccessor startingScale(startingScaleString.Data());
    jet::JetFourMomAccessor caloMassScale(caloMassScaleString.Data());
    jet::JetFourMomAccessor taMassScale(taMassScaleString.Data());
    SG::AuxElement::Accessor<float> detectorEta(detectorEtaString.Data());
    SG::AuxElement::Accessor<float> trackMass(trackMassString.Data());
    SG::AuxElement::Accessor<float> trackPt(trackPtString.Data());
    

    // Create the calib tool
    asg::AnaToolHandle<IJetCalibrationTool> calibTool;
    calibTool.setTypeAndName("JetCalibrationTool/MyJetCalibTool");
    if ( !calibTool.isUserConfigured() )
    {
        if (ASG_MAKE_ANA_TOOL(calibTool, JetCalibrationTool).isFailure())
        {
            std::cout << "Failed to make ana tool" << std::endl;
            return 2;
        }
        if (calibTool.setProperty("JetCollection",jetAlgo.Data()).isFailure())
        {
            std::cout << "Failed to set JetCollection: " << jetAlgo.Data() << std::endl;
            return 3;
        }
        if (calibTool.setProperty("ConfigFile",config.Data()).isFailure())
        {
            std::cout << "Failed to set ConfigFile: " << config.Data() << std::endl;
            return 3;
        }
        if (calibTool.setProperty("CalibSequence",calibSeq.Data()).isFailure())
        {
            std::cout << "Failed to set CalibSequence: " << calibSeq.Data() << std::endl;
            return 3;
        }
        if (calibTool.setProperty("IsData",isData).isFailure())
        {
            std::cout << "Failed to set IsData: " << (isData ? std::string("true") : std::string("false")) << std::endl;
            return 3;
        }
        if (isDevMode && calibTool.setProperty("DEVmode",isDevMode).isFailure())
        {
            std::cout << "Failed to set DEVmode" << std::endl;
            return 4;
        }
        if (calibTool.retrieve().isFailure())
        {
            std::cout << "Failed to initialize the JetCalibTool" << std::endl;
            return 5;
        }
    }


    // Build a jet container and a jet for us to manipulate later
    xAOD::TEvent event;
    xAOD::TStore store;
    xAOD::JetContainer* jets = new xAOD::JetContainer();
    jets->setStore(new xAOD::JetAuxContainer());
    jets->push_back(new xAOD::Jet());
    xAOD::Jet* jet = jets->at(0);
    
    
    // Make the histograms to fill
    std::vector<TH2D*> hists_pt_eta;
    std::vector<TH2D*> hists_pt_mpt;
    if (doCaloMass)
    {
        hists_pt_eta.push_back(new TH2D("JMS_calo_pt_eta",Form("JMS (calo) for jets with mass=%.1f GeV",massForScan/1.e3),1150,200,2500,60,-2,2));
        hists_pt_mpt.push_back(new TH2D("JMS_calo_pt_mpt",Form("JMS (calo): for jets with #eta=%.1f",etaForScan),1150,200,2500,100,0,1));
    }
    if (doTAMass)
    {
        hists_pt_eta.push_back(new TH2D("JMS_TA_pt_eta",Form("JMS (TA) for jets with mass=%.1f GeV (TA factor = %.1f)",massForScan/1.e3,trackMassFactor/trackPtFactor),1150,200,2500,60,-3,3));
        hists_pt_mpt.push_back(new TH2D("JMS_TA_pt_mpt",Form("JMS (TA) for jets with #eta=%.1f",etaForScan),1150,200,2500,100,0,1));
    }
    if (doCombMass)
    {
        hists_pt_eta.push_back(new TH2D("JMS_comb_pt_eta",Form("JMS (comb) for jets with mass=%.1f GeV",massForScan/1.e3),1150,200,2500,60,-3,3));
        hists_pt_mpt.push_back(new TH2D("JMS_comb_pt_mpt",Form("JMS (comb) for jets with eta=%.1f",etaForScan),1150,200,2500,100,0,1));
    }
    
    // Fill the pt vs eta histogram
    for (int xBin = 1; xBin <= hists_pt_eta.at(0)->GetNbinsX(); ++xBin)
    {
        const double pt = hists_pt_eta.at(0)->GetXaxis()->GetBinCenter(xBin)*1.e3;
        for (int yBin = 1; yBin <= hists_pt_eta.at(0)->GetNbinsY(); ++yBin)
        {
            const double eta = hists_pt_eta.at(0)->GetYaxis()->GetBinCenter(yBin);
            
            // Set the main 4-vector and scale 4-vector
            jet->setJetP4(xAOD::JetFourMom_t(pt,eta,0,massForScan));
            detectorEta(*jet) = eta;
            trackMass(*jet) = trackMassFactor*massForScan;
            trackPt(*jet) = trackPtFactor*pt;
            startingScale.setAttribute(*jet,xAOD::JetFourMom_t(pt,eta,0,massForScan));

            // Jet kinematics set, now apply calibration
            xAOD::Jet* calibJet = nullptr;
            calibTool->calibratedCopy(*jet,calibJet);

            // Calculate the scale factors
            const double JMS     = calibJet->m()/startingScale(*jet).mass();
            const double JMScalo = doCombMass ? caloMassScale(*calibJet).mass()/startingScale(*jet).mass() : JMS;
            const double JMSTA   = doCombMass ? taMassScale(*calibJet).mass()/trackMass(*jet) : calibJet->m()/trackMass(*jet);

            // JMS retrieved, fill the plot(s)
            size_t plotIndex = 0;
            if (doCaloMass)
                hists_pt_eta.at(plotIndex++)->SetBinContent(xBin,yBin,JMScalo);
            if (doTAMass)
                hists_pt_eta.at(plotIndex++)->SetBinContent(xBin,yBin,JMSTA);
            if (doCombMass)
                hists_pt_eta.at(plotIndex++)->SetBinContent(xBin,yBin,JMS);
            
            // Clean up
            delete calibJet;
        }
    }

    // Fill the pt vs m/pt histogram
    for (int xBin = 1; xBin <= hists_pt_mpt.at(0)->GetNbinsX(); ++xBin)
    {
        const double pt = hists_pt_mpt.at(0)->GetXaxis()->GetBinCenter(xBin)*1.e3;
        for (int yBin = 1; yBin <= hists_pt_mpt.at(0)->GetNbinsY(); ++yBin)
        {
            const double mpt = hists_pt_mpt.at(0)->GetYaxis()->GetBinCenter(yBin);
            const double mass = pt*mpt;
            
            // Set the main 4-vector and scale 4-vector
            jet->setJetP4(xAOD::JetFourMom_t(pt,etaForScan,0,mass));
            detectorEta(*jet) = etaForScan;
            trackMass(*jet) = trackMassFactor*massForScan;
            trackPt(*jet) = trackPtFactor*pt;
            startingScale.setAttribute(*jet,xAOD::JetFourMom_t(pt,etaForScan,0,mass));

            // Jet kinematics set, now apply calibration
            xAOD::Jet* calibJet = nullptr;
            calibTool->calibratedCopy(*jet,calibJet);

            // Calculate the scale factors
            const double JMS     = calibJet->m()/startingScale(*jet).mass();
            const double JMScalo = doCombMass ? caloMassScale(*calibJet).mass()/startingScale(*jet).mass() : JMS;
            const double JMSTA   = doCombMass ? taMassScale(*calibJet).mass()/trackMass(*jet) : calibJet->m()/trackMass(*jet);

            // JMS retrieved, fill the plot(s)
            size_t plotIndex = 0;
            if (doCaloMass)
                hists_pt_mpt.at(plotIndex++)->SetBinContent(xBin,yBin,JMScalo);
            if (doTAMass)
                hists_pt_mpt.at(plotIndex++)->SetBinContent(xBin,yBin,JMSTA);
            if (doCombMass)
                hists_pt_mpt.at(plotIndex++)->SetBinContent(xBin,yBin,JMS);
            
            // Clean up
            delete calibJet;
        }
    }

    
    // Make the plots

    // First the canvas
    TCanvas* canvas = new TCanvas("canvas");
    canvas->SetMargin(0.07,0.13,0.1,0.10);
    canvas->SetFillStyle(4000);
    canvas->SetFillColor(0);
    canvas->SetFrameBorderMode(0);
    canvas->cd();
    canvas->SetLogx(true);
    
    // Now labels/etc
    for (TH2* hist : hists_pt_eta)
    {
        hist->SetStats(false);
        hist->GetXaxis()->SetTitle("Jet #it{p}_{T} [GeV]");
        hist->GetXaxis()->SetTitleOffset(1.35);
        hist->GetXaxis()->SetMoreLogLabels();
        hist->GetYaxis()->SetTitle("#eta");
        hist->GetYaxis()->SetTitleOffset(0.9);
        hist->GetZaxis()->SetTitle("m_{JES+JMS} / m_{JES}");
    }
    for (TH2* hist : hists_pt_mpt)
    {
        hist->SetStats(false);
        hist->GetXaxis()->SetTitle("Jet #it{p}_{T} [GeV]");
        hist->GetXaxis()->SetTitleOffset(1.35);
        hist->GetXaxis()->SetMoreLogLabels();
        hist->GetYaxis()->SetTitle("m / #it{p}_{T}");
        hist->GetYaxis()->SetTitleOffset(0.9);
        hist->GetZaxis()->SetTitle("m_{JES+JMS} / m_{JES}");
    }

    // Now write them out
    if (outFileIsExtensible)
    {
        canvas->Print(outFile+"[");
        for (size_t iHist = 0; iHist < hists_pt_eta.size(); ++iHist)
        {
            hists_pt_eta.at(iHist)->Draw("colz");
            canvas->Print(outFile);
            hists_pt_mpt.at(iHist)->Draw("colz");
            canvas->Print(outFile);
        }
        canvas->Print(outFile+"]");
    }
    else
    {
        unsigned int counter = 1;
        for (size_t iHist = 0; iHist < hists_pt_eta.size(); ++iHist)
        {
            hists_pt_eta.at(iHist)->Draw("colz");
            canvas->Print(Form("%u-fixMass-%s",counter,outFile.Data()));
            hists_pt_mpt.at(iHist)->Draw("colz");
            canvas->Print(Form("%u-fixMass-%s",counter++,outFile.Data()));
        }
    }

    // Clean up a bit (although the program is about to end...)
    for (TH2D* hist : hists_pt_eta)
        delete hist;
    hists_pt_eta.clear();

    for (TH2D* hist : hists_pt_mpt)
        delete hist;
    hists_pt_mpt.clear();

    return 0;
}
