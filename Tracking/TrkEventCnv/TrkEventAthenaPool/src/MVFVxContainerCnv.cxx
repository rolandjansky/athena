/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   MVFVxContainerCnv.cxx
// author: Kirill Prokofiev <Kirill.Prokofiev@cern.ch>
//
//-----------------------------------------------------------------------------


#include "MVFVxContainerCnv.h"


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

MVFVxContainerCnv::MVFVxContainerCnv( ISvcLocator *svcloc ):
                           MVFVxContainerCnvBase(svcloc)
 {      

 }

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
MVFVxContainerCnv::~MVFVxContainerCnv()
{}

//-----------------------------------------------------------------------------
// Initializer
//-----------------------------------------------------------------------------
StatusCode MVFVxContainerCnv::initialize()
{
    MsgStream log(msgSvc());
    
 StatusCode sc = MVFVxContainerCnvBase::initialize();
  if( sc.isFailure() ) 
  {
    log << MSG::FATAL << "Could not initialize MVFVxContainerCnvBase" << endmsg;
    return sc;
  }
  
//-------------------------------------------------------------------------
// Set up the message stream
//-------------------------------------------------------------------------
  // log.setLevel( m_msgSvc->outputLevel() );
  log << MSG::INFO << "MVFVxContainerCnv::initialize()" << endmsg; 
  return StatusCode::SUCCESS;
}


MVFVxContainer_PERS * MVFVxContainerCnv::createPersistent( MVFVxContainer* )
{ 
   MsgStream log(msgSvc(), "MVFVxContainerCnv" );
   log << MSG::ERROR << "createPersistent() is obsolete" << endmsg;
   return nullptr;
}


MVFVxContainer * MVFVxContainerCnv::createTransient()
{
    MsgStream log(msgSvc());
 static pool::Guid p1_guid( "D7BAA7AD-1A46-4DA3-9CA7-350A1A3F0656" );
 static pool::Guid p0_guid( "6C6999B7-F961-4B72-B6D9-DF71CB2364CC" );

 MVFVxContainer *p_collection = 0;
 
 if( compareClassGuid( p1_guid ) ) {
    // std::cout << "MVFVxContainerCnv::createTransient()" << std::endl;
    /*
    usingTPCnvForReading( m_TPConverter );
    std::unique_ptr< MVFVxContainer_PERS >  p_coll( poolReadObject< MVFVxContainer_PERS >() );
    */
    poolReadObject< MVFVxContainer_PERS >( m_TPConverter );
    p_collection = m_TPConverter.createTransient( log );
  
 } else if( compareClassGuid( p0_guid ) ) {
    // std::cout << "MVFVxContainerCnv::createTransient: use old converter" << std::endl;  
    p_collection = poolReadObject< MVFVxContainer >(); 
 } else
    throw std::runtime_error( "Unsupported persistent version of MVFVxContainer (unknown GUID)" );
    
 return p_collection; 
}


void        MVFVxContainerCnv::readObjectFromPool( const std::string& token )
{
  static pool::Guid p1_guid( "D7BAA7AD-1A46-4DA3-9CA7-350A1A3F0656" );
  
   // set the POOL token which will be used for reading from POOL
   setToken( token );

   // select the object type based on its GUID 
   if( compareClassGuid( p1_guid ) ) {
      // read the object using the main TP converter
      poolReadObject< MVFVxContainer_PERS >( m_TPConverter );
   }
   else
      throw std::runtime_error( "Unsupported version of MVFVxContainer (unknown GUID)" );
}

// void MVFVxContainerCnv::updateLog(){  
//      DataObject* dObj = getDataObject();
//      if (dObj==0) return; // Can't do much if this fails.
//      const std::string  key = (dObj->name());
//  
//      log.m_source="MVFVxContainerCnv: "+key; // A hack - relies on getting access to private data of MsgStream via #define trick. EJWM.
// }

