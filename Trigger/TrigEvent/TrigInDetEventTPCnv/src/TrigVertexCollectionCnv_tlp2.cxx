/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEventTPCnv/TrigVertexCollectionCnv_tlp2.h"
  
TrigVertexCollectionCnv_tlp2::TrigVertexCollectionCnv_tlp2()
{
  // add the "main" base class converter (ie. TrigVertexCollectionCnv)
  addMainTPConverter();              
  
  // Add all converters defined in this top level converter:
  // never change the order of adding converters!                             
  addTPConverter( &m_VertexCnv); 
  addTPConverter( &m_TrigIDtrackCnv); 
  addTPConverter( &m_TrigIDtrackFitParCnv);    
}

void TrigVertexCollectionCnv_tlp2::setPStorage( TrigVertexCollection_tlp2 *storage )
{
  setMainCnvPStorage( &storage->m_TrigVertexCollection );
  
  m_VertexCnv.              setPStorage(&storage->m_Vertex);
  m_TrigIDtrackCnv.         setPStorage(&storage->m_Track);
  m_TrigIDtrackFitParCnv.   setPStorage(&storage->m_TrackFitPar);
}


