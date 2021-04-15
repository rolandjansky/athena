/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "JetCalibTools/JetCalibrationTool.h"

#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/TStore.h"

#include "AsgTools/StandaloneToolHandle.h"

#include <vector>
#include <ctime>
#include "TString.h"
#include "TH1D.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TRandom3.h"


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
    if (argc < 5 || argc > 7)
    {
        std::cout << "USAGE: " << argv[0] << " <JetCollection> <ConfigFile> <OutputFile> <isData> (dev mode switch) (timing test switch)" << std::endl;
        return 1;
    }

    // Store the arguments
    const TString jetAlgo = argv[1];
    const TString config  = argv[2];
    const TString outFile = argv[3];
    const bool isData     = (TString(argv[4]) == "true");
    const bool isDevMode  = ( argc > 5 && (TString(argv[5]) == "true" || TString(argv[5]) == "dev") ) ? true : false;
    const bool isTimeTest = ( argc > 6 && TString(argv[6]) == "true" ) ? true : false;

    // Derived information
    const bool outFileIsExtensible = outFile.EndsWith(".pdf") || outFile.EndsWith(".ps");
    const bool outFileIsRoot = outFile.EndsWith(".root");

    // Assumed constants
    const TString calibSeq = "Smear"; // only want to apply the smearing correction here
    const std::vector<float> ptVals = {20, 40, 60, 100, 400, 1000};
    const float eta = 0.202;
    const float phi = 0;
    const float mass = 10;
    const int maxNumIter = 1e5;
    const int numTimeIter = 100;

    // Accessor strings
    const TString startingScaleString = "JetGSCScaleMomentum";
    const TString endingScaleString   = "JetSmearedMomentum";

    // Accessors
    jet::JetFourMomAccessor startingScale(startingScaleString.Data());
    jet::JetFourMomAccessor endingScale(endingScaleString.Data());

    // Create the calib tool
    asg::StandaloneToolHandle<IJetCalibrationTool> calibTool;
    calibTool.setTypeAndName("JetCalibrationTool/MyJetCalibTool");
    {
        if (calibTool.initialize().isFailure())
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
    std::vector<TH1D*> hists_pt;
    std::vector<TH1D*> hists_m;
    for (float pt : ptVals)
    {
        const TString baseName = Form("Smear_%.0f",pt);
        hists_pt.push_back(new TH1D(baseName+"_pT",baseName+"_pT",100,0.25*pt,1.75*pt));
        hists_m.push_back(new TH1D(baseName+"_mass",baseName+"_mass",100,0.25*mass,1.75*mass));
    }

    // Run the timing test if specified
    if (isTimeTest)
    {
        TRandom3 rand;
        rand.SetSeed(0); // Deterministic random test

        for (int numTest = 0; numTest < numTimeIter; ++numTest)
        {
            // Make a new calibration tool each time
            JetCalibrationTool* jetCalibTool = new JetCalibrationTool(Form("mytool_%d",numTest));
            if (jetCalibTool->setProperty("JetCollection",jetAlgo.Data()).isFailure())
                exit(1);
            if (jetCalibTool->setProperty("ConfigFile",config.Data()).isFailure())
                exit(1);
            if (jetCalibTool->setProperty("CalibSequence",calibSeq.Data()).isFailure())
                exit(1);
            if (jetCalibTool->setProperty("IsData",isData).isFailure())
                exit(1);
            if (isDevMode && jetCalibTool->setProperty("DEVmode",isDevMode).isFailure())
                exit(1);
            if (jetCalibTool->initialize().isFailure())
                exit(1);

            clock_t startTime = clock();
            for (int numIter = 0; numIter < maxNumIter; ++numIter)
            {
                xAOD::JetFourMom_t fourvec(rand.Uniform(20.e3,1000.e3),rand.Uniform(-2,2),rand.Uniform(-3.14,3.14),10.e3);
                jet->setJetP4(fourvec);
                startingScale.setAttribute(*jet,fourvec);
                // Apply calibration
                if(jetCalibTool->modify(*jets).isFailure())
                  exit(1);
            }
            delete jetCalibTool;
            printf("Iteration %d: %f seconds\n",numTest+1,(clock()-startTime)/((double)CLOCKS_PER_SEC));
        }
    }

    // Fill the histograms
    for (size_t index = 0; index < ptVals.size(); ++index)
    {
        // Get the relevant vector entries
        const float pt = ptVals.at(index);
        TH1D* hist_pt  = hists_pt.at(index);
        TH1D* hist_m   = hists_m.at(index);
        hist_pt->Sumw2();
        hist_m->Sumw2();


        printf("Running for pT of %.0f\n",pt);

        for (int numIter = 0; numIter < maxNumIter; ++numIter)
        {
            // Set the jet four-vector
            xAOD::JetFourMom_t fourvec(pt*1.e3,eta,phi,mass*1.e3);
            jet->setJetP4(fourvec);
            startingScale.setAttribute(*jet,fourvec);

            // Jet kinematics set, now apply the smearing correction
            if(calibTool->modify(*jets).isFailure())
              exit(1);

            // Ensure the expected scale was written (within 1 MeV, for floating point differences)
            if (fabs(endingScale(*jet).pt() - jet->pt()) > 1.e-3)
            {
                printf("ERROR: mismatch between ending scale (%.3f) and jet pT (%.3f)\n",endingScale(*jet).pt(),jet->pt());
                exit(1);
            }
            if (endingScale(*jet).pt() == startingScale(*jet).pt())
            {
                // This can happen (smearing factor can be exactly 1), but it should be rare
                printf("WARNING: starting and ending scales are identical: %.3f\n",endingScale(*jet).pt());
            }

            // Fill the histograms
            hist_pt->Fill(jet->pt()/1.e3);
            hist_m->Fill(jet->m()/1.e3);
        }
    }


    // Get the nominal resolution
    printf("Getting nominal resolutions\n");
    TH1D nominalResData("NominalResData","NominalResData",1000,20,2020);
    TH1D nominalResMC(  "NominalResMC",  "NominalResMC",  1000,20,2020);
    for (Long64_t binX = 1; binX < nominalResData.GetNbinsX()+1; ++binX)
    {
        //  Set the jet four-vector
        const float pt = nominalResData.GetXaxis()->GetBinCenter(binX);
        xAOD::JetFourMom_t fourvec(pt*1.e3,eta,phi,mass*1.e3);
        jet->setJetP4(fourvec);

        // Jet kinematics set, now get the nominal resolutions
        double resolution = 0;
        if (calibTool->getNominalResolutionData(*jet,resolution).isFailure())
        {
            printf("ERROR: Failed to get nominal data resolution\n");
            exit(1);
        }
        nominalResData.SetBinContent(binX,resolution);
        if (calibTool->getNominalResolutionMC(*jet,resolution).isFailure())
        {
            printf("ERROR: Failed to get nominal MC resolution\n");
            exit(1);
        }
        nominalResMC.SetBinContent(binX,resolution);
    }


    // Make the plots
    // First the canvas
    TCanvas* canvas = new TCanvas("canvas");
    canvas->SetMargin(0.07,0.13,0.1,0.10);
    canvas->SetFillStyle(4000);
    canvas->SetFillColor(0);
    canvas->SetFrameBorderMode(0);
    canvas->cd();

    // Make the fits
    std::vector<TF1*> fits_pt;
    std::vector<TF1*> fits_m;
    for (size_t index = 0; index < hists_pt.size(); ++index)
    {
        TH1D* hist_pt = hists_pt.at(index);
        TH1D* hist_m  = hists_m.at(index);

        TF1* fit_pt = new TF1(Form("fitPt_%zu",index),"gaus");
        TF1* fit_m  = new TF1(Form("fitMass_%zu",index),"gaus");

        hist_pt->Fit(fit_pt,"E");
        hist_m->Fit(fit_m,"E");

        fits_pt.push_back(fit_pt);
        fits_m.push_back(fit_m);
    }

    // Set plot labels
    for (TH1D* hist : hists_pt)
    {
        hist->SetStats(false);
        hist->GetXaxis()->SetTitle("Jet #it{p}_{T} [GeV]");
        hist->GetXaxis()->SetTitleOffset(1.35);
        hist->GetYaxis()->SetTitle("Number of events");
        hist->GetYaxis()->SetTitleOffset(0.9);
    }
    for (TH1D* hist : hists_m)
    {
        hist->SetStats(false);
        hist->GetXaxis()->SetTitle("Jet mass [GeV]");
        hist->GetXaxis()->SetTitleOffset(1.35);
        hist->GetYaxis()->SetTitle("Number of events");
        hist->GetYaxis()->SetTitleOffset(0.9);
    }
    
    // Prepare to add labels
    TLatex tex;
    tex.SetNDC();
    tex.SetTextFont(42);
    tex.SetTextSize(0.04);
    
    // Prepare to write out
    if (outFileIsExtensible)
    {
        canvas->Print(outFile+"[");

        // Nominal resolutions
        canvas->SetLogx(true);
        nominalResData.Draw();
        canvas->Print(outFile);
        nominalResMC.Draw();
        canvas->Print(outFile);
        canvas->SetLogx(false);

        // Smearing plots
        for (size_t index = 0; index < hists_pt.size(); ++index)
        {
            hists_pt.at(index)->Draw("pe");
            tex.DrawLatex(0.70,0.9,"Gaussian fit results");
            tex.DrawLatex(0.70,0.85,Form("#mu = %.0f GeV",fits_pt.at(index)->GetParameter(1)));
            tex.DrawLatex(0.70,0.80,Form("#sigma = %.1f GeV",fits_pt.at(index)->GetParameter(2)));
            tex.DrawLatex(0.70,0.75,Form("#mu/#sigma = %.1f%%",fits_pt.at(index)->GetParameter(2)/fits_pt.at(index)->GetParameter(1)*100));
            canvas->Print(outFile);

            hists_m.at(index)->Draw("pe");
            tex.DrawLatex(0.70,0.9,"Gaussian fit results");
            tex.DrawLatex(0.70,0.85,Form("#mu = %.0f GeV",fits_m.at(index)->GetParameter(1)));
            tex.DrawLatex(0.70,0.80,Form("#sigma = %.1f GeV",fits_m.at(index)->GetParameter(2)));
            tex.DrawLatex(0.70,0.75,Form("#mu/#sigma = %.1f%%",fits_m.at(index)->GetParameter(2)/fits_m.at(index)->GetParameter(1)*100));
            canvas->Print(outFile);
        }
        canvas->Print(outFile+"]");
    }
    else if (outFileIsRoot)
    {
        TFile* outRootFile = new TFile(outFile,"RECREATE");
        std::cout << "Writing to output ROOT file: " << outFile << std::endl;
        outRootFile->cd();

        // Nominal resolutions
        nominalResData.Write();
        nominalResMC.Write();

        //  Smearing plots
        for (size_t index = 0; index < hists_pt.size(); ++index)
        {
            hists_pt.at(index)->Write();
            hists_m.at(index)->Write();
        }
        outRootFile->Close();
    }
    else
    {
        unsigned int counter = 1;

        // Nominal resolutions
        canvas->SetLogx(true);
        nominalResData.Draw();
        canvas->Print(Form("%u-%s",counter++,outFile.Data()));
        nominalResMC.Draw();
        canvas->Print(Form("%u-%s",counter++,outFile.Data()));
        canvas->SetLogx(false);

        // Smearing plots
        for (size_t index = 0; index < hists_pt.size(); ++index)
        {
            hists_pt.at(index)->Draw("pe");
            canvas->Print(Form("%u-%s",counter,outFile.Data()));

            hists_m.at(index)->Draw("pe");
            canvas->Print(Form("%u-%s",counter++,outFile.Data()));
        }
    }


    return 0;
}
