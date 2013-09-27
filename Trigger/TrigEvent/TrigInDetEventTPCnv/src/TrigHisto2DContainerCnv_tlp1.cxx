/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigInDetEventTPCnv/TrigHisto2DContainerCnv_tlp1.h"


//constructor
TrigHisto2DContainerCnv_tlp1::TrigHisto2DContainerCnv_tlp1(){

  // add the "main" base class converter (ie. TrigHisto2DContainerCnv)
  addMainTPConverter();

  // add all converters needed in the top level converter
  // do not change the order of adding converters
  addTPConverter( &m_trigHisto2DCnv );


}

void TrigHisto2DContainerCnv_tlp1::setPStorage( TrigHisto2DContainer_tlp1* storage ){


   //for the base class converter
   setMainCnvPStorage( &storage->m_trigHisto2DContainerVec );

   //for all other converters defined in the base class
   m_trigHisto2DCnv.setPStorage( &storage->m_trigHisto2DVec );

}


