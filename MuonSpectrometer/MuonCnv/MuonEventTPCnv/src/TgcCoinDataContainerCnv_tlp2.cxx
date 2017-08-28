/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/* Takashi Kubota - June 30, 2008 */
#include "MuonEventTPCnv/TgcCoinDataContainerCnv_tlp2.h"
#include "MuonEventTPCnv/CreateTransientTemplates.h"

TgcCoinDataContainerCnv_tlp2::TgcCoinDataContainerCnv_tlp2()
{
   // // Add all converters defined in this top level converter:
   // // never change the order of adding converters!  
   addMainTPConverter();
   addTPConverter( &m_tgcCoinDataCnv );   
   addTPConverter( &m_locPosCnv);
   addTPConverter( &m_errorMatCnv);
   // 

}



void TgcCoinDataContainerCnv_tlp2::setPStorage( Muon::TgcCoinDataContainer_tlp2* storage )
{
 setMainCnvPStorage( &storage->m_pixDCCont );
  
 m_tgcCoinDataCnv.setPStorage( &storage->m_tgcCoinDatas );
 m_locPosCnv.setPStorage( &storage->m_locPos );
 m_errorMatCnv.setPStorage( &storage->m_errorMat );
}   
 
