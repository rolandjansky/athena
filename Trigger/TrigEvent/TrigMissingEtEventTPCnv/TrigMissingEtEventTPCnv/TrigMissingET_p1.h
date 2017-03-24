/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMissingEtEventTPCnv
 * @class  : TrigMissingET_p1
 *
 * @brief persistent partner for TrigMissingET
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigMissingET_p1.h,v 1.2 2009-04-01 22:10:11 salvator Exp $
 **********************************************************************************/
#ifndef TRIGMISSINGETEVENTTPCNV_TRIGMISSINGET_P1_H
#define TRIGMISSINGETEVENTTPCNV_TRIGMISSINGET_P1_H

#include <stdint.h>
#include <string>
#include "TrigMissingEtEvent/TrigMissingET.h"

class TrigMissingET_p1 
{
  friend class TrigMissingETCnv;

 public:
  
  TrigMissingET_p1() {}
  virtual ~TrigMissingET_p1(){}
  
  //private:
  double m_ex = 0;
  double m_ey = 0;
  double m_sum_et = 0;
  long   m_roiWord = 0;

};

#endif
