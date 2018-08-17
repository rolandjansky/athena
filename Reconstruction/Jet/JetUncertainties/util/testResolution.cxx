/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/JetUncertaintiesTool.h"
#include "JetUncertainties/Helpers.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODEventInfo/EventInfoContainer.h"
#include "xAODEventInfo/EventInfoAuxContainer.h"
    
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicVariation.h" 
#include "PATInterfaces/SystematicsUtil.h"

#ifdef ROOTCORE
#   include "xAODRootAccess/TEvent.h"
#   include "xAODRootAccess/TStore.h"
#endif // ROOTCORE

#include "OptionHelper.h"

#include "TH1D.h"
#include "TCanvas.h"

int main (int argc, char* argv[])
{
    StatusCode::enableFailure();
    jet::OptionHelper optHelper;


    if (argc != 7 && argc != 8)
    {
        printf("Expected arguments:\n");
        printf("\t1. Output file (pdf)\n");
        printf("\t2. Jet definition\n");
        printf("\t3. MC type\n");
        printf("\t4. Config file\n");
        printf("\t5. Component name\n");
        printf("\t6. IsData (\"true\" or \"false\")\n");
        printf("\t7. Options (optional argument), semi-colon delimited, examples:\n");
        printf("\t\tisDijet=false\n");
        printf("\t\tisLargeR=false\n");
        exit(1);
    }
    TString outFile    = argv[1];
    TString jetDef     = argv[2];
    TString mcType     = argv[3];
    TString config     = argv[4];
    TString component  = argv[5];
    TString isDataStr  = argv[6];
    if (argc == 8) optHelper.Initialize(jet::utils::vectorize<TString>(argv[7],";"));
    else           optHelper.Initialize(std::vector<TString>());

    if (!outFile.EndsWith(".pdf"))
    {
        printf("Only pdf output files are currently supported\n");
        exit(1);
    }

    bool isData = false;
    if (!isDataStr.CompareTo("true",TString::kIgnoreCase))
        isData = true;
    else if (!isDataStr.CompareTo("false",TString::kIgnoreCase))
        isData = false;
    else
    {
        printf("Unable to determine whether to configure for data or MC: %s\n",isDataStr.Data());
        exit(1);
    }

    JetUncertaintiesTool* tool = new JetUncertaintiesTool("JESprov");
    if (tool->setProperty("JetDefinition",jetDef.Data()).isFailure())
        exit(1);
    if (tool->setProperty("MCType",mcType.Data()).isFailure())
        exit(1);
    if (tool->setProperty("ConfigFile",config.Data()).isFailure())
        exit(1);
    if (tool->setProperty("IsData",isData).isFailure())
        exit(1);
    if (optHelper.GetPath() != "")
        if (tool->setProperty("Path",optHelper.GetPath().Data()).isFailure())
            exit(1);
    if (tool->setScaleToGeV().isFailure())
        exit(1);
    if (tool->initialize().isFailure())
        exit(1);
    

    // Build a jet container and a jet for us to manipulate later
    xAOD::TEvent event;
    xAOD::TStore store;
    xAOD::JetContainer* jets = new xAOD::JetContainer();
    jets->setStore(new xAOD::JetAuxContainer());
    jets->push_back(new xAOD::Jet());
    xAOD::Jet* jet = jets->at(0);
        
    // Build an EventInfo object for us to manipulate later
    xAOD::EventInfoContainer* eInfos = new xAOD::EventInfoContainer();
    eInfos->setStore(new xAOD::EventInfoAuxContainer());
    eInfos->push_back(new xAOD::EventInfo());
    xAOD::EventInfo* eInfo = eInfos->at(0);

    // Ensure that the specified component is a valid systematic
    // This is a +1sigma variation
    CP::SystematicVariation variation(Form("JET_%s",component.Data()),1);
    if (!tool->isAffectedBySystematic(variation))
    {
        printf("The specified variation was not recognized: JET_%s\n",component.Data());
        exit(1);
    }
    CP::SystematicSet syst;
    syst.insert(variation);
    if (tool->applySystematicVariation(syst) != CP::SystematicCode::Ok)
    {
        printf("Failed to apply systematic variation\n");
        exit(1);
    }

    // Prepare the canvas
    TCanvas canvas("canvas");
    canvas.SetMargin(0.12,0.04,0.15,0.04);
    canvas.SetFillStyle(4000);
    canvas.SetFillColor(0);
    canvas.SetFrameBorderMode(0);
    canvas.cd();
    canvas.Print(outFile+"[");
    

    // Define the jet that we want to smear repeatedly
    // Center the jet around pT of 1 TeV and mass of 100 GeV (easy numbers)
    // eta_bins = [0.0,0.2,0.7,1.3,1.8,2.5,3.2,3.5,4.5]
    const double pT   = 1000;
    const double mass = 100;
    const double phi  = 0;
    const std::vector<double> etaVals = {0,0.1,0.45,1.0,1.55,2.15,2.85,3.35,4.0};
    const int numSmear = 10000;

    for (const double eta : etaVals)
    {
        // Prepare a histogram to fill repeatedly as we smear the value
        TH1D smearPt(Form("smearPt_eta%.2f",eta),"",100,500,1500);
        TH1D smearMass(Form("smearMass_eta%.2f",eta),"",100,50,150);

        // Now smear repeatedly from the same starting jet
        for (int iSmear = 0; iSmear < numSmear; ++iSmear)
        {
            jet->setJetP4(xAOD::JetFourMom_t(pT,eta,phi,mass));
            if (tool->applyCorrection(*jet,*eInfo) != CP::CorrectionCode::Ok)
            {
                printf("Error while smearing, iteration %d\n",iSmear);
                exit(1);
            }
            // Fill the histograms with the smeared jet
            smearPt.Fill(jet->pt());
            smearMass.Fill(jet->m());
        }

        // Draw and print the plot
        smearPt.Draw();
        canvas.Print(outFile);
        smearMass.Draw();
        canvas.Print(outFile);
    }
    
    // Done printing
    canvas.Print(outFile+"]");

    return 0;
}
