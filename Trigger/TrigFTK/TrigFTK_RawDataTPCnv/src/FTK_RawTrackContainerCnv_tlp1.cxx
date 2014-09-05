/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigFTK_RawDataTPCnv/FTK_RawTrackContainerCnv_tlp1.h"


//constructor
FTK_RawTrackContainerCnv_tlp1::FTK_RawTrackContainerCnv_tlp1(){

  // add the "main" base class converter
  addMainTPConverter();

  // add all converters needed in the top level converter
  // do not change the order of adding converters
  addTPConverter( &m_FTK_RawTrackCnv );

}

void FTK_RawTrackContainerCnv_tlp1::setPStorage( FTK_RawTrackContainer_tlp1* storage ){

   //for the base class converter
   setMainCnvPStorage( &storage->m_FTK_RawTrackContainers );

   //for all other converters defined in the base class
   m_FTK_RawTrackCnv.           setPStorage( &storage->m_FTK_RawTracks );

}


