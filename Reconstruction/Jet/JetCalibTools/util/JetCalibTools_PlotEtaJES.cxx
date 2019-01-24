/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetCalibTools/JetCalibrationTool.h"

#include "PATInterfaces/CorrectionTool.h"

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

    // Protection
    if (!outFile.EndsWith(".root") ){
      std::cout << "Output file should end with .root" << std::endl;
      return 0;
    }

    // Minimum pT if vsE
    double minpT = 20000.; // MeV

    // Assumed constants
    const TString calibSeq = "EtaJES"; // only want to apply the JES here
    const bool isData = false; // doesn't actually matter for JES, which is always active
    float massForScan = 0;

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
    auto jets = std::make_unique< xAOD::JetContainer >();
    auto jetsAux = std::make_unique< xAOD::JetAuxContainer >();
    jets->setStore( jetsAux.get() );
    jets->push_back(std::make_unique<xAOD::Jet>());
    auto jet = jets->at(0);
   
    // Make the histogram to fill
    std::unique_ptr<TH2D> hist_pt_eta;
    if ( jetAlgo.Contains("AntiKt10") ) hist_pt_eta = std::make_unique<TH2D>("JES_pt_eta",Form("JES for jets with mass=%.1f GeV",massForScan/1.e3),1200,100,2500,30,0,3);
    else { hist_pt_eta = std::make_unique<TH2D>("JES_pt_eta",Form("JES for jets with mass=%.1f GeV",massForScan/1.e3),2500,20,5000,45,0,4.5); }
    
    // Fill the histogram
    for (int xBin = 1; xBin <= hist_pt_eta->GetNbinsX(); ++xBin)
    {
        const double pt = hist_pt_eta->GetXaxis()->GetBinCenter(xBin)*1.e3; // E if vsE
        for (int yBin = 1; yBin <= hist_pt_eta->GetNbinsY(); ++yBin)
        {

	    // Positive eta case
            double eta = hist_pt_eta->GetYaxis()->GetBinCenter(yBin);
	    bool fill = false; // only used if vsE
	    bool fillpos = false; // only used if vsE
	    bool fillneg = false; // only used if vsE

            // Set the main 4-vector and scale 4-vector
	    if ( !vsE ){
              jet->setJetP4(xAOD::JetFourMom_t(pt,eta,0,massForScan));
              detectorEta(*jet) = eta;
              startingScale.setAttribute(*jet,xAOD::JetFourMom_t(pt,eta,0,massForScan));
	    } else {
              const double E = pt; // pt is actually E if vsE
	      const double pT = sqrt((E*E)-(massForScan*massForScan))/cosh(eta);
	      if ( pT >= minpT ) fillpos = true;
              jet->setJetP4(xAOD::JetFourMom_t(pT,eta,0,massForScan));
              detectorEta(*jet) = eta;
              startingScale.setAttribute(*jet,xAOD::JetFourMom_t(pT,eta,0,massForScan));
	    }

            // Jet kinematics set, now apply calibration
            xAOD::Jet* calibJet = nullptr;
            if(calibTool->calibratedCopy(*jet,calibJet) != CP::CorrectionCode::Ok){
              std::cout << "ERROR: calibratedCopy returned a non OK status" << std::endl;
              return 0;
	    }
	    std::unique_ptr<xAOD::Jet> calibJetPtr (calibJet);

            // Calculate etaReco - etaTruth
            const double PosEtaJES = startingScale(*jet).eta()-calibJetPtr->eta();

	    // Negative eta case
            eta *= -1.;

            // Set the main 4-vector and scale 4-vector
	    if ( !vsE ){
              jet->setJetP4(xAOD::JetFourMom_t(pt,eta,0,massForScan));
              detectorEta(*jet) = eta;
              startingScale.setAttribute(*jet,xAOD::JetFourMom_t(pt,eta,0,massForScan));
	    } else {
              const double E = pt; // pt is actually E if vsE
	      const double pT = sqrt((E*E)-(massForScan*massForScan))/cosh(eta);
	      if ( pT >= minpT ) fillneg = true;
              jet->setJetP4(xAOD::JetFourMom_t(pT,eta,0,massForScan));
              detectorEta(*jet) = eta;
              startingScale.setAttribute(*jet,xAOD::JetFourMom_t(pT,eta,0,massForScan));
	    }

	    if (fillneg && fillpos) fill = true;

            // Jet kinematics set, now apply calibration
            calibJet = nullptr;
            if(calibTool->calibratedCopy(*jet,calibJet) != CP::CorrectionCode::Ok){
              std::cout << "ERROR: calibratedCopy returned a non OK status" << std::endl;
              return 0;
	    }
	    calibJetPtr = std::unique_ptr<xAOD::Jet>(calibJet);

            // Calculate sgn(eta) * (etaReco - etaTruth)
            double NegEtaJES = calibJetPtr->eta()-startingScale(*jet).eta();

	    // Average results
	    const double EtaJES = 0.5*(PosEtaJES + NegEtaJES);

            // Fill the plot
	    if ( !vsE ) hist_pt_eta->SetBinContent(xBin,yBin,EtaJES);
	    else { // JES_vs_E
	      if ( fill ){ 
		hist_pt_eta->SetBinContent(xBin,yBin,EtaJES);
	        hist_pt_eta->SetBinError(xBin,yBin,0.00005); // Set the errors to some randomly small value!
	      }
	    }
            
        }
    }

    // Now labels/etc
    hist_pt_eta->SetStats(false);
    if ( !vsE ) hist_pt_eta->GetXaxis()->SetTitle("Jet #it{p}_{T} [GeV]");
    else { hist_pt_eta->GetXaxis()->SetTitle("Jet E [GeV]"); }
    hist_pt_eta->GetXaxis()->SetTitleOffset(1.35);
    hist_pt_eta->GetXaxis()->SetMoreLogLabels();
    hist_pt_eta->GetYaxis()->SetTitle("#eta");
    hist_pt_eta->GetYaxis()->SetTitleOffset(0.9);
    hist_pt_eta->GetZaxis()->SetTitle("Eta_{Factor}");

    // Now write out the histogram
    std::unique_ptr<TFile> fout = std::make_unique<TFile>(outFile.Data(),"RECREATE");
    hist_pt_eta->Write();
    fout->Close();

    return 0;
}
