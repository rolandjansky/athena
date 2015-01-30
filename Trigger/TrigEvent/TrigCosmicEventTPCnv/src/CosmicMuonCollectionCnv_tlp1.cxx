/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCosmicEventTPCnv/CosmicMuonCollectionCnv_tlp1.h"


//constructor
CosmicMuonCollectionCnv_tlp1::CosmicMuonCollectionCnv_tlp1(){

  // add the "main" base class converter (ie. CosmicMuonCollectionCnv)
  addMainTPConverter();

  // add all converters needed in the top level converter
  // do not change the order of adding converters
  addTPConverter( &m_cosmicMuonCnv );


}

void CosmicMuonCollectionCnv_tlp1::setPStorage( CosmicMuonCollection_tlp1* storage ){


   //for the base class converter
   setMainCnvPStorage( &storage->m_cosmicMuonCollectionVec );

   //for all other converters defined in the base class
   m_cosmicMuonCnv.setPStorage( &storage->m_cosmicMuonVec );

}
