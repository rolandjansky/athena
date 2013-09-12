/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**********************************************************************************
 * @Project: Trigger
 * @Package: TrigCosmicEventTPCnv
 * @Class  : CosmicMuonCollectionCnv_p1
 *
 * @brief transient-persistent converter for CosmicMuonCollection_p1
 *
 * @author Andrew Hamilton  <Andrew.Hamilton@cern.ch>  - U. Geneva
 *
 * File and Version Information:
 * $Id: CosmicMuonCollectionCnv_p1.h,v 1.2 2009-04-01 22:02:51 salvator Exp $
 **********************************************************************************/
#ifndef TRIGCOSMICEVENTTPCNV_COSMICMUONCONTAINER_CNV_P1_H
#define TRIGCOSMICEVENTTPCNV_COSMICMUONCONTAINER_CNV_P1_H
  
#include "AthenaPoolCnvSvc/T_AthenaPoolTPConverter.h"
 
#include "TrigCosmicEvent/CosmicMuonCollection.h"

#include "TrigCosmicEventTPCnv/CosmicMuonCollection_p1.h"
#include "TrigCosmicEventTPCnv/CosmicMuonCnv_p1.h"

class CosmicMuonCollectionCnv_p1 : public T_AthenaPoolTPPtrVectorCnv< CosmicMuonCollection, CosmicMuonCollection_p1, CosmicMuonCnv_p1 >

{
 
 public:
  CosmicMuonCollectionCnv_p1() {}
 
}; //end of class definitions
 
#endif
