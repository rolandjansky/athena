/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEventTPCnv/TrigTauTracksInfoCollectionCnv_tlp1.h"


//constructor
TrigTauTracksInfoCollectionCnv_tlp1::TrigTauTracksInfoCollectionCnv_tlp1(){

  // add the "main" base class converter (ie. TrigTauTracksInfoCollectionCnv)
  addMainTPConverter();

  // add all converters needed in the top level converter
  // do not change the order of adding converters
  addTPConverter( &m_trigTauTracksInfoCnv );
  addTPConverter( &m_p4PtEtaPhiMCnv);

}

void TrigTauTracksInfoCollectionCnv_tlp1::setPStorage( TrigTauTracksInfoCollection_tlp1* storage ){


   //for the base class converter
   setMainCnvPStorage( &storage->m_trigTauTracksInfoCollectionVec );

   //for all other converters defined in the base class
   m_trigTauTracksInfoCnv.setPStorage( &storage->m_trigTauTracksInfoVec );
   m_p4PtEtaPhiMCnv.setPStorage(&storage->m_p4PtEtaPhiM);

}

