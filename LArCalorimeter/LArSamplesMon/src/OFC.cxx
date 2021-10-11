/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSamplesMon/OFC.h"

#include "TMath.h"
#include "TMatrixTSym.h"
#include "LArCafJobs/AbsShape.h"
#include "LArSamplesMon/Data.h"
#include "LArSamplesMon/ShapeErrorData.h"
#include "LArSamplesMon/ClassCounts.h"

#include <iostream>
using std::cout;
using std::endl;

using namespace LArSamples;


OFC::OFC(const AbsShape& shape, const AbsShape& data, int lw, int up, const ShapeErrorData* sed, bool withAutoCorr)
  : m_g(lw, up), m_gp(lw, up)
{
  ClassCounts::incrementInstanceCount("OFC"); 
  if (!initGValues(shape, data, sed)) return;
  if (!initOFCs(data, CovMatrix(), withAutoCorr)) return;
}
    

OFC::OFC(const AbsShape& shape, const AbsShape& data, const CovMatrix& refErr, 
                     int lw, int up, const ShapeErrorData* sed, bool withAutoCorr)
  : m_g(lw, up), m_gp(lw, up)
{
  ClassCounts::incrementInstanceCount("OFC"); 
  if (!initGValues(shape, data, sed)) return;
  if (!initOFCs(data, refErr, withAutoCorr)) return;
}


OFC::~OFC()
{
  ClassCounts::decrementInstanceCount("OFC"); 
}

    
bool OFC::initGValues(const AbsShape& shape, const AbsShape& data, const ShapeErrorData* sed)
{
  CovMatrix errors;
  if (!shape.interpolate(data, m_g, errors, lwb(), upb())) {
    cout <<"ERROR in OFC::initGValues : indices were invalid!" << endl;
    m_g.ResizeTo(0);
    return false;
  }  
  if (!shape.interpolateDiff(data, m_gp, lwb(), upb())) { // lwb and upb come from g, so set above correctly...
    cout <<"ERROR in OFC::initGValues : (diff) indices were invalid!" << endl;
    m_g.ResizeTo(0);
    m_gp.ResizeTo(0);
    return false;
  }
  m_g.ResizeTo(m_gp.GetLwb(), m_gp.GetUpb()); // in case gp was more restricted
  if (sed) {
    TVectorD xi = sed->xi(m_gp.GetLwb(), m_gp.GetUpb());
    TVectorD xip = sed->xip(m_gp.GetLwb(), m_gp.GetUpb());
    m_g += xi;
    m_gp += xip;
  }
  return true;
}


bool OFC::initOFCs(const AbsShape& data, const CovMatrix& refErr, bool withAutoCorr)
{
  m_a.ResizeTo(lwb(), upb());
  m_b.ResizeTo(lwb(), upb());

  //cout << nSamples() << endl;
  //data.covarianceMatrix().Print();
  
  CovMatrix noiseCov = data.covarianceMatrix(lwb(), upb(), refErr, withAutoCorr);
  CovMatrix noiseInvCov = data.invCovarianceMatrix(lwb(), upb(), refErr, withAutoCorr);
  //noiseCov.Print();
  
  TVectorD gam = m_g;
  TVectorD gamp = m_gp;
  gam *= noiseInvCov;
  gamp *= noiseInvCov;

  m_G   = m_g*gam;
  m_Gp  = m_g*gamp;
  m_Gpp = m_gp*gamp;

  double denom = G()*Gpp() - Gp()*Gp();
  //cout << m_G   << " " << m_g*gam << endl;
  //cout << m_Gp  << " " << m_g*gamp << " " << m_gp*gam << endl;
  //cout << m_Gpp << " " << m_gp*gamp << endl;

  for (int i = lwb(); i <= upb(); i++) {
    m_a(i) = (Gpp()*gam(i) - Gp()*gamp(i))/denom;
    m_b(i) = (Gp()*gam(i) - G()*gamp(i))/denom;
  }

  m_resProj.ResizeTo(lwb(), upb(), lwb(), upb());
  for (int i = lwb(); i <= upb(); i++) {
    for (int j = lwb(); j <= upb(); j++) {
      m_resProj(i,j) = (i == j ? 1 : 0) - g(i)*a(j) + gp(i)*b(j);
    }
  }

  //m_a.Print();
  //cout << m_a*m_g << " " << m_a*m_gp << " " << m_b*m_g << " "<< m_b*m_gp << endl;
  
  CovMatrix resNorm(lwb(), upb());
  for (int i = lwb(); i <= upb(); i++) {
    for (int j = lwb(); j <= upb(); j++) {
      double rn = noiseCov(i, j) - (Gpp()*g(i)*g(j) - Gp()*(g(i)*gp(j) + g(j)*gp(i)) + G()*gp(i)*gp(j))/denom;
      resNorm(i, j) = rn;
    }
  }
  
  m_Gamma.ResizeTo(lwb(), upb(), lwb(), upb());
  m_Gamma = noiseCov;
  m_Gamma.Similarity(m_resProj);

  m_invGamma.ResizeTo(lwb(), upb(), lwb(), upb());
  m_invGamma = noiseInvCov;  
  m_invGamma.Similarity(m_resProj);
  return true;
}


bool OFC::initRVectors()
{
  m_r.clear();
  m_rIdx.clear();
  for (unsigned int k = 0; k < nSamples() - 2; k++) {
    TVectorD res(nSamples());
    res[k] = 1;
    m_r.push_back(residual(res));
    m_rIdx.push_back(k);
  }
  return true;
}


double OFC::A(const AbsShape& data) const
{
  TVectorD vals(lwb(), upb());

  for (int i = lwb(); i <= upb(); i++)
    vals[i] = data.value(i);

  return dot(a(), vals);
}


double OFC::B(const AbsShape& data) const
{
  TVectorD vals(lwb(), upb());

  for (int i = lwb(); i <= upb(); i++)
    vals[i] = data.value(i);

  return dot(b(), vals);
}


TVectorD OFC::residual(const TVectorD& v) const
{
  return v + (-1)*dot(a(), v)*g() + (-1)*dot(b(), v)*gp();
}


double OFC::dot(const TVectorD& form, const TVectorD& v) const
{
  double s = 0;
   for (int i = lwb(); i <= upb(); i++) s += form[i]*v[i];
  return s;
}

