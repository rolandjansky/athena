/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// ElementTableCnv.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "ElementTableCnv.h"
#include "GaudiKernel/MsgStream.h"


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

ElementTableCnv::ElementTableCnv( ISvcLocator *svcloc ) :
 ElementTableCnvBase(svcloc),
 m_msgSvc( msgSvc() ),
 m_log( m_msgSvc, "ElementTableCnv" )
 {}
 
//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
ElementTableCnv::~ElementTableCnv()
{}

//-----------------------------------------------------------------------------
// Initializer
//-----------------------------------------------------------------------------
StatusCode ElementTableCnv::initialize()
{
  if (ElementTableCnvBase::initialize().isFailure() )
  {
    m_log << MSG::FATAL << "Could not initialize ElementTableCnvBase" << endreq;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
 
}

ElementTable_PERS* ElementTableCnv::createPersistent(Trk::ElementTable *etTrans) {

   // Message stream handling
   m_log.setLevel( m_msgSvc->outputLevel() );
   updateLog(); 

   // call to the converter
   ElementTable_PERS * etPers = m_TPConverter.createPersistent(etTrans, m_log );
 
   return etPers;

}//end of create persistent method


Trk::ElementTable* ElementTableCnv::createTransient()
{
   // Message stream handling
   m_log.setLevel( m_msgSvc->outputLevel() );
   updateLog(); 

   static pool::Guid p1_guid( "B157B642-94C0-11E3-B1C2-02163E00A511" );

   Trk::ElementTable* tCollection = 0;
   if( compareClassGuid( p1_guid ) ) {

      std::auto_ptr< ElementTable_PERS >  p_coll( poolReadObject< ElementTable_PERS >() );
      tCollection = m_TPConverter.createTransient( p_coll.get(), m_log );
   }
 
   return tCollection;

}

void ElementTableCnv::updateLog(){ 
    
     const DataObject* dObj = getDataObject();
     if (dObj==0) return; // Can't do much if this fails.
     const std::string  key = (dObj->name());
 
     m_log.m_source="ElementTableCnv: "+key; // A hack - relies on getting access to private data of MsgStream via #define trick. EJWM.
}
