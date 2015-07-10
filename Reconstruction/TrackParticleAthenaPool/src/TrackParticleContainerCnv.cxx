/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TrackParticleContainerCnv.cxx
// author: Andreas Wildauer
//
//-----------------------------------------------------------------------------
#include "TrackParticleContainerCnv.h"
#include "StoreGate/StoreGateSvc.h"
#include <iostream>
//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------

TrackParticleContainerCnv::TrackParticleContainerCnv( ISvcLocator *svcloc ):
                           TrackParticleContainerCnvBase(svcloc),
                           m_msgSvc( msgSvc() ),
                           m_log( m_msgSvc, "TrackParticleContainerCnv" )
 {}
 
//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
TrackParticleContainerCnv::~TrackParticleContainerCnv()
{}

//-----------------------------------------------------------------------------
// Initializer
//-----------------------------------------------------------------------------
StatusCode TrackParticleContainerCnv::initialize()
{
  StatusCode sc = TrackParticleContainerCnvBase::initialize();
  if( sc.isFailure() ) 
  {
    m_log << MSG::FATAL << "Could not initialize TrackParticleContainerCnvBase" << endreq;
    return sc;
  }
  
//-------------------------------------------------------------------------
// Set up the message stream
//-------------------------------------------------------------------------
  m_log.setLevel( m_msgSvc->outputLevel() );
  m_log << MSG::INFO << "TrackParticleContainerCnv::initialize()" << endreq;

  return StatusCode::SUCCESS;
  
}//end of initialize method

TrackParticleContainer_PERS *
TrackParticleContainerCnv::createPersistent( Rec::TrackParticleContainer *transCont)
{
  const DataObject* dObj = getDataObject();
  if (dObj) {
    const std::string  key = (dObj->name());
    m_log.m_source="TrackParticleContainerCnv['"+key+"']"; // A hack - relies on getting access to private data of MsgStream via #define trick. EJWM.
  }
  // std::cout<<m_log.m_source<<" About to call m_TPConverter_tlp3.createPersistent. "<<std::endl;
  
  TrackParticleContainer_PERS * tpBaseCont = m_TPConverter_tlp3.createPersistent( transCont, m_log );

  return tpBaseCont;
}//end of create persistent method

Rec::TrackParticleContainer * TrackParticleContainerCnv::createTransient()
{  
  
  static pool::Guid p3_guid( "8C84D957-1899-4C98-A49D-D6AC0F85C5EC" );
  static pool::Guid p2_guid( "170211F9-C4E1-4173-B0FB-71322899C8B9" );
  static pool::Guid p1_guid( "4A8CEB2C-0833-4C83-8514-A69928DE4672" );
  static pool::Guid p0_guid( "35BE1E01-658C-438A-AC86-F951006ECC6B" );

  Rec::TrackParticleContainer *p_collection = 0;
  if( compareClassGuid( p3_guid ) ) {
    // std::cout<<"TrackParticleContainerCnv::createTransient p3_guid"<<std::endl;
    
    //m_log << MSG::INFO << __FILE__<<", line "<<__LINE__<<": reading _p2" << endreq;
    poolReadObject< Rec::TrackParticleContainer_tlp3 >(m_TPConverter_tlp3);
    p_collection = m_TPConverter_tlp3.createTransient(m_log);
  } 
  else if( compareClassGuid( p2_guid ) ) {
    // std::cout<<"TrackParticleContainerCnv::createTransient p2_guid"<<std::endl;
    
    //m_log << MSG::INFO << __FILE__<<", line "<<__LINE__<<": reading _p2" << endreq;
    poolReadObject< Rec::TrackParticleContainer_tlp2 >(m_TPConverter_tlp2);
    p_collection = m_TPConverter_tlp2.createTransient(m_log);
  } 
  else if( compareClassGuid( p1_guid ) ) {
    //m_log << MSG::INFO << __FILE__<<", line "<<__LINE__<<": reading _p1" << endreq;
   poolReadObject< Rec::TrackParticleContainer_tlp1 >(m_TPConverter_tlp1);
   p_collection = m_TPConverter_tlp1.createTransient(m_log);
  }
  else if( compareClassGuid( p0_guid ) ) {
    //m_log << MSG::INFO << __FILE__<<", line "<<__LINE__<<": reading _p0" << endreq;
   p_collection = poolReadObject< Rec::TrackParticleContainer >();
  }
  else {
    throw std::runtime_error( "Unsupported persistent version of Rec::TrackParticleContainer" );
  }
   
  return p_collection;
  
}//end of create transient method


