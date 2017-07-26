/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonEventTPCnv/MdtPrepDataContainerCnv_tlp1.h"
#include "MuonEventTPCnv/CreateTransientTemplates.h"

MdtPrepDataContainerCnv_tlp1::MdtPrepDataContainerCnv_tlp1()
{
   // Add all converters defined in this top level converter:
   // never change the order of adding converters!  
   addMainTPConverter();
   addTPConverter( &m_muonPrdCnv );   
   addTPConverter( &m_prepDataCnv );   
   addTPConverter( &m_locPosCnv);
   addTPConverter( &m_errorMatCnv);


}



void MdtPrepDataContainerCnv_tlp1::setPStorage( Muon::MdtPrepDataContainer_tlp1 *storage )
{
 setMainCnvPStorage( &storage->m_pixDCCont );

 m_muonPrdCnv.setPStorage( &storage->m_muonPrds );
 m_prepDataCnv.setPStorage( &storage->m_prepData );
 m_locPosCnv.setPStorage( &storage->m_locPos );
 m_errorMatCnv.setPStorage( &storage->m_errorMat );
}   
 
