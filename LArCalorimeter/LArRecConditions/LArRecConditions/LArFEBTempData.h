/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARRECCONDITIONS_LARFEBTEMPDATA_H
#define LARRECCONDITIONS_LARFEBTEMPDATA_H

#include "Identifier/HWIdentifier.h"
#include <vector>
#include <map>

class LArFEBTempData {

  friend class LArFEBTempAlg; //The conditions alg filling this object

 public:

  typedef std::pair<float,float> FEBTemp; 
  LArFEBTempData(){};

  // Given a FEB ID, return std::vector of temp1 and temp2 values
  FEBTemp getFebTemp( const HWIdentifier& id ) const ;
  

 private:
  std::map<HWIdentifier,FEBTemp> m_mapVec;

};


//Inline methods:
inline LArFEBTempData::FEBTemp LArFEBTempData::getFebTemp(const HWIdentifier& h) const{
  return m_mapVec.at(h);
}

#include "AthenaKernel/CLASS_DEF.h"
CLASS_DEF( LArFEBTempData, 37052556, 1)
#include "AthenaKernel/CondCont.h"
CLASS_DEF( CondCont<LArFEBTempData>, 52616604, 1)

#endif
