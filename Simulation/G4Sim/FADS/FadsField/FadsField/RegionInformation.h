/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef RegionInformation_H
#define RegionInformation_H

#include "G4VUserRegionInformation.hh"
#include "FadsField/FieldIntParameters.h"

namespace FADS {

class RegionInformation : public G4VUserRegionInformation
{
 public:
  RegionInformation();
  virtual ~RegionInformation();

 public:
  void Print() const;

 private:
  FADS::FieldIntParameters* m_fieldIntPara;

 public:
  inline FADS::FieldIntParameters* GetFieldIntParameters() const
  { return m_fieldIntPara; }
  inline void SetFieldIntParameters(FADS::FieldIntParameters* fip)
  { m_fieldIntPara = fip; }
};

}

#endif

