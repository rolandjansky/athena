/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUETMISSL1OBJECT_H
#define TRIGTAUETMISSL1OBJECT_H
//=====================================================================
//  TrigTauEtMissL1Object - contains EtMiss variables
//
//  author : Sarah Demers (Sarah.Demers@cern.ch) 01.2007
//=====================================================================


#include "TObject.h"
#include "TrigTauPerformNtuple/TrigTauEtMissObject.h"


class TrigTauEtMissL1Object : public TrigTauEtMissObject
{
 public :
  TrigTauEtMissL1Object();
  TrigTauEtMissL1Object(const TrigTauEtMissL1Object & other);

  virtual ~TrigTauEtMissL1Object();
  virtual void reset();

  //----------- public data members
  float m_EtMissHit;
  float m_EtSumHit;

 private:
  ClassDef(TrigTauEtMissL1Object, 2)  ;
};

#endif //TRIGTAUETMISSL1OBJECT_H
