/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// LayerMaterialMapCnv.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "LayerMaterialMapCnv.h"
#include "GaudiKernel/IToolSvc.h"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
LayerMaterialMapCnv::LayerMaterialMapCnv(ISvcLocator* svcloc):
    LayerMaterialMapCnvBase(svcloc),
    m_msgSvc( msgSvc() )
{
}

//-----------------------------------------------------------------------------
// Initializer
//-----------------------------------------------------------------------------
StatusCode LayerMaterialMapCnv::initialize()
{
    StatusCode sc = LayerMaterialMapCnvBase::initialize();
    MsgStream log (m_msgSvc, "ElementTableCnv");
    if( sc.isFailure() ) {
        log << MSG::FATAL << "Could not initialize cnv base" << endmsg;
        return sc;
    }
    //-------------------------------------------------------------------------
    // Set up the message stream
    //-------------------------------------------------------------------------
    log << MSG::INFO << "LayerMaterialMapCnv::initialize()" << endmsg;
   
    return StatusCode::SUCCESS;
}


LayerMaterialMap_PERS*
LayerMaterialMapCnv::createPersistentWithKey (Trk::LayerMaterialMap* transCont,
                                              const std::string& key)
{   
  MsgStream log (m_msgSvc, "LayerMaterialMapCnv: " + key);
  return m_TPConverter_tlp1.createPersistent( transCont, log );
}


//-----------------------------------------------------------------------------
// Create transient collection
//-----------------------------------------------------------------------------
Trk::LayerMaterialMap*
LayerMaterialMapCnv::createTransientWithKey (const std::string& key)
{
  static const pool::Guid tlp1_guid( "3DA92DBD-DA78-43A2-BFDF-9E19E2BF1E8A" );

  Trk::LayerMaterialMap *p_collection = nullptr;

  MsgStream log (m_msgSvc, "LayerMaterialMapCnv: " + key);

  if( compareClassGuid( tlp1_guid ) ) {
    poolReadObject< Trk::LayerMaterialMap_tlp1 >( m_TPConverter_tlp1 );
    p_collection = m_TPConverter_tlp1.createTransient( log );
  }       
  return p_collection;
}
