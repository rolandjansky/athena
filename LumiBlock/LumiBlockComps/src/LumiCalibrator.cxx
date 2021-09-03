/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LumiCalibrator.h"

#include "CoralBase/Attribute.h"
#include "CoralBase/Blob.h"
#include "CoolKernel/StorageType.h"

#include <cmath>
#include <iostream>

//--------------------------------------------------                                                                  

LumiCalibrator::LumiCalibrator() :
  m_nPar(0),
  m_fType(""),
  m_muToLumi(0.)
{
  m_parVec.clear();
}

// Return false on error
bool 
LumiCalibrator::setCalibration(const coral::AttributeList& attrList)
{
  if (attrList["NumOfParameters"].isNull()) return false;
  m_nPar = attrList["NumOfParameters"].data<uint32_t>();

  if (attrList["Function"].isNull()) return false;
  m_fType = attrList["Function"].data<std::string>();

  if (attrList["MuToLumi"].isNull()) return false;
  m_muToLumi = attrList["MuToLumi"].data<float>();

  if (attrList["Parameters"].isNull()) return false;
  const coral::Blob& blob = attrList["Parameters"].data<coral::Blob>();


  // Verify length
  if ( static_cast<uint32_t>( blob.size() ) != 4*m_nPar) return false;

  // Read calibration parameters
  const float* p = static_cast<const float*>(blob.startingAddress());
  for (unsigned int i=0; i<m_nPar; i++, p++) 
    m_parVec.push_back(*p);

  return true;
}

// Return False on error
bool
LumiCalibrator::calibrateLumi(float rawLumi, float& calLumi) const
{
  calLumi = 0;
  float calMu = 0.;
  if (!calibrateMu(rawLumi, calMu)) return false;
  calLumi = calMu * m_muToLumi;
  return true;
}
 
// Return False on error                                                                                            
bool 
LumiCalibrator::calibrateMu(float rawLumi, float& calMu) const 
{
  calMu = 0.;

  if (m_fType == "Polynomial") {

    // Check parameter length
    if (m_parVec.empty()) return false;
 
    unsigned int nrange = m_parVec[0];

    // Check parameter size again
    if (m_parVec.size() < (8*nrange + 1)) return false;

    for (unsigned int i=0; i<nrange; i++) {
      float rmin = m_parVec[8*i+1];
      float rmax = m_parVec[8*i+2];
      if (rawLumi < rmax and rawLumi >= rmin) {
	calMu += m_parVec[8*i+3] * pow(rawLumi, 0);
	calMu += m_parVec[8*i+4] * pow(rawLumi, 1);
	calMu += m_parVec[8*i+5] * pow(rawLumi, 2);
	calMu += m_parVec[8*i+6] * pow(rawLumi, 3);
	calMu += m_parVec[8*i+7] * pow(rawLumi, 4);
	calMu += m_parVec[8*i+8] * pow(rawLumi, 5);
	break;
      }
    }
    return true;
  }

  if (m_fType == "Logarithm") {

    // Check parameter length
    if (m_parVec.size() != 1) return false;

    // Check input for physically allowed range
    if ((1.-rawLumi) <= 0.) return false;

    // Convert negative luminosity to zero
    if (rawLumi < 0.) return false;

    calMu = -m_parVec[0] * log(1.-rawLumi);
    return true;
  }

  if (m_fType == "HitLogarithm") {

    // Check parameter length
    if (m_parVec.size() != 4) return false;

    // Channel count must be > 0
    if (m_parVec[1] <= 0.) return false;

    // Check input value for physically allowed range
    if ((1.-rawLumi/m_parVec[1]) <= 0.) return false;

    // Convert negative luminosity to zero
    if (rawLumi < 0.) return false;

    calMu = -m_parVec[0] * log(1.-rawLumi/m_parVec[1])/(1.-m_parVec[2]);
    return true;
  }

  if (m_fType == "LookupTable_EventAND_Lin") {

    // Check parameter size
    if (m_parVec.size() != 6) return false;

    if (rawLumi < 0.) return false;

    float sigo = m_parVec[0];
    float siga = m_parVec[1];
    calMu = m_parVec[5] * getMuVis(rawLumi, sigo, siga);

    if (calMu < 0.) { // Indicates an error, try again
      calMu = m_parVec[5] * getMuVis2(rawLumi, sigo, siga);
    }

    if (calMu < 0.) {  // Indicates an error
      calMu = 0.;
      return false;
    }

    return true;
  }

  if (m_fType == "LookupTable_EventAND_Log") {

    // Check parameter size
    if (m_parVec.size() != 8) return false;

    if (rawLumi < 0.) return false;

    float sigo = m_parVec[0];
    float siga = m_parVec[1];
    calMu = m_parVec[7] * getMuVis(rawLumi, sigo, siga);

    if (calMu < 0.) { // Indicates an error, try again
      calMu = m_parVec[7] * getMuVis2(rawLumi, sigo, siga);
    }

    if (calMu < 0.) {  // Indicates an error
      calMu = 0.;
      return false;
    }

    return true;
  }

  // Unknown type
  return false;
}

// Vincent's Newton-Rhapson method
float
LumiCalibrator::getMuVis(float rawPerBX, float sigo, float siga) 
{

  //std::cout << "getMuVis("<<rawPerBX<<","<<sigo<<","<<siga<<")"<<std::endl;

  float mu, y, dy;
  float munew = 0.01;
  float a = (sigo/siga + 1) / 2.;
  float b = sigo/siga;

  // Set a fixed number of cycles, but break if we converge faster
  for (int i=0; i<30; i++) {
    mu = munew;
    y = rawPerBX - 1. - std::exp(-b * mu) + 2. * std::exp(-a * mu);
    dy = b * std::exp(-b * mu) - 2. * a * std::exp(-a * mu);
    munew = mu-y/dy;

    //std::cout << i <<" - munew: " << munew << " mu:" << mu << " diff:" 
    //	      << fabs(munew-mu) << std::endl;

    // Protect against unphysical values
    if (munew <= 0.) return -1.;
    if (std::abs(munew-mu)/munew < 1.E-5) break;
  }

  return munew; 
}

// Mika's original brute-force method
float rpbx(float sr, float mu) {
  return 1. - 2.*std::exp(-(1+sr)*mu/2.) + std::exp(-sr*mu);
}

float
LumiCalibrator::getMuVis2(float rawPerBX, float sigo, float siga) 
{
  float muvl=1e-10;
  float muvu=100.;
  float muvm=10.;
  float sr=sigo/siga;
  float rbxl=rpbx(sr,muvl);
  float rbxu=rpbx(sr,muvu);
  float rbxm=rpbx(sr,muvm);

  // Check that starting value is in the valid range
  if (rawPerBX < rbxl || rawPerBX > rbxu) return -1.;

  int i=0;
  while (++i <= 50) {
    if (rbxl<rawPerBX && rbxm>rawPerBX) {
      rbxu=rbxm;
      muvu=muvm;
      muvm=0.5*(muvu+muvl);
    } else {
      rbxl=rbxm;
      muvl=muvm;
      muvm=0.5*(muvu+muvl);
    }

    rbxm = rpbx(sr, muvm);
    //std::cout << i <<" - munew: " << muvu << " mu:" << muvl << " diff:" << fabs(muvu-muvl) << std::endl;

    if ((muvu-muvl)/muvl < 1e-5) return muvm;
  }

  // Didn't converge
  return -1.;
}

// Dump out parameters
MsgStream&
LumiCalibrator::dump(MsgStream &stream) const {
  stream << m_fType << " Npar = " << m_nPar << " MuToLumi = " << m_muToLumi << " ParVec: " << m_parVec;
  return stream;
}
