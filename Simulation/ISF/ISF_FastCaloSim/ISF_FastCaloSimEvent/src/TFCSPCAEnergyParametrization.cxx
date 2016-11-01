/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimEvent/TFCSPCAEnergyParametrization.h"
#include "ISF_FastCaloSimEvent/FastCaloSim_CaloCell_ID.h"

#include "TFile.h"
#include <iostream>
#include "TKey.h"
#include "TClass.h"
#include "TRandom3.h"
#include "TMatrixD.h"
#include "TMatrixDSymEigen.h"

//=============================================
//======= TFCSPCAEnergyParametrization =========
//=============================================

TFCSPCAEnergyParametrization::TFCSPCAEnergyParametrization(const char* name, const char* title):TFCSEnergyParametrization(name,title)
{
}

void TFCSPCAEnergyParametrization::simulate(TFCSSimulationState& simulstate,const TFCSTruthState* /*truth*/, const TFCSExtrapolationState* /*extrapol*/)
{
 
 int verbose=0;
 
 int thisbin=simulstate.Ebin();
 if(verbose) std::cout<<"--- Simulation of bin "<<thisbin<<std::endl;
 
 /*
 for(unsigned int i=0;i<CaloCell_ID_FCS::MaxSample;++i)
  simulstate.add_E(i,truth->Ekin()/CaloCell_ID_FCS::MaxSample);
 */
 
 TRandom3* Random3=new TRandom3();
 Random3->SetSeed(0);
 
 //get relevant layers for that bin
 std::vector<std::string> layer;
 std::vector<int> layerNr;
 
 for(int i=0;i<m_RelevantLayers->GetSize();i++)
  layerNr.push_back(m_RelevantLayers->GetAt(i));
 
 for(unsigned int i=0;i<layerNr.size();i++)
 {
     std::string thislayer=Form("layer%i",layerNr[i]);
  layer.push_back(thislayer);
 }
 layer.push_back("totalE");
 
 //for(unsigned int i=0;i<layer.size();i++)  cout<<layer[i]<<endl;
 
 double* gauss_means=m_Gauss_means->GetMatrixArray();
 double* gauss_rms  =m_Gauss_rms->GetMatrixArray();
 double* lowerBounds=m_LowerBounds->GetMatrixArray();
 
 double output_data[layer.size()];
 double input_data[layer.size()];
 
 for(unsigned int l=0;l<layer.size();l++)
 {
  double mean=gauss_means[l];
  double rms =gauss_rms[l];
  double gauszz=Random3->Gaus(mean,rms);
  input_data[l]=gauszz;
 }
 
 TMatrixDSymEigen* eigen=new TMatrixDSymEigen(*m_symCov);
 TMatrixD myEvectors(eigen->GetEigenVectors());
  
 //P2X(myEvectors, layer.size(), input_data, output_data, layer.size());
 P2X(layer.size(), input_data, output_data, layer.size());
 
 double simdata_uniform[layer.size()];
 double simdata[layer.size()];
 double simdata_scaled[layer.size()];
 double sum_fraction=0.0; //this is needed for the rescaling in the next step
 for(unsigned int l=0;l<layer.size();l++)
 {
  simdata_uniform[l]=(TMath::Erf(output_data[l]/1.414213562)+1)/2.f;
  
  if(simdata_uniform[l]<lowerBounds[l]) simdata_uniform[l]=lowerBounds[l];
  
  simdata[l]=m_cumulative[l]->rnd_to_fct(simdata_uniform[l]);
  //simdata[l]=InvCumulant(h_cumulative[l],simdata_uniform[l]);
  
  if(simdata[l]<0) simdata[l]=0;
  if(layer[l]!="totalE" && simdata[l]>1) simdata[l]=1;  
  if(layer[l]!="totalE") sum_fraction+=simdata[l];
 }
 
 //RECSCALE SO THAT THE SUM GIVES 1
 double scale=1.0/sum_fraction;
 
 sum_fraction=0.0;
 
 for(unsigned int l=0;l<layer.size();l++)
 {
 	if(layer[l]!="totalE")
 	{
   simdata_scaled[l]=simdata[l]*scale;
   if(l<layerNr.size())
   {
    sum_fraction+=simdata_scaled[l];
   }
  }
 } //for layer
 
 //save the result in the SimulState object:
 
 for(int s=0;s<CaloCell_ID_FCS::MaxSample;s++)
 {
 	double energy=0.0;
 	for(unsigned int l=0;l<layerNr.size();l++)
 	{
 	 if(layerNr[l]==s)
 	 	energy=simdata_scaled[l];
  }
  simulstate.set_E(s,energy);
 }
 simulstate.set_E(simdata[layer.size()-1]);
 if(verbose)
 {
     std::cout<<"Result:"<<std::endl;
  for(unsigned int l=0;l<layerNr.size();l++)
   std::cout<<"  Energy fraction deposited in Layer "<<layerNr[l]<<" = "<<simdata_scaled[l]<<std::endl;
  std::cout<<"  Sum of energy fractions = "<<sum_fraction<<std::endl;
  std::cout<<"  Total Energy = "<<simulstate.E()<<" MeV "<<std::endl;
 }
 
 delete Random3;
 
}


void TFCSPCAEnergyParametrization::P2X(TMatrixD EigenVectors, int gNVariables, double *p, double *x, int nTest)
{
 
 double* gSigmaValues  = m_SigmaValues->GetMatrixArray();
 double* gMeanValues   = m_MeanValues->GetMatrixArray();
 double* gEigenVectors = EigenVectors.GetMatrixArray();
 
 for(int i = 0; i < gNVariables; i++)
 {
  x[i] = gMeanValues[i];
  for(int j = 0; j < nTest; j++)
  {
   x[i] += p[j] * gSigmaValues[i] * (double)(gEigenVectors[i *  gNVariables + j]);
  }
 } 
}

void TFCSPCAEnergyParametrization::P2X(int gNVariables, double *p, double *x, int nTest)
{
 
 double* gSigmaValues  = m_SigmaValues->GetMatrixArray();
 double* gMeanValues   = m_MeanValues->GetMatrixArray();
 double* gEigenVectors = m_EV->GetMatrixArray();
 
 for(int i = 0; i < gNVariables; i++)
 {
  x[i] = gMeanValues[i];
  for(int j = 0; j < nTest; j++)
  {
   x[i] += p[j] * gSigmaValues[i] * (double)(gEigenVectors[i *  gNVariables + j]);
  }
 } 
}

double TFCSPCAEnergyParametrization::InvCumulant(TH1D* hist,double y)
{
    int bin = 0;
    int nbin = hist->GetNbinsX();
    double min = 99999999;
    for (int i=1; i<=hist->GetNbinsX()-2; i++)
    {
     if(fabs(hist->GetBinContent(i)-y)<min)
     {
       min = fabs(hist->GetBinContent(i)-y);
       bin = i ;
     }
    }
    bin = TMath::Max(bin,1);
    bin = TMath::Min(bin,hist->GetNbinsX());

    //std::cout<<bin <<std::endl;
    
    double AvNuEvPerBin;
    double Tampon = 0 ;
    for (int i=1; i<=nbin; i++)
    {
      Tampon += hist->GetBinContent(i);
    }
    
    AvNuEvPerBin = Tampon/nbin;
    
    double x;
    double x0, x1, y0, y1;
    double total = hist->GetNbinsX()*AvNuEvPerBin;
    double supmin = 0.5/total;
    
    x0 = hist->GetBinLowEdge(TMath::Max(bin,1));
    x1 = hist->GetBinLowEdge(TMath::Min(bin,hist->GetNbinsX())+1);
    
    y0 = hist->GetBinContent(TMath::Max(bin-1,0)); // Y0 = F(x0); Y0 >= 0
    y1 = hist->GetBinContent(TMath::Min(bin, hist->GetNbinsX()+1));  // Y1 = F(x1);  Y1 <= 1
    
    //Zero bin
    if (bin == 0)
    {
        y0 = supmin;
        y1 = supmin;
    }
    if (bin == 1) {
        y0 = supmin;
    }
    if (bin > hist->GetNbinsX()) {
        y0 = 1.-supmin;
        y1 = 1.-supmin;
    }
    if (bin == hist->GetNbinsX()) {
        y1 = 1.-supmin;
    }
    
 ////////////////////////
    
 if(y0 == x1)
 {
  x = x0;
 }
 else
 {
  x = x0 + (y-y0)*(x1-x0)/(y1-y0);
 }
 
 return x;

}


void TFCSPCAEnergyParametrization::loadInputs(TFile* file, int bin)
{
 
 file->cd(Form("bin%i",bin));
 
 m_symCov       =(TMatrixDSym*)gDirectory->Get("symCov");
 m_EV           =(TMatrixD*)gDirectory->Get("EigenVectors");
 m_MeanValues   =(TVectorD*)gDirectory->Get("MeanValues");
 m_SigmaValues  =(TVectorD*)gDirectory->Get("SigmaValues");
 m_Gauss_means  =(TVectorD*)gDirectory->Get("Gauss_means");
 m_Gauss_rms    =(TVectorD*)gDirectory->Get("Gauss_rms");
 m_LowerBounds  =(TVectorD*)gDirectory->Get("LowerBounds");
 m_RelevantLayers  =(IntArray*)gDirectory->Get("RelevantLayers");
 
 std::vector<std::string> layer;
 std::vector<int> layerNr;
 for(int i=0;i<m_RelevantLayers->GetSize();i++)
  layerNr.push_back(m_RelevantLayers->GetAt(i));
 for(unsigned int i=0;i<layerNr.size();i++)
 {
     std::string thislayer=Form("layer%i",layerNr[i]);
  layer.push_back(thislayer);
 }
 layer.push_back("totalE");
 
 //for(unsigned int i=0;i<layer.size();i++)  cout<<layer[i]<<endl;
 
 for(unsigned int l=0;l<layer.size();l++)
 {
  file->cd(Form("bin%i/%s",bin,layer[l].c_str()));
 	TFCS1DFunction* fct;
 	fct=(TFCS1DFunction*)gDirectory->Get("TFCS1DFunctionRegression");
 	if(!fct)
 	{
 	 fct=(TFCS1DFunction*)gDirectory->Get("TFCS1DFunctionRegressionTF");
  }
  if(!fct)
  {
   fct=(TFCS1DFunction*)gDirectory->Get("TFCS1DFunctionHistogram");
  }
 	m_cumulative.push_back(fct);
 	
 	file->cd(Form("bin%i",bin));
 	TH1D* hist=(TH1D*)gDirectory->Get(Form("h_cumul_%s",layer[l].c_str())); hist->SetName(Form("h_cumul_%s",layer[l].c_str()));
 	h_cumulative.push_back(hist);
  
 }
 
}

void TFCSPCAEnergyParametrization::loadInputs(TFile* file)
{
 
 file->cd("pca");
 
 m_symCov       =(TMatrixDSym*)gDirectory->Get("symCov");
 m_EV           =(TMatrixD*)gDirectory->Get("EigenVectors");
 m_MeanValues   =(TVectorD*)gDirectory->Get("MeanValues");
 m_SigmaValues  =(TVectorD*)gDirectory->Get("SigmaValues");
 m_Gauss_means  =(TVectorD*)gDirectory->Get("Gauss_means");
 m_Gauss_rms    =(TVectorD*)gDirectory->Get("Gauss_rms");
 m_LowerBounds  =(TVectorD*)gDirectory->Get("LowerBounds");
 m_RelevantLayers  =(IntArray*)gDirectory->Get("RelevantLayers");
 
 std::vector<std::string> layer;
 std::vector<int> layerNr;
 for(int i=0;i<m_RelevantLayers->GetSize();i++)
  layerNr.push_back(m_RelevantLayers->GetAt(i));
 for(unsigned int i=0;i<layerNr.size();i++)
 {
     std::string thislayer=Form("layer%i",layerNr[i]);
  layer.push_back(thislayer);
 }
 layer.push_back("totalE");
 
 //for(unsigned int i=0;i<layer.size();i++)  cout<<layer[i]<<endl;
 
 for(unsigned int l=0;l<layer.size();l++)
 {
  file->cd(Form("%s",layer[l].c_str()));
 	TFCS1DFunction* fct;
 	fct=(TFCS1DFunction*)gDirectory->Get("TFCS1DFunctionRegression");
 	if(!fct)
 	{
 	 fct=(TFCS1DFunction*)gDirectory->Get("TFCS1DFunctionRegressionTF");
  }
  if(!fct)
  {
   fct=(TFCS1DFunction*)gDirectory->Get("TFCS1DFunctionHistogram");
  }
 	m_cumulative.push_back(fct);
 	  
 }
 
 file->cd("pca");
 for(unsigned int l=0;l<layer.size();l++)
 {
  TH1D* hist=(TH1D*)gDirectory->Get(Form("h_cumul_%s",layer[l].c_str())); hist->SetName(Form("h_cumul_%s",layer[l].c_str()));
  h_cumulative.push_back(hist);
 }
 
}

//=============================================
//========== ROOT persistency stuff ===========
//=============================================

ClassImp(TFCSPCAEnergyParametrization)

