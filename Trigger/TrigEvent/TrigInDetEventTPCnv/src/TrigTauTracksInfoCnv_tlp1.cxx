/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEventTPCnv/TrigTauTracksInfoCnv_tlp1.h"


//constructor
TrigTauTracksInfoCnv_tlp1::TrigTauTracksInfoCnv_tlp1(){

  // add the "main" base class converter (ie. TrigTauTracksInfoCnv)
  addMainTPConverter();

  // add all converters needed in the top level converter
  // do not change the order of adding converters
  addTPConverter( &m_p4PtEtaPhiMCnv );


}

void TrigTauTracksInfoCnv_tlp1::setPStorage( TrigTauTracksInfo_tlp1* storage ){


   //for the base class converter
   setMainCnvPStorage( &storage->m_trigTauTracksInfo );

   //for all other converters defined in the base class
   m_p4PtEtaPhiMCnv.              setPStorage( &storage->m_p4PtEtaPhiM );

}

