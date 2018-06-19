/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   SegmentCollectionCnv.cxx
// author: Andreas Wildauer
// author: Edward Moyse
//
//-----------------------------------------------------------------------------

#include "SegmentCollectionCnv.h"
#include "GaudiKernel/IToolSvc.h"


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
SegmentCollectionCnv::SegmentCollectionCnv(ISvcLocator* svcloc):
    SegmentCollectionCnvBase(svcloc),
    m_msgSvc( msgSvc() ),
    m_log( m_msgSvc, "SegmentCollectionCnv" )
{
}

//-----------------------------------------------------------------------------
// Destructor
//-----------------------------------------------------------------------------
SegmentCollectionCnv::~SegmentCollectionCnv() {}

//-----------------------------------------------------------------------------
// Initializer
//-----------------------------------------------------------------------------
void SegmentCollectionCnv::initializeOldExtConverters()
{
    if( m_oldExtCnvInitialized )  return;

    //-------------------------------------------------------------------------
    // Set up the message stream
    //-------------------------------------------------------------------------
    m_log.setLevel( m_msgSvc->outputLevel() );
    m_log << MSG::INFO << "SegmentCollectionCnv::initialize()" << endmsg;
    
    bool doInDet(true);
    bool doMuon(true);
    
    IConverter	*converter =  m_athenaPoolCnvSvc->converter( CLID(17001567) );
    m_log << MSG::DEBUG << "SegmentCollectionCnv: InDetTrack converter=" << converter << endmsg;
    if(converter != 0) registerExtendingCnv( converter );    
    else doInDet=false;
    
    converter =  m_athenaPoolCnvSvc->converter( CLID(239128586) );
    m_log << MSG::DEBUG << "SegmentCollectionCnv: MuonMeasurements converter=" << converter << endmsg;
    if(converter !=0 ) registerExtendingCnv( converter );
    else doMuon=false;
    
    //-------------------------------------------------------------------------
    // We cannot proceed if none of the tools is present
    //-------------------------------------------------------------------------
    if( !doInDet && !doMuon ) {
        m_log << MSG::FATAL << "Failed to load Extending T/P converters!" << endmsg;
    }
    m_oldExtCnvInitialized = true;
}


SegmentCollection_PERS *
SegmentCollectionCnv::createPersistent( Trk::SegmentCollection *transCont)
{   
    m_log.setLevel( m_msgSvc->outputLevel() );
    updateLog(); // Make m_log indicate the current key
    return m_TPConverterForPER.createPersistent( transCont, m_log );
}


//-----------------------------------------------------------------------------
// Create transient collection
//-----------------------------------------------------------------------------
Trk::SegmentCollection *SegmentCollectionCnv::createTransient()
{
    m_log.setLevel( m_msgSvc->outputLevel() ); 
    static pool::Guid tlp4_guid( "27FB33E0-3284-11E8-9390-0800271C02BC" );
    static pool::Guid tlp3_guid( "1AA73E8A-AF6D-11E3-A8C8-6C3BE51AB9F1" );
    static pool::Guid tlp2_guid( "61A4056D-4FDF-4798-93D7-1BAC351FE32E" );
    static pool::Guid tlp1_guid( "03A83988-8E38-45E9-95A5-9CB17B98074C" );
    static pool::Guid p0_guid( "AAE2BF35-4971-4368-B0B1-BE209CC81538" );

    Trk::SegmentCollection *p_collection = 0;

    if( compareClassGuid( tlp4_guid ) ) {
       poolReadObject< SegmentCollection_PERS >( m_TPConverterForPER );
       p_collection = m_TPConverterForPER.createTransient( m_log );
    } else if( compareClassGuid( tlp3_guid ) ) {
       initializeOldExtConverters();
       poolReadObject< Trk::SegmentCollection_tlp3 >( m_TPConverter_tlp3 );
       p_collection = m_TPConverter_tlp3.createTransient( m_log );
    } else if( compareClassGuid( tlp2_guid ) ) {
       initializeOldExtConverters();
       poolReadObject< Trk::SegmentCollection_tlp2 >( m_TPConverter_tlp2 );
       p_collection = m_TPConverter_tlp2.createTransient( m_log );
    } else if( compareClassGuid( tlp1_guid ) ) {
       initializeOldExtConverters();
       poolReadObject< Trk::SegmentCollection_tlp1 >( m_TPConverter_tlp1 );
       p_collection = m_TPConverter_tlp1.createTransient( m_log );
    }
    else if( compareClassGuid( p0_guid ) )  {
       p_collection = poolReadObject< Trk::SegmentCollection >();
    }
    else
        throw std::runtime_error( "Unsupported persistent version of Trk::SegmentCollection (unknown GUID)" );
        
    return p_collection;
}

void SegmentCollectionCnv::updateLog(){  
     const DataObject* dObj = getDataObject();
     if (dObj==0) return; // Can't do much if this fails.
     const std::string  key = (dObj->name());
 
     m_log.m_source="SegmentCollectionCnv: "+key; // A hack - relies on getting access to private data of MsgStream via #define trick. EJWM.
}
