/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGTAUETMISSL2OBJECT_H
#define TRIGTAUETMISSL2OBJECT_H
//=====================================================================
//  TrigTauEtMissL2Object - contains L2 EtMiss variables
//
//  author : Diego Casadei (Diego.Casadei@cern.ch) June 2008
//=====================================================================

#include <string>
#include "TObject.h"
#include "TrigTauPerformNtuple/TrigTauEtMissObject.h"


class TrigTauEtMissL2Object : public TrigTauEtMissObject
{
 public :
  TrigTauEtMissL2Object();
  TrigTauEtMissL2Object(const TrigTauEtMissL2Object & other);

  virtual ~TrigTauEtMissL2Object();
  virtual void reset();

  //----------- public data members
  // basic information: global quantities
  float m_Ez;   // longitudinal component of missing energy
  float m_SumE; // scalar sum of total energy
  int   m_flag; // event flag
  // auxiliar information: muons contribution
  std::string    m_c_name;
  short          m_c_status;
  float          m_c_ex;
  float          m_c_ey;
  float          m_c_ez;
  float          m_c_sumEt;
  float          m_c_sumE;
  float          m_c_calib0;
  float          m_c_calib1;
  short          m_c_sumOfSigns;
  unsigned short m_c_usedChannels;

 private:
  ClassDef(TrigTauEtMissL2Object, 1)  ;
};

#endif //TRIGTAUETMISSL2OBJECT_H
