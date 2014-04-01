/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigEMClusterContainerCnv_tlp1
 *
 * @brief "top level" transient-persistent converter for TrigEMClusterContainer
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 * @author Francesca Bucci  <F.Bucci@cern.ch>          - U. Geneva
 *
 * File and Version Information:
 * $Id: TrigEMClusterContainerCnv_tlp1.h,v 1.2 2009-04-01 22:01:37 salvator Exp $
 **********************************************************************************/
#ifndef  TRIGCALOEVENTTPCNV_TRIGEMCLUSTERCONTAINER_CNV_TLP1_H
#define  TRIGCALOEVENTTPCNV_TRIGEMCLUSTERCONTAINER_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigCaloEventTPCnv/TrigEMClusterContainer_tlp1.h"
 
#include "TrigCaloEventTPCnv/TrigEMClusterContainerCnv_p1.h"
#include "TrigCaloEventTPCnv/TrigEMClusterCnv_p1.h" 
#include "TrigCaloEventTPCnv/TrigEMClusterCnv_p2.h" 
#include "TrigCaloEventTPCnv/TrigCaloClusterCnv_p1.h" 
#include "TrigCaloEventTPCnv/TrigCaloClusterCnv_p2.h" 

 
class TrigEMClusterContainerCnv_tlp1 : public AthenaPoolTopLevelTPConverter< TrigEMClusterContainerCnv_p1, TrigEMClusterContainer_tlp1 >
{
 
 public:
 
  TrigEMClusterContainerCnv_tlp1();
  virtual ~TrigEMClusterContainerCnv_tlp1() {}
 
  //default methods  
  virtual void  setPStorage( TrigEMClusterContainer_tlp1 *storage );
  
 protected: 
  TrigEMClusterContainerCnv_p1    m_trigEMClusterContCnv;
  TrigEMClusterCnv_p1             m_EMClusterCnv;
  TrigCaloClusterCnv_p1          m_CaloClusterCnv;
  TrigEMClusterCnv_p2             m_EMClusterCnv_p2;
  TrigCaloClusterCnv_p2          m_CaloClusterCnv_p2;
 
}; //end of class definitions
 

#endif
