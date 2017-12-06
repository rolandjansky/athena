/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TMatrixF.h"
#include "TMatrixD.h"
#include "TMatrixDSym.h"
#include "TMatrixDSymEigen.h"
#include "TVectorF.h"
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
#include "secondPCA.h"
#include "firstPCA.h"
#include "TFCSFunction.h"
#include "ISF_FastCaloSimEvent/TFCS1DFunction.h"
#include "ISF_FastCaloSimParametrization/TreeReader.h"
#include "ISF_FastCaloSimEvent/IntArray.h"

#include <iostream>
#include <sstream>

using namespace std;

secondPCA::secondPCA(string firstpcafilename, string outfilename)
{
 m_firstpcafilename=firstpcafilename;
 m_outfilename=outfilename;
 
 m_numberfinebins   =5000;
 m_storeDetails     =0;
 m_PCAbin           =-1; //-1 means all bins
 m_skip_regression  =0;
 m_neurons_start    =2;
 m_neurons_end      =8;
 m_ntoys            =1000;
 m_maxdev_regression=5;
 m_maxdev_smartrebin=5;
}

void secondPCA::set_cut_maxdeviation_regression(double val)
{
 m_maxdev_regression=val;
}

void secondPCA::set_cut_maxdeviation_smartrebin(double val)
{
 m_maxdev_smartrebin=val;
}

void secondPCA::set_Ntoys(int val)
{
 m_ntoys=val;
}

void secondPCA::set_neurons_iteration(int start,int end)
{
 m_neurons_start=start;
 m_neurons_end=end;
}

void secondPCA::set_storeDetails(int flag)
{
 m_storeDetails=flag;
}

void secondPCA::set_cumulativehistobins(int bins)
{
 m_numberfinebins=bins;
}

void secondPCA::set_PCAbin(int bin)
{
 m_PCAbin=bin;
}

void secondPCA::set_skip_regression(int flag)
{
 m_skip_regression=flag;
}

void secondPCA::run()
{
 
 //Open inputfile:
 TFile* inputfile=TFile::Open(m_firstpcafilename.c_str(),"READ");
 if(!inputfile) {
   cout<<"ERROR: Inputfile could not be opened"<<endl;
   // I don't think we can continue...
   return;
 }
 
 int nbins; int nbins0=1;
 vector<int> layerNr=getLayerBins(inputfile, nbins);
 //if a specific PCA bin was set,check if this is available, and set nbins to that
 if(m_PCAbin>0)
 {
 	if(m_PCAbin<=nbins)
 	{
 	 nbins =m_PCAbin;
 	 nbins0=m_PCAbin;
 	 string binlabel=Form("_bin%i",m_PCAbin);
   m_outfilename=m_outfilename+binlabel;
 	}
 	else cout<<"ERROR: The PCA bin you set is not available"<<endl;
 }
 
 vector<string> layer;
 for(unsigned int l=0;l<layerNr.size();l++)
  layer.push_back(Form("layer%i",layerNr[l]));
 layer.push_back("totalE");
 
 int* samplings=new int[layerNr.size()];
 for(unsigned int i=0;i<layerNr.size();i++)
  samplings[i]=layerNr[i];
 
 cout<<endl;
 cout<<"****************"<<endl;
 cout<<"     2nd PCA"<<endl;
 cout<<"****************"<<endl;
 cout<<endl;
 cout<<"Now running 2nd PCA with the following parameters:"<<endl;
 cout<<"   Input file (1st PCA): "<<m_firstpcafilename<<endl;
 cout<<"   Number of bins of the cumulative histos: "<<m_numberfinebins<<endl;
 cout<<"   storeDetails: "<<m_storeDetails<<endl;
 cout<<"   PCA bin number(s): ";
 for(int b=nbins0;b<=nbins;b++) cout<<b<<" "; cout<<endl;
 cout<<"   skip regression: "<<m_skip_regression<<endl;
 cout<<"   Regression test toys:" <<m_ntoys<<endl;
 cout<<"   Neurons in the regression iteration:" <<m_neurons_start<<" - "<<m_neurons_end<<endl;
 cout<<"   Maximal deviation of approximated histogram (regression):  "<<m_maxdev_regression<<"%"<<endl;
 cout<<"   Maximal deviation of approximated histogram (smart-rebin): "<<m_maxdev_smartrebin<<"%"<<endl;
 cout<<endl;
 cout<<"--- Init the TreeReader"<<endl;
 TTree* InputTree = (TTree*)inputfile->Get("tree_1stPCA");
 TreeReader* read_inputTree = new TreeReader();
 read_inputTree->SetTree(InputTree);
 
 TFile* output=new TFile(m_outfilename.c_str(),"RECREATE");
 for(int b=nbins0;b<=nbins;b++)
 {
  output->mkdir(Form("bin%i",b));
  output->mkdir(Form("bin%i/pca",b));
	for(unsigned int l=0;l<layer.size();l++)
   output->mkdir(Form("bin%i/%s",b,layer[l].c_str()));
 }
 output->Write();
 output->Close();

 for(int b=nbins0;b<=nbins;b++)
 {
  cout<<"--- now performing 2nd PCA in bin "<<b<<endl;
  do_pca(layer, b, read_inputTree, samplings);
 }
 
 cout<<"2nd PCA is done. Output: "<<m_outfilename<<endl;

 // cleanup
 delete read_inputTree;
 delete[] samplings;
 
}

void secondPCA::do_pca(vector<string> layer, int bin, TreeReader* read_inputTree, int* samplings)
{
 
 //make a tree that holds only the events for that 
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
 
 vector<TH1D*> histos_data=get_histos_data(layer, read_bintree);
 vector<TH1D*> cumul_data =get_cumul_histos(layer, histos_data);
 
 TPrincipal* principal = new TPrincipal(layer.size(),"ND"); //ND means normalize cov matrix and store data
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

 cout<<"check1"<<endl;
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
 std::vector<TH1D*> h_data_PCA;
 for(unsigned int l=0;l<layer.size();l++)
 {
 	h_data_PCA.push_back(new TH1D(Form("h_data_PCA_%s",layer[l].c_str()),Form("h_data_PCA_%s",layer[l].c_str()),1000,data_PCA_min[l],data_PCA_max[l]));
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
 }
 
 if(m_storeDetails)
 {
  TFile* output=TFile::Open(m_outfilename.c_str(),"UPDATE");
  output->cd(Form("bin%i/",bin));
  for(unsigned int l=0;l<layer.size();l++)
  {
   h_data_PCA[l]->Write(Form("h_PCA_component%i",l));
   histos_data[l]->Write(Form("h_input_%s",layer[l].c_str()));
   cumul_data[l]->Write(Form("h_cumul_%s",layer[l].c_str()));
  }
  output->Write();
  output->Close();
 }
 
 // cleanup
 delete bintree;
 delete read_bintree;
 delete reader_treeGauss;
 delete T_Gauss;
 delete[] data;
 delete[] data_Gauss;
 for (auto it = h_data_PCA.begin(); it != h_data_PCA.end(); ++it)
  delete *it;
 h_data_PCA.clear();
 
 //get the lower ranges and store them:
 double* lowerBound=new double[layer.size()];
 for(unsigned int l=0;l<layer.size();l++)
 {
  lowerBound[l]=get_lowerBound(cumul_data[l]);
 }
 
 //Save EigenValues/EigenVectors/CovarianceMatrix in the output file
 IntArray* myArray=new IntArray((int)(layer.size()-1));
 myArray->Set(layer.size()-1,samplings);
 
 TMatrixD* EigenVectors =(TMatrixD*)principal->GetEigenVectors();
 TMatrixD* CovarianceMatrix =(TMatrixD*)principal->GetCovarianceMatrix();
 TMatrixDSym *symCov=new TMatrixDSym();
 symCov->Use(CovarianceMatrix->GetNrows(),CovarianceMatrix->GetMatrixArray());  //symCov to be stored!
 
 TVectorD* MeanValues  =(TVectorD*)principal->GetMeanValues();
 TVectorD* SigmaValues =(TVectorD*)principal->GetSigmas();
 TVectorD* Gauss_means =new TVectorD((int)(layer.size()),gauss_means); 
 TVectorD* Gauss_rms   =new TVectorD((int)(layer.size()),gauss_rms); 
 TVectorD* LowerBounds =new TVectorD((int)(layer.size()),lowerBound); 
 
 TFile* output=TFile::Open(m_outfilename.c_str(),"UPDATE");
 output->cd(Form("bin%i/pca",bin));
 symCov->Write("symCov");
 //EigenVectors->Write("EigenVectors");
 MeanValues  ->Write("MeanValues");
 SigmaValues ->Write("SigmaValues");
 Gauss_means ->Write("Gauss_means");
 Gauss_rms   ->Write("Gauss_rms");
 myArray     ->Write("RelevantLayers");
 LowerBounds ->Write("LowerBounds");
 output->Write();
 output->Close();
 
 //call the TFCS1DFunction to decide whether or not to use regression:
 for(unsigned int l=0;l<layer.size();l++)
 {
 	cout<<endl;
 	cout<<"====> Now create the fct object for "<<layer[l]<<" <===="<<endl;
  cout<<endl;
  stringstream ss;
  ss << bin;
  string binstring = ss.str();
  TFCS1DFunction* fct=TFCSFunction::Create(cumul_data[l],m_skip_regression,m_neurons_start,m_neurons_end,m_maxdev_regression,m_maxdev_smartrebin,m_ntoys);
  
  //Store it:
  TFile* output=TFile::Open(m_outfilename.c_str(),"UPDATE");
  output->cd(Form("bin%i/%s/",bin,layer[l].c_str()));
  fct->Write();
  output->Write();
  output->Close();
  
 }
  
} //do_pca


double secondPCA::get_lowerBound(TH1D* h_cumulative)
{
 
 /*
 double range_low=0;
 int bin_start,bin_end;
 bin_start=bin_end=-1;
 
 for(int b=1;b<=h_cumulative->GetNbinsX();b++)
 {
 	if(h_cumulative->GetBinContent(b)>0 && bin_start<0)
 	{
 	 bin_start=b;
 	 range_low=(double)(h_cumulative->GetBinContent(b));
 	}
 	if(h_cumulative->GetBinContent(b)>0.9999999 && bin_end<0)
 	{
 	 bin_end=b;
 	}
 }
 return range_low;
 */
 return h_cumulative->GetBinContent(1);
 
}


vector<TH1D*> secondPCA::get_histos_data(vector<string> layer, TreeReader* read_bintree)
{
 
 vector<TH1D*> data;
 
 //get the maxima per layer:
 vector<double> MaxInputs;
 for(unsigned int l=0;l<layer.size();l++) MaxInputs.push_back(0.0);
 
 vector<double> MinInputs;
 for(unsigned int l=0;l<layer.size();l++) MinInputs.push_back(1000000.0);

 for(int event=0;event<read_bintree->GetEntries();event++)
 {
 	read_bintree->GetEntry(event);
  for(unsigned int l=0;l<layer.size();l++)
  {
   double val = read_bintree->GetVariable(Form("energy_%s",layer[l].c_str()));
   if(val>MaxInputs[l])
    MaxInputs[l]=val;
   if(val<MinInputs[l])
    MinInputs[l]=val;
  }
 }
 
 for(unsigned int l=0; l<layer.size(); l++)
 {
 	TH1D* h_data;
  h_data = new TH1D(Form("h_data_%s",layer[l].c_str()),Form("h_data_%s",layer[l].c_str()),m_numberfinebins,MinInputs[l],MaxInputs[l]);
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

