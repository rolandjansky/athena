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
    if (argc < 4 || argc > 6)
    {
        std::cout << "USAGE: " << argv[0] << " <JetCollection> <ConfigFile> <OutputFile> (dev mode switch) (JES_vs_E switch)" << std::endl;
        return 1;
    }

    // Store the arguments
    const TString jetAlgo = argv[1];
    const TString config  = argv[2];
    const TString outFile = argv[3];
    const bool isDevMode  = ( argc > 4 && (TString(argv[4]) == "true" || TString(argv[4]) == "dev") ) ? true : false;
    const bool vsE        = ( argc > 5 && (TString(argv[5]) == "true") ) ? true : false;
    
    // Derived information
    const bool outFileIsExtensible = outFile.EndsWith(".pdf") || outFile.EndsWith(".ps") || outFile.EndsWith(".root");

    // Assumed constants
    const TString calibSeq = "EtaJES"; // only want to apply the JES here
    const bool isData = false; // doesn't actually matter for JES, which is always active
    float massForScan = 80.385e3; // W-boson
    if ( !jetAlgo.Contains("AntiKt10") ) massForScan = 0;

    // Accessor strings
    TString startingScaleString = "JetConstitScaleMomentum";
    if ( !jetAlgo.Contains("AntiKt10") ) startingScaleString = "JetPileupScaleMomentum";
    const TString endingScaleString   = "JetEtaJESScaleMomentum";
    const TString detectorEtaString   = "DetectorEta";
    
    // Accessors
    jet::JetFourMomAccessor startingScale(startingScaleString.Data());
    jet::JetFourMomAccessor endingScale(endingScaleString.Data());
    SG::AuxElement::Accessor<float> detectorEta(detectorEtaString.Data());
    

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
    TH2D* hist_pt_eta;
    if ( jetAlgo.Contains("AntiKt10") ) hist_pt_eta = new TH2D("JES_pt_eta",Form("JES for jets with mass=%.1f GeV",massForScan/1.e3),1200,100,2500,60,-3,3);
    else { hist_pt_eta = new TH2D("JES_pt_eta",Form("JES for jets with mass=%.1f GeV",massForScan/1.e3),2500,20,5000,90,-4.5,4.5); }
    
    // Fill the histogram
    for (int xBin = 1; xBin <= hist_pt_eta->GetNbinsX(); ++xBin)
    {
        const double pt = hist_pt_eta->GetXaxis()->GetBinCenter(xBin)*1.e3; // E if vsE
        for (int yBin = 1; yBin <= hist_pt_eta->GetNbinsY(); ++yBin)
        {
            const double eta = hist_pt_eta->GetYaxis()->GetBinCenter(yBin);

            // Set the main 4-vector and scale 4-vector
	    if ( !vsE ){
              jet->setJetP4(xAOD::JetFourMom_t(pt,eta,0,massForScan));
              detectorEta(*jet) = eta;
              startingScale.setAttribute(*jet,xAOD::JetFourMom_t(pt,eta,0,massForScan));
	    } else {
              const double E = pt; // pt is actually E if vsE
	      const double pT = sqrt((E*E)-(massForScan*massForScan))/cosh(eta);
              jet->setJetP4(xAOD::JetFourMom_t(pT,eta,0,massForScan));
              detectorEta(*jet) = eta;
              startingScale.setAttribute(*jet,xAOD::JetFourMom_t(pT,eta,0,massForScan));
	    }

            // Jet kinematics set, now apply calibration
            xAOD::Jet* calibJet = nullptr;
            calibTool->calibratedCopy(*jet,calibJet);

            // Calculate the scale factors
            const double JES     = calibJet->e()/startingScale(*jet).e();

            // JMS retrieved, fill the plot(s)
            hist_pt_eta->SetBinContent(xBin,yBin,JES);
            
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
    hist_pt_eta->SetStats(false);
    if ( !vsE ) hist_pt_eta->GetXaxis()->SetTitle("Jet #it{p}_{T} [GeV]");
    else { hist_pt_eta->GetXaxis()->SetTitle("Jet E [GeV]"); }
    hist_pt_eta->GetXaxis()->SetTitleOffset(1.35);
    hist_pt_eta->GetXaxis()->SetMoreLogLabels();
    hist_pt_eta->GetYaxis()->SetTitle("#eta");
    hist_pt_eta->GetYaxis()->SetTitleOffset(0.9);
    hist_pt_eta->GetZaxis()->SetTitle("JES_{Factor}");

    // Now write them out
    if (outFileIsExtensible)
    {

        if ( outFile.EndsWith(".pdf") || outFile.EndsWith(".ps") ){
          canvas->Print(outFile+"[");
          hist_pt_eta->Draw("colz");
          canvas->Print(outFile);
          canvas->Print(outFile+"]");
	} else if ( outFile.EndsWith(".root") ){
	  TFile *fout = new TFile(outFile.Data(),"RECREATE");
	  hist_pt_eta->Write();
	  fout->Close();
	  delete fout;
	}

    }
    else
    {
        unsigned int counter = 1;
        hist_pt_eta->Draw("colz");
        canvas->Print(Form("%u-fixMass-%s",counter++,outFile.Data()));
    }


    return 0;
}
