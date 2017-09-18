/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TFCS2Function_h
#define TFCS2Function_h

#include "ISF_FastCaloSimParametrization/TFCS2DFunction.h"

class TFCS2Function {
  public:
  static TFCS2DFunction* Create(std::string, int, std::string, std::string, int, float,bool);
  virtual ~TFCS2Function() {}
  private:

  ClassDef(TFCS2Function,1)  //TFCS2Function
};

#endif
