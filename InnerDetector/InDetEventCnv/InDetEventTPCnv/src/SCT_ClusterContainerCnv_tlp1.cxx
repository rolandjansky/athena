/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "InDetEventTPCnv/SCT_ClusterContainerCnv_tlp1.h"


SCT_ClusterContainerCnv_tlp1::SCT_ClusterContainerCnv_tlp1()
{
   // Add all converters defined in this top level converter:
   // never change the order of adding converters!  
   addMainTPConverter();
   addTPConverter( &m_sctClusterCnv );   
   addTPConverter( &m_siClusterCnv );   
   addTPConverter( &m_siWidthCnv );   
   addTPConverter( &m_prepDataCnv);
   addTPConverter( &m_locPosCnv);
   addTPConverter( &m_errorMatCnv);


}



void SCT_ClusterContainerCnv_tlp1::setPStorage( InDet::SCT_ClusterContainer_tlp1 *storage )
{
 setMainCnvPStorage( &storage->m_sctDCCont );

 m_sctClusterCnv.setPStorage( &storage->m_sctClusters );
 m_siClusterCnv.setPStorage( &storage->m_siClusters );
 m_siWidthCnv.setPStorage( &storage->m_siWidths );
 m_prepDataCnv.setPStorage( &storage->m_prepData );
 m_locPosCnv.setPStorage( &storage->m_locPos );
 m_errorMatCnv.setPStorage( &storage->m_errorMat );
}   
 
