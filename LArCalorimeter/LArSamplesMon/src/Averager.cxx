/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSamplesMon/Averager.h"

#include "TMath.h"
#include <iostream>
using std::cout;
using std::endl;

using namespace LArSamples;


Averager::Averager(unsigned int n)
 : m_n(0), m_wTot(0), m_lwb(0)
{
  for (unsigned int i = 0; i < n; i++) add();
}


Averager::Averager(unsigned int lwb, unsigned int upb)
 : m_n(0), m_wTot(0), m_lwb(lwb)
{
  for (unsigned int i = lwb; i <= upb; i++) add();
}


Averager& Averager::operator=(const Averager& other)
{
  m_n = other.m_n;
  m_wTot = other.m_wTot;
  m_lwb = other.lwb();
  m_xMin = other.m_xMin;
  m_xMax = other.m_xMax;
  prepare();
  m_sum1 = other.sum1();
  m_sum2 = other.sum2();
  m_sum11Matrix = other.sum11Matrix();
  m_sum21Matrix = other.sum21Matrix();
  m_sum22Matrix = other.sum22Matrix();
  return *this;
}


bool Averager::add(double xMin, double xMax)
{
  if (m_n > 0) {
    cout << "Cannot add more variables to the Averager, data filling has started already." << endl;
    return false;
  }
  m_xMin.push_back(xMin);
  m_xMax.push_back(xMax);
  return true;
}


bool Averager::prepare()
{
  m_sum1.ResizeTo(lwb(), upb());
  m_sum2.ResizeTo(lwb(), upb());
  m_sum11Matrix.ResizeTo(lwb(), upb(), lwb(), upb());                         
  m_sum21Matrix.ResizeTo(lwb(), upb(), lwb(), upb());
  m_sum22Matrix.ResizeTo(lwb(), upb(), lwb(), upb());
  return true;
}


bool Averager::fill(const TVectorD& values, double w)
{
  if (!hasSameRange(values)) {
    cout << "Filling with wrong indices!" << endl;
    return false;
  }
  if (m_n == 0 && !prepare()) return false;
  
  for (int i1 = lwb(); i1 <= upb(); i1++) {
    double val1 = values(i1);
    m_sum1(i1) += val1*w;
    m_sum2(i1) += val1*val1*w;
    for (int i2 = lwb(); i2 <= upb(); i2++) {
      double val2 = values(i2);      
      m_sum11Matrix(i1, i2) += val1*val2*w;
      m_sum21Matrix(i1, i2) += val1*val1*val2*w;
      m_sum22Matrix(i1, i2) += val1*val1*val2*val2*w;
    }
  }
  m_n++;
  m_wTot += w;
  return true;
}


bool Averager::append(const Averager& other)
{
  if (nEntries() == 0) {
    *this = other;
    return true;
  }
  if (!hasSameRange(other)) return false;
  m_sum1 += other.sum1();
  m_sum2 += other.sum2();
  m_sum11Matrix += other.sum11Matrix();
  m_sum21Matrix += other.sum21Matrix();
  m_sum22Matrix += other.sum22Matrix();
  m_n += other.nEntries();
  m_wTot += other.totalWeight();
  return true;
}


TVectorD Averager::means() const
{
  TVectorD m(lwb(), upb());
  if (m_n == 0) {
    cout << "No entries filled yet!" << endl;
    return m;
  }
  for (int i = lwb(); i <= upb(); i++) m(i) = m_sum1(i)/m_wTot;
  return m;
}


TVectorD Averager::meanErrors() const
{
  TVectorD dm(lwb(), upb());
  if (m_n == 0) {
    cout << "No entries filled yet!" << endl;
    return dm;
  }
  CovMatrix cm = covarianceMatrix();
  for (int i1 = lwb(); i1 <= upb(); i1++)
      dm(i1) = safeSqrt(cm(i1, i1)/m_wTot);
  return dm;
}


CovMatrix Averager::meanErrorMatrix() const
{
  CovMatrix errs(lwb(), upb());
  if (m_n == 0) {
    cout << "No entries filled yet!" << endl;
    return errs;
  }
  CovMatrix cm = covarianceMatrix();
  for (int i1 = lwb(); i1 <= upb(); i1++)
    for (int i2 = lwb(); i2 <= upb(); i2++)
      errs(i1, i2) = cm(i1, i2)/m_wTot;
  return errs;
}


CovMatrix Averager::covarianceMatrix() const
{
  CovMatrix cm(lwb(), upb());
  if (m_n == 0) {
    cout << "No entries filled yet!" << endl;
    return cm;
  }
  for (int i1 = lwb(); i1 <= upb(); i1++)
    for (int i2 = lwb(); i2 <= upb(); i2++)
      cm(i1, i2) = (m_sum11Matrix(i1, i2)/m_wTot) - (m_sum1(i1)/m_wTot)*(m_sum1(i2)/m_wTot);
  return cm;
}


CovMatrix Averager::covarianceMatrixErrors() const
{
  CovMatrix dcm(lwb(), upb());
  if (m_n == 0) {
    cout << "No entries filled yet!" << endl;
    return dcm;
  }
  
  for (int i1 = lwb(); i1 <= upb(); i1++)
    for (int i2 = lwb(); i2 <= upb(); i2++) {
      dcm(i1, i2) = safeSqrt((
      (m_sum22Matrix(i1, i2)/m_wTot) 
      - 2*(m_sum21Matrix(i1, i2)/m_wTot)*(m_sum1(i2)/m_wTot) - 2*(m_sum21Matrix(i2, i1)/m_wTot)*(m_sum1(i1)/m_wTot)
      + (m_sum11Matrix(i1, i1)/m_wTot)*(m_sum1(i2)/m_wTot)*(m_sum1(i2)/m_wTot) + (m_sum11Matrix(i2, i2)/m_wTot)*(m_sum1(i1)/m_wTot)*(m_sum1(i1)/m_wTot)
      - (m_sum11Matrix(i1, i2)/m_wTot)*(m_sum11Matrix(i1, i2)/m_wTot)
      + 6*(m_sum11Matrix(i1, i2)/m_wTot)*(m_sum1(i1)/m_wTot)*(m_sum1(i2)/m_wTot)
      - 4*(m_sum1(i1)/m_wTot)*(m_sum1(i1)/m_wTot)*(m_sum1(i2)/m_wTot)*(m_sum1(i2)/m_wTot)
      )/m_wTot);
    }
    
  return dcm;
}


double Averager::mean(unsigned int i) const
{
  if (!isInRange(i)) {
    cout << "Index " << i << " out of range" << endl;
    return 0;
  }
  return means()(i);
}


double Averager::meanError(unsigned int i) const
{
  if (!isInRange(i)) {
    cout << "Index " << i << " out of range" << endl;
    return 0;
  }
  return meanErrors()(i);
}


double Averager::rms(unsigned int i) const
{
  if (!isInRange(i)) {
    cout << "Index " << i << " out of range" << endl;
    return 0;
  }
  return safeSqrt(covarianceMatrix()(i, i));
}

      
double Averager::covariance(unsigned int i, unsigned j) const
{
  if (!isInRange(i) || !isInRange(j)) {
    cout << "Indices (" << i << ", " << j << ") out of range" << endl;
    return 0;
  }
  return covarianceMatrix()(i, j);
}


double Averager::covarianceError(unsigned int i, unsigned j) const
{
  if (!isInRange(i) || !isInRange(j)) {
    cout << "Indices (" << i << ", " << j << ") out of range" << endl;
    return 0;
  }
  return covarianceMatrixErrors()(i, j);
}


double Averager::safeSqrt(double x)
{
  if (x < 0 && x > -1E-10) return 0;
  return TMath::Sqrt(x);
}
