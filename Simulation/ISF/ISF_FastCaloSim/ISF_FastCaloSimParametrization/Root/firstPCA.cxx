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
#include "ISF_FastCaloSimParametrization/firstPCA.h"
#include "ISF_FastCaloSimParametrization/TreeReader.h"

#include <iostream>

using namespace std;

firstPCA::firstPCA()
{
}

string firstPCA::run(string inputfile, string label, int nbins1, int nbins2, int numberfinebins, double cut_edeposit)
{
 
 int debug=0;
 
 //Open inputfile:
 cout<<"--- Open input file "<<inputfile<<endl;
 TFile* file=TFile::Open(inputfile.c_str());
 if(!file) cout<<"ERROR: Inputfile could not be opened"<<endl;
 
 cout<<"--- Init the TreeReader"<<endl;
 TTree* InputTree = (TTree*)file->Get("FCS_ParametrizationInput");
 TreeReader* read_inputTree = new TreeReader();
 read_inputTree->SetTree(InputTree);
 
 cout<<"--- Get relevant layers"<<endl;
 vector<int> layerNr=get_relevantlayers(read_inputTree, cut_edeposit);
 vector<string> layer;
 for(unsigned int l=0;l<layerNr.size();l++)
  layer.push_back(Form("layer%i",layerNr[l]));
 layer.push_back("totalE");
 
 cout<<"--- Obtain data histograms and Cumulative Distributions from the input"<<endl;
 vector<TH1D*> histos_data=get_histos_data(layerNr, layer, read_inputTree, InputTree, numberfinebins);
 vector<TH1D*> cumul_data =get_cumul_data(layer, histos_data);
 
 cout<<"--- Now define the TPrincipal"<<endl;
 TPrincipal* principal = new TPrincipal(layer.size(),"ND");  //ND means normalize cov matrix and store data
 
 TTree *T_Gauss = new TTree("T_Gauss","T_Gauss");
 double* data_Gauss = new double[layer.size()];
 int eventNumber;
 for(unsigned int l=0;l<layer.size();l++)
 {
  T_Gauss->Branch(Form("data_Gauss_%s",layer[l].c_str()),&data_Gauss[l],Form("data_Gauss_%s/D",layer[l].c_str()));
  T_Gauss->Branch("eventNumber",&eventNumber,"eventNumber/I");
 }
 
 cout<<"--- Uniformization/Gaussianization"<<endl;
 for(int event=0;event<read_inputTree->GetEntries();event++)
 {
  read_inputTree->GetEntry(event);
  eventNumber=event;
  for(unsigned int l=0;l<layer.size();l++)
  {
    double data/*,data_uniform*/;
   
   if(l==layer.size()-1)
   	data = read_inputTree->GetVariable("total_cell_energy");
   else
    data = read_inputTree->GetVariable(Form("cell_energy[%d]",layerNr[l]))/read_inputTree->GetVariable("total_cell_energy");
   
   //Uniformization
   double cumulant = get_cumulant(data,cumul_data[l]);
   cumulant = TMath::Min(cumulant,1.-10e-10);
   cumulant = TMath::Max(cumulant,0.+10e-10);
   //data_uniform = cumulant;
   
   //Gaussianization
   double maxErfInvArgRange = 0.99999999;
   double arg = 2.0*cumulant - 1.0;
   arg = TMath::Min(+maxErfInvArgRange,arg);
   arg = TMath::Max(-maxErfInvArgRange,arg);
   data_Gauss[l] = 1.414213562*TMath::ErfInverse(arg);
   
  } //for layers
  
  //Add this datapoint to the PCA
  principal->AddRow(data_Gauss);
  T_Gauss->Fill();
  
 } //event loop
 
 cout<<"--- MakePrincipals()"<<endl;
 principal->MakePrincipals();
 
 cout<<"--- PCA Results"<<endl;
 principal->Print("MSE");
 
 cout<<"--- PCA application and binning of first principal component"<<endl;
 
 TreeReader* tree_Gauss = new TreeReader();
 tree_Gauss->SetTree(T_Gauss);
 int entries_Gauss = tree_Gauss->GetEntries();
 
 int Bin_1stPC1=0;
 int Bin_1stPC2=0;
 
 TH1D* hPCA_first_component = new TH1D("hPCA_first_component","hPCA_first_component",10000,-20,20);
 
 for(int event=0;event<entries_Gauss;event++)
 {
  tree_Gauss->GetEntry(event);
  
  double* data_PCA = new double[layer.size()];
  double* input_data = new double[layer.size()];
 	
  for (unsigned int l=0;l<layer.size(); l++)
   input_data[l] = tree_Gauss->GetVariable(Form("data_Gauss_%s",layer[l].c_str()));
  
  principal->X2P(input_data,data_PCA);  
  hPCA_first_component->Fill(data_PCA[0]);
  
 }
 
 cout<<"--- Binning 1st Principal Component" <<endl;
 double xq[nbins1]; double yq[nbins1];
 
 //2D binning:
 quantiles( hPCA_first_component, nbins1, xq , yq );
 for (int m = 0; m < nbins1 ; m++)
 {
	int a=1;
 	if(m>0) a=hPCA_first_component->FindBin(yq[m-1]);
 	int b=hPCA_first_component->FindBin(yq[m]);
 	cout<<"Quantiles "<<m+1<<"  "<<xq[m]<<" "<<yq[m]<<" -> Events "<<hPCA_first_component->Integral(a,b-1)<<endl;
 }
 
 TH1D* h_compo1[nbins1];
 for(int m=0;m<nbins1;m++)
  h_compo1[m]=new TH1D(Form("h_compo1_bin%i",m),Form("h_compo1_bin%i",m),20000,-20,20);
 
 cout<<"--- PCA application and binning 2nd principal component"<<endl;
 for(int event=0;event<entries_Gauss;event++)
 {
  tree_Gauss->GetEntry(event);
  double* data_PCA = new double[layer.size()];
  double* input_data = new double[layer.size()];

  for (unsigned int l=0;l<layer.size();l++)
   input_data[l] = tree_Gauss->GetVariable(Form("data_Gauss_%s",layer[l].c_str()));
  
  principal->X2P(input_data,data_PCA);
  
  int firstbin=-42;
  //Binning 1st PC
  for (int m = 0 ; m < nbins1 ; m++)
  {
   if( m==0 && data_PCA[0] <= yq[0])
    firstbin = 0;
   if(m > 0 && data_PCA[0] > yq[m-1] && data_PCA[0] <= yq[m])
    firstbin = m;
  }
  
  h_compo1[firstbin]->Fill(data_PCA[1]);
  
 }

 std::vector<std::vector<double> > yq2d (nbins1, std::vector<double>(nbins2));
 
 for(int m=0;m<nbins1;m++)
 {
 	if(debug) cout<<"now do m "<<m<<endl;
  double xq2[nbins2]; double yq2[nbins2];
  quantiles( h_compo1[m], nbins2, xq2 , yq2);
  if(debug) cout<<"1stPCA bin# "<<m<<" Events "<<h_compo1[m]->Integral()<<endl;
  for (int u = 0; u < nbins2 ; u++)
  {
   int a=0;
 	 if(u>0) a=h_compo1[m]->FindBin(yq2[u-1]);
 	 int b=h_compo1[m]->FindBin(yq2[u]);
   cout<<"Quantile # "<<u<<"  "<<xq2[u]<<" "<<yq2[u]<<" -> Events "<<h_compo1[m]->Integral(a,b-1)<<endl;
  }
  
  for(int u=0;u<nbins2;u++)
   yq2d[m][u]=yq2[u];
 }
 
 cout<<"--- Fill a tree that has the bin information"<<endl;
 int firstPCAbin;
 double* data = new double[layer.size()];
 TTree* tree_1stPCA=new TTree(Form("tree_1stPCA"),Form("tree_1stPCA"));
 tree_1stPCA->Branch("firstPCAbin",&firstPCAbin,"firstPCAbin/I");
 for(unsigned int l=0;l<layer.size();l++)
  tree_1stPCA->Branch(Form("energy_%s",layer[l].c_str()),&data[l],Form("energy_%s/D",layer[l].c_str()));
 
 for(int event=0;event<entries_Gauss;event++)
 {
  tree_Gauss->GetEntry(event);
  double* data_PCA = new double[layer.size()];
  double* input_data = new double[layer.size()];
  int eventNumber=tree_Gauss->GetVariable("eventNumber");
  
  for(unsigned int l=0; l<layer.size();l++)
   input_data[l] = tree_Gauss->GetVariable(Form("data_Gauss_%s",layer[l].c_str()));
  
  //PCA Application
  principal->X2P(input_data,data_PCA);
  
  //Binning 1st and 2nd PC
  for(int m = 0 ; m < nbins1 ; m++)
  {
   if(m==0 && data_PCA[0]<=yq[m])
   {
    Bin_1stPC1 = 0;
    for(int u=0;u<nbins2;u++)
    {
     if(u==0 && data_PCA[1]<=yq2d[0][u])
      Bin_1stPC2 = 0;
     if(u>0 && data_PCA[1]>yq2d[0][u-1] && data_PCA[1]<=yq2d[0][u])
      Bin_1stPC2 = u;
    }
   }
   if(m>0 && data_PCA[0]>yq[m-1] && data_PCA[0]<=yq[m])
   {
    Bin_1stPC1 = m;
    for(int u=0;u<nbins2;u++)
    {
     if(u==0 && data_PCA[1]<=yq2d[m][u])
      Bin_1stPC2 = 0;
     if(u>0 && data_PCA[1]>yq2d[m][u-1] && data_PCA[1]<=yq2d[m][u])
      Bin_1stPC2 = u;
    }
   }   
  }
  
  firstPCAbin=Bin_1stPC1+nbins1*Bin_1stPC2;
    
  //find the energy fractions and total energy for that given event
  read_inputTree->GetEntry(eventNumber);
  for(unsigned int l=0;l<layer.size();l++)
  {
   if(l==layer.size()-1)
   	data[l] = read_inputTree->GetVariable("total_cell_energy");
   else
    data[l] = read_inputTree->GetVariable(Form("cell_energy[%d]",layerNr[l]))/read_inputTree->GetVariable("total_cell_energy");
  }
  
  tree_1stPCA->Fill();
  
 } //for events in gauss
 
 //add a histogram that holds the relevant layer:
 
 int totalbins=nbins1*nbins2;
 
 TH2I* h_layer=new TH2I("h_layer","h_layer",totalbins,-0.5,totalbins-0.5,25,-0.5,24.5);
 h_layer->GetXaxis()->SetTitle("PCA bin");
 h_layer->GetYaxis()->SetTitle("Layer");
 for(int b=0;b<totalbins;b++)
 {
  for(int l=0;l<25;l++)
  {
   int is_relevant=0;
   for(unsigned int i=0;i<layerNr.size();i++)
   {
   	if(l==layerNr[i]) is_relevant=1;
   }
   h_layer->SetBinContent(b+1,l+1,is_relevant);
  }
 }
 
 //Output file:
 TFile* output=new TFile(Form("output/firstPCA_%s_nbins1st%i_nbins2nd%i.root",label.c_str(),nbins1,nbins2),"RECREATE");
 output->Add(h_layer);
 output->Add(tree_1stPCA);
 output->Write();
 
 cout<<"1st PCA is done. Output file: "<<output->GetName()<<endl;
 
 return output->GetName();
 
} //run()

vector<TH1D*> firstPCA::get_cumul_data(vector<string> layer, vector<TH1D*> histos_data)
{
 
 vector<TH1D*> cumul;
 
 for(unsigned int i=0;i<histos_data.size();i++)
 {
 	TH1D* h_cumul=(TH1D*)histos_data[i]->Clone(Form("h_cumul_%s",layer[i].c_str()));
 	for (int b=1; b<=h_cumul->GetNbinsX(); b++)
  {
   h_cumul->SetBinContent(b,histos_data[i]->Integral(1,b));
  }
  cumul.push_back(h_cumul);
 }

 return cumul;
 
}

vector<TH1D*> firstPCA::get_histos_data(vector<int> layerNr, vector<string> layer, TreeReader* read_inputTree, TTree* InputTree, int numberfinebins)
{
 
 vector<TH1D*> data;
 
 //get the maxima per layer:
 vector<double> MaxInputs;
 for(unsigned int l=0;l<layerNr.size();l++) MaxInputs.push_back(0.0);
 
 for(int event=0;event<read_inputTree->GetEntries();event++)
 {
 	read_inputTree->GetEntry(event);
  for(unsigned int l=0;l<layerNr.size();l++)
  {
   double efraction = read_inputTree->GetVariable(Form("cell_energy[%d]",layerNr[l]))/read_inputTree->GetVariable("total_cell_energy");
   if(efraction>MaxInputs[l])
    MaxInputs[l]=efraction;
  }
 }
 
 for(unsigned int l=0; l<layer.size(); l++)
 {
 	TH1D* h_data;
  if(l==layer.size()-1)  //Total energy
  {
   h_data=new TH1D(Form("h_data_%s",layer[l].c_str()),Form("h_data_%s",layer[l].c_str()),numberfinebins,InputTree->GetMinimum("total_cell_energy"),InputTree->GetMaximum("total_cell_energy"));
   for(int event=0;event<read_inputTree->GetEntries();event++)
   {
    read_inputTree->GetEntry(event);
    h_data->Fill(read_inputTree->GetVariable("total_cell_energy"));
   }
  }
  else  //Efraction Per Layer
  {
   h_data = new TH1D(Form("h_data_%s",layer[l].c_str()),Form("h_data_%s",layer[l].c_str()),numberfinebins,0,MaxInputs[l]);
   for(int event=0;event<read_inputTree->GetEntries();event++ )
   {
    read_inputTree->GetEntry(event);
    h_data->Fill( read_inputTree->GetVariable(Form("cell_energy[%d]",layerNr[l]))/read_inputTree->GetVariable("total_cell_energy") );
   }
  }
  
  h_data->Sumw2();
  h_data->Scale(1.0/h_data->Integral());
  data.push_back(h_data);
 } //for layer
 
 return data;
 
}


vector<int> firstPCA::get_relevantlayers(TreeReader* read_inputTree, double cut_edeposit)
{
 
 double sum_efraction[25];
 for(int i=0;i<25;i++) sum_efraction[i]=0.0;
 
 for(int event=0;event<read_inputTree->GetEntries();event++ )
 {
  read_inputTree->GetEntry(event);
  for (int j=0 ; j<25 ; j++)
  {
   double efraction = read_inputTree->GetVariable(Form("cell_energy[%d]",j))/read_inputTree->GetVariable("total_cell_energy");
   sum_efraction[j] += efraction;
  }
 }
 
 vector<int> Relevant_Layers;
 for (int i=0; i<25 ; i++)
 {
  if(sum_efraction[i]/read_inputTree->GetEntries() >= cut_edeposit)
  {
   Relevant_Layers.push_back(i);
   cout<<"Layer "  <<i <<" is relevant! sum_efraction= "<<sum_efraction[i]<<" sum/entries= "<<sum_efraction[i]/read_inputTree->GetEntries()<<endl;
  }
 }
  
 return Relevant_Layers;
  
}



double firstPCA::get_cumulant(double x, TH1D* h)
{
    
    //Cumulant "à la TMVA"
    
    int nbin = h->GetNbinsX();
    int bin = h->FindBin(x);
    bin = TMath::Max(bin,1);
    bin = TMath::Min(bin,h->GetNbinsX());
    
    
    double AvNuEvPerBin;
    double Tampon = 0 ;
    for (int i=1; i<=nbin; i++) {
        Tampon += h->GetBinContent(i);
    }
    
    AvNuEvPerBin = Tampon/nbin;
    
    double cumulant;
    double x0, x1, y0, y1;
    double total = h->GetNbinsX()*AvNuEvPerBin;
    double supmin = 0.5/total;
    
    x0 = h->GetBinLowEdge(TMath::Max(bin,1));
    x1 = h->GetBinLowEdge(TMath::Min(bin,h->GetNbinsX())+1);
    
    y0 = h->GetBinContent(TMath::Max(bin-1,0)); // Y0 = F(x0); Y0 >= 0
    y1 = h->GetBinContent(TMath::Min(bin, h->GetNbinsX()+1));  // Y1 = F(x1);  Y1 <= 1
    
    //Zero bin
    if (bin == 0) {
        y0 = supmin;
        y1 = supmin;
    }
    if (bin == 1) {
        y0 = supmin;
    }
    if (bin > h->GetNbinsX()) {
        y0 = 1.-supmin;
        y1 = 1.-supmin;
    }
    if (bin == h->GetNbinsX()) {
        y1 = 1.-supmin;
    }
    
    ////////////////////////
    
    if (x0 == x1) {
        cumulant = y1;
    } else {
        cumulant = y0 + (y1-y0)*(x-x0)/(x1-x0);
    }
    
    if (x <= h->GetBinLowEdge(1)){
        cumulant = supmin;
    }
    if (x >= h->GetBinLowEdge(h->GetNbinsX()+1)){
        cumulant = 1-supmin;
    }
    
    return cumulant;
    
}


void firstPCA::quantiles(TH1D* h, int nq, double* xq, double* yq)
{
    
    //Function for quantiles
    // h Input histo
    // nq number of quantiles
    // xq position where to compute the quantiles in [0,1]
    // yq array to contain the quantiles
    
    for (int i=0;i<nq;i++)
    {
      xq[i] = double(i+1)/nq;
      h->GetQuantiles(nq,yq,xq);
    }
    
}
