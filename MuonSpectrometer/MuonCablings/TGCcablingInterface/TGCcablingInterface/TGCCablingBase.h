/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
         TGCcablingBase.h

    Author  : H.Kurashige           Aug. 2007
    Email   : Hisaya.Kurashige@cern.ch
    Description : It is  a dummy class for outside package 
                  to give a pointer of LVL1TGCCabling12::TGCCabling

***************************************************************************/

#ifndef TGCCABLINGBASE_HH
#define TGCCABLINGBASE_HH

#include <string>
  
class TGCIdBase;

class TGCCablingBase
{
 public:
  TGCCablingBase ( ) {}
 
  virtual ~TGCCablingBase (void){};
  
  virtual TGCIdBase* getASDOutChannel(const TGCIdBase* slb_in) const =0;

  
};

#endif
