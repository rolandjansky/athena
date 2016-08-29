/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

#include "TrkMaterialOnTrack/MaterialEffectsBase.h"
#include "TrkSurfaces/Surface.h"
#include "TrkParameters/TrackParameters.h"
#include "TrkRIO_OnTrack/RIO_OnTrack.h"
#include "TrkMeasurementBase/MeasurementBase.h"
#include "TrkDetElementBase/TrkDetElementBase.h"

#include "TrkPseudoMeasurementOnTrack/PseudoMeasurementOnTrack.h"

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
// Destructor
//-----------------------------------------------------------------------------
TrackCollectionCnv::~TrackCollectionCnv()
{
}

//-----------------------------------------------------------------------------
// Initializer
//-----------------------------------------------------------------------------
StatusCode TrackCollectionCnv::initialize()
{
    StatusCode sc = TrackCollectionCnvBase::initialize();
    if( sc.isFailure() ) {
        m_log << MSG::FATAL << "Could not initialize cnv base" << endmsg;
        return sc;
    }

    //-------------------------------------------------------------------------
    // Set up the message stream
    //-------------------------------------------------------------------------
    m_log.setLevel( MSG::VERBOSE );

    // m_log.setLevel( m_msgSvc->outputLevel() );
    m_log << MSG::VERBOSE << "TrackCollectionCnv::initialize()" << endmsg;

    IConverter	*converter =  m_athenaPoolCnvSvc->converter( CLID(17001567) );
    bool doInDet(true);
    bool doMuon(true);
    bool doCalo(true);

    if(converter != 0) registerExtendingCnv( converter );    
    else doInDet=false;

    converter =  m_athenaPoolCnvSvc->converter( CLID(239128586) );
    if(converter !=0 ) registerExtendingCnv( converter );
    else doMuon=false;

    converter =  m_athenaPoolCnvSvc->converter( CLID(1213853277) );
    if(converter !=0 ) registerExtendingCnv( converter );
    else doCalo=false;
    
    //-------------------------------------------------------------------------
    // We cannot proceed if none of the tools is present
    //-------------------------------------------------------------------------
    if( !doInDet && !doMuon && !doCalo) {
        m_log << MSG::WARNING << "Could not get any of the extending convertors, and so might have problems later on.";
        m_log << endmsg;
    }

    return StatusCode::SUCCESS;
}



TrackCollection_PERS * TrackCollectionCnv::createPersistent( TrackCollection *transCont)
{
    m_log.setLevel( m_msgSvc->outputLevel() );
    updateLog(); // Make m_log indicate the current key
    return m_TPConverterForPER.createPersistent( transCont, m_log );
}

//-----------------------------------------------------------------------------
// Create transient collection
//-----------------------------------------------------------------------------
TrackCollection *TrackCollectionCnv::createTransient()
{
    m_log.setLevel( m_msgSvc->outputLevel() );
    static pool::Guid p5_guid( "436E4996-9D6E-11E3-AD2A-6C3BE51AB9F1" );
    static pool::Guid p4_guid( "3BEB819F-6ED2-48F6-9F95-E65E1759E781" );
    static pool::Guid p3_guid( "A1E9FDCB-2F4A-4AC8-BF4E-2D70B9C70F8A" );
    static pool::Guid p2_guid( "2D8B19DC-DB2E-4F56-BB94-D7C4544D501A" );
    static pool::Guid p1_guid( "ECB12567-B999-4908-B0C6-C43CF9F9A987" );
    static pool::Guid p0_guid( "70ECEBFC-BE00-46C2-8B35-4CC12D18DE39" );

    TrackCollection *p_collection = 0;
    if( compareClassGuid( p5_guid )){
      poolReadObject< Trk::TrackCollection_tlp5 >( m_TPConverterForPER);
      p_collection = m_TPConverterForPER.createTransient( m_log );
    }   
    else if( compareClassGuid( p4_guid )){
      poolReadObject< Trk::TrackCollection_tlp4 >( m_TPConverter_tlp4);
      p_collection = m_TPConverter_tlp4.createTransient( m_log );
    }
    else if( compareClassGuid( p3_guid )){
      poolReadObject< Trk::TrackCollection_tlp3 >( m_TPConverter_tlp3);
      p_collection = m_TPConverter_tlp3.createTransient( m_log );
    }
    else if( compareClassGuid( p2_guid ) ) {
      poolReadObject< Trk::TrackCollection_tlp2 >( m_TPConverter_tlp2 );
      p_collection = m_TPConverter_tlp2.createTransient( m_log );
    }
    else if( compareClassGuid( p1_guid ) )  {
        /*
       usingTPCnvForReading( m_TPConverter );
       std::unique_ptr< TrackCollection_PERS >
	  p_coll( poolReadObject< TrackCollection_PERS >() );
       p_collection = m_TPConverter.createTransient( p_coll.get(), m_log );
       */
       poolReadObject< Trk::TrackCollection_tlp1 >( m_TPConverter_tlp1 );
       p_collection = m_TPConverter_tlp1.createTransient( m_log );
    }
    else if( compareClassGuid( p0_guid ) )  {
        p_collection = poolReadObject< TrackCollection >();
    }
    else
        throw std::runtime_error( "Unsupported persistent version of Data Collection" );
    return p_collection;
}

void TrackCollectionCnv::updateLog(){  
     const DataObject* dObj = getDataObject();
     if (dObj==0) return; // Can't do much if this fails.
     const std::string  key = (dObj->name());
 
     m_log.m_source="TrackCollectionCnv: "+key; // A hack - relies on getting access to private data of MsgStream via #define trick. EJWM.
}
