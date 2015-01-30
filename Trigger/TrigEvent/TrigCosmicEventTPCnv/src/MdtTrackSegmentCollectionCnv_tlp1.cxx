/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCosmicEventTPCnv/MdtTrackSegmentCollectionCnv_tlp1.h"


//constructor
MdtTrackSegmentCollectionCnv_tlp1::MdtTrackSegmentCollectionCnv_tlp1(){

  // add the "main" base class converter (ie. MdtTrackSegmentCollectionCnv)
  addMainTPConverter();

  // add all converters needed in the top level converter
  // do not change the order of adding converters
  addTPConverter( &m_mdtTrackSegmentCnv );


}

void MdtTrackSegmentCollectionCnv_tlp1::setPStorage( MdtTrackSegmentCollection_tlp1* storage ){


   //for the base class converter
   setMainCnvPStorage( &storage->m_mdtTrackSegmentCollectionVec );

   //for all other converters defined in the base class
   m_mdtTrackSegmentCnv.setPStorage( &storage->m_mdtTrackSegmentVec );

}

