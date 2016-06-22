/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "JetUncertainties/JetUncertaintiesTool.h"
#include "JetUncertainties/Helpers.h"

#include "../testingMacros/atlasstyle/AtlasStyle.C"
#include "../testingMacros/atlasstyle/AtlasLabels.C"

#include "TString.h"
#include "TCanvas.h"
#include "TH2D.h"
#include "TFile.h"

#include <vector>

const double fixedRangeDiff = 0;
const bool addExtremumInfo = true;
const bool addScenarioInfo = false;
const bool makeGrid = true;
const bool minRepStyle = false;
const bool testJER = false;

TFile* outHistFile = NULL;
bool useRelativeMetric = true;
int iEPS = 0;


double relativeMetric(const double numerator, const double denominator)
{
    if (fabs(denominator) < 1.e-3)
    {
        if (fabs(numerator) < 1.e-3)
            return 1;
        else
            return JESUNC_ERROR_CODE;
    }
    return sqrt(numerator/denominator);
}

void DrawLabels(const TH2D* histo, const double fixedValue1, const double fixedValue2, const JetUncertaintiesTool* uncTool, const JetUncertaintiesTool* uncToolDiff = NULL, const double mean = 0, const double extremum = 0, const int extremeX = -1, const int extremeY = -1)
{
    // Create the tex making object
    static TLatex* tex = NULL;
    if (!tex)
    {
        tex = new TLatex();
        tex->SetNDC();
        tex->SetTextSize(0.045);
        tex->SetTextColor(kBlack);
    }

    // Add the ATLAS label
    ATLASLabel(0.13,minRepStyle?0.825:0.905,"Internal",kBlack);

    // Determine the configuration type (data year, correlation scenario, reduction if applicable)
    const TString config1 = uncTool->getConfigFile();
    const TString config2 = uncToolDiff ? uncToolDiff->getConfigFile() : "";
    const TString year = config1.Contains("_2011/") ? "2011"/*, #sqrt{s} = 7 TeV"*/ : (config1.Contains("_2012/") ? "2012"/*, #sqrt{s} = 8 TeV"*/ : "UNKNOWN");
    const TString jetDef = uncTool->getJetDef();
    const TString scenario1 = config1.Contains("StrongerCorrelations") ? "stronger" : (config1.Contains("WeakerCorrelations") ? "weaker" : "nominal");
    const TString scenario2 = config2.Contains("StrongerCorrelations") ? "stronger" : (config2.Contains("WeakerCorrelations") ? "weaker" : "nominal");
    const TString scenarioDiff = Form("%s - %s correlation scenarios",scenario1.Data(),scenario2.Data());
    const TString reduction1 = config1.Contains("_ByCategory") ? "category reduction" : (config1.Contains("NP") ? "global reduction" : "original");
    const TString reduction2 = config2.Contains("_ByCategory") ? "category reduction" : (config2.Contains("NP") ? "global reduction" : "original");
    const TString reductionDiff = Form("%s - %s",reduction1.Data(),reduction2.Data());

    TString configDescString;
    if (uncToolDiff)
    {
        if (scenario1 == scenario2 && reduction1 == reduction2)
            configDescString = "UNKNOWN COMPARISON TYPE";
        else if (scenario1 == scenario2 && reduction1 != reduction2)
            configDescString = Form("%s correlation scenario, %s",scenario1.Data(),reductionDiff.Data());
        else if (scenario1 != scenario2 && reduction1 == reduction2)
            configDescString = Form("%s%s",scenarioDiff.Data(),reduction1 == "original" ? "" : Form(", %s",reduction1.Data()));
        else
            configDescString = Form("%s, %s",scenarioDiff.Data(),reductionDiff.Data());
    }
    else
    {
        configDescString = Form("%s correlation scenario%s",scenario1.Data(),reduction1 == "original" ? "" : Form(", %s",reduction1.Data()));
    }


    // Add the configuration info
    const TString fixedValString = TString(histo->GetXaxis()->GetTitle()).Contains("#it{p}") ? Form("(#eta^{jet1},#eta^{jet2}) = (%.1f,%.1f)",fixedValue1,fixedValue2) : Form("(#it{p}_{T}^{jet1},#it{p}_{T}^{jet2}) = (%ld,%ld) GeV",lround(fixedValue1),lround(fixedValue2));
    //tex->DrawLatex(0.13,minRepStyle?0.95:0.91,Form("anti-k_{t} #it{R} = %s, %s+JES+GSC, Data %s, %s",jetDef.Contains("AntiKt4") ? "0.4" : jetDef.Contains("AntiKt6") ? "0.6" : "UNKNOWN", jetDef.Contains("EM") ? "EM" : jetDef.Contains("LC") ? "LCW" : "UNKNOWN", year.Data(),fixedValString.Data()));
    if (minRepStyle)
    {
        const TString reductionString = "str.red"; //"evdm";
        const TString selector = ""; //"#it{p}_{T}^{jet},";
        const TString scenarioString = TString(histo->GetName()).BeginsWith("diff_")?Form("Correlation difference, Rep_{full}^{%sJES} - Rep_{%s}^{%sJES}, %s",selector.Data(),reductionString.Data(),selector.Data(),fixedValString.Data()):Form("Correlation matrix, Rep_{%s}^{%sJES}, %s",TString(histo->GetName()).Contains("_0_")?"full":reductionString.Data(),selector.Data(),fixedValString.Data());
        tex->DrawLatex(0.13,0.8751,Form("anti-k_{t} #it{R} = %s, %s+JES %s",jetDef.Contains("AntiKt4") ? "0.4" : jetDef.Contains("AntiKt6") ? "0.6" : "UNKNOWN", jetDef.Contains("EM") ? "EM" : jetDef.Contains("LC") ? "LCW" : "UNKNOWN", year.Data()));
        tex->DrawLatex(0.13,0.9305,scenarioString.Data());
    }
    else
    {
        tex->DrawLatex(0.13,0.955,Form("anti-k_{t} #it{R} = %s, %s+JES",jetDef.Contains("AntiKt4") ? "0.4" : jetDef.Contains("AntiKt6") ? "0.6" : "UNKNOWN", jetDef.Contains("EM") ? "EM" : jetDef.Contains("LC") ? "LCW" : "UNKNOWN"));
        tex->DrawLatex(0.56,0.955,fixedValString.Data());
        tex->DrawLatex(0.56,0.905,Form("Data %s, #sqrt{s} = %d TeV",year.Data(),year=="2012"?8:year=="2011"?7:year=="2015"?13:0));
    }

    
    if (addScenarioInfo)
    {
        tex->DrawLatex(0.15,0.84,configDescString.Data());
        tex->DrawLatex(0.15,0.79,TString(uncTool->getAnalysisFile().c_str()).Contains("Unknown") ? "unknown flavour composition" : TString(uncTool->getAnalysisFile().c_str()).Contains("InclusiveJets") ? "inclusive jets composition" : "Unexpected flavour composition file");
    }


    if (addExtremumInfo && extremeX > 0 && extremeY > 0)
    {
        if (TString(histo->GetXaxis()->GetTitle()).Contains("#it{p}"))
            tex->DrawLatex(0.01,0.015,Form("Mean value %.1f%%, max %.1f%% at (%ld,%ld) GeV",mean*100,extremum*100,lround(histo->GetXaxis()->GetBinCenter(extremeX)),lround(histo->GetYaxis()->GetBinCenter(extremeY))));
        else
            tex->DrawLatex(0.01,0.015,Form("Mean value %.1f%%, max %.1f%% at (%.2f,%.2f)",mean*100,extremum*100,histo->GetXaxis()->GetBinCenter(extremeX),histo->GetYaxis()->GetBinCenter(extremeY)));
    }
}

void FormatHisto(TH2D* histo)
{
    histo->SetTitleSize(0.06,"z");
    histo->SetLabelSize(0.04,"z");
    histo->SetTitleOffset(1.15,"x");
    histo->SetTitleOffset(1.05,"y");
    histo->SetTitleOffset(0.76,"z");
    histo->SetLabelOffset(0.003,"x");
    histo->GetXaxis()->SetMoreLogLabels();
}

void PlotCorrelationHistos(const TString& outFile,TCanvas* canvas,const std::vector<JetUncertaintiesTool*>& providers, const std::vector<TH2D*>& corrMats, const double fixedValue1, const double fixedValue2)
{
    // First make the difference histograms
    // Must be now as colz bug fixes axis range once draw is called
    std::vector<TH2D*> corrMatDiffs;
    std::vector< std::pair<JetUncertaintiesTool*,JetUncertaintiesTool*> > diffProviders;
    for (size_t iHisto = 0; iHisto + 1 < corrMats.size(); ++iHisto)
    {
        // Take the difference
        TH2D* diff = new TH2D(*corrMats.at(iHisto));
        diff->SetName(Form("diff_%s",diff->GetName()));

        // Subtract, watching for kinematic limits and relative metric
        for (int binX = 1; binX <= diff->GetNbinsX(); ++binX)
            for (int binY = 1; binY <= diff->GetNbinsY(); ++binY)
                if (corrMats.at(iHisto)->GetBinContent(binX,binY) < -100)
                    diff->SetBinContent(binX,binY,corrMats.at(iHisto)->GetBinContent(binX,binY));
                else if (useRelativeMetric)
                    diff->SetBinContent(binX,binY,relativeMetric(corrMats.at(iHisto)->GetBinContent(binX,binY),corrMats.at(iHisto+1)->GetBinContent(binX,binY)));
                else
                    diff->SetBinContent(binX,binY,corrMats.at(iHisto)->GetBinContent(binX,binY) - corrMats.at(iHisto+1)->GetBinContent(binX,binY));
        
        //diff->Add(corrMats.at(iHisto+1),-1);
        //for (int binX = 1; binX <= diff->GetNbinsX(); ++binX)
        //    for (int binY = 1; binY <= diff->GetNbinsY(); ++binY)
        //        if (corrMats.at(iHisto)->GetBinContent(binX,binY) < -100)
        //            diff->SetBinContent(binX,binY,corrMats.at(iHisto)->GetBinContent(binX,binY));

        // Now store
        corrMatDiffs.push_back(diff);
        diffProviders.push_back(std::make_pair(providers.at(iHisto),providers.at(iHisto+1)));
    }

    // Now make the initial plots
    for (size_t iHisto = 0; iHisto < corrMats.size(); ++iHisto)
    {
        TH2D* histo = corrMats.at(iHisto);
        histo->GetZaxis()->SetTitle("correlation");
        FormatHisto(histo);
        histo->Draw("colz");
        DrawLabels(histo,fixedValue1,fixedValue2,providers.at(iHisto));

        if (outFile != "NONE" && !outFile.EndsWith(".eps"))
            canvas->Print(outFile);
        else if (outFile != "NONE")
            canvas->Print(TString(outFile).ReplaceAll(".eps",Form("-%d.eps",++iEPS)));

        if (outHistFile)
        {
            outHistFile->cd();
            histo->Write(histo->GetName());
            canvas->cd();
        }
    }

    // Now make the difference plots
    for (size_t iHisto = 0; iHisto < corrMatDiffs.size(); ++iHisto)
    {
        TH2D* histo = corrMatDiffs.at(iHisto);

        // Determine the maximum difference
        // Matrix is not necessarily symmetric (only symmetric if fixed1 == fixed2)
        // Be careful to ignore kinematic limits (set outside scale on purpose)
        double meanDiff = 0;
        double maxDiff = 0;
        int maxX = -1;
        int maxY = -1;
        unsigned long long numValidBins = 0;
        for (int binX = 1; binX <= histo->GetNbinsX(); ++binX)
            for (int binY = 1; binY <= histo->GetNbinsY(); ++binY)
            {
                // Check kinematic limits
                if (histo->GetBinContent(binX,binY) < -100)
                    continue;
                
                // Adjust the mean and max
                meanDiff += histo->GetBinContent(binX,binY);
                numValidBins++;
                if (fabs(histo->GetBinContent(binX,binY)) > fabs(maxDiff))
                {
                    maxDiff = histo->GetBinContent(binX,binY);
                    maxX = binX;
                    maxY = binY;
                }
            }
        //meanDiff /= histo->GetNbinsX()*histo->GetNbinsY();
        meanDiff /= numValidBins;
        
        // Set the range to the maximum (rounded up to nearest multiple of 5) if requested
        if (fixedRangeDiff < 1.e-3)
        {
            const double maxDiffRounded = static_cast<int>(fabs(maxDiff)*20+1)/20.;
            histo->GetZaxis()->SetRangeUser(-maxDiffRounded,maxDiffRounded);
        }
        else
            histo->GetZaxis()->SetRangeUser(-fixedRangeDiff,fixedRangeDiff);
        
        // Now plot
        histo->GetZaxis()->SetTitle("correlation difference");
        FormatHisto(histo);
        histo->Draw("colz");
        DrawLabels(histo,fixedValue1,fixedValue2,diffProviders.at(iHisto).first,diffProviders.at(iHisto).second,meanDiff,maxDiff,maxX,maxY);
        
        if (outFile != "NONE" && !outFile.EndsWith(".eps"))
            canvas->Print(outFile);
        else if (outFile != "NONE")
            canvas->Print(TString(outFile).ReplaceAll(".eps",Form("-%d.eps",++iEPS)));
        
        if (outHistFile && corrMatDiffs.size() == 1)
        {
            outHistFile->cd();
            histo->Write(histo->GetName());
            canvas->cd();
        }
    }
}

void MakeCorrelationPlots(const TString& outFile,TCanvas* canvas,const std::vector<JetUncertaintiesTool*>& providers,const std::vector< std::pair<double,double> >& fixedEta,const std::vector< std::pair<double,double> >& fixedPt)
{
    const TString release  = providers.at(0)->getRelease();
    const double minPtVal  = release.EndsWith("TLA") ? 75 : 17;
    const double maxPtVal  = testJER ? 1500 : release.EndsWith("TLA") ? 1000 : (release.BeginsWith("2011") || release.BeginsWith("2012")) ? 2500 : 3000;
    const double minEtaVal = 0;
    const double maxEtaVal = release.EndsWith("TLA") ? 2.8 : 4.5;

    // First the pT correlation plots
    canvas->SetLogx(true);
    canvas->SetLogy(true);
    for (size_t iFixed = 0; iFixed < fixedEta.size(); ++iFixed)
    {
        const double etaVal1 = fixedEta.at(iFixed).first;
        const double etaVal2 = fixedEta.at(iFixed).second;

        printf("Processing (eta1,eta2) = (%.2f,%.2f)\n",etaVal1,etaVal2);
        
        // Make the initial histograms
        std::vector<TH2D*> corrPt;
        for (size_t iProv = 0; iProv < providers.size(); ++iProv)
        {
            corrPt.push_back(providers.at(iProv)->getPtCorrelationMatrix(100,minPtVal,maxPtVal,etaVal1,etaVal2));
            corrPt.back()->GetXaxis()->SetTitle("#it{p}_{T}^{jet} [GeV]");
            corrPt.back()->GetYaxis()->SetTitle(corrPt.back()->GetXaxis()->GetTitle());
            
            // Switch to 1-corr if Sasha's requested metric
            if (useRelativeMetric)
                for (int binX = 1; binX <= corrPt.back()->GetNbinsX(); ++binX)
                    for (int binY = 1; binY <= corrPt.back()->GetNbinsY(); ++binY)
                        if (corrPt.back()->GetBinContent(binX,binY) > -100)
                            corrPt.back()->SetBinContent(binX,binY,1-corrPt.back()->GetBinContent(binX,binY));
        }
        
        // Now plot them and the difference histograms
        PlotCorrelationHistos(outFile,canvas,providers,corrPt,etaVal1,etaVal2);

    }
    
    // Now the eta correlation plots
    canvas->SetLogx(false);
    canvas->SetLogy(false);
    for (size_t iFixed = 0; iFixed < fixedPt.size(); ++iFixed)
    {
        const double ptVal1 = fixedPt.at(iFixed).first;
        const double ptVal2 = fixedPt.at(iFixed).second;
        
        printf("Processing (pT1,pT2) = (%.0f,%.0f)\n",ptVal1,ptVal2);

        // Make the initial histograms
        std::vector<TH2D*> corrEta;
        for (size_t iProv = 0; iProv < providers.size(); ++iProv)
        {
            corrEta.push_back(providers.at(iProv)->getEtaCorrelationMatrix(round(10*(maxEtaVal-minEtaVal)),minEtaVal,maxEtaVal,ptVal1,ptVal2));
            corrEta.back()->GetXaxis()->SetTitle("#eta^{jet}");
            corrEta.back()->GetYaxis()->SetTitle(corrEta.back()->GetXaxis()->GetTitle());
            
            // Switch to 1-corr if Sasha's requested metric
            if (useRelativeMetric)
                for (int binX = 1; binX <= corrEta.back()->GetNbinsX(); ++binX)
                    for (int binY = 1; binY <= corrEta.back()->GetNbinsY(); ++binY)
                        if (corrEta.back()->GetBinContent(binX,binY) > -100)
                            corrEta.back()->SetBinContent(binX,binY,1-corrEta.back()->GetBinContent(binX,binY));
        }

        // Now plot them and the difference histograms
        PlotCorrelationHistos(outFile,canvas,providers,corrEta,ptVal1,ptVal2);
    }
}



int main (int argc, char* argv[])
{
    if (argc != 7 && argc != 8 && argc != 9)
    {
        printf("Expected arguments:\n");
        printf("\t1. Output file (ps, pdf, eps)\n");
        printf("\t2. Jet definition(s), semicolon delimited\n");
        printf("\t3. MC type\n");
        printf("\t4. Config file(s), semicolon delimited\n");
        printf("\t\tDifferences will be performed by left minus right\n");
        printf("\t\tExample: A;B;C --> A,B,C,A-B,B-C\n");
        printf("\t5. Fixed eta values to consider for pT correlations, semicolon delimited\n");
        printf("\t   Note: Comma-delimited pairs if specifying both eta1 and eta2\n");
        printf("\t6. Fixed pT values to consider for eta correlations, semicolon delimited\n");
        printf("\t   Note: Comma-delimited pairs if specifying both pt1 and pt2\n");
        printf("\t   Note: Expected units are GeV, not MeV\n");
        printf("\t7. Output histogram root file (OPTIONAL)\n");
        printf("\t8. Whether to switch to relative differences (OPTIONAL, default \"false\")\n");
        exit(1);
    }
    TString outFile = argv[1];
    std::vector<TString> jetDefs = jet::utils::vectorize<TString>(argv[2],";");
    TString mcType  = argv[3];
    std::vector<TString> configs   = jet::utils::vectorize<TString>(argv[4],";");
    std::vector<TString> fixedEtaS = jet::utils::vectorize<TString>(argv[5],";");
    std::vector<TString> fixedPtS  = jet::utils::vectorize<TString>(argv[6],";");
    if (argc > 7)
        outHistFile = TFile::Open(argv[7],"RECREATE");
    if (argc > 8)
        useRelativeMetric = jet::utils::getTypeObjFromString<bool>(std::string(argv[8]));

    StatusCode::enableFailure();

    // Ensure we have to do something
    if (!fixedEtaS.size() && !fixedPtS.size())
    {
        printf("No fixed pT or eta values were specified, nothing to do.\n");
        exit(2);
    }

    // Convert fixed value singlets or pairs to pairs
    std::vector< std::pair<double,double> > fixedEta;
    std::vector< std::pair<double,double> > fixedPt;
    if (makeGrid)
    {
        for (size_t iEta1 = 0; iEta1 < fixedEtaS.size(); ++iEta1)
            for (size_t iEta2 = 0; iEta2 < fixedEtaS.size(); ++iEta2)
                fixedEta.push_back(std::make_pair(jet::utils::getTypeObjFromString<double>(fixedEtaS.at(iEta1)),jet::utils::getTypeObjFromString<double>(fixedEtaS.at(iEta2))));
    }
    else
    {
        for (size_t iEta = 0; iEta < fixedEtaS.size(); ++iEta)
        {
            std::vector<double> temp = jet::utils::vectorize<double>(fixedEtaS.at(iEta),",");
            if (temp.size() == 1 && !makeGrid)
                fixedEta.push_back(std::make_pair(temp.at(0),temp.at(0)));
            else if (temp.size() == 2)
                fixedEta.push_back(std::make_pair(temp.at(0),temp.at(1)));
            else
            {
                printf("Specified a fixed eta term which is not 1 or 2 values: %s\n",fixedEtaS.at(iEta).Data());
                printf("Unsure of how to interpret this term, exiting.\n");
                exit(3);
            }
        }
    }

    if (makeGrid)
    {
        for (size_t iPt1 = 0; iPt1 < fixedPtS.size(); ++iPt1)
            for (size_t iPt2 = 0; iPt2 < fixedPtS.size(); ++iPt2)
                fixedPt.push_back(std::make_pair(jet::utils::getTypeObjFromString<double>(fixedPtS.at(iPt1)),jet::utils::getTypeObjFromString<double>(fixedPtS.at(iPt2))));
    }
    else
    {
        for (size_t iPt = 0; iPt < fixedPtS.size(); ++iPt)
        {
            std::vector<double> temp = jet::utils::vectorize<double>(fixedPtS.at(iPt),",");
            if (temp.size() == 1)
                fixedPt.push_back(std::make_pair(temp.at(0),temp.at(0)));
            else if (temp.size() == 2)
                fixedPt.push_back(std::make_pair(temp.at(0),temp.at(1)));
            else
            {
                printf("Specified a fixed pt term which is not 1 or 2 values: %s\n",fixedPtS.at(iPt).Data());
                printf("Unsure of how to interpret this term, exiting.\n");
                exit(4);
            }
        }
    }
    
    // Create the canvas
    SetAtlasStyle();
    gStyle->SetPalette(1);
    TCanvas* canvas = new TCanvas("canvas");
    canvas->SetMargin(0.12,0.15,minRepStyle?0.13:0.15,minRepStyle?0.18:0.10);
    canvas->SetFillStyle(4000);
    canvas->SetFillColor(0);
    canvas->SetFrameBorderMode(0);
    canvas->cd();
    
    // If this is not an eps, start the output
    if (outFile != "NONE" && !outFile.EndsWith(".eps"))
        canvas->Print(outFile+"[");
    
    
    // Run once per jet type
    for (size_t iJetDef = 0; iJetDef < jetDefs.size(); ++iJetDef)
    {
        const TString jetDef = jetDefs.at(iJetDef);

        // Create the providers
        std::vector<JetUncertaintiesTool*> providers;
        for (size_t iConfig = 0; iConfig < configs.size(); ++iConfig)
        {
            // Make a new provider
            providers.push_back(new JetUncertaintiesTool(Form("%s_%zu",jetDef.Data(),iConfig)));
            
            //Set properties
            if (providers.back()->setProperty("JetDefinition",jetDef.Data()).isFailure())
            {
                printf("Failed to set JetDefinition to %s\n",jetDef.Data());
                exit(5);
            }
            if (providers.back()->setProperty("MCType",mcType.Data()).isFailure())
            {
                printf("Failed to set MCType to %s\n",mcType.Data());
                exit(6);
            }
            if (providers.back()->setProperty("ConfigFile",configs.at(iConfig).Data()).isFailure())
            {
                printf("Failed to set ConfigFile to %s\n",configs.at(iConfig).Data());
                exit(7);
            }

            //// Check if we want to change topology from unknown to dijet
            //if (IsDijet())
            //{
            //    const TString dijetAnalysisFile = "../testingMacros/random_stuff/MJESForInclusiveJets_MC11b.root";
            //    if (providers.back()->setProperty("AnalysisFile",dijetAnalysisFile.Data()).isFailure())
            //    {
            //        printf("Failed to set AnalysisFile to %s\n",dijetAnalysisFile.Data());
            //        exit(7);
            //    }
            //}

            // Set to use GeV
            if (providers.back()->setScaleToGeV().isFailure())
            {
                printf("Failed to set tool scale to GeV for config: %s\n",configs.at(iConfig).Data());
                exit(8);
            }

            // Done setting properties, initialize the tool
            if (providers.back()->initialize().isFailure())
            {
                printf("Failed to initialize tool for config: %s\n",configs.at(iConfig).Data());
                exit(9);
            }
        }

        // Make the plots
        MakeCorrelationPlots(outFile,canvas,providers,fixedEta,fixedPt);

        // Clean up
        for (size_t iProv = 0; iProv < providers.size(); ++iProv)
            delete providers.at(iProv);
        providers.clear();
    }

    // If this is not an eps, end the output
    if (outFile != "NONE" && !outFile.EndsWith(".eps"))
        canvas->Print(outFile+"]");
    
    // If an output histogram file exists, close it
    if (outHistFile)
    {
        outHistFile->Close();
        outHistFile = NULL;
    }

    return 0;
}

