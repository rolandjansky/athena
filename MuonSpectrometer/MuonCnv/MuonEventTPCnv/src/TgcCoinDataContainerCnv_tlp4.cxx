/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonEventTPCnv/TgcCoinDataContainerCnv_tlp4.h"


TgcCoinDataContainerCnv_tlp4::TgcCoinDataContainerCnv_tlp4()
{
   // // Add all converters defined in this top level converter:
   // // never change the order of adding converters!  
   addMainTPConverter();
   addTPConverter( &m_tgcCoinDataCnv );   
   addTPConverter( &m_locPosCnv);
   addTPConverter( &m_errorMatCnv);
   // 

}



void TgcCoinDataContainerCnv_tlp4::setPStorage( Muon::TgcCoinDataContainer_tlp4* storage )
{
 setMainCnvPStorage( &storage->m_pixDCCont );
  
 m_tgcCoinDataCnv.setPStorage( &storage->m_tgcCoinDatas );
 m_locPosCnv.setPStorage( &storage->m_locPos );
 m_errorMatCnv.setPStorage( &storage->m_errorMat );
}   
 
