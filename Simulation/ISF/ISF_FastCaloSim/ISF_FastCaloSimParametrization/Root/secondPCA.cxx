/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TMatrixD.h"
#include "TVectorD.h"
#include "TH1D.h"
#include "TFile.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TApplication.h"
#include "TTree.h"
#include "TSystem.h"
#include "TH2D.h"
#include "TPrincipal.h"
#include "TMath.h"
#include "TBrowser.h"
#include "ISF_FastCaloSimParametrization/secondPCA.h"
#include "ISF_FastCaloSimParametrization/firstPCA.h"
#include "ISF_FastCaloSimParametrization/TreeReader.h"

#include <iostream>

using namespace std;

secondPCA::secondPCA()
{
 
}

void secondPCA::run(string inputfile, string label, int nbins1, int nbins2, int numberfinebins)
{
 
 //Open inputfile:
 cout<<endl;
 cout<<"****************"<<endl;
 cout<<"     2nd PCA"<<endl;
 cout<<"****************"<<endl;
 cout<<endl;
 cout<<"--- Open input file "<<inputfile<<endl;
 TFile* file=TFile::Open(inputfile.c_str());
 if(!file) cout<<"ERROR: Inputfile could not be opened"<<endl;
 
 cout<<"--- Init the TreeReader"<<endl;
 TTree* InputTree = (TTree*)file->Get("tree_1stPCA");
 TreeReader* read_inputTree = new TreeReader();
 read_inputTree->SetTree(InputTree);
 
 int nbins;
 vector<int> layerNr=getLayerBins(file, nbins);
 cout<<"Bins: "<<nbins<<endl;
 
 vector<string> layer;
 for(unsigned int l=0;l<layerNr.size();l++)
  layer.push_back(Form("layer%i",layerNr[l]));
 layer.push_back("totalE");
 
 for(unsigned int l=0;l<layer.size();l++)
  cout<<layer[l]<<endl;

 double* samplings=new double[layerNr.size()];
 for(unsigned int i=0;i<layerNr.size();i++)
 {
  samplings[i]=layerNr[i];
//  cout<<"i "<<i<<" samplings "<<samplings[i]<<endl;
 }
 
 TFile* output=new TFile(Form("output/secondPCA_%s_nbins1st%i_nbins2nd%i.root",label.c_str(),nbins1,nbins2),"RECREATE");
 for(int b=0;b<nbins;b++)
 {
  cout<<"--- now performing 2nd PCA in bin "<<b<<endl;
  do_pca(layer, b, read_inputTree, InputTree, numberfinebins, output, samplings);
 }
 output->Write();
 
 cout<<"2nd PCA outputfile: "<<output->GetName()<<endl;
 
}

void secondPCA::do_pca(vector<string> layer, int bin, TreeReader* read_inputTree, TTree* InputTree, int numberfinebins, TFile* output, double *samplings)
{
 
 TDirectory *cdbin = (TFile*)output->mkdir(Form("bin%i",bin));
 cdbin->cd();
 
 //make a tree that holds only the events for that bin
 TTree* bintree=new TTree("bintree","bintree");
 double* data=new double[layer.size()];
 for(unsigned int l=0;l<layer.size();l++)
  bintree->Branch(Form("energy_%s",layer[l].c_str()),&data[l],Form("energy_%s/D",layer[l].c_str()));
 for(int event=0;event<read_inputTree->GetEntries();event++)
 {
  read_inputTree->GetEntry(event);
  int firstPCAbin=read_inputTree->GetVariable("firstPCAbin");
  if(firstPCAbin==bin)
  {
   for(unsigned int l=0;l<layer.size();l++)
    data[l]=read_inputTree->GetVariable(Form("energy_%s",layer[l].c_str()));
   bintree->Fill();
  }
 }
 
 //initialize the reader for this bintree
 TreeReader* read_bintree = new TreeReader();
 read_bintree->SetTree(bintree);
 
 vector<TH1D*> histos_data=get_histos_data(layer, read_bintree, InputTree, numberfinebins);
 vector<TH1D*> cumul_data=get_cumul_data(layer, histos_data);
 
 TPrincipal* principal = new TPrincipal(layer.size(),"ND");
 TTree* T_Gauss=new TTree("T_Gauss","T_Gauss");
 double* data_Gauss=new double[layer.size()];
 for(unsigned int l=0;l<layer.size();l++)
  T_Gauss->Branch(Form("energy_gauss_%s",layer[l].c_str()),&data_Gauss[l],Form("energy_gauss_%s/D",layer[l].c_str()));
 for(int event=0;event<read_bintree->GetEntries();event++)
 {
  read_bintree->GetEntry(event);
  for(unsigned int l=0;l<layer.size();l++)
  {
   double data=read_bintree->GetVariable(Form("energy_%s",layer[l].c_str()));
   double cumulant = get_cumulant(data,cumul_data[l]);
   cumulant = TMath::Min(cumulant,1.-10e-10);
   cumulant = TMath::Max(cumulant,0.+10e-10);
   //Gaussianization
   double maxErfInvArgRange = 0.99999999;
   double arg = 2.0*cumulant - 1.0;
   arg = TMath::Min(+maxErfInvArgRange,arg);
   arg = TMath::Max(-maxErfInvArgRange,arg);
   data_Gauss[l] = 1.414213562*TMath::ErfInverse(arg);
  }
  principal->AddRow(data_Gauss);
  T_Gauss->Fill();
 } //event loop
 
 principal->MakePrincipals();
 cout<<std::endl <<"- Principal Component Analysis Results in bin "<<bin<<endl;
 principal->Print("MSE");
 
 cout<<"--- Application to get Mean and RMS of the PCA transformed data"<<endl;
 TreeReader* reader_treeGauss = new TreeReader();
 reader_treeGauss->SetTree(T_Gauss);
 
 vector<double> data_PCA_min; vector<double> data_PCA_max;
 for(unsigned int l=0;l<layer.size();l++)
 {
 	data_PCA_min.push_back(100000.0);
 	data_PCA_max.push_back(-100000.0);
 }
 
 for(int event=0;event<reader_treeGauss->GetEntries();event++)
 { 
  reader_treeGauss->GetEntry(event);
  double input_data[layer.size()];
  double data_PCA[layer.size()];
  for(unsigned int l=0;l<layer.size();l++)
   input_data[l]=reader_treeGauss->GetVariable(Form("energy_gauss_%s",layer[l].c_str()));
  principal->X2P(input_data,data_PCA);
  for(unsigned int l=0;l<layer.size();l++)
  {
   if(data_PCA[l]>data_PCA_max[l]) data_PCA_max[l]=data_PCA[l];
   if(data_PCA[l]<data_PCA_min[l]) data_PCA_min[l]=data_PCA[l];
  }
 }
 //fill histograms
 TH1D* h_data_PCA[layer.size()];
 for(unsigned int l=0;l<layer.size();l++)
 {
 	h_data_PCA[l]=new TH1D(Form("h_data_PCA_%s",layer[l].c_str()),Form("h_data_PCA_%s",layer[l].c_str()),1000,data_PCA_min[l],data_PCA_max[l]);
 }
 for(int event=0;event<reader_treeGauss->GetEntries();event++)
 {
  reader_treeGauss->GetEntry(event);
  double input_data[layer.size()];
  double data_PCA[layer.size()];
  for(unsigned int l=0;l<layer.size();l++)
   input_data[l]=reader_treeGauss->GetVariable(Form("energy_gauss_%s",layer[l].c_str()));
  principal->X2P(input_data,data_PCA);
  for(unsigned int l=0;l<layer.size();l++)
 	 h_data_PCA[l]->Fill(data_PCA[l]);
 }
 double* gauss_means=new double[layer.size()];
 double* gauss_rms=new double[layer.size()];
 for(unsigned int l=0;l<layer.size();l++)
 {
 	gauss_means[l]=h_data_PCA[l]->GetMean();
 	gauss_rms[l]=h_data_PCA[l]->GetRMS();
 	//cout<<layer[l]<<" mean "<<gauss_means[l]<<" gauss_rms[l] "<<gauss_rms[l]<<endl;
 }
  
 delete T_Gauss;
 delete bintree;
 
 //Save EigenValues/EigenVectors/CovarianceMatrix
 TMatrixD* CovarianceMatrix =(TMatrixD*)principal->GetCovarianceMatrix();
 TVectorD* EigenValues  =(TVectorD*)principal->GetEigenValues();
 TMatrixD* EigenVectors =(TMatrixD*)principal->GetEigenVectors();
 TVectorD* MeanValues   =(TVectorD*)principal->GetMeanValues();
 TVectorD* SigmaValues  =(TVectorD*)principal->GetSigmas();
 TVectorD* Layers=new TVectorD((int)(layer.size()-1),samplings); 
 TVectorD* Gauss_means=new TVectorD((int)(layer.size()-1),gauss_means); 
 TVectorD* Gauss_rms  =new TVectorD((int)(layer.size()-1),gauss_rms); 
 
 CovarianceMatrix->Write("CovarianceMatrix");
 EigenValues ->Write("EigenValues");
 EigenVectors->Write("EigenVectors");
 MeanValues  ->Write("MeanValues");
 SigmaValues ->Write("SigmaValues");
 Layers      ->Write("RelevantLayers");
 Gauss_means ->Write("Gauss_means");
 Gauss_rms   ->Write("Gauss_rms");
 
} //do_pca


vector<TH1D*> secondPCA::get_histos_data(vector<string> layer, TreeReader* read_bintree, TTree* bintree, int numberfinebins)
{
 
 vector<TH1D*> data;
 
 //get the maxima per layer:
 vector<double> MaxInputs;
 for(unsigned int l=0;l<layer.size();l++) MaxInputs.push_back(0.0);
 
 for(int event=0;event<bintree->GetEntries();event++)
 {
 	read_bintree->GetEntry(event);
  for(unsigned int l=0;l<layer.size();l++)
  {
   double val = read_bintree->GetVariable(Form("energy_%s",layer[l].c_str()));
   if(val>MaxInputs[l])
    MaxInputs[l]=val;
  }
 }
 
 for(unsigned int l=0; l<layer.size(); l++)
 {
 	TH1D* h_data;
  h_data = new TH1D(Form("h_data_%s",layer[l].c_str()),Form("h_data_%s",layer[l].c_str()),numberfinebins,0,MaxInputs[l]);
  for(int event=0;event<read_bintree->GetEntries();event++)
  {
   read_bintree->GetEntry(event);
   h_data->Fill(read_bintree->GetVariable(Form("energy_%s",layer[l].c_str())));
  }
  
  h_data->Sumw2();
  h_data->Scale(1.0/h_data->Integral());
  data.push_back(h_data);
 } //for layer
 
 return data;
 
}

vector<int> secondPCA::getLayerBins(TFile* file, int &bins)
{
 
 vector<int> layer;
 
 TH2I* h_layer=(TH2I*)file->Get("h_layer");
 
 //the layers are stored in the y axis
 for(int i=1;i<=h_layer->GetNbinsY();i++)
 {
 	if(h_layer->GetBinContent(1,i)==1) 
 	 layer.push_back(h_layer->GetYaxis()->GetBinCenter(i));
 }

 
 bins=h_layer->GetNbinsX();
 
 return layer;
 
}

