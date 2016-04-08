/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <TTree.h>
#include <TFile.h>
#include <TEventList.h>
#include "Riostream.h"
#include <TH1D.h>
#include <TString.h>
#include <vector>
#include <iostream>

using namespace std;

void makeHisto(TString title, double value)
{
  TH1D* newHisto=new TH1D(title,title,1,0,1);
  newHisto->Fill(0.5,value);
  newHisto->Write();
}


void getAllValues(TString fileName,TString jetCollection,int flavourType)
{

  TString flavourString;

  if (flavourType==5)
  {
    flavourString=TString("bottom");
  }
  if (flavourType==4)
  {
    flavourString=TString("charm");
  }
  if (flavourType==1)
  {
    flavourString=TString("light");
  }

  TString identificationFile("../trainingResultsLikelihood/coefficientsHistos_");
  identificationFile+=jetCollection;
  identificationFile+="_";
  identificationFile+=flavourString;
  identificationFile+=".root";

  TString identificationFile2("../trainingResultsLikelihood/coefficientsFile_");
  identificationFile2+=jetCollection;
  identificationFile2+="_";
  identificationFile2+=flavourString;
  identificationFile2+=".txt";

  TString flavourCondition="cat_flavour==";
  flavourCondition+=flavourType;

  ofstream cronology(identificationFile2,ios_base::out|ios_base::app);

  TFile file(fileName);
  TTree* myTTree=(TTree*)file.Get("SVTree");

  TFile* output=new TFile(identificationFile,"recreate");

  TH1::AddDirectory(kFALSE);

  myTTree->Draw(">>myList",flavourCondition+"&&nTracksAtVtx>-998.");
  TEventList* myEventList=(TEventList*)gDirectory->Get("myList");
  int allnumber=myTTree->GetEntries();
  cronology << flavourString <<  "_"<<jetCollection<<"_coef = " << myEventList->GetN()  << " L(0 - 1e+08)" <<  endl;
  makeHisto(flavourString+"_coef",(double)myEventList->GetN());

  myTTree->Draw(">>myList2",flavourCondition+"&&nTracksAtVtx>-998.&&nVTX==1");
  TEventList* myEventList2=(TEventList*)gDirectory->Get("myList2");
  cronology << flavourString <<  "_"<<jetCollection<<"_e_1VTX = " << (double)myEventList2->GetN()/(double)myEventList->GetN() << " L(0 - 1) " << endl;
  makeHisto(flavourString+"_e_1VTX",(double)myEventList2->GetN()/(double)myEventList->GetN());

  myTTree->Draw(">>myList3",flavourCondition+"&&nTracksAtVtx>-998.&&nVTX==1&&nTracksAtVtx==2");
  TEventList* myEventList3=(TEventList*)gDirectory->Get("myList3");
  cronology << flavourString <<  "_"<<jetCollection<<"_e_1VTX_2TrackVTX = " << (double)myEventList3->GetN()/(double)myEventList2->GetN() << " L(0 - 1) " << endl;
  makeHisto(flavourString+"_e_1VTX_2TrackVTX",(double)myEventList3->GetN()/(double)myEventList2->GetN());

  myTTree->Draw(">>myList4",flavourCondition+"&&nTracksAtVtx>-998.&&nVTX==1&&nTracksAtVtx==3");
  TEventList* myEventList4=(TEventList*)gDirectory->Get("myList4");
  cronology << flavourString <<  "_"<<jetCollection<<"_e_1VTX_3TrackVTX = " << (double)myEventList4->GetN()/(double)myEventList2->GetN() << " L(0 - 1) " << endl;
  makeHisto(flavourString+"_e_1VTX_3TrackVTX",(double)myEventList4->GetN()/(double)myEventList2->GetN());

  myTTree->Draw(">>myList5",flavourCondition+"&&nTracksAtVtx>-998.&&nVTX==1&&nTracksAtVtx==4");
  TEventList* myEventList5=(TEventList*)gDirectory->Get("myList5");
  cronology << flavourString <<  "_"<<jetCollection<<"_e_1VTX_4TrackVTX = " << (double)myEventList5->GetN()/(double)myEventList2->GetN() << " L(0 - 1) " << endl;
  makeHisto(flavourString+"_e_1VTX_4TrackVTX",(double)myEventList5->GetN()/(double)myEventList2->GetN());

  myTTree->Draw(">>myList6",flavourCondition+"&&nTracksAtVtx>-998.&&nVTX==1&&(nTracksAtVtx==5||nTracksAtVtx==6)");
  TEventList* myEventList6=(TEventList*)gDirectory->Get("myList6");
  cronology << flavourString <<  "_"<<jetCollection<<"_e_1VTX_5or6TrackVTX = " << (double)myEventList6->GetN()/(double)myEventList2->GetN() << " L(0 - 1) " << endl;
  makeHisto(flavourString+"_e_1VTX_5or6TrackVTX",(double)myEventList6->GetN()/(double)myEventList2->GetN());

  myTTree->Draw(">>myList7",flavourCondition+"&&nTracksAtVtx>-998.&&nVTX==1&&nSingleTracks==1");
  TEventList* myEventList7=(TEventList*)gDirectory->Get("myList7");
  cronology << flavourString <<  "_"<<jetCollection<<"_e_1Vtx_1SingleTrack = " << (double)myEventList7->GetN()/(double)myEventList2->GetN() << " L(0 - 1) " << endl;
  makeHisto(flavourString+"_e_1Vtx_1SingleTrack",(double)myEventList7->GetN()/(double)myEventList2->GetN());

  myTTree->Draw(">>myList8",flavourCondition+"&&nTracksAtVtx>-998.&&nVTX==1&&nSingleTracks==0");
  TEventList* myEventList8=(TEventList*)gDirectory->Get("myList8");
  cronology << flavourString <<  "_"<<jetCollection<<"_e_1Vtx_NoSingleTrack = " << (double)myEventList8->GetN()/(double)myEventList2->GetN() << " L(0 - 1) " << endl;
  makeHisto(flavourString+"_e_1Vtx_NoSingleTrack",(double)myEventList8->GetN()/(double)myEventList2->GetN());

  myTTree->Draw(">>myList11",flavourCondition+"&&nTracksAtVtx>-998.&&nVTX>0&&nTracksAtVtx>3");
  TEventList* myEventList11=(TEventList*)gDirectory->Get("myList11");
  //  cronology << flavourString <<  "_"<<jetCollection<<"_e_1VTX " << (double)myEventList2->GetN()/(double)myEventList->GetN() << " L(0 - 1) " << endl;

  myTTree->Draw(">>myList28",flavourCondition+"&&nTracksAtVtx>-998.&&nVTX>0&&nTracksAtVtx==2&&energyFraction<.9999");
  TEventList* myEventList28=(TEventList*)gDirectory->Get("myList28");
  cronology << flavourString <<  "_"<<jetCollection<<"_e_1orMoreVTX_2TrackVTX_energyFraction_normal = " << (double)myEventList28->GetN()/(double)myEventList3->GetN() << " L(0 - 1) " << endl;
  makeHisto(flavourString+"_e_1orMoreVTX_2TrackVTX_energyFraction_normal",(double)myEventList28->GetN()/(double)myEventList3->GetN());

  myTTree->Draw(">>myList9",flavourCondition+"&&nTracksAtVtx>-998.&&nVTX>0&&nTracksAtVtx==3&&energyFraction<.9999");
  TEventList* myEventList9=(TEventList*)gDirectory->Get("myList9");
  cronology << flavourString <<  "_"<<jetCollection<<"_e_1orMoreVTX_3TrackVTX_energyFraction_normal = " << (double)myEventList9->GetN()/(double)myEventList4->GetN() << " L(0 - 1) " << endl;
  makeHisto(flavourString+"_e_1orMoreVTX_3TrackVTX_energyFraction_normal",(double)myEventList9->GetN()/(double)myEventList4->GetN());

  myTTree->Draw(">>myList10",flavourCondition+"&&nTracksAtVtx>-998.&&nVTX>0&&nTracksAtVtx>3&&energyFraction<.9999");
  TEventList* myEventList10=(TEventList*)gDirectory->Get("myList10");
  cronology << flavourString <<  "_"<<jetCollection<<"_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal = " << (double)myEventList10->GetN()/(double)myEventList11->GetN() << " L(0 - 1) " << endl;
  makeHisto(flavourString+"_e_1orMoreVTX_4orMoreTrackVTX_energyFraction_normal" ,(double)myEventList10->GetN()/(double)myEventList11->GetN());

  myTTree->Draw(">>myList12",flavourCondition+"&&nTracksAtVtx>-998.&&nVTX>1");
  TEventList* myEventList12=(TEventList*)gDirectory->Get("myList12");

  myTTree->Draw(">>myList13",flavourCondition+"&&nTracksAtVtx>-998.&&nVTX>1&&nTracksAtVtx==4");
  TEventList* myEventList13=(TEventList*)gDirectory->Get("myList13");
  cronology << flavourString <<  "_"<<jetCollection<<"_e_2orMoreVTX_4TrackVTX = " << (double)myEventList13->GetN()/(double)myEventList12->GetN() << " L(0 - 1) " << endl;
  makeHisto(flavourString+"_e_2orMoreVTX_4TrackVTX",(double)myEventList13->GetN()/(double)myEventList12->GetN());


  myTTree->Draw(">>myList14",flavourCondition+"&&nTracksAtVtx>-998.&&nVTX==0");
  TEventList* myEventList14=(TEventList*)gDirectory->Get("myList14");
  cronology << flavourString <<  "_"<<jetCollection<<"_e_NoVTX = " << (double)myEventList14->GetN()/(double)myEventList->GetN() << " L(0 - 1) " << endl;
  makeHisto(flavourString+"_e_NoVTX",(double)myEventList14->GetN()/(double)myEventList->GetN());

  myTTree->Draw(">>myList15",flavourCondition+"&&nTracksAtVtx>-998.&&nVTX==0&&nSingleTracks==1");
  TEventList* myEventList15=(TEventList*)gDirectory->Get("myList15");
  cronology << flavourString <<  "_"<<jetCollection<<"_e_NoVtx_1SingleTrack = " << (double)myEventList15->GetN()/(double)myEventList14->GetN() << " L(0 - 1) " << endl;
  makeHisto(flavourString+"_e_NoVtx_1SingleTrack",(double)myEventList15->GetN()/(double)myEventList14->GetN());

  myTTree->Draw(">>myList16",flavourCondition+"&&nTracksAtVtx>-998.&&nVTX==0&&nSingleTracks==2");
  TEventList* myEventList16=(TEventList*)gDirectory->Get("myList16");
  cronology << flavourString <<  "_"<<jetCollection<<"_e_NoVtx_2SingleTrack = " << (double)myEventList16->GetN()/(double)myEventList14->GetN() << " L(0 - 1) " << endl;
  makeHisto(flavourString+"_e_NoVtx_2SingleTrack",(double)myEventList16->GetN()/(double)myEventList14->GetN());

  myTTree->Draw(">>myList17",flavourCondition+"&&nTracksAtVtx>-998.&&nVTX==0&&nSingleTracks>1");
  TEventList* myEventList17=(TEventList*)gDirectory->Get("myList17");
  //  cronology << flavourString <<  "_"<<jetCollection<<"_e_NoVtx_3SingleTrack " << (double)myEventList17->GetN()/(double)myEventList14->GetN() << " L(0 - 1) " << endl;
  myTTree->Draw(">>myList18",flavourCondition+"&&nTracksAtVtx>-998.&&nVTX==0&&nSingleTracks==0");
  TEventList* myEventList18=(TEventList*)gDirectory->Get("myList18");
  cronology << flavourString <<  "_"<<jetCollection<<"_e_NoVtx_NoSingleTrack = " << (double)myEventList18->GetN()/(double)myEventList14->GetN() << " L(0 - 1) " << endl;
  makeHisto(flavourString+"_e_NoVtx_NoSingleTrack",(double)myEventList18->GetN()/(double)myEventList14->GetN());

  myTTree->Draw(">>myList19",flavourCondition+"&&nTracksAtVtx>-998.&&nVTX==0&&nSingleTracks==1&&energyFraction<.9999");
  TEventList* myEventList19=(TEventList*)gDirectory->Get("myList19");
  cronology << flavourString <<  "_"<<jetCollection<<"_e_NoVtx_1SingleTrack_energyFraction_normal = " << (double)myEventList19->GetN()/(double)myEventList15->GetN() << " L(0 - 1) " << endl;
  makeHisto(flavourString+"_e_NoVtx_1SingleTrack_energyFraction_normal", (double)myEventList19->GetN()/(double)myEventList15->GetN());

  myTTree->Draw(">>myList20",flavourCondition+"&&nTracksAtVtx>-998.&&nVTX==0&&nSingleTracks>1&&energyFraction<.9999");
  TEventList* myEventList20=(TEventList*)gDirectory->Get("myList20");
  cronology << flavourString <<  "_"<<jetCollection<<"_e_NoVtx_2orMoreSingleTrack_energyFraction_normal = " << (double)myEventList20->GetN()/(double)myEventList17->GetN() << " L(0 - 1) " << endl;
  makeHisto(flavourString+"_e_NoVtx_2orMoreSingleTrack_energyFraction_normal",(double)myEventList20->GetN()/(double)myEventList17->GetN());
  
  output->Write();
  output->Close();

  cronology.close();

}

void mergeFiles(TString jetCollection)
{

  std::vector<int> allFlavours;
  allFlavours.push_back(1);
  allFlavours.push_back(4);
  allFlavours.push_back(5);
  
  std::vector<int>::const_iterator begin=allFlavours.begin();
  std::vector<int>::const_iterator end=allFlavours.end();
  
  TString identificationFile("../trainingResultsLikelihood/coefficientsHistos_");
  identificationFile+=jetCollection;
  identificationFile+=".root";

  TFile* outputFile=new TFile(identificationFile,"recreate");

  for (std::vector<int>::const_iterator iter=begin;iter!=end;++iter)
  {

    TString flavourString;
    
    int flavourType=*iter;

    if (flavourType==5)
    {
      flavourString=TString("bottom");
    }
    if (flavourType==4)
    {
      flavourString=TString("charm");
    }
    if (flavourType==1)
    {
      flavourString=TString("light");
    }

    TString identificationFile("../trainingResultsLikelihood/coefficientsHistos_");
    identificationFile+=jetCollection;
    identificationFile+="_";
    identificationFile+=flavourString;
    identificationFile+=".root";
    
    TFile openFile(identificationFile);
    
    outputFile->cd();
    
    TList* list=openFile.GetListOfKeys();
    TIterator* firstIter=list->MakeIterator();


    do {
    
      TObject* inList=firstIter->Next();
      
      if (inList==0)
      {
        break;
      }
      
      std::cout << " Name: " << inList->ClassName() << std::endl;

      if ( openFile.Get(inList->GetName())->ClassName()!=TString("TH1D") ){
        continue;
      }

      TH1D* histo=(TH1D*) openFile.Get(inList->GetName());

      histo->Write();
      
   } while(true);
  } 
  
  outputFile->Write();
  outputFile->Close();
  
}


