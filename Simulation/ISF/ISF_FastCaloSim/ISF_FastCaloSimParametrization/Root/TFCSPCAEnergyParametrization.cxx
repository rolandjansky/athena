/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "ISF_FastCaloSimParametrization/TFCSPCAEnergyParametrization.h"
#include "ISF_FastCaloSimParametrization/FastCaloSim_CaloCell_ID.h"

#include "TFile.h"
#include <iostream>
#include "TKey.h"
#include "TClass.h"
#include "TRandom3.h"

//=============================================
//======= TFCSPCAEnergyParametrization =========
//=============================================

TFCSPCAEnergyParametrization::TFCSPCAEnergyParametrization(const char* name, const char* title):TFCSEnergyParametrization(name,title)
{
}

void TFCSPCAEnergyParametrization::simulate(TFCSSimulationState& simulstate,const TFCSTruthState* truth, const TFCSExtrapolationState* /*extrapol*/)
{
 
 int thisbin=simulstate.Ebin();
 std::cout<<"--- Simulation of bin "<<thisbin<<std::endl;
  
 for(unsigned int i=0;i<CaloCell_ID_FCS::MaxSample;++i)
  simulstate.add_E(i,truth->Ekin()/CaloCell_ID_FCS::MaxSample);
 
 TRandom3* Random3=new TRandom3();
 Random3->SetSeed(0);
 
 //get relevant layers for that bin
 std::vector<std::string> layer;
 std::vector<int> layerNr;
 double* samplings=m_RelevantLayers[thisbin]->GetMatrixArray();
 for(int i=0;i<m_RelevantLayers[thisbin]->GetNrows();i++)
  layerNr.push_back(samplings[i]);
 for(unsigned int i=0;i<layerNr.size();i++)
 {
     std::string thislayer="layer"+layerNr[i];
  layer.push_back(thislayer);
 }
 layer.push_back("totalE");
 
 double output_data[layer.size()];
 double input_data[layer.size()];
 
 double* gauss_means=m_Gauss_means[thisbin]->GetMatrixArray();
 double* gauss_rms=m_Gauss_rms[thisbin]->GetMatrixArray();
 
 for(unsigned int l=0;l<layer.size();l++)
 {
  double mean=gauss_means[l];
  double rms =gauss_rms[l];
  double gauszz=Random3->Gaus(mean,rms);
  input_data[l]=gauszz;
 }
  
 P2X(layer.size(), m_EigenVectors[thisbin], m_MeanValues[thisbin], m_SigmaValues[thisbin], input_data, output_data, layer.size());
 
 std::cout<<"--- Inverse Regression"<<std::endl;
 //double simdata_uniform[layer.size()];
 //double simdata[layer.size()];
 //double total_fraction; //this is needed for the rescaling in the next step
 /*
 for(int l=0;l<layer.size();l++)
 {
  simdata_uniform[l]=(TMath::Erf(output_data[l]/1.414213562)+1)/2.f;
  
  if(do_range)
  {
   if(Simulated_Uniform2[l]<range_low[l][randombin])
    Simulated_Uniform2[l]=range_low[l][randombin];
  }
  
  //simdata[l] = regression_application(simdata_uniform[l],thisbin,label,layer[l]);
  if(simdata[l]<0) simdata[l]=0;
  if(layer[l]!="totalE" && simdata[l]>1) simdata[l]=1;  
  if(layer[l]!="totalE") total_fraction+=simdata[l];
 }
 */
 
}


void TFCSPCAEnergyParametrization::P2X(int gNVariables, TMatrixD *EigenVectors,TVectorD *MeanValues, TVectorD *SigmaValues, double *p, double *x, int nTest)
{
 
 double* gEigenVectors = EigenVectors->GetMatrixArray();
 double* gMeanValues = MeanValues->GetMatrixArray();
 double* gSigmaValues = SigmaValues->GetMatrixArray();
 
 for(int i = 0; i < gNVariables; i++)
 {
  x[i] = gMeanValues[i];
  for(int j = 0; j < nTest; j++)
   x[i] += p[j] * gSigmaValues[i] * gEigenVectors[i *  gNVariables + j];       
 }
 
}


void TFCSPCAEnergyParametrization::loadInputs(TFile* file)
{
 
 TIter next(file->GetListOfKeys());
 TKey *key;
 while ((key = (TKey*)next()))
 {
  TDirectory* bindir=(TDirectory*)key->ReadObj();
  
  bindir->cd();
  TMatrixD* EigenVectors = (TMatrixD*)bindir->Get("EigenVectors");
  TVectorD* MeanValues   =(TVectorD*)bindir->Get("MeanValues");
  TVectorD* SigmaValues  =(TVectorD*)bindir->Get("Sigmas");
  TVectorD* RelevantLayers  =(TVectorD*)bindir->Get("RelevantLayers");
  TVectorD* Gauss_means  =(TVectorD*)bindir->Get("Gauss_means");
  TVectorD* Gauss_rms  =(TVectorD*)bindir->Get("Gauss_rms");
  
  m_RelevantLayers.push_back(RelevantLayers);
  m_EigenVectors.push_back(EigenVectors);
  m_MeanValues.push_back(MeanValues);
  m_SigmaValues.push_back(SigmaValues);
  m_Gauss_means.push_back(Gauss_means);
  m_Gauss_rms.push_back(Gauss_rms); 
 }
 
 
 
}

//=============================================
//========== ROOT persistency stuff ===========
//=============================================

ClassImp(TFCSPCAEnergyParametrization)

