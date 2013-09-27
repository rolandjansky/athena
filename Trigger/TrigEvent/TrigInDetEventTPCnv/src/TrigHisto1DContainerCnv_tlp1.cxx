/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEventTPCnv/TrigHisto1DContainerCnv_tlp1.h"


//constructor
TrigHisto1DContainerCnv_tlp1::TrigHisto1DContainerCnv_tlp1(){

  // add the "main" base class converter (ie. TrigHisto1DContainerCnv)
  addMainTPConverter();

  // add all converters needed in the top level converter
  // do not change the order of adding converters
  addTPConverter( &m_trigHisto1DCnv );


}

void TrigHisto1DContainerCnv_tlp1::setPStorage( TrigHisto1DContainer_tlp1* storage ){


   //for the base class converter
   setMainCnvPStorage( &storage->m_trigHisto1DContainerVec );

   //for all other converters defined in the base class
   m_trigHisto1DCnv.setPStorage( &storage->m_trigHisto1DVec );

}

