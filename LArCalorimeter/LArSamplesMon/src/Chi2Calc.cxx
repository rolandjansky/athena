/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSamplesMon/Chi2Calc.h"

#include "LArSamplesMon/OFC.h"
#include "LArSamplesMon/ScaledErrorData.h"
#include "LArSamplesMon/ScaledShiftedShape.h"
#include "LArCafJobs/Definitions.h"
#include "TMath.h"
#include <iostream>
using std::cout;
using std::endl;

using namespace LArSamples;


double Chi2Calc::scalarProduct(const TVectorD& values1, const TVectorD& values2, 
                                             const CovMatrix& invCovMat) const
{
  if (values1.GetLwb() != invCovMat.GetRowLwb() || values1.GetUpb() != invCovMat.GetRowUpb()) return -1;
  if (values2.GetLwb() != invCovMat.GetColLwb() || values2.GetUpb() != invCovMat.GetColUpb()) return -1;

  double chi2Val = 0;
  
  for (int i = values1.GetLwb(); i <= values1.GetUpb(); i++) {
    for (int j = values2.GetLwb(); j <= values2.GetUpb(); j++) {
      chi2Val += invCovMat(i, j)*values1(i)*values2(j);
    }
  }
  return chi2Val;
}


TVectorD Chi2Calc::deltas(const AbsShape& data, const AbsShape& reference, CovMatrix& errors, 
                          const ScaledErrorData* shapeError, int lw, int up, bool noDataError) const
{
  TVectorD refVals;
  if (!reference.interpolate(data, refVals, errors, lw, up)) {
    cout << "Chi2Calc::deltas : Could not find overlap region between data and reference in [" << lw << ", " << up << "]." << endl;
    return TVectorD();
  }
  m_lwb = refVals.GetLwb();
  m_upb = refVals.GetUpb();

  TVectorD deltas = data.values(lwb(), upb());
  deltas -= refVals;
  if (shapeError && shapeError->providesRange(*this)) {
    TVectorD offsets = shapeError->offsets(lwb(), upb());
    deltas -= offsets;
    errors += shapeError->errors(lwb(), upb());
  }

  if (!noDataError) errors = data.covarianceMatrix(lwb(), upb(), errors, useCorrs());
  return deltas;
}


double Chi2Calc::chi2(const AbsShape& data, const AbsShape& reference, const ScaledErrorData* shapeError,
                                  int lw, int up) const
{ 
  double chi2Val = 0;
  CovMatrix errors;
  TVectorD dv = deltas(data, reference, errors, shapeError, lw, up, true);
  if (dv.GetNrows() == 0) return -1;
  
  // OFC version (gives correct error terms for individual samplings after OFC fit is performed
  if (m_pars & OFCChi2) {
    OFC ofc(reference, data, errors, lwb(), upb(), shapeError, useCorrs());
    return scalarProduct(dv, dv, ofc.invGamma());
  }
  // Basic version --- fast, simple
  if (m_pars & BasicChi2) {
    for (int k = lwb(); k < upb(); k++)
      chi2Val += dv(k)*dv(k)/(TMath::Power(data.error(k),2) + errors(k,k));
    return chi2Val;
  }
  // Default version : 
  CovMatrix invCovMat = data.invCovarianceMatrix(lwb(), upb(), errors, useCorrs());
/*  dv.Print();
  data.covarianceMatrix(lwb(), upb()).Print();
  data.covarianceMatrix(lwb(), upb(), errors, useCorrs()).Print();*/
  return scalarProduct(dv, dv, invCovMat);
}


bool Chi2Calc::bestRescale(const AbsShape& data, const AbsShape& reference, double& k, 
                                       double& chi2, double deltaT, const ScaledErrorData* sed, unsigned int minNDof) const
{
  k = 1;
  chi2 = -1;
  ScaledShiftedShape shiftedData(data, 1, -deltaT);
  TVectorD r;
  CovMatrix errors;  
  bool result = reference.interpolate(shiftedData, r, errors, -1, -1); // lwb and upb will come from data
  if (!result || r.GetNrows() < (int)minNDof) {
    k = 0;
    chi2 = 1E99;
    return true;
  }
  
  if (sed) {
    ScaledErrorData sed2(*sed, sed->scale(), sed->time() + deltaT);
    if (sed2.providesRange(r)) r += sed2.offsets(r.GetLwb(), r.GetUpb());
  }
  
  TVectorD v = data.values(r.GetLwb(), r.GetUpb());
  CovMatrix invCovMat = data.invCovarianceMatrix(r.GetLwb(), r.GetUpb(), errors, useCorrs());
  double sumR2 = scalarProduct(r, r, invCovMat);
  double sumRV = scalarProduct(r, v, invCovMat);
  double sumV2 = scalarProduct(v, v, invCovMat);
  k = sumRV/sumR2; // optimal K-factor
  chi2 = sumV2 - 2*k*sumRV + k*k*sumR2; // chi2

  return true;
}
