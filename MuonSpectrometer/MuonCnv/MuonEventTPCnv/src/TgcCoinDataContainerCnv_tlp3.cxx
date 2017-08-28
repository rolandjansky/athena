/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonEventTPCnv/TgcCoinDataContainerCnv_tlp3.h"
#include "MuonEventTPCnv/CreateTransientTemplates.h"


TgcCoinDataContainerCnv_tlp3::TgcCoinDataContainerCnv_tlp3()
{
   // // Add all converters defined in this top level converter:
   // // never change the order of adding converters!  
   addMainTPConverter();
   addTPConverter( &m_tgcCoinDataCnv );   
   addTPConverter( &m_locPosCnv);
   addTPConverter( &m_errorMatCnv);
   // 

}



void TgcCoinDataContainerCnv_tlp3::setPStorage( Muon::TgcCoinDataContainer_tlp3* storage )
{
 setMainCnvPStorage( &storage->m_pixDCCont );
  
 m_tgcCoinDataCnv.setPStorage( &storage->m_tgcCoinDatas );
 m_locPosCnv.setPStorage( &storage->m_locPos );
 m_errorMatCnv.setPStorage( &storage->m_errorMat );
}   
 
