/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/


#include "MissingETCompositionCnv.h"

#include "RecTPCnv/MissingETCompositionCnv_p1.h"
#include "RecTPCnv/MissingETComposition_p1.h"

MissingETComposition_PERS* MissingETCompositionCnv::createPersistent(MissingETComposition* transObj)
{
  MsgStream report( messageService(), "MissingETCompositionConverter" );
  // report << MSG::INFO << "Writing MissingETComposition_p2" << endreq; 
  if ( ! bool(m_TPconverter_p2) )
    m_TPconverter_p2 = new MissingETCompositionCnv_p2();
  MissingETComposition_PERS *persObj = m_TPconverter_p2->createPersistent( transObj, report );
  return persObj;
}

MissingETComposition* MissingETCompositionCnv::createTransient()
{
    static pool::Guid   p1_guid("78E9CC2F-1FB4-4740-AD44-7C849D9CC865");
    static pool::Guid   p2_guid("CF4110FB-35B4-4C8B-BB51-FE8C6CBE0277");
    
  if( compareClassGuid(p2_guid) ){
      if ( ! bool(m_TPconverter_p2) )
	      m_TPconverter_p2 = new MissingETCompositionCnv_p2();
      // using unique_ptr ensures deletion of the persistent object
      std::unique_ptr<MissingETComposition_p2> shapestore( poolReadObject< MissingETComposition_p2 >() );
      MsgStream report( messageService(), "MissingETCompositionCnv" );
      // report << MSG::INFO << "Reading MissingETComposition_p1" << endreq; 
      return m_TPconverter_p2->createTransient( shapestore.get(), report );
    }
    else if( compareClassGuid(p1_guid) ) {
  	    MissingETCompositionCnv_p1 TPconverter_p1;
        std::unique_ptr<MissingETComposition_p1> shapestore( poolReadObject< MissingETComposition_p1 >() );
        MsgStream report( messageService(), "MissingETCompositionCnv" );
        // report << MSG::INFO << "Reading MissingETComposition_p1" << endreq; 
        return TPconverter_p1.createTransient( shapestore.get(), report );
    }
  throw std::runtime_error("Unsupported persistent version of MissingETComposition");
}
