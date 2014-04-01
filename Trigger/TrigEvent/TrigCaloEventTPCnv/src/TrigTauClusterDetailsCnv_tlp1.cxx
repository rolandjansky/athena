/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEventTPCnv/TrigTauClusterDetailsCnv_tlp1.h"


//constructor
TrigTauClusterDetailsCnv_tlp1::TrigTauClusterDetailsCnv_tlp1(){

  // add the "main" base class converter (ie. TrigTauClusterDetailsCnv)
  addMainTPConverter();

}

void TrigTauClusterDetailsCnv_tlp1::setPStorage( TrigTauClusterDetails_tlp1* storage ){


   //for the base class converter
   setMainCnvPStorage( &storage->m_trigTauCluster );

}

