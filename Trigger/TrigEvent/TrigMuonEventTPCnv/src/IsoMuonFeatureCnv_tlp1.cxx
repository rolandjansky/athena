/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigMuonEventTPCnv/IsoMuonFeatureCnv_tlp1.h"


//constructor
IsoMuonFeatureCnv_tlp1::IsoMuonFeatureCnv_tlp1(){

  // add the "main" base class converter (ie. IsoMuonFeatureCnv)
  addMainTPConverter();
}

void IsoMuonFeatureCnv_tlp1::setPStorage( IsoMuonFeature_tlp1* storage ){


   //for the base class converter
   setMainCnvPStorage( &storage->m_isoMu );

}
