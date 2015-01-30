/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCosmicEventTPCnv
 * @Class  : CosmicMuonCollectionCnv_tlp1
 *
 * @brief "top-level" transient-persistent converter for CosmicMuonCollection_tlp1
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 *
 * File and Version Information:
 * $Id: CosmicMuonCollectionCnv_tlp1.h,v 1.2 2009-04-01 22:02:51 salvator Exp $
 **********************************************************************************/
#ifndef  TRIGCOSMICEVENTTPCNV_COSMICMUONCOLLECTION_CNV_TLP1_H
#define  TRIGCOSMICEVENTTPCNV_COSMICMUONCOLLECTION_CNV_TLP1_H
 
#include "AthenaPoolCnvSvc/AthenaPoolTopLevelTPConverter.h"

#include "TrigCosmicEventTPCnv/CosmicMuonCollectionCnv_p1.h"   

#include "TrigCosmicEventTPCnv/CosmicMuonCollection_tlp1.h"
 
 
class CosmicMuonCollectionCnv_tlp1 : public AthenaPoolTopLevelTPConverter< CosmicMuonCollectionCnv_p1, CosmicMuonCollection_tlp1 >
{
 
 public:
 
  CosmicMuonCollectionCnv_tlp1();
  virtual ~CosmicMuonCollectionCnv_tlp1() {}
 
  //default methods  
  virtual void  setPStorage( CosmicMuonCollection_tlp1 *storage );
 
 protected: 
  CosmicMuonCollectionCnv_p1    m_cosmicMuonCollectionCnv;
  CosmicMuonCnv_p1              m_cosmicMuonCnv;
 
}; //end of class definitions
 

#endif
