/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MuonEventTPCnv/MuonMeasurementsCnv_tlp1.h"


MuonMeasurementsCnv_tlp1::MuonMeasurementsCnv_tlp1()
{
   // Add all converters defined in this top level converter:
   // never change the order of adding converters!  
   addTPConverter( &m_muonClustersOTCnv );
   addTPConverter( &m_cscClustersOTCnv );   
   addTPConverter( &m_rpcClustersOTCnv );     
   addTPConverter( &m_tgcClustersOTCnv );     
   addTPConverter( &m_mdtDriftCirclesOTCnv );
   addTPConverter( &m_muonSegmentsCnv );  
   addTPConverter( &m_muonSegmentQualitiesCnv );  
   addTPConverter( &m_muonCompetingROTsCnv );
}



void MuonMeasurementsCnv_tlp1::setPStorage( TPCnv::MuonMeasurements_tlp1 *storage )
{
   m_muonClustersOTCnv.      setPStorage( &storage->m_muonClustersOT );
   m_cscClustersOTCnv.       setPStorage( &storage->m_cscClustersOT  );
   m_rpcClustersOTCnv.       setPStorage( &storage->m_rpcClustersOT );
   m_tgcClustersOTCnv.       setPStorage( &storage->m_tgcClustersOT );
   m_mdtDriftCirclesOTCnv.   setPStorage( &storage->m_mdtDriftCirclesOT );
   m_muonSegmentsCnv.        setPStorage( &storage->m_muonSegments );   
   m_muonSegmentQualitiesCnv.setPStorage( &storage->m_muonSegmentQualities );  
   m_muonCompetingROTsCnv.   setPStorage( &storage->m_muonCompetingROT ); 
}   
 
