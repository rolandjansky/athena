/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigT2JetCnv_tlp1
 *
 * @brief "top level" transient-persistent converter for TrigT2Jet
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigT2JetCnv_tlp1.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef  TRIGCALOEVENTTPCNV_TRIGT2JETCNV_TLP1_H
#define  TRIGCALOEVENTTPCNV_TRIGT2JETCNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigCaloEvent/TrigT2Jet.h"
#include "TrigCaloEventTPCnv/TrigT2Jet_tlp1.h"
#include "TrigCaloEventTPCnv/TrigT2JetCnv_p1.h"   

 
class TrigT2JetCnv_tlp1 : public AthenaPoolTopLevelTPConverter< TrigT2JetCnv_p1, TrigT2Jet_tlp1 >
{
 
 public:
 
  TrigT2JetCnv_tlp1();
  virtual ~TrigT2JetCnv_tlp1() {}
 
  //default methods  
  virtual void  setPStorage( TrigT2Jet_tlp1 *storage );
  
 protected: 
  TrigT2JetCnv_p1         m_trigT2JetCnv     ;
   
}; //end of class definitions
 

#endif
