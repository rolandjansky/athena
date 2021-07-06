/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//-----------------------------------------------------------------------------
//
// file:   TrackCollectionCnv.cxx
// author: Lukasz Janyst <ljanyst@cern.ch>
//
// Ed's converter rewritten and T/P split stuff added
//
// TODO: MultiComponentTSoS, integrity checking
//
//-----------------------------------------------------------------------------
#include "TrackCollectionCnv.h"

#include "GaudiKernel/IToolSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "AtlasDetDescr/AtlasDetectorID.h"


//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
TrackCollectionCnv::TrackCollectionCnv(ISvcLocator* svcloc):
    TrackCollectionCnvBase(svcloc),
    m_msgSvc( msgSvc() ),
    m_log( m_msgSvc, "TrackCollectionCnv" )
{
}

//-----------------------------------------------------------------------------
// Initializer
//-----------------------------------------------------------------------------
StatusCode TrackCollectionCnv::initialize()
{
   // leaving empty method for future use
   return TrackCollectionCnvBase::initialize();
}


void TrackCollectionCnv::initializeOldExtConverters()
{
   if( m_oldExtCnvInitialized )  return;

   // m_log.setLevel( m_msgSvc->outputLevel() );
   m_log << MSG::INFO << "TrackCollectionCnv::initializeOldExtConverters()" << endmsg;

   bool doInDet(true);
   bool doMuon(true);
   bool doCalo(true);

    IConverter	*converter =  m_athenaPoolCnvSvc->converter( CLID(17001567) );
    if(converter != nullptr) registerExtendingCnv( converter );    
    else doInDet=false;

    converter =  m_athenaPoolCnvSvc->converter( CLID(239128586) );
    if(converter !=nullptr ) registerExtendingCnv( converter );
    else doMuon=false;

    converter =  m_athenaPoolCnvSvc->converter( CLID(1213853277) );
    if(converter !=nullptr ) registerExtendingCnv( converter );
    else doCalo=false;
    
    //-------------------------------------------------------------------------
    // We cannot proceed if none of the tools is present
    //-------------------------------------------------------------------------
    if( !doInDet && !doMuon && !doCalo) {
        m_log << MSG::WARNING << "Could not get any of the extending convertors, and so might have problems later on.";
        m_log << endmsg;
    }
    m_oldExtCnvInitialized = true;
}



TrackCollection_PERS * TrackCollectionCnv::createPersistentWithKey( TrackCollection *transCont,
                                                                    const std::string& key)
{
    std::string logname = "TrackCollectionCnv";
    if (const DataObject* dObj = getDataObject()) {
      logname += dObj->name();
    }

    MsgStream log (m_msgSvc, logname );

    return m_TPConverter.createPersistentWithKey ( transCont, key, log );
}

//-----------------------------------------------------------------------------
// Create transient collection
//-----------------------------------------------------------------------------
TrackCollection *TrackCollectionCnv::createTransientWithKey(const std::string& key)
{
    m_log.setLevel( m_msgSvc->outputLevel() );
    static const pool::Guid p6_guid( "3228B252-2C5D-11E8-B170-0800271C02BC" );
    static const pool::Guid p5_guid( "436E4996-9D6E-11E3-AD2A-6C3BE51AB9F1" );
    static const pool::Guid p4_guid( "3BEB819F-6ED2-48F6-9F95-E65E1759E781" );
    static const pool::Guid p3_guid( "A1E9FDCB-2F4A-4AC8-BF4E-2D70B9C70F8A" );
    static const pool::Guid p2_guid( "2D8B19DC-DB2E-4F56-BB94-D7C4544D501A" );
    static const pool::Guid p1_guid( "ECB12567-B999-4908-B0C6-C43CF9F9A987" );
    static const pool::Guid p0_guid( "70ECEBFC-BE00-46C2-8B35-4CC12D18DE39" );

    TrackCollection *p_collection = nullptr;
    if( compareClassGuid( p6_guid )){
      poolReadObject< TrackCollection_PERS >( m_TPConverter );
      p_collection = m_TPConverter.createTransientWithKey( key, m_log );
    }
    else if( compareClassGuid( p5_guid )){
      initializeOldExtConverters();
      poolReadObject< Trk::TrackCollection_tlp5 >( m_TPConverter_tlp5);
      p_collection = m_TPConverter_tlp5.createTransientWithKey( key, m_log );
    }   
    else if( compareClassGuid( p4_guid )){
      initializeOldExtConverters();
      poolReadObject< Trk::TrackCollection_tlp4 >( m_TPConverter_tlp4);
      p_collection = m_TPConverter_tlp4.createTransientWithKey( key, m_log );
    }
    else if( compareClassGuid( p3_guid )){
      initializeOldExtConverters();
      poolReadObject< Trk::TrackCollection_tlp3 >( m_TPConverter_tlp3);
      p_collection = m_TPConverter_tlp3.createTransientWithKey( key, m_log );
    }
    else if( compareClassGuid( p2_guid ) ) {
      initializeOldExtConverters();
      poolReadObject< Trk::TrackCollection_tlp2 >( m_TPConverter_tlp2 );
      p_collection = m_TPConverter_tlp2.createTransientWithKey( key, m_log );
    }
    else if( compareClassGuid( p1_guid ) )  {
       initializeOldExtConverters();
       poolReadObject< Trk::TrackCollection_tlp1 >( m_TPConverter_tlp1 );
       p_collection = m_TPConverter_tlp1.createTransientWithKey( key, m_log );
    }
    else if( compareClassGuid( p0_guid ) )  {
        p_collection = poolReadObject< TrackCollection >();
    }
    else
        throw std::runtime_error( "Unsupported persistent version of Data Collection" );
    return p_collection;
}

