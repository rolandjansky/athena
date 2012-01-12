/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUETMISSOBJECT_H
#define TRIGTAUETMISSOBJECT_H
//=====================================================================
//  TrigTauEtMissObject - contains EtMiss variables
//
//  author : Sarah Demers (Sarah.Demers@cern.ch) 01.2007
//=====================================================================


#include "TObject.h"

class TrigTauEtMissObject : public TObject
{
 public :
  TrigTauEtMissObject();
  TrigTauEtMissObject(const TrigTauEtMissObject & other);

  virtual ~TrigTauEtMissObject();
  virtual void reset();

  //----------- public data members
  float m_Ex;
  float m_Ey;
  float m_EtMiss;
  float m_EtSum;   

 private:
  ClassDef(TrigTauEtMissObject, 1)  ;
};

#endif //TRIGTAUETMISSOBJECT_H
