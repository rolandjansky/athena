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
    if (argc != 4 && argc != 5)
    {
        std::cout << "USAGE: " << argv[0] << " <JetCollection> <ConfigFile> <OutputFile> (dev mode switch)" << std::endl;
        return 1;
    }

    // Store the arguments
    const TString jetAlgo = argv[1];
    const TString config  = argv[2];
    const TString outFile = argv[3];
    const bool isDevMode  = ( argc > 4 && (TString(argv[4]) == "true" || TString(argv[4]) == "dev") ) ? true : false;
    
    // Derived information
    const bool outFileIsExtensible = outFile.EndsWith(".pdf") || outFile.EndsWith(".ps");

    // Assumed constants
    const TString startingScaleString = "JetEtaJESScaleMomentum";
    const TString calibSeq = "JMS"; // only want to apply the JMS here
    const bool isData = false; // doesn't actually matter for JMS, which is always active
    const float massForScan = 80.385e3; // W-boson
    const float etaForScan = 0; // central jets

    // Scale accessors
    jet::JetFourMomAccessor startingScale(startingScaleString.Data());

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
    
    
    // Make the histogram to fill
    TH2D* hist_pt_eta = new TH2D("JMS_pt_eta","JMS: pt and eta, m=W-boson",1150,200,2500,40,-2,2);
    TH2D* hist_pt_mpt = new TH2D("JMS_pt_mpt","JMS: pt and m/pt, eta=0",1150,200,2500,100,0,1);
    
    // Fill the pt vs eta histogram
    for (int xBin = 1; xBin <= hist_pt_eta->GetNbinsX(); ++xBin)
    {
        const double pt = hist_pt_eta->GetXaxis()->GetBinCenter(xBin)*1.e3;
        for (int yBin = 1; yBin <= hist_pt_eta->GetNbinsY(); ++yBin)
        {
            const double eta = hist_pt_eta->GetYaxis()->GetBinCenter(yBin);
            
            // Set the main 4-vector and scale 4-vector
            jet->setJetP4(xAOD::JetFourMom_t(pt,eta,0,massForScan));
            jet->setAttribute<float>("DetectorEta",eta);
            startingScale.setAttribute(*jet,xAOD::JetFourMom_t(pt,eta,0,massForScan));

            // Jet kinematics set, now apply calibration
            xAOD::Jet* calibJet = nullptr;
            calibTool->calibratedCopy(*jet,calibJet);
            const double JMS = calibJet->m()/jet->m();
            delete calibJet;

            // JMS retrieved, fill the plot
            hist_pt_eta->SetBinContent(xBin,yBin,JMS);
        }
    }

    // Fill the pt vs m/pt histogram
    for (int xBin = 1; xBin <= hist_pt_mpt->GetNbinsX(); ++xBin)
    {
        const double pt = hist_pt_mpt->GetXaxis()->GetBinCenter(xBin)*1.e3;
        for (int yBin = 1; yBin <= hist_pt_mpt->GetNbinsY(); ++yBin)
        {
            const double mpt = hist_pt_mpt->GetYaxis()->GetBinCenter(yBin);
            const double mass = pt*mpt;
            
            // Set the main 4-vector and scale 4-vector
            jet->setJetP4(xAOD::JetFourMom_t(pt,etaForScan,0,mass));
            jet->setAttribute<float>("DetectorEta",etaForScan);
            startingScale.setAttribute(*jet,xAOD::JetFourMom_t(pt,etaForScan,0,mass));

            // Jet kinematics set, now apply calibration
            xAOD::Jet* calibJet = nullptr;
            calibTool->calibratedCopy(*jet,calibJet);
            const double JMS = calibJet->m()/jet->m();
            delete calibJet;

            // JMS retrieved, fill the plot
            hist_pt_mpt->SetBinContent(xBin,yBin,JMS);
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
    hist_pt_eta->SetStats(false);
    hist_pt_mpt->SetStats(false);
    hist_pt_eta->SetTitle(Form("JMS for jets with mass=%.1f GeV",massForScan/1.e3));
    hist_pt_mpt->SetTitle(Form("JMS for jets with #eta=%.1f",etaForScan));
    hist_pt_eta->GetXaxis()->SetTitle("Jet #it{p}_{T} [GeV]");
    hist_pt_mpt->GetXaxis()->SetTitle("Jet #it{p}_{T} [GeV]");
    hist_pt_eta->GetXaxis()->SetTitleOffset(1.35);
    hist_pt_mpt->GetXaxis()->SetTitleOffset(1.35);
    hist_pt_eta->GetYaxis()->SetTitle("#eta");
    hist_pt_mpt->GetYaxis()->SetTitle("m / #it{p}_{T}");
    hist_pt_eta->GetYaxis()->SetTitleOffset(0.9);
    hist_pt_mpt->GetYaxis()->SetTitleOffset(0.9);
    hist_pt_eta->GetZaxis()->SetTitle("m_{JES+JMS} / m_{JES}");
    hist_pt_mpt->GetZaxis()->SetTitle("m_{JES+JMS} / m_{JES}");

    // Now write them out
    if (outFileIsExtensible)
    {
        canvas->Print(outFile+"[");
        hist_pt_eta->GetXaxis()->SetMoreLogLabels();
        hist_pt_eta->Draw("colz");
        canvas->Print(outFile);
        hist_pt_mpt->GetXaxis()->SetMoreLogLabels();
        hist_pt_mpt->Draw("colz");
        canvas->Print(outFile);
        canvas->Print(outFile+"]");
    }
    else
    {
        hist_pt_eta->GetXaxis()->SetMoreLogLabels();
        hist_pt_eta->Draw("colz");
        canvas->Print("1-"+outFile);
        hist_pt_mpt->GetXaxis()->SetMoreLogLabels();
        hist_pt_mpt->Draw("colz");
        canvas->Print("2-"+outFile);
    }

    return 0;
}
