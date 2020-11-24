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
#include "TF1.h"
#include "TLatex.h"


void setJetKinematics(xAOD::Jet& jet, double pt, double eta, double phi, double mass)
{
    static jet::JetFourMomAccessor scaleCalo(jet::CompMassDef::getJetScaleString(jet::CompMassDef::CaloMass).Data());
    static jet::JetFourMomAccessor scaleTA(jet::CompMassDef::getJetScaleString(jet::CompMassDef::TAMass).Data());
    static jet::JetFourMomAccessor scaleCombQCD(jet::CompMassDef::getJetScaleString(jet::CompMassDef::CombMassQCD).Data());
    static jet::JetFourMomAccessor scaleCombWZ(jet::CompMassDef::getJetScaleString(jet::CompMassDef::CombMassWZ).Data());
    static jet::JetFourMomAccessor scaleCombHbb(jet::CompMassDef::getJetScaleString(jet::CompMassDef::CombMassHbb).Data());
    static jet::JetFourMomAccessor scaleCombTop(jet::CompMassDef::getJetScaleString(jet::CompMassDef::CombMassTop).Data());
    
    jet.setJetP4(                  xAOD::JetFourMom_t(pt,eta,phi,mass));
    scaleCalo.setAttribute(   jet, xAOD::JetFourMom_t(pt,eta,phi,mass));
    scaleTA.setAttribute(     jet, xAOD::JetFourMom_t(pt,eta,phi,mass));
    scaleCombQCD.setAttribute(jet, xAOD::JetFourMom_t(pt,eta,phi,mass));
    scaleCombWZ.setAttribute( jet, xAOD::JetFourMom_t(pt,eta,phi,mass));
    scaleCombHbb.setAttribute(jet, xAOD::JetFourMom_t(pt,eta,phi,mass));
    scaleCombTop.setAttribute(jet, xAOD::JetFourMom_t(pt,eta,phi,mass));
}

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
        printf("\t5. Component name and variation\n");
        printf("\t\tExample: \"FourVecResUnc,+1\"\n");
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

    if (jet::utils::vectorize<TString>(component,",").size() != 2)
    {
        printf("Bad component formatting, got \"%s\"\n",component.Data());
        exit(1);
    }
    TString compName = jet::utils::vectorize<TString>(component,",").at(0);
    float   shift    = atof(jet::utils::vectorize<TString>(component,",").at(1));

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
    CP::SystematicVariation variation(Form("JET_%s",compName.Data()),shift);
    if (!tool->isAffectedBySystematic(variation))
    {
        printf("The specified variation was not recognized: JET_%s\n",compName.Data());
        exit(1);
    }
    CP::SystematicSet syst;
    syst.insert(variation);
    if (tool->applySystematicVariation(syst) != StatusCode::SUCCESS)
    {
        printf("Failed to apply systematic variation\n");
        exit(1);
    }

    // Get info on the variation
    const size_t compIndex = tool->getComponentIndex("JET_"+compName);
    const std::set<jet::CompScaleVar::TypeEnum> scaleVars = tool->getComponentScaleVars(compIndex);
    const jet::CompScaleVar::TypeEnum scaleVar = scaleVars.size() == 1 ? *(scaleVars.begin()) : jet::CompScaleVar::UNKNOWN;
    const jet::JetTopology::TypeEnum topology = tool->getComponentTopology(compIndex);

    printf("Component is index %zu, with ScaleVar %s and topology %s\n",compIndex,jet::CompScaleVar::enumToString(scaleVar).Data(),jet::JetTopology::enumToString(topology).Data());

    // Prepare the canvas
    TCanvas canvas("canvas");
    canvas.SetMargin(0.12,0.04,0.15,0.04);
    canvas.SetFillStyle(4000);
    canvas.SetFillColor(0);
    canvas.SetFrameBorderMode(0);
    canvas.cd();
    canvas.Print(outFile+"[");
    

    // Prepare to add labels
    TLatex tex;
    tex.SetNDC();
    tex.SetTextFont(42);
    tex.SetTextSize(0.04);


    // Define the jet that we want to smear repeatedly
    // Center the jet around pT of 1 TeV and mass of 100 GeV (easy numbers)
    // eta_bins = [0.0,0.2,0.7,1.3,1.8,2.5,3.2,3.5,4.5]
    const double pT   = 1000;
    const double mass = 100;
    const double phi  = 0;
    const std::vector<double> etaVals = optHelper.GetFixedEtaVals();
    const int numSmear = 100000;

    for (const double eta : etaVals)
    {
        // Prepare a histogram to fill repeatedly as we smear the value
        TH1D smearPt(Form("smearPt_eta%.2f",eta),"",100,500,1500);
        TH1D smearMass(Form("smearMass_eta%.2f",eta),"",100,50,150);
        smearPt.Sumw2();
        smearMass.Sumw2();
        smearPt.SetStats(0);
        smearMass.SetStats(0);
        smearPt.GetXaxis()->SetTitle("#it{p}_{T} [GeV]");
        smearMass.GetXaxis()->SetTitle("#it{m} [GeV]");
        smearPt.GetYaxis()->SetTitle("Number of events");
        smearMass.GetYaxis()->SetTitle("Number of events");

        // Now smear repeatedly from the same starting jet
        for (int iSmear = 0; iSmear < numSmear; ++iSmear)
        {
            setJetKinematics(*jet,pT,eta,phi,mass);
            if (tool->applyCorrection(*jet,*eInfo) != CP::CorrectionCode::Ok)
            {
                printf("Error while smearing, iteration %d\n",iSmear);
                exit(1);
            }
            // Fill the histograms with the smeared jet
            smearPt.Fill(jet->pt());
            smearMass.Fill(jet->m());
        }
        
        // Get info on the expected values
        setJetKinematics(*jet,pT,eta,phi,mass);
        const double nomData = tool->getNominalResolutionData(*jet,scaleVar,topology);
        const double nomMC   = tool->getNominalResolutionMC(*jet,scaleVar,topology);
        const double uncert  = tool->getUncertainty(compIndex,*jet,*eInfo,scaleVar);
        
        const double fullUncMC   = jet::CompScaleVar::isRelResolutionType(scaleVar) ? nomMC * uncert : uncert;
        const double fullUncData = jet::CompScaleVar::isRelResolutionType(scaleVar) ? nomData * uncert : uncert;
        const double smearMC   = sqrt(pow(nomMC + fabs(shift*fullUncMC),2) - pow(nomMC,2));
        const double smearData = nomData != JESUNC_ERROR_CODE ? sqrt(pow(nomData + fabs(shift*fullUncData),2) - pow(nomData,2)) : 0;

        // Fit a Gaussian
        TF1 fitPt("fitPt","gaus");
        smearPt.Fit(&fitPt,"E");
        TF1 fitMass("fitMass","gaus");
        smearMass.Fit(&fitMass,"E");

        // Draw and print the plot
        smearPt.Draw();
        tex.DrawLatex(0.70,0.9,"Gaussian fit results");
        tex.DrawLatex(0.70,0.85,Form("#mu = %.0f GeV",fitPt.GetParameter(1)));
        tex.DrawLatex(0.70,0.80,Form("#sigma = %.1f GeV",fitPt.GetParameter(2)));
        tex.DrawLatex(0.70,0.75,Form("#mu/#sigma = %.1f%%",fitPt.GetParameter(2)/fitPt.GetParameter(1)*100));
        tex.DrawLatex(0.15,0.9,"Expectation from tool");
        tex.DrawLatex(0.15,0.85,"#sigma_{smear}^{2} = (#sigma_{nom} + |N#delta#sigma|)^{2} - (#sigma_{nom})^{2}");
        if (nomData != JESUNC_ERROR_CODE)
            tex.DrawLatex(0.15,0.80,Form("#sigma_{nom}^{data}/#it{p}_{T} = %.1f%%",100*nomData));
        else
            tex.DrawLatex(0.15,0.80,Form("#sigma_{nom}^{data}/#it{p}_{T} = N/A"));
        tex.DrawLatex(0.15,0.75,Form("#sigma_{nom}^{MC}/#it{p}_{T} = %.1f%%",100*nomMC));
        tex.DrawLatex(0.15,0.70,Form("#delta#sigma/#it{p}_{T} = %.1f%%",100*uncert));
        tex.DrawLatex(0.15,0.65,Form("N_{sigma} = %+.1f",shift));
        if (nomData != JESUNC_ERROR_CODE)
            tex.DrawLatex(0.15,0.55,Form("#sigma_{smear}^{data}/#it{p}_{T} = %.1f%%",100*smearData));
        else
            tex.DrawLatex(0.15,0.55,Form("#sigma_{smear}^{data}/#it{p}_{T} = N/A"));
        tex.DrawLatex(0.15,0.50,Form("#sigma_{smear}^{MC}/#it{p}_{T} = %.1f%%",100*smearMC));
        canvas.Print(outFile);



        smearMass.Draw();
        tex.DrawLatex(0.70,0.9,"Gaussian fit results");
        tex.DrawLatex(0.70,0.85,Form("#mu = %.0f GeV",fitMass.GetParameter(1)));
        tex.DrawLatex(0.70,0.80,Form("#sigma = %.1f GeV",fitMass.GetParameter(2)));
        tex.DrawLatex(0.70,0.75,Form("#mu/#sigma = %.1f%%",fitMass.GetParameter(2)/fitMass.GetParameter(1)*100));
        tex.DrawLatex(0.15,0.9,"Expectation from tool");
        tex.DrawLatex(0.15,0.85,"#sigma_{smear}^{2} = (#sigma_{nom} + |N#delta#sigma|)^{2} - (#sigma_{nom})^{2}");
        if (nomData != JESUNC_ERROR_CODE)
            tex.DrawLatex(0.15,0.80,Form("#sigma_{nom}^{data}/#it{p}_{T} = %.1f%%",100*nomData));
        else
            tex.DrawLatex(0.15,0.80,Form("#sigma_{nom}^{data}/#it{p}_{T} = N/A"));
        tex.DrawLatex(0.15,0.75,Form("#sigma_{nom}^{MC}/#it{p}_{T} = %.1f%%",100*nomMC));
        tex.DrawLatex(0.15,0.70,Form("#delta#sigma/#it{p}_{T} = %.1f%%",100*uncert));
        tex.DrawLatex(0.15,0.65,Form("N_{sigma} = %+.1f",shift));
        if (nomData != JESUNC_ERROR_CODE)
            tex.DrawLatex(0.15,0.55,Form("#sigma_{smear}^{data}/#it{p}_{T} = %.1f%%",100*smearData));
        else
            tex.DrawLatex(0.15,0.55,Form("#sigma_{smear}^{data}/#it{p}_{T} = N/A"));
        tex.DrawLatex(0.15,0.50,Form("#sigma_{smear}^{MC}/#it{p}_{T} = %.1f%%",100*smearMC));
        canvas.Print(outFile);
    }
    
    // Done printing
    canvas.Print(outFile+"]");

    return 0;
}
