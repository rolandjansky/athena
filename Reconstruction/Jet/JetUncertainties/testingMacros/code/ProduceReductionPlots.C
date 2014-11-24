/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/testarea/17.2.0.4.1/Reconstruction/Jet/JetUncertainties/StandAlone/
//g++ -o ProduceReductionPlots.exe ProduceReductionPlots.C `$ROOTSYS/bin/root-config --cflags --glibs` -I../.. -L../../StandAlone -lJetUncertainties

#include "JetUncertainties/JESUncertaintyProvider.h"
#include "JetUncertainties/JESUtils.h"

#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TLegend.h"
#include "TLatex.h"

#include "../atlasstyle/AtlasStyle.C"
#include "../atlasstyle/AtlasLabels.C"


int MakeGlobalReductionPlots(TString configFile,StrV jetTypes,StrV mcTypes,TString shareDir,VecD etaBins,VecD ptBins,TLatex* tex, char* ATLASlabel)
{
  for (unsigned int iJetType = 0; iJetType < jetTypes.size(); iJetType++)
  {
    for (unsigned int iMCType = 0; iMCType < mcTypes.size(); iMCType++)
    {
      JESUncertaintyProvider* jesProv = new JESUncertaintyProvider(configFile,jetTypes[iJetType],mcTypes[iMCType],shareDir);
      jesProv->useGeV();
      
      TString outFileName = "GlobalReductionPlots_"+jetTypes[iJetType]+"_"+mcTypes[iMCType]+".ps";
      TCanvas* canvas = new TCanvas("canvas_global");
      canvas->SetFillStyle(4000);
      canvas->SetFillColor(0);
      canvas->SetFrameBorderMode(0);
      canvas->SetLogx();
      canvas->cd();
      canvas->Print(outFileName+"[");

      for (unsigned int etaBin = 0; etaBin < etaBins.size(); etaBin++)
      {
        double eta = etaBins[etaBin];
        TString etaLabel = Form("%.1f",eta);

        // Have the components, now create the plots with all effective components

        TH1F* frame = new TH1F("frame_global","",ptBins.size()-1,&ptBins[0]);
        frame->SetTitleSize(0.06,"xy");
        frame->SetLabelSize(0.05,"xy");
        frame->SetTitleOffset(1.10,"xy");
        frame->SetXTitle(Form("#it{p}_{T}^{jet} [GeV]"));
        //frame->SetYTitle("Relative uncertainty");
        frame->SetYTitle("Fractional JES uncertainty");
        frame->GetXaxis()->SetMoreLogLabels();
        frame->GetXaxis()->SetRangeUser(17,1e3);
        frame->GetYaxis()->SetRangeUser(-0.035,0.035);
        // Fill histogram with -1 so line is drawn outside of frame
        for (unsigned int ptBin = 1; ptBin < ptBins.size(); ptBin++)
          frame->SetBinContent(ptBin,-1);
        frame->Draw("");

        int numComponents = jesProv->getNUncertaintyComponents();
        VecD totalUncerts;
        totalUncerts.reserve(ptBins.size()-1);
        for (unsigned int i = 0; i < ptBins.size()-1; i++)
          totalUncerts[i] = 0.;
        for (unsigned int iComp = 0; iComp < numComponents; iComp++)
        {
          TString compName = jesProv->getComponentNames().at(iComp);
          // Want all components of name "EffectiveNP_*"
          if (! compName.Contains("EffectiveNP_",TString::kIgnoreCase) )
            continue;
          TGraphErrors* compGraph = new TGraphErrors();
          compGraph->SetName(compName+"_"+etaLabel.Data());
          compGraph->SetTitle(compName+" #eta="+etaLabel.Data());
          
          for (unsigned int ptBin = 0; ptBin < ptBins.size() - 1; ptBin++)
          {
            double unc = jesProv->getRelUncertComponent(compName,ptBins[ptBin],eta);
            compGraph->SetPoint(ptBin,ptBins[ptBin],unc);
            totalUncerts[ptBin] += unc*unc;
          }
          if (iComp+2 < 5)
            compGraph->SetLineColor(iComp+2);
          else
            compGraph->SetLineColor(iComp+3);
            
          // Only works for reductions of <=9 final effective parameters
          if (compName.Contains("restTerm",TString::kIgnoreCase))
            compGraph->SetLineStyle(2);
          else
            compGraph->SetLineStyle(1);
          compGraph->SetLineWidth(2);
          compGraph->Draw("L");
        }
        if (!strcmp(ATLASlabel,"Preliminary"))
          ATLASLabel(0.65,0.89,ATLASlabel,kBlack);
        else if (!strcmp(ATLASlabel,"Public"))
          ATLASLabel(0.70,0.89,"",kBlack);
        else
          ATLASLabel(0.70,0.89,ATLASlabel,kBlack);
        tex->DrawLatex(0.70,0.83,Form("#sqrt{s} = 7 TeV, |#eta| = %s",etaLabel.Data()));
        tex->DrawLatex(0.70,0.76,"#int L dt = 4.7 fb^{-1}");
        tex->DrawLatex(0.20,0.86,Form("anti-#it{k}_{t} R=0.%d, %s", jetTypes[iJetType].Contains("4") ? 4 : 6, jetTypes[iJetType].Contains("EM") ? "EM+JES" : "LCW+JES"));
        tex->DrawLatex(0.20,0.80,"Data 2011");

        
        TGraphErrors* totalGraph = new TGraphErrors();
        totalGraph->SetName(Form("TotalEffNP_",etaLabel.Data()));
        totalGraph->SetTitle(Form("Total of EffNPs #eta=%s",etaLabel.Data()));
        for (unsigned int ptBin = 0; ptBin < ptBins.size() - 1; ptBin++)
          totalGraph->SetPoint(ptBin,ptBins[ptBin],sqrt(totalUncerts[ptBin]));
        totalGraph->SetLineStyle(3);
        totalGraph->SetLineWidth(2);
        totalGraph->Draw("L");

        canvas->Print(outFileName);

      }
      canvas->Print(outFileName+"]");
    }
  }
}

int MakeCategoryReductionPlots(TString configFile,StrV jetTypes,StrV mcTypes,TString shareDir,VecD etaBins,VecD ptBins,TLatex* tex, char* ATLASlabel)
{
  const bool onlyEffectiveParams = true;
  
  for (unsigned int iJetType = 0; iJetType < jetTypes.size(); iJetType++)
  {
    for (unsigned int iMCType = 0; iMCType < mcTypes.size(); iMCType++)
    {
      JESUncertaintyProvider* jesProv = new JESUncertaintyProvider(configFile,jetTypes[iJetType],mcTypes[iMCType],shareDir);
      jesProv->useGeV();
      
      TString outFileName = "CategoryReductionPlots_"+jetTypes[iJetType]+"_"+mcTypes[iMCType]+".ps";

      // First do each individual category plot
      std::vector<compCategory> categories = jesProv->getListOfCategories();
      for (unsigned int iCat = 0; iCat < categories.size(); iCat++)
      {
        TString categoryName = jesProv->getCategoryStringFromEnum(categories[iCat]);
        StrV compNamesInCategory;
        if (onlyEffectiveParams)
        {
          StrV temp = jesProv->getComponentNamesInCategory(categories[iCat]);
          for (unsigned int iComp = 0; iComp < temp.size(); iComp++)
            if (temp[iComp].Contains("EffectiveNP"))
              compNamesInCategory.push_back(temp[iComp]);
        }
        else
          compNamesInCategory = jesProv->getComponentNamesInCategory(categories[iCat]);
        
        TCanvas* canvas = new TCanvas("canvas_"+categoryName);
        canvas->SetFillStyle(4000);
        canvas->SetFillColor(0);
        canvas->SetFrameBorderMode(0);
        canvas->SetLogx();
        canvas->cd();
        canvas->Print(categoryName+outFileName+"[");
        
        for (unsigned int etaBin = 0; etaBin < etaBins.size(); etaBin++)
        {
          double eta = etaBins[etaBin];
          TString etaLabel = Form("%.1f",eta);
          
          // Have all of the components, now create the plot

          TH1F* frame = new TH1F("frame_"+categoryName,"",ptBins.size()-1,&ptBins[0]);
          frame->SetTitleSize(0.06,"xy");
          frame->SetLabelSize(0.05,"xy");
          frame->SetTitleOffset(1.10,"xy");
          frame->SetXTitle(Form("#it{p}_{T}^{jet} [GeV]"));
          //frame->SetYTitle("Relative uncertainty");
          frame->SetYTitle("Fractional JES uncertainty");
          frame->GetXaxis()->SetMoreLogLabels();
          frame->GetXaxis()->SetRangeUser(17,1e3);
          if (!strcmp(ATLASlabel,"Preliminary") || !strcmp(ATLASlabel,"Public"))
            frame->GetYaxis()->SetRangeUser(-0.01,0.03);
          else
            frame->GetYaxis()->SetRangeUser(-0.035,0.035);
          // Fill histogram with -1 so line is drawn outside of frame
          for (unsigned int ptBin = 1; ptBin < ptBins.size(); ptBin++)
            frame->SetBinContent(ptBin,-1);
          frame->Draw("");

          for (unsigned int iCompInCat = 0; iCompInCat < compNamesInCategory.size(); iCompInCat++)
          {
            TString compName = compNamesInCategory[iCompInCat];
            TGraphErrors* compGraph = new TGraphErrors();
            compGraph->SetName(compName+"_"+etaLabel.Data());
            compGraph->SetTitle(compName+" #eta="+etaLabel.Data());

            for (unsigned int ptBin = 0; ptBin < ptBins.size() - 1; ptBin++)
            {
              double unc = jesProv->getRelUncertComponent(compName,ptBins[ptBin],eta);
              compGraph->SetPoint(ptBin,ptBins[ptBin],unc);
            }
            if (iCompInCat+2 < 5)
              compGraph->SetLineColor(iCompInCat+2);
            else
              compGraph->SetLineColor(iCompInCat+3);
            // Only works for reductions of <=9 final effective parameters
            if (compName.Contains(Form("%d",compNamesInCategory.size())))
              compGraph->SetLineStyle(2);
            else
              compGraph->SetLineStyle(1);
            compGraph->SetLineWidth(2);
            compGraph->Draw("L");
          }
          if (!strcmp(ATLASlabel,"Preliminary"))
            ATLASLabel(0.65,0.89,ATLASlabel,kBlack);
          else if (!strcmp(ATLASlabel,"Public"))
            ATLASLabel(0.70,0.89,"",kBlack);
          else
            ATLASLabel(0.70,0.89,ATLASlabel,kBlack);
          tex->DrawLatex(0.70,0.83,Form("#sqrt{s} = 7 TeV, |#eta| = %s",etaLabel.Data()));
          tex->DrawLatex(0.70,0.76,"#int L dt = 4.7 fb^{-1}");
          tex->DrawLatex(0.20,0.89,Form("Anti-#it{k}_{t} #it{R}=0.%d, %s", jetTypes[iJetType].Contains("4") ? 4 : 6, jetTypes[iJetType].Contains("EM") ? "EM+JES" : "LCW+JES"));
          tex->DrawLatex(0.20,0.84,"Data 2011");
          //tex->DrawLatex(0.20,0.79,(categoryName + " components").Data());
          TString lowerCatName = categoryName;
          lowerCatName.ToLower();
          tex->DrawLatex(0.20,0.965,Form("Effective nuisance parameters for the %s category",lowerCatName.Data()));

          canvas->Print(categoryName+outFileName);
        }
        canvas->Print(categoryName+outFileName+"]");
      }
      
      // Now do the plot showing the total for each category
      TCanvas* canvas = new TCanvas("canvas_TotalPerCategory");
      canvas->SetFillStyle(4000);
      canvas->SetFillColor(0);
      canvas->SetFrameBorderMode(0);
      canvas->SetLogx();
      canvas->cd();
      canvas->Print("Total"+outFileName+"[");
      
      for (unsigned int etaBin = 0; etaBin < etaBins.size(); etaBin++)
      {
        double eta = etaBins[etaBin];
        TString etaLabel = Form("%.1f",eta);

        TH1F* frame = new TH1F("frame_TotalPerCategory","",ptBins.size()-1,&ptBins[0]);
        frame->SetTitleSize(0.06,"xy");
        frame->SetLabelSize(0.05,"xy");
        frame->SetTitleOffset(1.10,"xy");
        frame->SetXTitle(Form("#it{p}_{T}^{jet} [GeV]"));
        //frame->SetYTitle("Relative uncertainty");
        frame->SetYTitle("Fractional JES uncertainty");
        frame->GetXaxis()->SetMoreLogLabels();
        frame->GetXaxis()->SetRangeUser(17,1e3);
        frame->GetYaxis()->SetRangeUser(0,0.035);
        // Fill histogram with -1 so line is drawn outside of frame
        for (unsigned int ptBin = 1; ptBin < ptBins.size(); ptBin++)
          frame->SetBinContent(ptBin,-1);
        frame->Draw("");
        
        // Create a legend for this plot
        TLegend* legend = new TLegend(0.75,0.50,0.90,0.70);
        legend->SetNColumns(1);
        legend->SetFillColor(0);
        legend->SetBorderSize(1);
        legend->SetShadowColor(0);


        // Create a vector to store the total uncertainties
        VecD totalUncerts;
        totalUncerts.reserve(ptBins.size()-1);
        for (unsigned int i = 0; i < ptBins.size()-1; i++)
          totalUncerts[i] = 0.;
        
        for (unsigned int iCat = 0; iCat < categories.size(); iCat++)
        {
          //if (categories[iCat] == cat_special)
            //continue;
          TString categoryName = jesProv->getCategoryStringFromEnum(categories[iCat]);
          StrV compNamesInCategory = jesProv->getComponentNamesInCategory(categories[iCat]);

          VecD totalUncertsInCategory;
          totalUncertsInCategory.reserve(ptBins.size()-1);
          for (unsigned int i = 0; i < ptBins.size()-1; i++)
            totalUncertsInCategory[i] = 0.;
          
            
          for (unsigned int iCompInCat = 0; iCompInCat < compNamesInCategory.size(); iCompInCat++)
          {
            TString compName = compNamesInCategory[iCompInCat];
            for (unsigned int ptBin = 0; ptBin < ptBins.size() - 1; ptBin++)
            {
              double unc = jesProv->getRelUncertComponent(compName,ptBins[ptBin],eta);
              totalUncertsInCategory[ptBin] += unc*unc;
              totalUncerts[ptBin] += unc*unc;
            }
          }
          TGraphErrors* catGraph = new TGraphErrors();
          catGraph->SetName(categoryName+"_"+etaLabel.Data());
          catGraph->SetTitle(categoryName+" #eta="+etaLabel.Data());
          catGraph->SetLineStyle(1);
          catGraph->SetLineWidth(2);
          if (iCat+2 < 5)
            catGraph->SetLineColor(iCat+2);
          else
            catGraph->SetLineColor(iCat+3);
          for (unsigned int ptBin = 0; ptBin < ptBins.size() - 1; ptBin++)
            catGraph->SetPoint(ptBin,ptBins[ptBin],sqrt(totalUncertsInCategory[ptBin]));
          catGraph->Draw("L");
          
          legend->AddEntry(catGraph,categoryName,"L");
        }
        if (!strcmp(ATLASlabel,"Preliminary"))
          ATLASLabel(0.65,0.89,ATLASlabel,kBlack);
        else if (!strcmp(ATLASlabel,"Public"))
          ATLASLabel(0.70,0.89,"",kBlack);
        else
          ATLASLabel(0.70,0.89,ATLASlabel,kBlack);
        tex->DrawLatex(0.70,0.83,Form("#sqrt{s} = 7 TeV, |#eta| = %s",etaLabel.Data()));
        tex->DrawLatex(0.70,0.76,"#int L dt = 4.7 fb^{-1}");
        tex->DrawLatex(0.20,0.86,Form("anti-#it{k}_{t} R=0.%d, %s", jetTypes[iJetType].Contains("4") ? 4 : 6, jetTypes[iJetType].Contains("EM") ? "EM+JES" : "LCW+JES"));
        tex->DrawLatex(0.20,0.80,"Data 2011");


        TGraphErrors* totalGraph = new TGraphErrors();
        totalGraph->SetName(Form("TotalEffNP_",etaLabel.Data()));
        totalGraph->SetTitle(Form("Total of EffNPs #eta=%s",etaLabel.Data()));
        for (unsigned int ptBin = 0; ptBin < ptBins.size() - 1; ptBin++)
          totalGraph->SetPoint(ptBin,ptBins[ptBin],sqrt(totalUncerts[ptBin]));
        totalGraph->SetLineStyle(3);
        totalGraph->SetLineWidth(2);
        totalGraph->Draw("L");
        
        legend->AddEntry(totalGraph,"Total Uncertainty","L");
        legend->Draw();
        
        canvas->Print("Total"+outFileName);
      }
      canvas->Print("Total"+outFileName+"]");
      delete canvas;

      // Now do the plot for each component, colour-coded by category
      // Ignore special components for now (just make plot more messy)
      canvas = new TCanvas("canvas_AllComponentsByCategory");
      canvas->SetFillStyle(4000);
      canvas->SetFillColor(0);
      canvas->SetFrameBorderMode(0);
      canvas->SetLogx();
      canvas->cd();
      canvas->Print("AllComponents"+outFileName+"[");
      
      for (unsigned int etaBin = 0; etaBin < etaBins.size(); etaBin++)
      {
        double eta = etaBins[etaBin];
        TString etaLabel = Form("%.1f",eta);

        TH1F* frame = new TH1F("frame_AllComponentsCategory","",ptBins.size()-1,&ptBins[0]);
        frame->SetTitleSize(0.06,"xy");
        frame->SetLabelSize(0.05,"xy");
        frame->SetTitleOffset(1.10,"xy");
        frame->SetXTitle(Form("#it{p}_{T}^{jet} [GeV]"));
        //frame->SetYTitle("Relative uncertainty");
        frame->SetYTitle("Fractional JES uncertainty");
        frame->GetXaxis()->SetMoreLogLabels();
        frame->GetXaxis()->SetRangeUser(17,1e3);
        frame->GetYaxis()->SetRangeUser(0,0.035);
        // Fill histogram with -1 so line is drawn outside of frame
        for (unsigned int ptBin = 1; ptBin < ptBins.size(); ptBin++)
          frame->SetBinContent(ptBin,-1);
        frame->Draw("");
        
        // Create a legend for this plot
        TLegend* legend = new TLegend(0.75,0.50,0.90,0.70);
        legend->SetNColumns(1);
        legend->SetFillColor(0);
        legend->SetBorderSize(1);
        legend->SetShadowColor(0);


        // Create a vector to store the total uncertainties
        VecD totalUncerts;
        totalUncerts.reserve(ptBins.size()-1);
        for (unsigned int i = 0; i < ptBins.size()-1; i++)
          totalUncerts[i] = 0.;
        
        for (unsigned int iCat = 0; iCat < categories.size(); iCat++)
        {
          //if (categories[iCat] == cat_special)
            //continue;
          TString categoryName = jesProv->getCategoryStringFromEnum(categories[iCat]);
          StrV compNamesInCategory = jesProv->getComponentNamesInCategory(categories[iCat]);

          for (unsigned int iCompInCat = 0; iCompInCat < compNamesInCategory.size(); iCompInCat++)
          {
            TString compName = compNamesInCategory[iCompInCat];
            TGraphErrors* compGraph = new TGraphErrors();
            compGraph->SetName(compName+"_"+etaLabel.Data());
            compGraph->SetTitle(compName+" #eta="+etaLabel.Data());
            
            for (unsigned int ptBin = 0; ptBin < ptBins.size() - 1; ptBin++)
            {
              double unc = jesProv->getRelUncertComponent(compName,ptBins[ptBin],eta);
              compGraph->SetPoint(ptBin,ptBins[ptBin],unc);
              totalUncerts[ptBin] += unc*unc;
            }
            if (compName.Contains(Form("%d",compNamesInCategory.size())))
              compGraph->SetLineStyle(2);
            else
              compGraph->SetLineStyle(1);
            compGraph->SetLineWidth(2);
            if (iCat+2 < 5)
              compGraph->SetLineColor(iCat+2);
            else
              compGraph->SetLineColor(iCat+3);
            compGraph->Draw("L");

            if (!iCompInCat)
              legend->AddEntry(compGraph,categoryName,"L");
          }
        }
        if (!strcmp(ATLASlabel,"Preliminary"))
          ATLASLabel(0.65,0.89,ATLASlabel,kBlack);
        else if (!strcmp(ATLASlabel,"Public"))
          ATLASLabel(0.70,0.89,"",kBlack);
        else
          ATLASLabel(0.70,0.89,ATLASlabel,kBlack);
        tex->DrawLatex(0.70,0.83,Form("#sqrt{s} = 7 TeV, |#eta| = %s",etaLabel.Data()));
        tex->DrawLatex(0.70,0.76,"#int L dt = 4.7 fb^{-1}");
        tex->DrawLatex(0.20,0.86,Form("anti-#it{k}_{t} R=0.%d, %s", jetTypes[iJetType].Contains("4") ? 4 : 6, jetTypes[iJetType].Contains("EM") ? "EM+JES" : "LCW+JES"));
        tex->DrawLatex(0.20,0.80,"Data 2011");


        TGraphErrors* totalGraph = new TGraphErrors();
        totalGraph->SetName(Form("TotalEffNP_",etaLabel.Data()));
        totalGraph->SetTitle(Form("Total of EffNPs #eta=%s",etaLabel.Data()));
        for (unsigned int ptBin = 0; ptBin < ptBins.size() - 1; ptBin++)
          totalGraph->SetPoint(ptBin,ptBins[ptBin],sqrt(totalUncerts[ptBin]));
        totalGraph->SetLineStyle(3);
        totalGraph->SetLineWidth(2);
        totalGraph->Draw("L");
        
        legend->AddEntry(totalGraph,"Total Uncertainty","L");
        legend->Draw();
        
        canvas->Print("AllComponents"+outFileName);
      }
      canvas->Print("AllComponents"+outFileName+"]");
    }
  }
}

int main (int argc, char* argv[])
{
  
  SetAtlasStyle();
  gStyle->SetPalette(1);
  TLatex *tex = new TLatex(); tex->SetNDC(); tex->SetTextSize(0.04); tex->SetTextFont(42);


  TString configFile = "JES_2011/Final/InsituJES2011_AllNuisanceParameters.config";
  StrV jetTypes      = JESUtils::Vectorize("AntiKt4TopoEM",",");
  StrV mcTypes       = JESUtils::Vectorize("MC11b",",");
  TString shareDir   = "../../share/";

  if (argc > 4)
  {
    configFile = argv[1];
    jetTypes   = JESUtils::Vectorize(argv[2],",");
    mcTypes    = JESUtils::Vectorize(argv[3],",");
    shareDir   = argv[4];
  }
  VecD etaBins = (argc > 5) ? JESUtils::VectorizeD(argv[5],",") : JESUtils::VectorizeD("0.1,0.5,1.0,1.7,2.45,3.0,3.4,4.0",",");
  VecD ptBins  = JESUtils::GetDefaultPtBins();

  char* ATLASlabel = (argc > 6) ? strdup(argv[6]) : strdup("Internal");
  
  if (configFile.Contains("ByCategory",TString::kIgnoreCase))
    MakeCategoryReductionPlots(configFile,jetTypes,mcTypes,shareDir,etaBins,ptBins,tex,ATLASlabel);
  else
    MakeGlobalReductionPlots(configFile,jetTypes,mcTypes,shareDir,etaBins,ptBins,tex,ATLASlabel);


  return 0;
}
