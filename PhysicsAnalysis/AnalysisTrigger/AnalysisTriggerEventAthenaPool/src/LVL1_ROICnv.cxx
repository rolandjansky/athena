/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


// Gaudi/Athena include(s):
#include "GaudiKernel/MsgStream.h"

// Local include(s):
#include "LVL1_ROICnv.h"

static LVL1_ROICnv_p1 TPConverter;

LVL1_ROI_PERS* LVL1_ROICnv::createPersistent( LVL1_ROI* transObj ) {

  MsgStream log( this->messageService(), "LVL1_ROICnv" );
  LVL1_ROI_PERS* persObj = TPConverter.createPersistent( transObj, log );
  return persObj;

}

LVL1_ROI* LVL1_ROICnv::createTransient() {

  static pool::Guid p1_guid( "5FBA83E0-77DC-43DD-A511-E4F0A49882E0" );
  static pool::Guid p0_guid( "3E1829DE-9AA7-489C-AB81-406DF6CC544F" );

  if( this->compareClassGuid( p1_guid ) ) {

    std::auto_ptr< LVL1_ROI_p1 > pers_ref( this->poolReadObject< LVL1_ROI_p1 >() );
    MsgStream log( this->messageService(), "LVL1_ROICnv" );
    return TPConverter.createTransient( pers_ref.get(), log );

  } else if( this->compareClassGuid( p0_guid ) ) {

    return this->poolReadObject< LVL1_ROI >();

  }

  throw std::runtime_error( "Unsupported persistent version of LVL1_ROI" );
  return 0;

}
