/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigT2JetContainerCnv_tlp1
 *
 * @brief "top level" transient-persistent converter for TrigT2JetContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 * @modified Tobias Kruker  <kruker@cern.ch>					 - U. Bern
 *
 * File and Version Information:
 * $Id: TrigT2JetContainerCnv_tlp1.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef  TRIGCALOEVENTTPCNV_TRIGT2JETCONTAINER_CNV_TLP1_H
#define  TRIGCALOEVENTTPCNV_TRIGT2JETCONTAINER_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigCaloEventTPCnv/TrigT2JetContainer_tlp1.h"
#include "TrigCaloEventTPCnv/TrigT2JetContainerCnv_p1.h"   
#include "TrigCaloEventTPCnv/TrigT2JetCnv_p1.h"  // added by tobias
#include "TrigCaloEventTPCnv/TrigT2JetCnv_p2.h"  // added by tobias
 
class TrigT2JetContainerCnv_tlp1 : public AthenaPoolTopLevelTPConverter< TrigT2JetContainerCnv_p1, TrigT2JetContainer_tlp1 >
{
 
 public:
 
  TrigT2JetContainerCnv_tlp1();
  virtual ~TrigT2JetContainerCnv_tlp1() {}
 
  //default methods  
  virtual void  setPStorage( TrigT2JetContainer_tlp1 *storage );
  
 protected: 
  TrigT2JetContainerCnv_p1    m_trigT2JetContCnv;
  TrigT2JetCnv_p1             m_T2JetCnv;
  TrigT2JetCnv_p2             m_T2JetCnv_p2;
 
}; //end of class definitions


#endif
