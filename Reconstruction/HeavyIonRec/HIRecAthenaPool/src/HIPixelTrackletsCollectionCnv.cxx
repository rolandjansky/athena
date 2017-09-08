/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**
 * @file HIPixelTrackletsCollectionCnv.cxx
 * @brief Generated implementation file which includes header files needed by HIPixelTrackletsCollectionCnv
 * @author RD Schaffer <R.D.Schaffer@cern.ch>
 */


#include "HIPixelTrackletsCollectionCnv.h"

//constructor
HIPixelTrackletsCollectionCnv::HIPixelTrackletsCollectionCnv(ISvcLocator *svcloc ):
  HIPixelTrackletsCollectionCnvBase(svcloc),
  m_msgSvc( msgSvc() ),
  m_log(m_msgSvc,"HIPixelTrackletsCollectionCnv")
{}

//destructor
HIPixelTrackletsCollectionCnv::~HIPixelTrackletsCollectionCnv()
{}

StatusCode HIPixelTrackletsCollectionCnv::initialize()
{
  StatusCode sc = HIPixelTrackletsCollectionCnvBase::initialize();
  if( sc.isFailure() ) {
    m_log << MSG::FATAL << "Could not initialize HIPixelTrackletsCollectionCnvBase" << endmsg;
    return sc;
  }

  m_log.setLevel( m_msgSvc->outputLevel() );
  m_log << MSG::INFO << "HIPixelTrackletsCollectionCnv::initialize()." << endmsg;

  //
  return StatusCode::SUCCESS;
}

HIPixelTrackletsCollection_PERS *
HIPixelTrackletsCollectionCnv::createPersistent( HIPixelTrackletsCollection* transCont) {
  m_log.setLevel( m_msgSvc->outputLevel() );
  updateLog();
  HIPixelTrackletsCollection_PERS *pTrkletsCont = m_TPConverter.createPersistent( transCont, m_log );
  
  return pTrkletsCont;
}


HIPixelTrackletsCollection*
HIPixelTrackletsCollectionCnv::createTransient() {
  static pool::Guid p1_guid( "8FE8FB83-AA9B-4E4A-8365-FBDDBC5B5400" );
  //static pool::Guid p0_guid( " " );

  HIPixelTrackletsCollection *p_collection = 0;
  if( compareClassGuid( p1_guid ) ) {
    std::auto_ptr< HIPixelTrackletsCollection_PERS > p_coll( poolReadObject< HIPixelTrackletsCollection_PERS >() );
    p_collection = m_TPConverter.createTransient( p_coll.get(), m_log );
  }
  else throw std::runtime_error( "Unsupported persistent version of HIPixelTrackletsCollection." );

  return p_collection;
}

void HIPixelTrackletsCollectionCnv::updateLog() {
  DataObject *dObj = getDataObject();
  if( dObj==0 ) return;
  const std::string key = (dObj->name());

  m_log.m_source = "HIPixelTrackletsCollectionCnv: " + key;

}
