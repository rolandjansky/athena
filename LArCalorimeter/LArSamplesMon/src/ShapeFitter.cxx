/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSamplesMon/ShapeFitter.h"
#include "TMinuit.h"

#include "LArCafJobs/CellInfo.h"
#include "LArCafJobs/DataContainer.h"

#include "LArSamplesMon/History.h"
#include "LArSamplesMon/Data.h"
#include "LArCafJobs/SimpleShape.h"

#include <iostream>
using std::cout;
using std::endl;

using namespace LArSamples;

namespace LArSamples { 
  namespace FitterData {
    const ShapeFitter* fitter;
    const AbsShape* data;
    const AbsShape* reference;
    const ScaledErrorData* sed;
    unsigned int minNDof;
  }
}


bool ShapeFitter::fit(const AbsShape& data, const AbsShape& reference, double& k, double& deltaT, double& chi2,
                                  const ScaledErrorData* sed) const
{
  TMinuit* minuit = new TMinuit(1);  //initialize TMinuit with 1 param
  gMinuit->SetFCN(adjusted_reference);
  
  FitterData::fitter = this;
  FitterData::data = &data;
  FitterData::reference = &reference;
  FitterData::sed = sed;

  double nData = (data.time(data.nPoints() - 1) - data.time(0))/Definitions::samplingInterval;
  double nRef  = (reference.time(data.nPoints() - 1) - reference.time(0))/Definitions::samplingInterval;

  FitterData::minNDof =  (nData < nRef ? ((unsigned int)nData+1)/2 : ((unsigned int)nRef+1)/2); // we want at least n/2 samples in range
  //std::cout << minNDof << std::endl;
  Double_t arglist[10];
  Int_t ierflg = 0;
  
  arglist[0] = -1;
  gMinuit->mnexcm("SET PRINT",arglist,1,ierflg);
  arglist[0] = 0;
  gMinuit->mnexcm("SET NOW",  arglist,0,ierflg);      

  arglist[0] = 1;
  minuit->mnexcm("SET ERR", arglist, 1, ierflg);
  minuit->mnparm(0, "deltaT", 0, Definitions::samplingInterval/24, -5*(double)Definitions::samplingInterval, 5*(double)Definitions::samplingInterval, ierflg);

  arglist[0] = 500;
  arglist[1] = 1.;
  minuit->mnexcm("MIGRAD", arglist, 2, ierflg);

  Double_t edm, errdef;
  Int_t nvpar, nparx, icstat;
  minuit->mnstat(chi2, edm, errdef, nvpar, nparx, icstat);

  double err, low, high;
  int iuint;
  TString name;
  minuit->mnpout(0, name, deltaT, err, low, high, iuint);
  delete minuit;
  return m_c2c.bestRescale(data, reference, k, chi2, deltaT, sed);
}


void ShapeFitter::adjusted_reference(Int_t& /*nPar*/, Double_t* /*grad*/, Double_t& f, Double_t* par, Int_t /*iflag*/)
{
  double deltaT = par[0], k = 1;
  //std::cout << "Eval at " << deltaT << std::endl;
  if (!FitterData::fitter->chi2Calc().bestRescale(*FitterData::data, *FitterData::reference, k, f, deltaT, 
                                                  FitterData::sed, FitterData::minNDof)) f = 1E99; 
  //std::cout << "-> " << k << " " << deltaT << " " << f << std::endl;
}
