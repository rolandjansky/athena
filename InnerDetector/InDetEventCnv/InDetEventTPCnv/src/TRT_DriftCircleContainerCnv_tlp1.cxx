/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetEventTPCnv/TRT_DriftCircleContainerCnv_tlp1.h"


TRT_DriftCircleContainerCnv_tlp1::TRT_DriftCircleContainerCnv_tlp1()
{
   // Add all converters defined in this top level converter:
   // never change the order of adding converters!  
   addMainTPConverter();
   addTPConverter( &m_trtDriftCirclesCnv );   
   addTPConverter( &m_prepDataCnv);
   addTPConverter( &m_locPosCnv);
   addTPConverter( &m_errorMatCnv);


}



void TRT_DriftCircleContainerCnv_tlp1::setPStorage( InDet::TRT_DriftCircleContainer_tlp1 *storage )
{
 setMainCnvPStorage( &storage->m_trtDCCont );

 m_trtDriftCirclesCnv.setPStorage( &storage->m_trtDriftCircles );
 m_prepDataCnv.setPStorage( &storage->m_prepData );
 m_locPosCnv.setPStorage( &storage->m_locPos );
 m_errorMatCnv.setPStorage( &storage->m_errorMat );
}   
 
