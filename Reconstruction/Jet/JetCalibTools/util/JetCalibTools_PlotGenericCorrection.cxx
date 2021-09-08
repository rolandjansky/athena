/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "JetCalibTools/JetCalibrationTool.h"

#include "PATInterfaces/CorrectionTool.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"

#include "AsgTools/AnaToolHandle.h"
#include "TString.h"
#include "TH1D.h"
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
        std::cout << "USAGE: " << argv[0] << " <JetCollection> <ConfigFile> <OutputFile> (dev mode switch) (input scale)" << std::endl;
        return 1;
    }

    // Store the arguments
    const TString jetAlgo = argv[1];
    const TString config  = argv[2];
    const TString outFile = argv[3];
    const bool isDevMode  = ( argc > 4 && (TString(argv[4]) == "true" || TString(argv[4]) == "dev") );
    const TString inScale = argc > 5 ? argv[5] : "";
    
    // Assumed constants
    const TString calibSeq = "GenericCorr"; // only want to apply the generic correction here
    const bool isData = false; // doesn't actually matter for JES, which is always active
    const float etaForScan  = 0;
    const float phiForScan  = 0;
    const float massForScan = 0;

    // Accessor strings
    const TString startingScaleString = inScale == "" ? "JetConstitScaleMomentum" : inScale;
    const TString endingScaleString   = "JetGenericCorrScaleMomentum";

    // Accessors
    jet::JetFourMomAccessor startingScale(startingScaleString.Data());
    jet::JetFourMomAccessor endingScale(endingScaleString.Data());
    
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
    TH1D hist("GenericCorr","GenericCorr",998,10,5000);

    // Fill the histogram
    for (int xBin {1}; xBin <= hist.GetNbinsX(); ++xBin)
    {
        const double pt {hist.GetXaxis()->GetBinCenter(xBin)*1.e3};
        
        // Set the jet kinematics
        jet->setJetP4(xAOD::JetFourMom_t(pt,etaForScan,phiForScan,massForScan));
        startingScale.setAttribute(*jet,xAOD::JetFourMom_t(pt,etaForScan,phiForScan,massForScan));

        // Apply the calibration
        xAOD::Jet* calibJet = nullptr;
        if (calibTool->calibratedCopy(*jet,calibJet) != CP::CorrectionCode::Ok)
        {
            std::cout << "ERROR: calibratedCopy returned a non-OK status" << std::endl;
            return 1;
        }
        std::unique_ptr<xAOD::Jet> calibJetPtr (calibJet);

        // Fill the plot
        hist.SetBinContent(xBin,calibJetPtr->pt()/startingScale(*jet).pt());
    }


    // First the canvas
    TCanvas canvas("canvas");
    canvas.SetMargin(0.07,0.13,0.1,0.10);
    canvas.SetFillStyle(4000);
    canvas.SetFillColor(0);
    canvas.SetFrameBorderMode(0);
    canvas.cd();
    canvas.SetLogx(true);
    
    // Add labels/etc to the plot
    hist.SetStats(false);
    hist.GetXaxis()->SetTitle("Jet #it{p}_{T} [GeV]");
    hist.GetYaxis()->SetTitle("Generic correction factor");
    hist.SetTitle(config.Data());

    // Now print out the histogram
    hist.Draw();
    canvas.Print(outFile.Data());

    return 0;
}

