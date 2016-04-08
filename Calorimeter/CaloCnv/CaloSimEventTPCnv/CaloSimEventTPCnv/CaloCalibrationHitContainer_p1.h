/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CALOSIMEVENTTPCNV_CALOCALIBRATIONHITCONTAINER_P1_H
#define CALOSIMEVENTTPCNV_CALOCALIBRATIONHITCONTAINER_P1_H

#include <vector>
#include <string>
#include "CaloSimEventTPCnv/CaloCalibrationHit_p1.h"

class CaloCalibrationHitContainer_p1   
{
 public:
  /// typedefs
  typedef std::vector<CaloCalibrationHit_p1> HitVector;
  typedef HitVector::const_iterator const_iterator;
  typedef HitVector::iterator       iterator;
  
  /// Default constructor
  CaloCalibrationHitContainer_p1();
  
  // Accessors
  const std::string&  name() const; 
  const HitVector&    getVector() const;
  //private:
  std::vector<CaloCalibrationHit_p1>   m_cont; 
  std::string                          m_name;
};

// inlines

inline CaloCalibrationHitContainer_p1::CaloCalibrationHitContainer_p1() {}

inline const std::string& CaloCalibrationHitContainer_p1::name() const {return m_name;}

inline const std::vector<CaloCalibrationHit_p1>& CaloCalibrationHitContainer_p1::getVector() const {return m_cont;}

#endif
