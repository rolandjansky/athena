/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////
// LayerMaterialCollectionCollectionCnv.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "LayerMaterialCollectionCnv.h"
#include "GaudiKernel/IToolSvc.h"

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
LayerMaterialCollectionCnv::LayerMaterialCollectionCnv(ISvcLocator* svcloc):
    LayerMaterialCollectionCnvBase(svcloc),
    m_msgSvc( msgSvc() ),
    m_log( m_msgSvc, "LayerMaterialCollectionCnv" )
{
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
LayerMaterialCollectionCnv::~LayerMaterialCollectionCnv() {}

//-----------------------------------------------------------------------------
// Initializer
//-----------------------------------------------------------------------------
StatusCode LayerMaterialCollectionCnv::initialize()
{
    StatusCode sc = LayerMaterialCollectionCnvBase::initialize();
    if( sc.isFailure() ) {
        m_log << MSG::FATAL << "Could not initialize cnv base" << endreq;
        return sc;
    }
    //-------------------------------------------------------------------------
    // Set up the message stream
    //-------------------------------------------------------------------------
    m_log.setLevel( m_msgSvc->outputLevel() );
    m_log << MSG::INFO << "LayerMaterialCollectionCnv::initialize()" << endreq;
   
    return StatusCode::SUCCESS;
}

LayerMaterialCollection_PERS *
LayerMaterialCollectionCnv::createPersistent( Trk::LayerMaterialCollection *transCont)
{   
    m_log.setLevel( m_msgSvc->outputLevel() );
    updateLog(); // Make m_log indicate the current key
    return m_TPConverter_tlp1.createPersistent( transCont, m_log );
}


//-----------------------------------------------------------------------------
// Create transient collection
//-----------------------------------------------------------------------------
Trk::LayerMaterialCollection *LayerMaterialCollectionCnv::createTransient()
{
    m_log.setLevel( m_msgSvc->outputLevel() );
    static pool::Guid tlp1_guid( "DFB29CDE-4618-4712-AB60-FEFEDB071415" );

    Trk::LayerMaterialCollection *p_collection = 0;

    if( compareClassGuid( tlp1_guid ) ) {
       poolReadObject< Trk::LayerMaterialCollection_tlp1 >( m_TPConverter_tlp1 );
       p_collection = m_TPConverter_tlp1.createTransient( m_log );
    } else
        throw std::runtime_error( "Unsupported persistent version of Trk::LayerMaterialCollection (unknown GUID)" );
       
    return p_collection;
}

void LayerMaterialCollectionCnv::updateLog(){ 
     DataObject* dObj = getDataObject();
     if (dObj==0) return; // Can't do much if this fails.
     const std::string  key = (dObj->name());
 
     m_log.m_source="LayerMaterialCollectionCnv: "+key; // A hack - relies on getting access to private data of MsgStream via #define trick. EJWM.
}