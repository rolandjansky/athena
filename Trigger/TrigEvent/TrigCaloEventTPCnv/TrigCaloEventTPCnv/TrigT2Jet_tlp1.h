/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigT2Jet_tlp1
 *
 * @brief "top level" persistent partner for TrigT2Jet
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigT2Jet_tlp1.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCALOEVENTTPCNV_TRIGT2JET_TLP1_H
#define TRIGCALOEVENTTPCNV_TRIGT2JET_TLP1_H

#include "TrigCaloEventTPCnv/TrigT2Jet_p1.h"

class TrigT2Jet_tlp1 
{
   
 public:
   
  TrigT2Jet_tlp1(){}
  friend class TrigT2JetCnv_tlp1;

 private:

  std::vector<TrigT2Jet_p1>    m_T2Jet;
  
};//end of class definitions
 
#endif
