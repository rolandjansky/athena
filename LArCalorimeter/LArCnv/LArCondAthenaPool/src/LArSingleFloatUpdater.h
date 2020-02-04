/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//Dear emacs, this is -*-c++-*-

#ifndef LARSINGLEFLOATUPDATER_H
#define LARSINGLEFLOATUPDATER_H

//class LArMphysOverMcalP1;
#include "LArRawConditions/LArMphysOverMcalP.h"
#include "LArRawConditions/LArDAC2uAP.h"
#include "LArRawConditions/LAruA2MeVP.h"
#include "LArRawConditions/LArfSamplP.h"


class LArDAC2uAP;

class LArSingleFloatUpdater {
 public:
  template<class P>
  void update(const LArConditionsSubset<P>* old,
	      LArConditionsSubset<LArSingleFloatP>* newObj)
  {
    newObj->assign (*old,
                    [] (const P& from,
                        LArSingleFloatP& to)
                    { to.m_data = getPayload(from); });
  }

 private:
  static float getPayload(const LArMphysOverMcalP& p) {
    return p.m_MphysOverMcal;
  }

//  float getPayload(const LArMphysOverMcalP1& p) {
//  return p.m_MphysOverMcal;
//}

  static float getPayload(const LArDAC2uAP& p) {
    return p.m_DAC2uA;
  }
  static float getPayload(const LAruA2MeVP& p) {
    return p.m_uA2MeV;
  }

  static float getPayload(const LArfSamplP& p) {
    return p.m_fSampl;
  }

  
};

#endif

