/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigTauClusterContainerCnv_tlp1
 *
 * @brief "top level" transient-persistent converter for TrigTauClusterContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigTauClusterContainerCnv_tlp1.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef  TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERCONTAINER_CNV_TLP1_H
#define  TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERCONTAINER_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigCaloEventTPCnv/TrigTauClusterContainer_tlp1.h"
 
#include "TrigCaloEventTPCnv/TrigTauClusterContainerCnv_p1.h"   
#include "TrigCaloEventTPCnv/TrigTauClusterCnv_p1.h" 
#include "TrigCaloEventTPCnv/TrigTauClusterCnv_p2.h" 
#include "TrigCaloEventTPCnv/TrigCaloClusterCnv_p1.h" 
#include "TrigCaloEventTPCnv/TrigCaloClusterCnv_p2.h" 

 
class TrigTauClusterContainerCnv_tlp1 : public AthenaPoolTopLevelTPConverter< TrigTauClusterContainerCnv_p1, TrigTauClusterContainer_tlp1 >
{
 
 public:
 
  TrigTauClusterContainerCnv_tlp1();
  virtual ~TrigTauClusterContainerCnv_tlp1() {}
 
  //default methods  
  virtual void  setPStorage( TrigTauClusterContainer_tlp1 *storage );
  
 protected: 
  TrigTauClusterContainerCnv_p1    m_trigTauClusterContCnv;
  TrigTauClusterCnv_p1             m_TauClusterCnv;
  TrigCaloClusterCnv_p1            m_CaloClusterCnv;
  TrigTauClusterCnv_p2             m_TauClusterCnv_p2;
  TrigCaloClusterCnv_p2            m_CaloClusterCnv_p2;
 
}; //end of class definitions
 

#endif
