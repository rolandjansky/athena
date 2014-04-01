/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCaloEventTPCnv
 * @Class  : TrigTauClusterDetailsContainerCnv_tlp1
 *
 * @brief "top level" transient-persistent converter for TrigTauClusterDetailsContainer
 *
 * @author M. Pilar Casado  <casado@ifae.es>  - IFAE-Barcelona
**********************************************************************************/
#ifndef  TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERDETAILSCONTAINER_CNV_TLP1_H
#define  TRIGCALOEVENTTPCNV_TRIGTAUCLUSTERDETAILSCONTAINER_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigCaloEventTPCnv/TrigTauClusterDetailsContainer_tlp1.h"
 
#include "TrigCaloEventTPCnv/TrigTauClusterDetailsContainerCnv_p1.h"   
#include "TrigCaloEventTPCnv/TrigTauClusterDetailsCnv_p1.h" 

 
class TrigTauClusterDetailsContainerCnv_tlp1 : public AthenaPoolTopLevelTPConverter< TrigTauClusterDetailsContainerCnv_p1, TrigTauClusterDetailsContainer_tlp1 >
{
 
 public:
 
  TrigTauClusterDetailsContainerCnv_tlp1();
  virtual ~TrigTauClusterDetailsContainerCnv_tlp1() {}
 
  //default methods  
  virtual void  setPStorage( TrigTauClusterDetailsContainer_tlp1 *storage );
  
 protected: 
  TrigTauClusterDetailsContainerCnv_p1    m_trigTauClusterContCnv;
  TrigTauClusterDetailsCnv_p1             m_TauClusterDetailsCnv;
 
}; //end of class definitions

#endif
