/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEventTPCnv/TrigT2MbtsBitsContainerCnv_tlp1.h"


//constructor
TrigT2MbtsBitsContainerCnv_tlp1::TrigT2MbtsBitsContainerCnv_tlp1(){

  // add the "main" base class converter (ie. TrigT2MbtsBitsContainerCnv)
  addMainTPConverter();

  // add all converters needed in the top level converter
  // do not change the order of adding converters
  addTPConverterForReading(&m_trigT2MbtsBitsCnv_p1);
  addTPConverter( &m_trigT2MbtsBitsCnv_p2 );

}

void TrigT2MbtsBitsContainerCnv_tlp1::setPStorage( TrigT2MbtsBitsContainer_tlp1* storage ){


   //for the base class converter
   setMainCnvPStorage( &storage->m_trigT2MbtsBitsContainerVec );

   //for all other converters defined in the base class
   m_trigT2MbtsBitsCnv_p1.setPStorage( &storage->m_trigT2MbtsBitsVec );
   m_trigT2MbtsBitsCnv_p2.setPStorage( &storage->m_trigT2MbtsBitsVec_p2 );
}
