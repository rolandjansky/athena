/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARELECCALIB_ILARADC2GEV_H
#define LARELECCALIB_ILARADC2GEV_H

#ifndef CLIDSVC_CLASSDEF_H
#include "CLIDSvc/CLASS_DEF.h"
#endif

#include "CaloIdentifier/LArID.h"

#include <vector> 

class ILArAdc2GeV {

public: 

  virtual ~ILArAdc2GeV() {};  
 
  virtual const std::vector<float>& AllFactors(const Identifier& id) const = 0 ;
  virtual const std::vector<float>& Ramp(const Identifier& id) const = 0 ;
  virtual float Dac2Volt() const = 0 ;
  virtual float Volt2uA(const Identifier& id) const = 0 ;
  virtual float uA2GeV(const Identifier& id) const = 0 ;
  virtual float HVcorrEC(const Identifier& id) const = 0 ;

};

CLASS_DEF( ILArAdc2GeV , 122618675 , 1 )

#endif 
