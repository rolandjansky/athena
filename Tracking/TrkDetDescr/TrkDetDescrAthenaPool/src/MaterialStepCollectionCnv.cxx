/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
 m_msgSvc( msgSvc() )
 {}
 
MaterialStepCollection_PERS*
MaterialStepCollectionCnv::createPersistentWithKey ( Trk::MaterialStepCollection *transCont,
                                                     const std::string& key)
{
  MsgStream log (m_msgSvc, "MaterialStepCollectionCnv:" + key);
  return m_TPConverter.createPersistent( transCont, log );
}


Trk::MaterialStepCollection*
MaterialStepCollectionCnv::createTransientWithKey (const std::string& key)
{
  MsgStream log (m_msgSvc, "MaterialStepCollectionCnv:" + key);

  static const pool::Guid p1_guid( "DF8A7FA8-693F-44E0-A5E5-F9907B8B429E" );

  Trk::MaterialStepCollection* tCollection = nullptr;
  if( compareClassGuid( p1_guid ) ) {

    std::unique_ptr< MaterialStepCollection_PERS >  p_coll( poolReadObject< MaterialStepCollection_PERS >() );
    tCollection = m_TPConverter.createTransient( p_coll.get(), log );
  }
 
  return tCollection;
}
