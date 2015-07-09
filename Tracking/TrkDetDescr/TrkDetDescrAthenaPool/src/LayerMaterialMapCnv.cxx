/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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
    m_msgSvc( msgSvc() ),
    m_log( m_msgSvc, "LayerMaterialMapCnv" )
{
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
LayerMaterialMapCnv::~LayerMaterialMapCnv() {}

//-----------------------------------------------------------------------------
// Initializer
//-----------------------------------------------------------------------------
StatusCode LayerMaterialMapCnv::initialize()
{
    StatusCode sc = LayerMaterialMapCnvBase::initialize();
    if( sc.isFailure() ) {
        m_log << MSG::FATAL << "Could not initialize cnv base" << endreq;
        return sc;
    }
    //-------------------------------------------------------------------------
    // Set up the message stream
    //-------------------------------------------------------------------------
    m_log.setLevel( m_msgSvc->outputLevel() );
    m_log << MSG::INFO << "LayerMaterialMapCnv::initialize()" << endreq;
   
    return StatusCode::SUCCESS;
}

LayerMaterialMap_PERS * LayerMaterialMapCnv::createPersistent( Trk::LayerMaterialMap *transCont)
{   
    m_log.setLevel( m_msgSvc->outputLevel() );
    updateLog(); // Make m_log indicate the current key
    return m_TPConverter_tlp1.createPersistent( transCont, m_log );
}


//-----------------------------------------------------------------------------
// Create transient collection
//-----------------------------------------------------------------------------
Trk::LayerMaterialMap *LayerMaterialMapCnv::createTransient()
{
    m_log.setLevel( m_msgSvc->outputLevel() );
    static pool::Guid tlp1_guid( "3DA92DBD-DA78-43A2-BFDF-9E19E2BF1E8A" );

    Trk::LayerMaterialMap *p_collection = 0;

    if( compareClassGuid( tlp1_guid ) ) {
       poolReadObject< Trk::LayerMaterialMap_tlp1 >( m_TPConverter_tlp1 );
       p_collection = m_TPConverter_tlp1.createTransient( m_log );
    }       
    return p_collection;
}

void LayerMaterialMapCnv::updateLog(){ 
     const DataObject* dObj = getDataObject();
     if (dObj==0) return; // Can't do much if this fails.
     const std::string  key = (dObj->name());
 
     m_log.m_source="LayerMaterialMapCnv: "+key; // A hack - relies on getting access to private data of MsgStream via #define trick. EJWM.
}
