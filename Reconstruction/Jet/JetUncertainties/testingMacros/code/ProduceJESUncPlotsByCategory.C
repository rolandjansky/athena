/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/testarea/17.2.0.4.1/Reconstruction/Jet/JetUncertainties/StandAlone/
//g++ -o ProduceJESUncPlotsByCategory.exe ProduceJESUncPlotsByCategory.C `$ROOTSYS/bin/root-config --cflags --glibs` -I../.. -L../../StandAlone -lJetUncertainties


#include "JetUncertainties/JESUncertaintyProvider.h"
#include "JetUncertainties/JESUtils.h"


#include "TGraphErrors.h"

int main (int argc, char* argv[])
{
  TString configFile = "JES_2011/Final/InsituJES2011_AllNuisanceParametersWithCategories.config";
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
  
  VecD etaBins = JESUtils::VectorizeD("0.1,0.5,1.0,1.7,2.45,3.0,3.4,4.0",",");
  VecD ptBins  = JESUtils::GetDefaultPtBins();

  for (unsigned int iJetType = 0; iJetType < jetTypes.size(); iJetType++)
  {
    for (unsigned int iMCType = 0; iMCType < mcTypes.size(); iMCType++)
    {
      JESUncertaintyProvider* jesProv = new JESUncertaintyProvider(configFile,jetTypes[iJetType],mcTypes[iMCType],shareDir);
      jesProv->useGeV();
      
      TFile* outFile = TFile::Open("JESPlots_"+jetTypes[iJetType]+"_"+mcTypes[iMCType]+".root","RECREATE");
      
      //for (int i = 0; i < numComps; i++)
        //printf("%s: %s\n",compNames[i].Data(),compCats[i].Data());
      std::vector<compCategory> categories = jesProv->getListOfCategories();
      for (unsigned int i = 0; i < categories.size(); i++)
      {
        TString categoryName = jesProv->getCategoryStringFromEnum(categories[i]);
        StrV compNamesInCategory = jesProv->getComponentNamesInCategory(categories[i]);
        // First do each component
        for (unsigned int j = 0; j < compNamesInCategory.size(); j++)
        {
          //printf("%s: %s\n",categoryName.Data(),compNamesInCategory[j].Data());
          TString compName = compNamesInCategory[j];
          
          for (unsigned int etaBin = 0; etaBin < etaBins.size(); etaBin++)
          {
            double eta = etaBins[etaBin];
            TString etaLabel = Form("%.1f",eta);
            TGraphErrors* gUnc = new TGraphErrors();
            gUnc->SetName("uncertainty_"+compName+"_"+etaLabel.Data());
            gUnc->SetTitle(compName+" ("+categoryName+") #eta="+etaLabel.Data());
            
            for (unsigned int ptbin = 0; ptbin < ptBins.size()-1; ptbin++)
            {
              double unc = jesProv->getRelUncertComponent(compName,ptBins[ptbin],eta);
              gUnc->SetPoint(ptbin,ptBins[ptbin],unc);
            }
            outFile->cd();
            gUnc->Write();
          }
        }
        // Now do the totals for each category
        for (unsigned int etaBin = 0; etaBin < etaBins.size(); etaBin++)
        {
          double eta = etaBins[etaBin];
          TString etaLabel = Form("%.1f",eta);
          
          TGraphErrors* gUncTotalInCat = new TGraphErrors();
          gUncTotalInCat->SetName("TotalUncertaintyInCategory_"+categoryName+"_#eta="+etaLabel.Data());
          gUncTotalInCat->SetTitle("Total Uncertainty in the "+categoryName+" Category at #eta="+etaLabel.Data());
          
          for (unsigned int ptbin = 0; ptbin < ptBins.size()-1; ptbin++)
          {
            double totalUnc = 0;
            for (unsigned int j = 0; j < compNamesInCategory.size(); j++)
              totalUnc += pow(jesProv->getRelUncertComponent(compNamesInCategory[j],ptBins[ptbin],eta),2);
            gUncTotalInCat->SetPoint(ptbin,ptBins[ptbin],sqrt(totalUnc));
          }
          outFile->cd();
          gUncTotalInCat->Write();
        }
      }
      // Now ensure that the total uncertainty that the provider calculated is the same as the by-component organized by categories sum
      for (unsigned int etaBin = 0; etaBin < etaBins.size(); etaBin++)
      {
        double eta = etaBins[etaBin];
        TString etaLabel = Form("%.1f",eta);

        TGraphErrors* totalUncByCategories = new TGraphErrors();
        totalUncByCategories->SetName("TotalJESUncFromComponents_#eta="+etaLabel);
        totalUncByCategories->SetTitle("Total JES Uncertainty, Calculated by Components, at #eta="+etaLabel);

        TGraphErrors* totalUncByProvider = new TGraphErrors();
        totalUncByProvider->SetName("TotalJESUncFromProvider_#eta="+etaLabel);
        totalUncByProvider->SetTitle("Total JES Uncertainty, Calculated by the Provider, at #eta="+etaLabel);
        
        TGraphErrors* totalUncDifference = new TGraphErrors();
        totalUncDifference->SetName("TotalJESUncDifference_#eta="+etaLabel);
        totalUncDifference->SetTitle("Total JES Difference, Components - Provider, at #eta="+etaLabel);

        for (unsigned int ptbin = 0; ptbin < ptBins.size()-1; ptbin++)
        {
          double totalUnc = 0;
          for (unsigned int i = 0; i < categories.size(); i++)
          {
            StrV compNamesInCategory = jesProv->getComponentNamesInCategory(categories[i]);
            for (unsigned int j = 0; j < compNamesInCategory.size(); j++)
              totalUnc += pow(jesProv->getRelUncertComponent(compNamesInCategory[j],ptBins[ptbin],eta),2);
          }
          totalUncByCategories->SetPoint(ptbin,ptBins[ptbin],sqrt(totalUnc));
          totalUncByProvider->SetPoint(ptbin,ptBins[ptbin],jesProv->getRelUncert(ptBins[ptbin],eta));
          totalUncDifference->SetPoint(ptbin,ptBins[ptbin],sqrt(totalUnc) - jesProv->getRelUncert(ptBins[ptbin],eta));
        }
        outFile->cd();
        totalUncByCategories->Write();
        totalUncByProvider->Write();
        totalUncDifference->Write();
      }


      outFile->Close();
    }
  }


  return 0;
}
