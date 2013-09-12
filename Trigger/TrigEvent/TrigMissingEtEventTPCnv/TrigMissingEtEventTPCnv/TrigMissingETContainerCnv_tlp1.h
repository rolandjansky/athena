/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigMissingEtEventTPCnv
 * @Class  : TrigMissingETCollectionCnv_tlp1
 *
 * @brief "top level" transient-persistent converter for TrigMissingETCollection
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigMissingETContainerCnv_tlp1.h,v 1.2 2009-04-01 22:10:11 salvator Exp $
 **********************************************************************************/
#ifndef  TRIGMISSINGETEVENTTPCNV_TrigMissingETContainer_CNV_TLP1_H
#define  TRIGMISSINGETEVENTTPCNV_TrigMissingETContainer_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigMissingEtEventTPCnv/TrigMissingETContainerCnv_p1.h"   
#include "TrigMissingEtEventTPCnv/TrigMissingETCnv_p1.h"   
#include "TrigMissingEtEventTPCnv/TrigMissingETCnv_p2.h"   

#include "TrigMissingEtEventTPCnv/TrigMissingETContainer_tlp1.h"
 

 
class TrigMissingETContainerCnv_tlp1 : public AthenaPoolTopLevelTPConverter< TrigMissingETContainerCnv_p1, TrigMissingETContainer_tlp1 >
{
 
 public:
 
  TrigMissingETContainerCnv_tlp1();
  virtual ~TrigMissingETContainerCnv_tlp1() {}
 
  //default methods  
  virtual void  setPStorage( TrigMissingETContainer_tlp1 *storage );
  
 protected: 
  TrigMissingETContainerCnv_p1    m_TrigMissingETContainerCnv_p1;
  TrigMissingETCnv_p1             m_TrigMissingETCnv_p1;
  TrigMissingETCnv_p2             m_TrigMissingETCnv_p2;

 
}; //end of class definitions
 

#endif
