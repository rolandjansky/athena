/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "AthenaMonitoring/MonitoredScope.h"
#include "TrigCaloDataAccessSvc.h"


TrigCaloDataAccessSvc::TrigCaloDataAccessSvc( const std::string& name, ISvcLocator* pSvcLocator )
  : base_class( name, pSvcLocator ) {
}

TrigCaloDataAccessSvc::~TrigCaloDataAccessSvc() {}

StatusCode TrigCaloDataAccessSvc::initialize() {
  CHECK( m_roiMapTool.retrieve() );
  CHECK( m_larDecoder.retrieve() );
  CHECK( m_robDataProvider.retrieve() );
  CHECK( m_regionSelector.retrieve() );

  return StatusCode::SUCCESS;
}

static const std::vector<unsigned> LAr { TTEM, TTHEC, FCALEM, FCALHAD };

TrigCaloDataAccessSvc::Status 
TrigCaloDataAccessSvc::prepareCollections( const EventContext& context,
					   const IRoiDescriptor& roi, 
					   DETID detid ) {
  if ( std::count( LAr.begin(), LAr.end(),  detid ) )
    return prepareLArCollections( context, roi, detid );
  else
    return Status::InternalError;
}

TrigCaloDataAccessSvc::Status 
TrigCaloDataAccessSvc::prepareFullCollections( const EventContext& context, DETID detid ) {

  if ( std::count( LAr.begin(), LAr.end(),  detid ) )
    return prepareLArFullCollections( context, detid );
  else
    return Status::InternalError;
  
}


TrigCaloDataAccessSvc::Status 
TrigCaloDataAccessSvc::loadCollections ( const EventContext& context,
					 const IRoiDescriptor& roi,
					 LArTT_Selector<LArCellCont>& loadedCells ) {

  std::vector<IdentifierHash> requestHashIDs;  
  Status status;

  ATH_MSG_DEBUG( "LArTT requested for event " << context << " and RoI " << roi );  
  status += prepareLArCollections( context, roi, TTEM );
  status += prepareLArCollections( context, roi, TTHEC );
  status += prepareLArCollections( context, roi, FCALEM );
  status += prepareLArCollections( context, roi, FCALHAD );
  
  if ( not status.success() )
    return status;
  
  
  m_regionSelector->DetHashIDList( TTEM, roi, requestHashIDs );
  
  ATH_MSG_DEBUG( "requestHashIDs.size() in LoadColl = " << requestHashIDs.size()  << " hash checksum " 
    		 << std::accumulate( requestHashIDs.begin(), requestHashIDs.end(), IdentifierHash( 0 ),  
    				     []( IdentifierHash h1, IdentifierHash h2 ){  return h1+h2; } ) );
  if ( msgLvl( MSG::VERBOSE ) ) {    
    for( unsigned int i = 0 ; i < requestHashIDs.size() ; i++ )
      ATH_MSG_VERBOSE( "m_rIds[" << i << "]=" << requestHashIDs[i] );
  }
  loadedCells.setContainer( & ( m_larcell.get( context )->container ) );
  loadedCells.setMap( m_roiMapTool.operator->() );    

  { 
    // this has to be guarded because getTT called on the LArCollection bu other threads updates internal map
    std::lock_guard<std::mutex> getCollClock{ m_larcell.get( context )->mutex };       
    loadedCells.setRoIs( requestHashIDs );
  }
  return status;
}





TrigCaloDataAccessSvc::Status 
TrigCaloDataAccessSvc::loadFullCollections ( const EventContext& context,
					     DETID detid,
					     LArTT_Selector<LArCellCont>::const_iterator& begin,
					     LArTT_Selector<LArCellCont>::const_iterator& end ) {
  std::lock_guard<std::mutex> getCollClock{ m_getCollMutex };   
  Status status = prepareLArFullCollections( context, detid );
  auto setIterators = [&]( const LArTT_Selector<LArCellCont>& selector ) {
    begin = selector.begin();
    end   = selector.end();
  };

  if ( detid == 0 ) {
    setIterators( m_larcell.get( context )->allSelector );
  } else if ( detid == TTEM ) {
    setIterators ( m_larcell.get( context )->ttemSelector );
  } else if ( detid == TTHEC ) {
    setIterators(  m_larcell.get( context )->tthecSelector );
  } else if ( detid == FCALEM ) {
    setIterators( m_larcell.get( context )->fcalemSelector );
  } else if ( detid == FCALHAD ) {
    setIterators( m_larcell.get( context )->fcalhadSelector );
  } else {
    ATH_MSG_ERROR( "Impossible detector ID " << detid );
    return Status::InternalError;
  }
      
  return status;
}


TrigCaloDataAccessSvc::Status 
TrigCaloDataAccessSvc::prepareLArFullCollections( const EventContext& context, DETID detid ) {

  ATH_MSG_DEBUG( "Full LArTT " << detid << " requested for event " << context );
  if ( lateInit().isFailure() ) {
    return Status::InternalError;
  }
  
  m_roiMapTool->CollectionID( 0 ); // triggers init of the inner structures

  const std::vector<uint32_t>* requestROBs = nullptr;
  if ( detid == 0 ) {
    requestROBs =  &m_allLArIDs.robs;
  } else {
    DETIDtoIdentifiers::const_iterator ids = m_detectorIDs.find( detid );
    if ( ids == m_detectorIDs.end() ) {
      ATH_MSG_ERROR( "Impossible detector ID " << detid );
      return Status::InternalError;
    }    
    requestROBs = &( ids->second.robs );
  }

  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> robFrags;
  {
    std::lock_guard<std::mutex> dataPrepLock { m_dataPrepMutex };
    m_robDataProvider->addROBData( *requestROBs );
    m_robDataProvider->getROBData( *requestROBs, robFrags );      
  }

  LArEventCache* cache = m_larcell.get( context );
  
  auto lockTime = Monitored::MonitoredTimer::declare ( "TIME_locking_LAr_FullDet" );  
  std::lock_guard<std::mutex> collectionLock { cache->mutex };  
  lockTime.stop();

  cache->container.eventNumber( context.evt() );
  
  Status status = convertROBs( robFrags, &( cache->container ) );

  if ( requestROBs->size() != robFrags.size() ) {
    ATH_MSG_DEBUG( "Missing ROBs, requested " << requestROBs->size() << " obtained " << robFrags.size() );
    status.addError( Status::MissingROB );
    clearMissing( *requestROBs, robFrags, &( cache->container ) );
  }
  auto detidMon = Monitored::MonitoredScalar::declare<int>( "det", detid );

  Monitored::MonitoredScope::declare( m_monTool, lockTime, detidMon );
  return status;  
}



StatusCode TrigCaloDataAccessSvc::lateInit() {
  std::lock_guard<std::mutex> lock( m_initMutex );
  if ( m_lateInitDone ) 
    return StatusCode::SUCCESS;
  
  ATH_MSG_DEBUG( "Performing late init" );
  // filling all sub det selectors
  for ( auto detector: {TTEM, TTHEC, FCALEM, FCALHAD } ) {
    m_regionSelector->DetROBIDListUint( detector, m_detectorIDs[detector].robs );
    m_regionSelector->DetHashIDList(    detector, m_detectorIDs[detector].ids );
    m_allLArIDs.merge( { m_detectorIDs[detector] } );
  }
    

  for ( LArEventCache& cache: m_larcell ) { 
    CHECK( cache.container.initialize( m_applyOffsetCorrection ) );

    auto prepareSelectors = [&]( LArTT_Selector<LArCellCont>& selector, const FullDetIDs& i ) {
      selector.setContainer( &( cache.container ) );
      selector.setMap( m_roiMapTool.operator->() );
      selector.setRoIs( i.ids );      

    };
    prepareSelectors( cache.allSelector,     m_allLArIDs );
    prepareSelectors( cache.ttemSelector,    m_detectorIDs[TTEM] );
    prepareSelectors( cache.tthecSelector,   m_detectorIDs[TTHEC] );
    prepareSelectors( cache.fcalemSelector,  m_detectorIDs[FCALEM] );
    prepareSelectors( cache.fcalhadSelector, m_detectorIDs[FCALHAD] );    
  }



  m_lateInitDone = true;
  return StatusCode::SUCCESS;
}

TrigCaloDataAccessSvc::Status 
TrigCaloDataAccessSvc::convertROBs( const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& robFrags, LArCellCont* larcell ) {

  Status status;
  ATH_MSG_DEBUG( "start decoding" );
  for ( auto rob: robFrags ) {
    uint32_t sourceID = rob->source_id();
    const std::vector<LArCellCollection*>::const_iterator it = larcell->find( sourceID );
    //TB The find also switches the state of the collection to "decoded" so repeated decoding is avoided

    if ( it != larcell->end() && ( *it )->size() != 0 ) { // Already decoded collection

      // TB if we have entered here it means the decoding did not occur yet ( or failed )
      // 
      LArCellCollection* coll = *it; 
      ATH_MSG_DEBUG( "ROB of ID " << sourceID << " to be decoded"   );

      //TB next two lines seem danger, as they seem to rely on the decoder state 
      m_larDecoder->setsecfeb( larcell->findsec( sourceID ) );
      if ( ! m_larDecoder->check_valid( rob, msg() ) ){
      	ATH_MSG_WARNING( "Error reading bytestream"<<
      			 "event: Bad ROB block ( eformat checks ) : 0x"
      			 << std::hex << sourceID << std::dec );
      	// Data seems corrupted
      	status.addError( Status::RawDataCorrupted );
      	reset_LArCol ( coll );

      } else {
	// Get Rod Data and size of fragment
	const uint32_t* roddata = 0;
	rob->rod_data( roddata );
	size_t roddatasize = rob->rod_ndata();
	if ( roddatasize < 3 ) {
	  ATH_MSG_WARNING( "Error reading bytestream"<<
			   "event: Empty ROD block ( less than 3 words ) : 0x"
			   << std::hex << sourceID << std::dec );
	  // Data seems corrupted
	  status.addError( Status::RawDataCorrupted );
	  reset_LArCol ( coll );
	  //return StatusCode::SUCCESS;
	} else { // End of if small size
	  //TB the converter has state
	  m_larDecoder->setRobFrag( rob );
	  m_larDecoder->fillCollectionHLT( roddata, roddatasize, *coll );


	  // size_t collsize = coll->size();	  
	  // for(int i=0;i<collsize;i++) {
	  //   std::cout << sourceID << " " << collsize << " " << i << " ";
	  //   std::cout << coll->at(i) << "ID " << coll->at(i)->ID() << " ";
	  //   std::cout << coll->at(i)->energy() << " DDE ";
	  //   std::cout << coll->at(i)->caloDDE() << "\n";
	  //   if ( coll->at(i)->caloDDE() ) {
	  //     std::cout << coll->at(i)->eta() << " ";
	  //     std::cout << coll->at(i)->phi() << " \n";
	  //   }
	  // }
	  

	  // Accumulates inferior byte from ROD Decoder
	  // TB the converter has state! we need to fix it
	  status.addError( m_larDecoder->report_error() );

	  if ( m_applyOffsetCorrection ) larcell->applyBCIDCorrection( sourceID );
	} 
	
      }
    } else {
      ATH_MSG_VERBOSE( "ROB of ID " <<  sourceID << " already decoded" );
    }
  }
  ATH_MSG_DEBUG( "finished decoding" );
  return status;  
}

void TrigCaloDataAccessSvc::missingROBs( const std::vector<uint32_t>& request,
					 const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& response,
					 std::set<uint32_t>& missing ) const {

  std::set<uint32_t> receivedROBsSet;
  for ( auto rob: response ) 
    receivedROBsSet.insert( rob->source_id() );
  std::set<uint32_t> requestedROBsSet( request.begin(), request.end() );
  
  std::set_difference( requestedROBsSet.begin(), requestedROBsSet.end(),
		       receivedROBsSet.begin(), receivedROBsSet.end(),
		       std::inserter( missing, missing.begin() ) );  
}


void TrigCaloDataAccessSvc::clearMissing( const std::vector<uint32_t>& request,
					  const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& response, 
					  LArCellCont* larcell ) {
  std::set<uint32_t> missing;
  missingROBs( request, response, missing );
  for ( uint32_t robID : missing ) {
    const std::vector<LArCellCollection*>::const_iterator it = larcell->find( robID );
    if ( it != larcell->end() && ( *it )->size()!=0 ) { // Already decoded collection
      reset_LArCol ( *it );
    } 
  }  
}


TrigCaloDataAccessSvc::Status 
TrigCaloDataAccessSvc::prepareLArCollections( const EventContext& context,
					      const IRoiDescriptor& roi, 
					      DETID detector ) {
  if ( lateInit().isFailure() ) {
    return Status::InternalError;
  }

  std::vector<uint32_t> requestROBs;

  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> robFrags;
  {
    std::lock_guard<std::mutex> dataPrepLock { m_dataPrepMutex };
    m_regionSelector->DetROBIDListUint( detector, roi, requestROBs ); // we know that the RegSelSvc is thread safe   
    m_robDataProvider->getROBData( requestROBs, robFrags );
  }
  if ( robFrags.empty() ) {
    return Status();
  }

  LArEventCache* cache = m_larcell.get( context );

  auto lockTime = Monitored::MonitoredTimer::declare ( "TIME_locking_LAr_RoI" );  
  std::lock_guard<std::mutex> collectionLock { cache->mutex };  
  lockTime.stop();

  // TB, what would happen from now inside the collection unapcking
  // if this event number is different than the one for each collection the unpacking will happen, 
  // if it is the same the unpacking will not be repeated
  // same in prepareLArFullCollections
  cache->container.eventNumber( context.evt() );
  
  Status status = convertROBs( robFrags, &( cache->container ) );

  if ( requestROBs.size() != robFrags.size() ) {
    ATH_MSG_DEBUG( "Missing ROBs, requested " << requestROBs.size() << " obtained " << robFrags.size() );
    status.addError( Status::MissingROB );
    clearMissing( requestROBs, robFrags, &( cache->container ) );
  }
  auto roiROBs = Monitored::MonitoredScalar::declare( "roiROBs_LAr", robFrags.size() );
  auto roiEta = Monitored::MonitoredScalar::declare( "roiEta_LAr", roi.eta() );
  auto roiPhi = Monitored::MonitoredScalar::declare( "roiPhi_LAr", roi.phi() );

  Monitored::MonitoredScope::declare( m_monTool, lockTime, roiEta, roiPhi, roiROBs );
  return status;
}

