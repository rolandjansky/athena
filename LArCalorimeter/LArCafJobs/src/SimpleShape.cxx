/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCafJobs/SimpleShape.h"
#include "TH1D.h"
#include "TMath.h"
#include "LArCafJobs/ShapeInfo.h"

#include <iostream>
using std::cout;
using std::endl;

using namespace LArSamples;


SimpleShape::SimpleShape(const std::vector<double>& values, const std::vector<double>& errors, 
                               double timeInterval, double startTime)
  : m_values(values), m_errors(errors), 
    m_timeInterval(timeInterval), m_startTime(startTime)
{
}


SimpleShape::SimpleShape(unsigned int nPoints, double timeInterval, double startTime)
  : m_values(nPoints), m_errors(nPoints), 
    m_timeInterval(timeInterval), m_startTime(startTime)
{
}


SimpleShape::SimpleShape(const AbsShape& shape, double scale, double shift)
{
  for (unsigned int i = 0; i < shape.nPoints(); i++) {
    m_values.push_back(shape.value(i)*scale);
    m_errors.push_back(shape.error(i)*scale);
  }
  
  m_timeInterval = (shape.time(shape.nPoints() - 1) - shape.time(0))/(shape.nPoints() - 1);
  m_startTime = shape.time(0) + shift;
}


SimpleShape::SimpleShape(const ShapeInfo& shapeInfo, double scale, double shift, bool samplingTimeOnly)
{
  for (unsigned int i = 0; i < shapeInfo.nPoints(); i++) {
    if (samplingTimeOnly && shapeInfo.phase(i) != 0) continue;
    m_values.push_back(shapeInfo.value(i)*scale);
    m_errors.push_back(0);
  }

  double timeInterval = 1.0*Definitions::samplingInterval/(samplingTimeOnly ? 1 : shapeInfo.nIntervals());
  m_timeInterval = timeInterval;
  m_startTime = shift - shapeInfo.shift();
}


double SimpleShape::time(unsigned int i) const 
{ 
  return startTime() + i*timeInterval();
}


SimpleShape* SimpleShape::diff() const
{
  SimpleShape* diff = new SimpleShape(nPoints(), timeInterval(), startTime());
  for (unsigned int i = 0; i < nPoints(); i++) {
    double val;
    interpolateDiff(time(i), val);
    diff->set(i, val, 0);
  }
  return diff;
}


SimpleShape* SimpleShape::add(const AbsShape& other, double scale, double shift)
{
  std::vector<double> values, errors;
  for (unsigned int k = 0; k < nPoints(); ++k) {
    double val, err;
    int pb = other.interpolate(time(k) - shift, val, err);
    if (pb != 0) val = err = 0;
    //if (pb == 1) break; //overflow: stop. underflow is OK : shape is assumed to be 0
    values.push_back(this->value(k) + scale*val);
    errors.push_back(TMath::Sqrt(TMath::Power(this->error(k),2) + TMath::Power(scale*err, 2)));
  }

  return new SimpleShape(values, errors, timeInterval(), startTime());
}


bool SimpleShape::add(unsigned int k, double val, double err)
{
  m_values[k] = m_values[k] + val;
  m_errors [k] = TMath::Sqrt(m_errors[k]*m_errors[k] + err*err);
  return true;
}


SimpleShape* SimpleShape::createEmpty() const
{
  return new SimpleShape(nPoints(), timeInterval(), startTime());
}


TH1D* SimpleShape::histogram(const char* name, const char* title, bool timeInUnitOfSamples) const
{
  double xMin = (timeInUnitOfSamples ? -1.5 : time(0) - 1.5*timeInterval());
  double xMax = (timeInUnitOfSamples ? nPoints() + 0.5 : time(nPoints()) + 0.5*timeInterval());
  TH1D* h = new TH1D(name, title, nPoints() + 2, xMin, xMax);
  h->GetXaxis()->SetTitle(timeInUnitOfSamples ? "Sample Index" : "Time (ns)");
  h->GetYaxis()->SetTitle("ADC counts");
  h->GetYaxis()->SetTitleOffset(1.1);
  h->SetMarkerStyle(20);
  h->SetMarkerSize(1);
  
  for (unsigned int i = 0; i < nPoints(); i++) {
    h->SetBinContent(i + 2, value(i));
    if (error(i) > 0) h->SetBinError(i + 2, error(i));
  }

  return h;
}


bool SimpleShape::add(SimpleShape*& s1, const AbsShape* s2)
{
  if (!s2) return false;
  if (!s1) {
    s1 = new SimpleShape(*s2);
    return true;
  }
  SimpleShape* sum = s1->add(*s2);
  if (!sum) return false;
  
  delete s1;
  s1 = sum;
  return true;
}


bool SimpleShape::scaleAndShift(SimpleShape*& s1, double scale, double shift)
{
  if (!s1) return 0;
  SimpleShape* newShape = new SimpleShape(*s1, scale, shift);
  delete s1;
  s1 = newShape;
  return true;
}
