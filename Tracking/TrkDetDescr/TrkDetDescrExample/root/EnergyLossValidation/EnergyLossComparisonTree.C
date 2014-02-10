/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#define EnergyLossComparisonTree_cxx
#include "EnergyLossComparisonTree.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <iostream>
#include <sstream>
#include <TF1.h>

std::vector<TH1D*>* EnergyLossComparisonTree::histograms(int bins, int mode)
{
   if (fChain == 0) return 0;

   Long64_t nentries = fChain->GetEntriesFast();

   Int_t nbytes = 0, nb = 0;

   std::vector<TH1D*>* histos = new std::vector<TH1D*>;
   std::vector<TH2D*>* twoDHistosTruth = new std::vector<TH2D*>;
   std::vector<TH2D*>* twoDHistosReco = new std::vector<TH2D*>;
   std::vector<TH2D*>* twoDHistosRecoSigma = new std::vector<TH2D*>;

   //std::cout << " [m] All Profile Histograms booked for input type : " << type << std::endl;
   std::cout << "looping over entries " << nentries << std::endl;
   TH2D* tempH2DTruth = 0;
   TH2D* tempH2DReco = 0;
   TH2D* tempH2DRecoSigma = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
   
      for(int i = 0; i < Surfaces; i++){
	//create the name of this histogram
	std::ostringstream nameTruth;
	std::ostringstream nameReco;
	if(UpdateSurfaceType[i] == 0){
	  nameTruth << "True:Eloss:Cylinder:R:";
	  nameTruth << UpdateSurfaceR[i];
	  nameTruth << ":hlen:";
	  nameTruth << UpdateSurfaceZ[i];

	  nameReco << "Reco:Eloss:Cylinder:R:";
	  nameReco << UpdateSurfaceR[i];
	  nameReco << ":hlen:";
	  nameReco << UpdateSurfaceZ[i];
	}else if(UpdateSurfaceType[i] == 1){
	  nameTruth << "True:Eloss:Disk:Zpos:";
	  nameTruth << UpdateSurfaceZ[i];
	  nameTruth << ":Rmax:";
	  nameTruth << UpdateSurfaceR[i];

	  nameReco << "Reco:Eloss:Disk:Zpos:";
	  nameReco << UpdateSurfaceZ[i];
	  nameReco << ":Rmax:";
	  nameReco << UpdateSurfaceR[i];
	}else{
	  break;
	}
	//check if this histogram has already been created
	tempH2DTruth = 0;
	tempH2DReco = 0;
	tempH2DRecoSigma = 0;
	//std::cout << "Finding histo " << std::endl; 
	for(int j = 0; j < twoDHistosTruth->size(); j++){
	  if(std::string(twoDHistosTruth->at(j)->GetName()) == nameTruth.str()){
	    tempH2DTruth = twoDHistosTruth->at(j);
	    tempH2DReco = twoDHistosReco->at(j);
	    tempH2DRecoSigma = twoDHistosRecoSigma->at(j);
	    break;
	  }
	}
	//std::cout << "Found histo " << tempH2DTruth << std::endl; 

	//if histogram does not exist, create it

	if(tempH2DTruth == 0){
	  tempH2DTruth = new TH2D(nameTruth.str().c_str(),nameTruth.str().c_str(),500,0,15000,61,-3.05,3.05);
	  tempH2DReco = new TH2D(nameReco.str().c_str(),nameReco.str().c_str(),500,0,15000,61,-3.05,3.05);
	  nameReco << "Sigma";
	  tempH2DRecoSigma = new TH2D(nameReco.str().c_str(),nameReco.str().c_str(),500,0,2000,61,-3.05,3.05);
	  twoDHistosTruth->push_back(tempH2DTruth);
	  twoDHistosReco->push_back(tempH2DReco);
	  twoDHistosRecoSigma->push_back(tempH2DRecoSigma);
	  tempH2DTruth->Sumw2();
	}
	tempH2DTruth->Fill(OriginalP-TrueNewP[i],OriginalEta);
	tempH2DReco->Fill(OriginalP-ReconNewP[i],OriginalEta);
	tempH2DRecoSigma->Fill(ElossError[i],OriginalEta);
	//std::cout << "HistoEntry " << OriginalP << " " << TrueNewP[i] << " " << ReconNewP[i] << std::endl;
      }
   }
   
   //Now fit Landaus for each eta bin and create the 1D plots
   TH1D* tempHisto = 0;
   double mpv = 0;
   double error = 0;
   //std::cout << "size " << twoDHistosTruth->size() << std::endl;

   for(int i = 0; i < twoDHistosTruth->size(); i++){
     tempH2DTruth = twoDHistosTruth->at(i); 
     tempH2DReco = twoDHistosReco->at(i);
     tempH2DRecoSigma = twoDHistosRecoSigma->at(i);
     m_twoDHistosTruth->push_back(tempH2DTruth);
     m_twoDHistosReco->push_back(tempH2DReco);
     m_twoDHistosRecoSigma->push_back(tempH2DRecoSigma);
     //tempInd = std::string(tempH2DTruth->GetName()).find(':');
     //std::string finalName = std::string("Compare ")+std::string(tempH2DTruth->GetName()).substr(tempInd);
     TH1D* finalHistoTruth = new TH1D((std::string("Compare:")+std::string(tempH2DTruth->GetName())).c_str(),(std::string("Compare:")+std::string(tempH2DTruth->GetName())).c_str(),61,-3.05,3.05);
     TH1D* finalHistoReco = new TH1D((std::string("Compare:")+std::string(tempH2DReco->GetName())).c_str(),(std::string("Compare:")+std::string(tempH2DReco->GetName())).c_str(),61,-3.05,3.05);
     int result = 0;
     for(int j = 1; j <= 61; j++){
       //double* bins = new double[1000];
       tempHisto = tempH2DTruth->ProjectionX("px",j,j);
       int entries = 0;
       for(int k = 1; k<=tempHisto->GetNbinsX(); k++) entries+=tempHisto->GetBinContent(k);
       if(entries != 0){
	 /*int tempPos = 1;
	 bins[0] = 0;
	 int tempEntries = 0;
	 for(int k = 1; k<=tempHisto->GetNbinsX(); k++){
	   tempEntries += tempHisto->GetBinContent(k);
	   if(tempEntries > tempPos*entries/1000){
	     bins[tempPos] = tempHisto->GetBinLowEdge(k+1);
	     //std::cout << tempPos << " " << bins[tempPos] << std::endl;
	     tempPos++;	     
	   }
	 }
	 TH1D* tempHist2 = (TH1D*) tempHisto->Rebin(999,"px2",bins);
	 delete tempHisto;
	 tempHisto = tempHist2;*/
	 //int tempEntries = 0;
	 //for(int k = 1; k<=tempHisto->GetNbinsX(); k++){
	 //tempEntries += tempHisto->GetBinContent(k);
	 //if(tempEntries >= 0.9*entries) tempHistoSetBinContent(k) = 0;
	 //}
	 result = tempHisto->Fit("landau","LI");
	 double chi2 = tempHisto->GetFunction("landau")->GetChisquare()/tempHisto->GetFunction("landau")->GetNDF();
	 //std::cout << "res " << result << " " << tempHisto->GetFunction("landau")->GetChisquare()/tempHisto->GetFunction("landau")->GetNDF() << std::endl;
	 if(tempHisto->GetFunction("landau")->GetParameter(1) < 0 
	    || result != 0 || chi2 > 5){
	   result = tempHisto->Fit("landau");
	 }
	 //std::cout << "res2 " << result << " " << tempHisto->GetFunction("landau")->GetChisquare()/tempHisto->GetFunction("landau")->GetNDF() << std::endl;

	 chi2 = tempHisto->GetFunction("landau")->GetChisquare()/tempHisto->GetFunction("landau")->GetNDF();
	 if(tempHisto->GetFunction("landau")->GetParameter(1) < 0 
	    || result != 0 || chi2 > 5){
	   result = tempHisto->Fit("landau","I");
	 }
	 //std::cout << "res3 " << result << " " << tempHisto->GetFunction("landau")->GetChisquare()/tempHisto->GetFunction("landau")->GetNDF() << std::endl;
	 if(result != 0){
	   result = tempHisto->Fit(&TF1(*tempHisto->GetFunction("landau")),"M");
	 }
	 chi2 = tempHisto->GetFunction("landau")->GetChisquare()/tempHisto->GetFunction("landau")->GetNDF();

	 //std::cout << "res4 " << result << " " << tempHisto->GetFunction("landau")->GetChisquare()/tempHisto->GetFunction("landau")->GetNDF() << std::endl;
	   /*	 if(tempHisto->GetFunction("landau")->GetParameter(1) < 0 
	    || result != 0){
	   result = tempHisto->Fit("landau","LMI");
	   }*/
	 /*if(j == 30){
	   TCanvas c;
	   tempHisto->Draw("e1p");
	   c.SaveAs("test.ps");
	   while(true) { }
	   }*/
	 if(mode == 0){
	   mpv = tempHisto->GetFunction("landau")->GetParameter(1);
	   error = tempHisto->GetFunction("landau")->GetParError(1);
	 }else if(mode == 1){
	   mpv = tempHisto->GetFunction("landau")->GetParameter(2);
	   error = tempHisto->GetFunction("landau")->GetParError(2);
	 }
	 
	 if(tempHisto->GetFunction("landau")->GetParameter(1) < 0 
	    || mpv/2 < error || result != 0 || chi2 > 5){
	   if(mode == 0){
	     mpv = tempHisto->GetBinCenter(tempHisto->GetMaximumBin());
	     error = tempHisto->GetBinWidth(tempHisto->GetMaximumBin());
	   }else if(mode == 1){
	     mpv = tempHisto->GetRMS()/4.04;
	     error = 0;
	   }
	 }
       }else{
	 mpv = 0;
	 error = 0;
       }
       finalHistoTruth->SetBinContent(j,mpv);
       finalHistoTruth->SetBinError(j,error);
       delete tempHisto;
       tempHisto = tempH2DReco->ProjectionX("px",j,j);
       finalHistoReco->SetBinContent(j,tempHisto->GetMean());
       finalHistoReco->SetBinError(j,tempHisto->GetRMS());
       delete tempHisto;
     } 
     histos->push_back(finalHistoTruth);
     histos->push_back(finalHistoReco);
   }
   std::cout << "done with profile hists " << std::endl;
   return histos;

}

EnergyLossComparisonTree::EnergyLossComparisonTree(TTree *tree)
{

   m_twoDHistosTruth = new std::vector<TH2D*>();
   m_twoDHistosReco = new std::vector<TH2D*>();
   m_twoDHistosRecoSigma = new std::vector<TH2D*>();
   if (tree == 0) {
     std::cout << " No tree specified ... expect a segfault " << std::endl;
   }
   Init(tree);
}

EnergyLossComparisonTree::~EnergyLossComparisonTree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t EnergyLossComparisonTree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t EnergyLossComparisonTree::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->IsA() > TChain::Class()) return centry;
   TChain *chain = (TChain*)fChain;
   if (chain->GetTreeNumber() > fCurrent) {
      fCurrent = chain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void EnergyLossComparisonTree::Init(TTree *tree)
{

   // Set branch addresses
   if (tree == 0) return;
   fChain = tree;
   fCurrent = -1;
   //fChain->SetMakeClass(1);

   //std::cout << " setting branch addresses " << std::endl;
   fChain->SetBranchAddress("Surfaces",&Surfaces);
   fChain->SetBranchAddress("UpdateSurfaceType",&UpdateSurfaceType);
   fChain->SetBranchAddress("UpdateSurfaceR",&UpdateSurfaceR);
   fChain->SetBranchAddress("UpdateSurfaceZ",&UpdateSurfaceZ);

   fChain->SetBranchAddress("UpdatePositionX",&TruePositionX);
   fChain->SetBranchAddress("UpdatePositionY",&TruePositionY);
   fChain->SetBranchAddress("UpdatePositionZ",&TruePositionZ);
   fChain->SetBranchAddress("UpdatePositionR",&TruePositionR);

   fChain->SetBranchAddress("ReconPositionX",&ReconPositionX);
   fChain->SetBranchAddress("ReconPositionY",&ReconPositionY);
   fChain->SetBranchAddress("ReconPositionZ",&ReconPositionZ);
   fChain->SetBranchAddress("ReconPositionR",&ReconPositionR);

   fChain->SetBranchAddress("OriginalMomentum",&OriginalP);
   fChain->SetBranchAddress("TrueNewMomentum",&TrueNewP);
   fChain->SetBranchAddress("ReconNewMomentum",&ReconNewP);

   fChain->SetBranchAddress("ElossError",&ElossError);

   fChain->SetBranchAddress("OriginalEta",&OriginalEta);
   fChain->SetBranchAddress("TrueNewEta",&TrueNewEta);
   fChain->SetBranchAddress("ReconNewEta",&ReconNewEta);

   //std::cout << " branch addresses set " << std::endl;

   Notify();
}

Bool_t EnergyLossComparisonTree::Notify()
{

   // Get branch pointers
   b_Surfaces = fChain->GetBranch("Surfaces");
   b_UpdateSurfaceType = fChain->GetBranch("UpdateSurfaceType");
   b_UpdateSurfaceR = fChain->GetBranch("UpdateSurfaceR");
   b_UpdateSurfaceZ = fChain->GetBranch("UpdateSurfaceZ");

   b_TruePositionX = fChain->GetBranch("UpdatePositionX");
   b_TruePositionY = fChain->GetBranch("UpdatePositionY");
   b_TruePositionZ = fChain->GetBranch("UpdatePositionZ");
   b_TruePositionR = fChain->GetBranch("UpdatePositionR");

   b_ReconPositionX = fChain->GetBranch("ReconPositionX");
   b_ReconPositionY = fChain->GetBranch("ReconPositionY");
   b_ReconPositionZ = fChain->GetBranch("ReconPositionZ");
   b_ReconPositionR = fChain->GetBranch("ReconPositionR");

   b_ElossError = fChain->GetBranch("ElossError");

   b_OriginalP = fChain->GetBranch("OriginalMomentum");
   b_TrueNewP = fChain->GetBranch("TrueNewMomentum");
   b_ReconNewP = fChain->GetBranch("ReconNewMomentum");

   b_OriginalEta = fChain->GetBranch("OriginalEta");
   b_TrueNewEta = fChain->GetBranch("TrueNewEta");
   b_ReconNewEta = fChain->GetBranch("ReconNewEta");

   return kTRUE;
}

void EnergyLossComparisonTree::Show(Long64_t entry)
{  if (!fChain) return;
   fChain->Show(entry);}


Int_t EnergyLossComparisonTree::Cut(Long64_t entry)
{return 1;}
   
TH2D* EnergyLossComparisonTree::combinedTrueHistos()
{
  TH2D* result = new TH2D("CombinedTruth","CombinedTruth",500,0,15000,61,-3.05,3.05);
  for(int i = 0; i < m_twoDHistosTruth->size(); i++) result->Add(m_twoDHistosTruth->at(i));
  return result;
}

std::vector<TH1D*>* EnergyLossComparisonTree::doTrueFits(TH2D* true2D)
{
  TH1D* finalHistoTruth = new TH1D("True","True",61,-3.05,3.05);
  TH1D* finalHistoTruthSigma = new TH1D("TrueSigma","TrueSigma",61,-3.05,3.05);
  TH1D* tempHisto = 0;

  for(int j = 1; j <= 61; j++){
    tempHisto = true2D->ProjectionX("px",j,j);
    int entr = 0;
    double mpv, error, sigma, errorsigma;
    for(int k = 1; k<=tempHisto->GetNbinsX(); k++) entr+=tempHisto->GetBinContent(k);
    if(entr != 0){
      double res = tempHisto->Fit("landau","LI");
      double chi2 = tempHisto->GetFunction("landau")->GetChisquare()/tempHisto->GetFunction("landau")->GetNDF();
      
      if(tempHisto->GetFunction("landau")->GetParameter(1) < 0 
	 || res != 0 || chi2 > 5){
	res = tempHisto->Fit("landau");
      }
      
      chi2 = tempHisto->GetFunction("landau")->GetChisquare()/tempHisto->GetFunction("landau")->GetNDF();
      if(tempHisto->GetFunction("landau")->GetParameter(1) < 0 
	 || res != 0 || chi2 > 5){
	res = tempHisto->Fit("landau","I");
      }
      if(res != 0){
	res = tempHisto->Fit(&TF1(*tempHisto->GetFunction("landau")),"M");
      }
      chi2 = tempHisto->GetFunction("landau")->GetChisquare()/tempHisto->GetFunction("landau")->GetNDF();
      
      mpv = tempHisto->GetFunction("landau")->GetParameter(1);
      error = tempHisto->GetFunction("landau")->GetParError(1);
      
      sigma = tempHisto->GetFunction("landau")->GetParameter(2);
      errorsigma = tempHisto->GetFunction("landau")->GetParError(2);
      
      if(tempHisto->GetFunction("landau")->GetParameter(1) < 0 
	 //|| mpv/2 < error || result != 0 || chi2 > 5
	 ){
	mpv = tempHisto->GetBinCenter(tempHisto->GetMaximumBin());
	error = 3*tempHisto->GetBinWidth(tempHisto->GetMaximumBin());
	sigma = tempHisto->GetRMS()/80;
	errorsigma = sigma/2;
      }
      //tempHisto->Draw();
      finalHistoTruth->SetBinContent(j,mpv);
      finalHistoTruth->SetBinError(j,error);
      finalHistoTruthSigma->SetBinContent(j,sigma);
      finalHistoTruthSigma->SetBinError(j,errorsigma);
      delete tempHisto;
    } 
  }

  std::vector<TH1D*>* result = new std::vector<TH1D*>();
  result->push_back(finalHistoTruth);
  result->push_back(finalHistoTruthSigma);
  return result;
}

TH2D* EnergyLossComparisonTree::combinedRecoHistoMPV()
{
  TH2D* result = new TH2D("CombinedRecoMPV","CombinedRecoMPV",500,0,15000,61,-3.05,3.05);
  for(int i = 0; i < m_twoDHistosReco->size(); i++) result->Add(m_twoDHistosReco->at(i));
  return result;
}

TH2D* EnergyLossComparisonTree::combinedRecoHistoSigma()
{
  TH2D* result = new TH2D("CombinedRecoSigma","CombinedRecoSigma",500,0,2000,61,-3.05,3.05);
  for(int i = 0; i < m_twoDHistosRecoSigma->size(); i++) result->Add(m_twoDHistosRecoSigma->at(i));
  return result;
}

TH1D* EnergyLossComparisonTree::doRecoFits(TH2D* reco2D, int l)
{

  TH1D* finalHistoReco = 0; 
  if(l == 0) finalHistoReco = new TH1D("Reco","Reco",61,-3.05,3.05);
  else finalHistoReco = new TH1D("RecoSigma","RecoSigma",61,-3.05,3.05);

  TH1D* tempHisto = 0;

  for(int j = 1; j <= 61; j++){
    double min = 0;
    double max = 0;
    tempHisto = reco2D->ProjectionX("px",j,j);
  
    for(int k = 500; k>=2; k--){
      if (tempHisto->GetBinContent(k) != 0){
	max = tempHisto->GetBinCenter(k)+tempHisto->GetBinWidth(k)/2;
	break;
      }
    }
    for(int k = 2; k<=500; k++){
      if (tempHisto->GetBinContent(k) != 0){
	min = tempHisto->GetBinCenter(k)-tempHisto->GetBinWidth(k)/2;
	if(min > max/3) break;
      }
    }
    double mpv = (max+min)/2;
    double error = (max-min)/2;
    //tempHisto->Draw();
    finalHistoReco->SetBinContent(j,mpv);
    finalHistoReco->SetBinError(j,error);
    delete tempHisto;
  }
  return finalHistoReco;
}

