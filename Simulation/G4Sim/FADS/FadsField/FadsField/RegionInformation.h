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
  FADS::FieldIntParameters* fieldIntPara;

 public:
  inline FADS::FieldIntParameters* GetFieldIntParameters() const
  { return fieldIntPara; }
  inline void SetFieldIntParameters(FADS::FieldIntParameters* fip)
  { fieldIntPara = fip; }
};

}

#endif

