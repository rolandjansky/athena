/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ISF_FASTCALOSIMEVENT_TFCS1DFunction_h
#define ISF_FASTCALOSIMEVENT_TFCS1DFunction_h

// STL includes
#include <string>

#include "TH1.h"
#include "TTree.h"

class TFCS1DFunction:public TObject
{
  public:
    TFCS1DFunction() {};
    ~TFCS1DFunction() {};

   virtual double rnd_to_fct(double rnd);
   static double get_maxdev(TH1* , TH1* );

  private:

  ClassDef(TFCS1DFunction,1)  //TFCS1DFunction

};

#endif
