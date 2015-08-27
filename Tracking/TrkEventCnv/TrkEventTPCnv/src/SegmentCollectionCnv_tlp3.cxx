/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrkEventTPCnv/SegmentCollectionCnv_tlp3.h"

SegmentCollectionCnv_tlp3::SegmentCollectionCnv_tlp3( )
{
//    std::cout << "SegmentCollectionCnv_tlp3::SegmentCollectionCnv_tlp3( )" << std::endl;
   // Add all converters defined in this top level converter:
   // never change the order of adding converters!

  addMainTPConverter();

  addTPConverter( &m_segmentCnv );

   // Surfaces
  addTPConverter( &m_coneSurfacesCnv       );
  addTPConverter( &m_cylSurfacesCnv        );
  addTPConverter( &m_discSurfacesCnv       );
  addTPConverter( &m_perigeeSurfacesCnv    );
  addTPConverter( &m_planeSurfacesCnv      );
  addTPConverter( &m_lineSurfacesCnv       );
  addTPConverter( &m_saggedLineSurfacesCnv );

  addTPConverter( &m_fitQualitiesCnv );
  addTPConverter( &m_errorMatricesCnv );   
  addTPConverter( &m_covarianceMatricesCnv );     
  addTPConverter( &m_localParametersCnv );            

  addTPConverter( &m_pseudoMeasurementOnTrackCnv );
  addTPConverter( &m_crotCnv );   
}

SegmentCollectionCnv_tlp3::~SegmentCollectionCnv_tlp3()
  {}

void SegmentCollectionCnv_tlp3::setPStorage( Trk::SegmentCollection_tlp3 *storage )
{
  setMainCnvPStorage( &storage->m_segmentCollections );

     // for all converters defined in this top level converter
  m_segmentCnv.                  setPStorage( &storage->m_segments );                

  m_coneSurfacesCnv          .setPStorage( &storage->m_surfaces );
  m_cylSurfacesCnv           .setPStorage( &storage->m_surfaces );
  m_discSurfacesCnv          .setPStorage( &storage->m_surfaces );
  m_perigeeSurfacesCnv       .setPStorage( &storage->m_surfaces );
  m_planeSurfacesCnv         .setPStorage( &storage->m_surfaces );
  m_lineSurfacesCnv          .setPStorage( &storage->m_surfaces );
  m_saggedLineSurfacesCnv    .setPStorage( &storage->m_surfaces );

  m_fitQualitiesCnv.             setPStorage( &storage->m_fitQualities );          
  m_covarianceMatricesCnv.       setPStorage( &storage->m_hepSymMatrices );        
  m_errorMatricesCnv.            setPStorage( &storage->m_hepSymMatrices );        
  m_localParametersCnv.          setPStorage( &storage->m_localParameters );       

  m_pseudoMeasurementOnTrackCnv. setPStorage( &storage->m_pseudoMeasurementOnTrack );  
  m_crotCnv.                     setPStorage( &storage->m_competingRotsOnTrack );          
}   

