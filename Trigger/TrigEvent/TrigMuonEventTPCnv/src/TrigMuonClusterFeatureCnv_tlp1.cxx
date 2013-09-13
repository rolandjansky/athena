/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEventTPCnv/TrigMuonClusterFeatureCnv_tlp1.h"


//constructor
TrigMuonClusterFeatureCnv_tlp1::TrigMuonClusterFeatureCnv_tlp1(){

  // add the "main" base class converter (ie. TrigMuonClusterFeatureCnv)
  addMainTPConverter();
}

void TrigMuonClusterFeatureCnv_tlp1::setPStorage( TrigMuonClusterFeature_tlp1* storage ){


   //for the base class converter
   setMainCnvPStorage( &storage->m_trigMuCl );

}
