/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "LArSamplesMon/ShapeErrorData.h"

#include <iostream>
using std::cout;
using std::endl;

using namespace LArSamples;


const TVectorD ShapeErrorData::xi(int first, int last) const
{ 
  if (!checkRange(first, last)) {
    cout << "ShapeErrorData::xi : range error" << endl;
    return TVectorD();
  }
  
  return m_xi.GetSub(first, last, "I"); // "I" => indexed as [first, last]
}


const TVectorD ShapeErrorData::xip(int first, int last) const
{ 
  if (!checkRange(first, last)) {
    cout << "ShapeErrorData::xip : range error" << endl;
    return TVectorD();
  }
  
  return m_xip.GetSub(first, last, "I"); // "I" => indexed as [first, last]
}


const CovMatrix ShapeErrorData::xiErr(int first, int last) const
{ 
  if (!checkRange(first, last)) {
    cout << "ShapeErrorData::xiErr : range error" << endl;
    return CovMatrix();
  }
  
  return m_xiErr.GetSub(first, last, first, last, "I"); // "I" => indexed as [first, last]
}


const CovMatrix ShapeErrorData::xipErr(int first, int last) const
{ 
  if (!checkRange(first, last)) {
    cout << "ShapeErrorData::xipErr : range error" << endl;
    return CovMatrix();
  }
  
  return m_xipErr.GetSub(first, last, first, last, "I"); // "I" => indexed as [first, last]
}


ShapeErrorData* ShapeErrorData::add(const ShapeErrorData& other) const
{
  int newLwb = commonLwb(other);
  int newUpb = commonUpb(other);
  if (newUpb < newLwb) return nullptr;
  TVectorD newXi = xi(newLwb, newUpb) + other.xi(newLwb, newUpb);
  TVectorD newXip = xip(newLwb, newUpb) + other.xip(newLwb, newUpb);
  CovMatrix newXiErr = xiErr(newLwb, newUpb)  + other.xiErr(newLwb, newUpb);
  CovMatrix newXipErr = xipErr(newLwb, newUpb)  + other.xipErr(newLwb, newUpb);
  int newN = n() + other.n();
  return new ShapeErrorData(newXi, newXip, newXiErr, newXipErr, newN);
}
