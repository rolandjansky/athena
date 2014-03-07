/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include "LArCafJobs/AbsShape.h"

#include "LArCafJobs/ShapeInfo.h"
#include "LArCafJobs/SimpleShape.h"
#include "TH1D.h"
#include "TGraphErrors.h"
#include "TMath.h"
#include "TVector.h"
#include "TMatrixD.h"

#include <iostream>
using std::cout;
using std::endl;

using namespace LArSamples;


double AbsShape::error(unsigned int i) const 
{ 
  return TMath::Sqrt(covariance(i, i)); 
}


double AbsShape::maxValue(bool withErrors) const
{
  double maxValue = -DBL_MAX;
  for (unsigned int i = 0; i < nPoints(); i++) {
    double val = value(i) + (withErrors ? error(i) : 0);
    if (val > maxValue) maxValue = val;
  }
  return maxValue;
}


double AbsShape::minValue(bool withErrors) const
{
  double minValue = DBL_MAX;
  for (unsigned int i = 0; i < nPoints(); i++) {
    double val = value(i) - (withErrors ? error(i) : 0);
    if (val < minValue) minValue = val;
  }
  return minValue;
}


int AbsShape::maxPosition() const
{
  int maxPosition = -1;
  double maxValue = -1;
  for (unsigned int i = 0; i < nPoints(); i++)
    if (value(i) > maxValue) { maxPosition = i; maxValue = value(i); }
  return maxPosition;
}
  
int AbsShape::minPosition() const
{
  int minPosition = -1;
  double minValue = Definitions::none;
  for (unsigned int i = 0; i < nPoints(); i++)
    if (value(i) < minValue) { minPosition = i; minValue = value(i); }
  return minPosition;
}
  

int AbsShape::findTimeInterval(double t) const
{
  if (t < time(0)) return -1;
  if (t > time(nPoints() - 1)) return int(nPoints()) - 1;
  
  for (unsigned int i = 0; i < nPoints() - 1; i++)
    if (time(i) <= t && t < time(i+1)) return i;

  return nPoints() - 1;
}


int AbsShape::interpolate(double t, double& val, double& err) const
{
  if (nPoints() < 2) return -1;
  
  int status = 0;
  int i1 = findTimeInterval(t);
  
  if (i1 < 0) {
    status = -1;
    i1 = 0;
  }

  if (i1 > int(nPoints()) - 2) {
    status = 1;
    i1 = nPoints() - 2;
  }

  double delta = (t - time(i1))/(time(i1+1) - time(i1));
  val = value(i1) + delta*(value(i1 + 1) - value(i1));
  err = error(i1) + delta*(error(i1 + 1) - error(i1));
  //cout << t << " " << i1 << " " << time(i1) << " " << delta << " " << value(i1) << " " << value(i1+1) << " " << val << " " << status << endl;
  return status;
}


int AbsShape::interpolateDiff(double t, double& diff) const
{
  int status = 0;
  int i1 = findTimeInterval(t);  
  if (i1 < 0) {
    i1 = 0;
    status = -1;
  }  
  if (i1 > int(nPoints()) - 1) {
    i1 = nPoints() - 1;
    status = 1;
  }  
  if (i1 > 1 && i1 < int(nPoints()) - 2)
    diff = (value(i1 - 2) - 8*value(i1 - 1) + 8*value(i1 + 1) - value(i1 + 2))/(time(i1 + 2) - time(i1 - 2))/3;
  else
    if (i1 > 0) 
      if (i1 <  int(nPoints()) - 1)
        diff = (value(i1 + 1) - value(i1 - 1))/(time(i1 + 1) - time(i1 - 1));
      else
        diff = (value(i1) - value(i1 - 1))/(time(i1) - time(i1 - 1));
    else 
      diff = (value(i1 + 1) - value(i1))/(time(i1 + 1) - time(i1));

  return status;
}
    

TVectorD AbsShape::values(int lwb, int upb) const
{
  if (lwb < 0) lwb = 0;
  if (upb < 0) upb = (int)nPoints() - 1;
  if (upb >= (int)nPoints()) upb = (int)nPoints() - 1;
  TVectorD vals(lwb, upb);
  for (int i = lwb; i <= upb; i++) 
    vals(i) = value(i);
  return vals;
}


bool AbsShape::interpolate(const AbsShape& other, TVectorD& values, CovMatrix& errors, int lwb, int upb) const
{
  if (lwb < 0) lwb = 0;
  if (upb < 0) upb = other.nPoints() - 1;
  values.ResizeTo(lwb, upb);
  errors.ResizeTo(lwb, upb, lwb, upb);  
  int actualLwb = -1, actualUpb = upb;
  for (int i = lwb; i <= upb; i++) {
    double val, err;
    int stat = interpolate(other.time(i), val, err);
    if (stat ==  0 && actualLwb < 0) actualLwb = i;
    if (stat == +1) { actualUpb = i - 1; break; }
    values(i) = val;
    errors(i, i) = err*err;
  }
  values.ResizeTo(actualLwb, actualUpb);
  errors.ResizeTo(actualLwb, actualUpb, actualLwb, actualUpb);  
  return (actualLwb >= 0);
}


bool AbsShape::interpolateDiff(const AbsShape& other, TVectorD& diffs, int lwb, int upb) const
{
  if (lwb < 0) lwb = 0;
  if (upb < 0) upb = other.nPoints() - 1;
  diffs.ResizeTo(lwb, upb);
  int actualLwb = -1, actualUpb = upb;  
  for (int i = lwb; i <= upb; i++) {
    double diff;
    int stat = interpolateDiff(other.time(i), diff);
    if (stat ==  0 && actualLwb < 0) actualLwb = i;
    if (stat == +1) { actualUpb = i - 1; break; }
    diffs(i) = diff;
  }
  diffs.ResizeTo(actualLwb, actualUpb);
  return (actualLwb >= 0);
}


TGraphErrors* AbsShape::graph(bool timeInUnitOfSamples) const
{
  TGraphErrors* graph = new TGraphErrors(nPoints());
  double unit = (timeInUnitOfSamples ? Definitions::samplingInterval : 1);
  for (unsigned int i = 0; i < nPoints(); i++) {
    graph->SetPoint(i, time(i)/unit, value(i));    
    graph->SetPointError(i, 0, error(i));
  }
  return graph;
}


CovMatrix AbsShape::covarianceMatrix(int lwb, int upb,
                                                             const CovMatrix& refErr,
                                                             bool withCorrs) const
{
  if (lwb < 0 || upb < 0) { lwb = 0; upb = nPoints() - 1; }
  CovMatrix c(lwb, upb);
  for (int i = lwb; i <= upb; i++) {
    for (int j = lwb; j <= upb; j++) {
      double cov = (withCorrs || i == j ? covariance(i, j) : 0);
      if (refErr.GetNrows() > 0) cov += refErr(i, j);
      c(i, j) = cov;
    }
  }
  return c;
}


CovMatrix AbsShape::invCovarianceMatrix(int lwb, int upb,
                                                                const CovMatrix& refErr,
                                                                bool withCorrs) const 
{ 
  return covarianceMatrix(lwb, upb, refErr, withCorrs).Invert(); 
}


CovMatrix AbsShape::covarianceMatrix(unsigned int n,
							    bool withCorrs) const
{
  return covarianceMatrix(0, n - 1, CovMatrix(), withCorrs);
}


CovMatrix AbsShape::invCovarianceMatrix(unsigned int n,
                                                                bool withCorrs) const
{
  return covarianceMatrix(n, withCorrs).Invert();
}


SimpleShape* AbsShape::resample(unsigned int nPts) const
{
  std::vector<double> values, errors;
  double t0 = time(0);
  double t1 = time(nPoints() - 1);
  double dT = (t1 - t0)/nPts;
  double t = t0;
  for (unsigned int i = 0; i < nPts; i++, t += dT) {
    double val, err;
    int inRange = interpolate(t, val, err);
    if (inRange != 0) return 0;
    values.push_back(val);
    errors.push_back(err);
  }
  
  return new SimpleShape(values, errors, dT, t0);
}

