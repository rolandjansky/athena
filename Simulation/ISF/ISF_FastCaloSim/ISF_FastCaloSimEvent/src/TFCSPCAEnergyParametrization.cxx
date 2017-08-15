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
  m_numberpcabins=1;
}


void TFCSPCAEnergyParametrization::simulate(TFCSSimulationState& simulstate,const TFCSTruthState* /*truth*/, const TFCSExtrapolationState* /*extrapol*/)
{

  int verbose=0;

  int pcabin=simulstate.Ebin();

  if(verbose) std::cout<<"--- Energy simulation of PCA bin "<<pcabin<<std::endl;

  TMatrixDSym* symCov        =m_symCov[pcabin-1];
  TMatrixD*    EV            =m_EV[pcabin-1];
  TVectorD*    MeanValues    =m_MeanValues[pcabin-1];
  TVectorD*    SigmaValues   =m_SigmaValues[pcabin-1];
  TVectorD*    Gauss_means   =m_Gauss_means[pcabin-1];
  TVectorD*    Gauss_rms     =m_Gauss_rms[pcabin-1];
  TVectorD*    LowerBounds   =m_LowerBounds[pcabin-1];
  std::vector<TFCS1DFunction*> cumulative=m_cumulative[pcabin-1];

  /*
    for(unsigned int i=0;i<CaloCell_ID_FCS::MaxSample;++i)
    simulstate.add_E(i,truth->Ekin()/CaloCell_ID_FCS::MaxSample);
  */

  TRandom3* Random3=new TRandom3(); Random3->SetSeed(0);

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

  double* vals_gauss_means=(double*)Gauss_means->GetMatrixArray();
  double* vals_gauss_rms  =Gauss_rms->GetMatrixArray();
  double* vals_lowerBounds=LowerBounds->GetMatrixArray();

  double *output_data = new double[layer.size()] ;
  double *input_data = new double[layer.size()]  ;

  for(unsigned int l=0;l<layer.size();l++)
    {
      double mean=vals_gauss_means[l];
      double rms =vals_gauss_rms[l];
      double gauszz=Random3->Gaus(mean,rms);
      input_data[l]=gauszz;
    }

  TMatrixDSymEigen* eigen=new TMatrixDSymEigen(*symCov);
  TMatrixD myEvectors(eigen->GetEigenVectors());

  delete eigen;

  //P2X(layer.size(), input_data, output_data, layer.size());
  P2X(SigmaValues, MeanValues, EV, layer.size(), input_data, output_data, layer.size());

  double *simdata_uniform = new double[layer.size()];
  double *simdata = new double[layer.size()];
  double *simdata_scaled = new double[layer.size()];
  double sum_fraction=0.0; //this is needed for the rescaling in the next step
  for(unsigned int l=0;l<layer.size();l++)
    {
      simdata_uniform[l]=(TMath::Erf(output_data[l]/1.414213562)+1)/2.f;

      if(simdata_uniform[l]<vals_lowerBounds[l]) simdata_uniform[l]=vals_lowerBounds[l];

      simdata[l]=cumulative[l]->rnd_to_fct(simdata_uniform[l]);

      if(verbose)
        std::cout<<"l "<<l<<" input_data[l] "<<input_data[l]<<" output_data[l] "<<output_data[l]<<" simdata_uniform[l] "<<simdata_uniform[l]<<" simdata[l] "<<simdata[l]<<std::endl;

      if(simdata[l]<0) simdata[l]=0;
      if(layer[l]!="totalE" && simdata[l]>1) simdata[l]=1;
      if(layer[l]!="totalE") sum_fraction+=simdata[l];

    }

  //RECSCALE SO THAT THE SUM GIVES 1
  double scale=1.0/sum_fraction;

  if(verbose) std::cout<<" scale factor: "<<scale<<" sum_fraction "<<sum_fraction<<std::endl;

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

  double total_energy=simdata[layer.size()-1];
  simulstate.set_E(total_energy);

  for(int s=0;s<CaloCell_ID_FCS::MaxSample;s++)
    {
      double energyfrac=0.0;
      for(unsigned int l=0;l<layerNr.size();l++)
        {
          if(layerNr[l]==s)
            energyfrac=simdata_scaled[l];
        }
      simulstate.set_Efrac(s,energyfrac);
      simulstate.set_E(s,energyfrac*total_energy);
    }

  delete Random3;
  // delete symCov;
  // delete EV;
  // delete MeanValues;
  // delete SigmaValues;
  // delete Gauss_means;
  // delete Gauss_rms;
  // delete LowerBounds;
  delete [] output_data;
  delete [] input_data;
  delete [] simdata;
  delete [] simdata_uniform;
  delete [] simdata_scaled;
  //cumulative.clear();


}

/*
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
*/

void TFCSPCAEnergyParametrization::P2X(TVectorD* SigmaValues, TVectorD* MeanValues, TMatrixD* EV, int gNVariables, double *p, double *x, int nTest)
{

  double* gSigmaValues  = SigmaValues->GetMatrixArray();
  double* gMeanValues   = MeanValues->GetMatrixArray();
  double* gEigenVectors = EV->GetMatrixArray();

  for(int i = 0; i < gNVariables; i++)
    {
      x[i] = gMeanValues[i];
      for(int j = 0; j < nTest; j++)
        {
          x[i] += p[j] * gSigmaValues[i] * (double)(gEigenVectors[i *  gNVariables + j]);
        }
    }
}

void TFCSPCAEnergyParametrization::loadInputs(TFile* file)
{

  //TO DO: give this folder as an argument to the loadinputs, as determined from the isf particle

  //std::string folder="EnergyParams/pdgid_211/EN_50000/eta_0_20";
  std::string folder="";

  //determine the number of pca bins from the file:
  int trynext=1;
  while(trynext)
    {
      IntArray* test  =(IntArray*)file->Get(Form("%s/bin%i/pca/RelevantLayers",folder.c_str(),m_numberpcabins));
      if(test)
        {
          m_numberpcabins++;
          delete test;
        }
      else
        trynext=0;
    }
  m_numberpcabins-=1;

  file->cd(Form("%s/bin1/pca",folder.c_str()));
  m_RelevantLayers=(IntArray*)gDirectory->Get("RelevantLayers");
  if(m_RelevantLayers == NULL) std::cout << "TFCSPCAEnergyParametrization::m_RelevantLayers in first pcabin is null!" << std::endl;

  for(int bin=1;bin<=m_numberpcabins;bin++)
    {

      file->cd(Form("%s/bin%i/pca",folder.c_str(),bin));

      TMatrixDSym* symCov     =(TMatrixDSym*)gDirectory->Get("symCov");
      TMatrixD* EV            =(TMatrixD*)gDirectory->Get("EigenVectors");
      TVectorD* MeanValues    =(TVectorD*)gDirectory->Get("MeanValues");
      TVectorD* SigmaValues   =(TVectorD*)gDirectory->Get("SigmaValues");
      TVectorD* Gauss_means   =(TVectorD*)gDirectory->Get("Gauss_means");
      TVectorD* Gauss_rms     =(TVectorD*)gDirectory->Get("Gauss_rms");
      TVectorD* LowerBounds   =(TVectorD*)gDirectory->Get("LowerBounds");

      // null check
      if(symCov == NULL)         std::cout << "TFCSPCAEnergyParametrization::m_symCov in pcabin "<<bin<<" is null!" << std::endl;
      if(EV == NULL)             std::cout << "TFCSPCAEnergyParametrization::m_EV in pcabin "<<bin<<" is null!" << std::endl;
      if(MeanValues == NULL)     std::cout << "TFCSPCAEnergyParametrization::m_MeanValues in pcabin "<<bin<<" is null!" << std::endl;
      if(SigmaValues == NULL)    std::cout << "TFCSPCAEnergyParametrization::m_SigmaValues in pcabin "<<bin<<" is null!" << std::endl;
      if(Gauss_means == NULL)    std::cout << "TFCSPCAEnergyParametrization::m_Gauss_means in pcabin "<<bin<<" is null!" << std::endl;
      if(Gauss_rms == NULL)      std::cout << "TFCSPCAEnergyParametrization::m_Gause_rms in pcabin "<<bin<<" is null!" << std::endl;
      if(LowerBounds == NULL)    std::cout << "TFCSPCAEnergyParametrization::m_LowerBounds in pcabin "<<bin<<" is null!" << std::endl;

      m_symCov.push_back(symCov);
      m_EV.push_back(EV);
      m_MeanValues.push_back(MeanValues);
      m_SigmaValues.push_back(SigmaValues);
      m_Gauss_means.push_back(Gauss_means);
      m_Gauss_rms.push_back(Gauss_rms);
      m_LowerBounds.push_back(LowerBounds);

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

      std::vector<TFCS1DFunction*> cumulative;

      for(unsigned int l=0;l<layer.size();l++)
        {
          file->cd(Form("%s/bin%i/%s",folder.c_str(),bin,layer[l].c_str()));

          TFCS1DFunction* fct;
          fct=(TFCS1DFunction*)gDirectory->Get("TFCS1DFunctionRegression");
          if(!fct)
            fct=(TFCS1DFunction*)gDirectory->Get("TFCS1DFunctionRegressionTF");
          if(!fct)
            fct=(TFCS1DFunction*)gDirectory->Get("TFCS1DFunctionHistogram");
          cumulative.push_back(fct);

        }

      m_cumulative.push_back(cumulative);

    } //for pcabins

  std::cout<<"done load inputs"<<std::endl;

} //loadInputs


void TFCSPCAEnergyParametrization::loadInputs(TFile* file, std::string folder)
{

  //TO DO: give this folder as an argument to the loadinputs, as determined from the isf particle

  //determine the number of pca bins from the file:
  int trynext=1;
  while(trynext)
    {
      IntArray* test  =(IntArray*)file->Get(Form("%s/bin%i/pca/RelevantLayers",folder.c_str(),m_numberpcabins));
      if(test)
        {
          m_numberpcabins++;
          delete test;
        }
      else
        trynext=0;
    }
  m_numberpcabins-=1;

  file->cd(Form("%s/bin1/pca",folder.c_str()));
  m_RelevantLayers=(IntArray*)gDirectory->Get("RelevantLayers");
  if(m_RelevantLayers == NULL) std::cout << "TFCSPCAEnergyParametrization::m_RelevantLayers in first pcabin is null!" << std::endl;

  for(int bin=1;bin<=m_numberpcabins;bin++)
    {

      file->cd(Form("%s/bin%i/pca",folder.c_str(),bin));

      TMatrixDSym* symCov     =(TMatrixDSym*)gDirectory->Get("symCov");
      TMatrixD* EV            =(TMatrixD*)gDirectory->Get("EigenVectors");
      TVectorD* MeanValues    =(TVectorD*)gDirectory->Get("MeanValues");
      TVectorD* SigmaValues   =(TVectorD*)gDirectory->Get("SigmaValues");
      TVectorD* Gauss_means   =(TVectorD*)gDirectory->Get("Gauss_means");
      TVectorD* Gauss_rms     =(TVectorD*)gDirectory->Get("Gauss_rms");
      TVectorD* LowerBounds   =(TVectorD*)gDirectory->Get("LowerBounds");

      // null check
      if(symCov == NULL)         std::cout << "TFCSPCAEnergyParametrization::m_symCov in pcabin "<<bin<<" is null!" << std::endl;
      if(EV == NULL)             std::cout << "TFCSPCAEnergyParametrization::m_EV in pcabin "<<bin<<" is null!" << std::endl;
      if(MeanValues == NULL)     std::cout << "TFCSPCAEnergyParametrization::m_MeanValues in pcabin "<<bin<<" is null!" << std::endl;
      if(SigmaValues == NULL)    std::cout << "TFCSPCAEnergyParametrization::m_SigmaValues in pcabin "<<bin<<" is null!" << std::endl;
      if(Gauss_means == NULL)    std::cout << "TFCSPCAEnergyParametrization::m_Gauss_means in pcabin "<<bin<<" is null!" << std::endl;
      if(Gauss_rms == NULL)      std::cout << "TFCSPCAEnergyParametrization::m_Gause_rms in pcabin "<<bin<<" is null!" << std::endl;
      if(LowerBounds == NULL)    std::cout << "TFCSPCAEnergyParametrization::m_LowerBounds in pcabin "<<bin<<" is null!" << std::endl;

      m_symCov.push_back(symCov);
      m_EV.push_back(EV);
      m_MeanValues.push_back(MeanValues);
      m_SigmaValues.push_back(SigmaValues);
      m_Gauss_means.push_back(Gauss_means);
      m_Gauss_rms.push_back(Gauss_rms);
      m_LowerBounds.push_back(LowerBounds);

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

      std::vector<TFCS1DFunction*> cumulative;

      for(unsigned int l=0;l<layer.size();l++)
        {
          file->cd(Form("%s/bin%i/%s",folder.c_str(),bin,layer[l].c_str()));

          TFCS1DFunction* fct;
          fct=(TFCS1DFunction*)gDirectory->Get("TFCS1DFunctionRegression");
          if(!fct)
            fct=(TFCS1DFunction*)gDirectory->Get("TFCS1DFunctionRegressionTF");
          if(!fct)
            fct=(TFCS1DFunction*)gDirectory->Get("TFCS1DFunctionHistogram");
          cumulative.push_back(fct);

        }

      m_cumulative.push_back(cumulative);

    } //for pcabins

  std::cout<<"done load inputs"<<std::endl;

} //loadInputs

/*
  void TFCSPCAEnergyParametrization::loadInputs(TFile* file, int bin)
  {

  file->cd(Form("bin%i/pca",bin));

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

  //file->cd(Form("bin%i",bin));
  //TH1D* hist=(TH1D*)gDirectory->Get(Form("h_cumul_%s",layer[l].c_str())); hist->SetName(Form("h_cumul_%s",layer[l].c_str()));
  //h_cumulative.push_back(hist);

  }

  }
*/
