/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "../preparedatasets/collectionsToProcess.h"
#include <TFile.h>
#include <TH1.h>
#include "../jetnetRoot/TNetworkToHistoTool.h"
#include "../jetnetRoot/TTrainedNetwork.h"
#include <vector>

using namespace std;

void prepareCalibrationFileForCOOL(bool withLikelihood=false,
                                   TString calibrationFileName="../finalCalibFile/BTaggingCalibV2_JetFitter",
                                   TString mainDirectoryName="../trainingResultsJetNet",
                                   TString tagToUse="NeuralNetwork",
                                   TString tagToUseForLikelihood="RooFitLikelihood",
                                   TString likelihoodPDFDir="../trainingResultsLikelihoodHistos",
                                   TString likelihoodCoefDir="../trainingResultsLikelihood")
{


  TNetworkToHistoTool networkToHisto;

  vector<TString> collectionsToProcess=getCollectionsToProcess();
  
  vector<TString>::const_iterator collectionsToProcessBegin=collectionsToProcess.begin();
  vector<TString>::const_iterator collectionsToProcessEnd=collectionsToProcess.end();
  
  for (vector<TString>::const_iterator collectionsToProcessIter=collectionsToProcessBegin;
       collectionsToProcessIter!=collectionsToProcessEnd;
       ++collectionsToProcessIter) 
  {


    
    TString calibrationFileToWrite(calibrationFileName);
    calibrationFileToWrite+="_";
    calibrationFileToWrite+=*collectionsToProcessIter;
    calibrationFileToWrite+=".root";

    cout << " Writing calibration file: " << calibrationFileToWrite << endl;
    TFile* calibrationFile=new TFile(calibrationFileToWrite,"recreate");
  
    calibrationFile->cd("/");

    TString calibrationFileToTake=*collectionsToProcessIter;
//    if (calibrationFileToTake=="Cone7H1TowerParticleJets")
//    {
//      calibrationFileToTake="Cone7H1TopoParticleJets";
//    }
    if (calibrationFileToTake=="Cone7H1TowerJets")
    {
      continue;
    }
    
    calibrationFile->cd("/");
    gDirectory->mkdir(tagToUse);
    gDirectory->cd(tagToUse);

    if (withLikelihood)
    {
      gDirectory->cd("/");
      gDirectory->mkdir(tagToUseForLikelihood);
    }


    TString histoDirBase=mainDirectoryName+"/"+calibrationFileToTake+"/";

    //now first work on combined with IP3
    gDirectory->cd("/"+tagToUse);

    gDirectory->mkdir("comb");
    gDirectory->cd("comb");

    TString inputRootFileName=histoDirBase+"comb/weights/weightMinimum.root";

    cout << " Considering " << inputRootFileName << endl;

    TFile* actualFile=new TFile(inputRootFileName);
    TTrainedNetwork* myNetwork=(TTrainedNetwork*)actualFile->Get("TTrainedNetwork");
    
    cout << " Hidden Layers: " << myNetwork->getnHidden() << endl;
    std::vector<TH1*> histoVector=networkToHisto.fromTrainedNetworkToHisto(myNetwork);

    calibrationFile->cd("/"+tagToUse+"/comb/");

    std::vector<TH1*>::const_iterator histoBegin=histoVector.begin();
    std::vector<TH1*>::const_iterator histoEnd=histoVector.end();
    
    for (std::vector<TH1*>::const_iterator histoIter=histoBegin;histoIter!=histoEnd;
         ++histoIter)
    {

      cout << " Iterator pointer: " << *histoIter << endl;
      if ((*histoIter)->GetName()!="TObject")
      {
        cout << "--> writing out  histogram: " << (*histoIter)->GetName() << endl;
        (*histoIter)->Write();
      }
      
    }


    //now first work on combined with IP3D
    gDirectory->cd("/"+tagToUse);
    gDirectory->mkdir("standalone");
    gDirectory->cd("standalone");

    inputRootFileName=histoDirBase+"standalone/weights/weightMinimum.root";

    cout << " Considering " << inputRootFileName << endl;

    TFile* actualFile2=new TFile(inputRootFileName);
    myNetwork=(TTrainedNetwork*)actualFile2->Get("TTrainedNetwork");
    histoVector=networkToHisto.fromTrainedNetworkToHisto(myNetwork);

    calibrationFile->cd("/"+tagToUse+"/standalone/");

    histoBegin=histoVector.begin();
    histoEnd=histoVector.end();
    
    for (std::vector<TH1*>::const_iterator histoIter=histoBegin;histoIter!=histoEnd;
         ++histoIter)
    {
     if ((*histoIter)->GetName()!="TObject")
     {
       cout << "--> writing out  histogram: " << (*histoIter)->GetName() << endl;
       (*histoIter)->Write();
     }
    }

//likelihoodPDFDir
//likelihoodCoefDir

    std::vector<TString> flavours;
    flavours.push_back("bottom");
    flavours.push_back("charm");
    flavours.push_back("light");
    

    if (withLikelihood)
    {
      TString histoDirBasePDFs=likelihoodPDFDir+"/"+calibrationFileToTake+"/trainingResultsLikelihood/smoothedhistos_"+calibrationFileToTake+"_";
//      gDirectory->cd("/"+tagToUseForLikelihood);
        
      for (std::vector<TString>::const_iterator iterFlavour=flavours.begin();iterFlavour!=flavours.end();++iterFlavour)
      {

        TString inputLikelihoodHistoFileName=histoDirBasePDFs+*iterFlavour+".root";
      
        cout << " Considering for likelihood " << inputLikelihoodHistoFileName << endl;

        TFile* actualHistoFile=new TFile(inputLikelihoodHistoFileName);
      
        TList* myHistoList=gDirectory->GetListOfKeys();
      
        TIterator* myHistoListIterator=myHistoList->MakeIterator();
      
        do
        {
          actualHistoFile->cd("/");
          TObject* next=myHistoListIterator->Next();
          if (next==0) break;
          if (gDirectory->Get(next->GetName())->ClassName()==TString("TH1F"))
          {
            TH1F* myHisto=(TH1F*)gDirectory->Get(next->GetName());
            cout << " Writing out histogram: " << next->GetName() << endl;
            calibrationFile->cd("/"+tagToUseForLikelihood);
            myHisto->Write();
          } else 
          {
            cout << " Cannot cast to a TH1F " << endl;
          }
        }
        while (true);

        actualHistoFile->Close();
        delete actualHistoFile;
        actualHistoFile=0;

      }//end flavour loop

      
      TString histoDirBaseCoef=likelihoodCoefDir+"/coefficientsHistos_"+calibrationFileToTake;
      gDirectory->cd("/"+tagToUseForLikelihood);
        
      TString inputLikelihoodCoefFileName=histoDirBaseCoef+".root";
      
      cout << " Considering for likelihood " << inputLikelihoodCoefFileName << endl;

      TFile* actualCoefFile=new TFile(inputLikelihoodCoefFileName);
      
      TList* myCoefList=gDirectory->GetListOfKeys();
      
      TIterator* myCoefListIterator=myCoefList->MakeIterator();
      
      do
      {
        actualCoefFile->cd("/");
        TObject* next=myCoefListIterator->Next();
        if (next==0) break;
        if (gDirectory->Get(next->GetName())->ClassName()==TString("TH1D"))
        {
          TH1D* myHisto=(TH1D*)gDirectory->Get(next->GetName());
          cout << " Writing out histogram: " << next->GetName() << endl;
          calibrationFile->cd("/"+tagToUseForLikelihood);
          myHisto->Write();
        } else 
        {
          cout << " Cannot cast to a TH1D " << endl;
        }
      }
      while (true);

      
      actualCoefFile->Close();
      delete actualCoefFile;
      actualCoefFile=0;
    }
    


    //now close collection file
    calibrationFile->Write();
    calibrationFile->Close();
    delete calibrationFile;
    calibrationFile=0;

  }//end collections

}
