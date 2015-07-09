/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// MaterialStepCollectionCnv.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "MaterialStepCollectionCnv.h"
#include "GaudiKernel/MsgStream.h"


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

MaterialStepCollectionCnv::MaterialStepCollectionCnv( ISvcLocator *svcloc ) :
 MaterialStepCollectionCnvBase(svcloc),
 m_msgSvc( msgSvc() ),
 m_log( m_msgSvc, "MaterialStepCollectionCnv" )
 {}
 
//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
MaterialStepCollectionCnv::~MaterialStepCollectionCnv()
{}

//-----------------------------------------------------------------------------
// Initializer
//-----------------------------------------------------------------------------
StatusCode MaterialStepCollectionCnv::initialize()
{
  if (MaterialStepCollectionCnvBase::initialize().isFailure() )
  {
    m_log << MSG::FATAL << "Could not initialize MaterialStepCollectionCnvBase" << endreq;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
 
}

MaterialStepCollection_PERS* MaterialStepCollectionCnv::createPersistent( Trk::MaterialStepCollection *transCont) {

   // Message stream handling
   m_log.setLevel( m_msgSvc->outputLevel() );
   updateLog(); 

   // call to the converter
   MaterialStepCollection_PERS * pVxCont = m_TPConverter.createPersistent( transCont, m_log );
 
   return pVxCont;

}//end of create persistent method


Trk::MaterialStepCollection * MaterialStepCollectionCnv::createTransient()
{
   // Message stream handling
   m_log.setLevel( m_msgSvc->outputLevel() );
   updateLog(); 

   static pool::Guid p1_guid( "DF8A7FA8-693F-44E0-A5E5-F9907B8B429E" );


   Trk::MaterialStepCollection* tCollection = 0;
   if( compareClassGuid( p1_guid ) ) {

      std::auto_ptr< MaterialStepCollection_PERS >  p_coll( poolReadObject< MaterialStepCollection_PERS >() );
      tCollection = m_TPConverter.createTransient( p_coll.get(), m_log );
   }
 
   return tCollection;

}

void MaterialStepCollectionCnv::updateLog(){ 
    
     const DataObject* dObj = getDataObject();
     if (dObj==0) return; // Can't do much if this fails.
     const std::string  key = (dObj->name());
 
     m_log.m_source="MaterialStepCollectionCnv: "+key; // A hack - relies on getting access to private data of MsgStream via #define trick. EJWM.
}
