/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/
#include "AthenaMonitoring/MonitoredScope.h"
#include "TrigCaloDataAccessSvc.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "CaloDetDescr/CaloDetDescrManager.h"

TrigCaloDataAccessSvc::TrigCaloDataAccessSvc( const std::string& name, ISvcLocator* pSvcLocator )
  : base_class( name, pSvcLocator ), m_lateInitDone(false), m_nSlots(0) {
}

TrigCaloDataAccessSvc::~TrigCaloDataAccessSvc() {}

StatusCode TrigCaloDataAccessSvc::initialize() {
  CHECK( m_roiMapTool.retrieve() );
  CHECK( m_larDecoder.retrieve() );
  CHECK( m_robDataProvider.retrieve() );
  CHECK( m_regionSelector.retrieve() );

  return StatusCode::SUCCESS;
}

StatusCode TrigCaloDataAccessSvc::finalize() {

  std::lock_guard<std::mutex> lock( m_initMutex ); // use the initMutex to finalize
  if ( m_lateInitDone ) { // otherwise nothing to delete
  m_vrodid32fullDet.clear();
  for( size_t ii=0;ii<m_vrodid32fullDetHG.size();ii++) { m_vrodid32fullDetHG[ii].clear(); }
  m_vrodid32fullDetHG.clear(); 
  for ( size_t slot  = 0; slot <  m_nSlots; ++ slot ) {
      EventContext ec;
      ec.setSlot( slot );
      HLTCaloEventCache *cache = m_larcell.get( ec );
      cache->container->finalize();
      delete cache->container;
      cache->lastFSEvent = 0xFFFFFFFF;
      delete cache->fullcont;
  } // end of for slots
  } // end of m_lateInitDone
  m_lateInitDone=false;
  return StatusCode::SUCCESS;
}


TrigCaloDataAccessSvc::Status 
TrigCaloDataAccessSvc::prepareFullCollections( const EventContext& context ) {

  return prepareLArFullCollections( context );
  
}


TrigCaloDataAccessSvc::Status 
TrigCaloDataAccessSvc::loadCollections ( const EventContext& context,
					 const IRoiDescriptor& roi,
					 const DETID detID,
					 const int sampling,
					 LArTT_Selector<LArCellCont>& loadedCells ) {

  std::vector<IdentifierHash> requestHashIDs;  
  Status status;

  ATH_MSG_DEBUG( "LArTT requested for event " << context << " and RoI " << roi );  
  status += prepareLArCollections( context, roi, sampling, detID );
  
  if ( not status.success() )
    return status;
  
  
  { 
    // this has to be guarded because getTT called on the LArCollection bu other threads updates internal map
    std::lock_guard<std::mutex> getCollClock{ m_larcell.get( context )->mutex };       
    m_regionSelector->DetHashIDList( detID, sampling, roi, requestHashIDs );
  }
  
  ATH_MSG_DEBUG( "requestHashIDs.size() in LoadColl = " << requestHashIDs.size()  << " hash checksum " 
    		 << std::accumulate( requestHashIDs.begin(), requestHashIDs.end(), IdentifierHash( 0 ),  
    				     []( IdentifierHash h1, IdentifierHash h2 ){  return h1+h2; } ) );
  if ( msgLvl( MSG::VERBOSE ) ) {    
    for( unsigned int i = 0 ; i < requestHashIDs.size() ; i++ )
      ATH_MSG_VERBOSE( "m_rIds[" << i << "]=" << requestHashIDs[i] );
  }
  loadedCells.setContainer( ( m_larcell.get( context )->container ) );
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
					     ConstDataVector<CaloCellContainer>& cont ) {

  Status status = prepareLArFullCollections( context );

  std::lock_guard<std::mutex> getCollClock{ m_getCollMutex };   
  CaloCellContainer* cont_to_copy = m_larcell.get(context)->fullcont ;
  cont.clear();
  cont.reserve( cont_to_copy->size() );
  for( const CaloCell* c : *cont_to_copy ) cont.push_back( c );
      
  return status;
}


TrigCaloDataAccessSvc::Status 
TrigCaloDataAccessSvc::prepareLArFullCollections( const EventContext& context ) {

  ATH_MSG_DEBUG( "Full Col " << " requested for event " << context );
  if ( !m_lateInitDone && lateInit().isFailure() ) {
    return Status::InternalError;
  }

  HLTCaloEventCache* cache = m_larcell.get( context );
  
  auto lockTime = Monitored::MonitoredTimer::declare ( "TIME_locking_LAr_FullDet" );  
  std::lock_guard<std::mutex> collectionLock { cache->mutex };  
  lockTime.stop();

  if ( cache->lastFSEvent == context.evt() ) return Status(0);

  Status status;

  for( size_t ii=0;ii<m_vrodid32fullDetHG.size();ii++) {
      std::vector<uint32_t>& vrodid32fullDet = m_vrodid32fullDetHG[ii];
      std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> robFrags;
      {
        std::lock_guard<std::mutex> dataPrepLock { m_dataPrepMutex };
        // To be confirmed whether we need this or not
        m_robDataProvider->addROBData( vrodid32fullDet );
        m_robDataProvider->getROBData( context, vrodid32fullDet, robFrags );      
      }
      cache->container->eventNumber( context.evt() ) ;
      
      Status status_loc = ( convertROBs( robFrags, ( cache->container ) ) );
      status.addError(status_loc.mask());
      
      if ( vrodid32fullDet.size() != robFrags.size() ) {
        ATH_MSG_DEBUG( "Missing ROBs, requested " << vrodid32fullDet.size() << " obtained " << robFrags.size() );
        status.addError( Status::MissingROB );
        clearMissing( vrodid32fullDet, robFrags, ( cache->container ) );
      }

  } // end of for m_vrodid32fullDetHG.size()

  int detid(0);
  auto detidMon = Monitored::MonitoredScalar::declare<int>( "det", detid );

  Monitored::MonitoredScope::declare( m_monTool, lockTime, detidMon );
  // collection prepared
  cache->lastFSEvent = context.evt();
  return status;  
}

StatusCode TrigCaloDataAccessSvc::lateInit() { // non-const this thing

  std::lock_guard<std::mutex> lock( m_initMutex );
  if ( m_lateInitDone ) 
    return StatusCode::SUCCESS;
  
  ATH_MSG_DEBUG( "Performing late init" );

  // allocate collections
  m_nSlots = SG::getNSlots();

  // preparing full container list of ROBs - tile will be included soon
  std::vector<uint32_t> vrodid32lar;
  std::vector<uint32_t> vrodid32em;
  std::vector<uint32_t> vrodid32hec;
  std::vector<uint32_t> vrodid32hec0;
  std::vector<uint32_t> vrodid32hec1;
  std::vector<uint32_t> vrodid32hec2;
  std::vector<uint32_t> vrodid32hec3;
  std::vector<uint32_t> vrodid32fcalem;
  std::vector<uint32_t> vrodid32fcalhad;
  std::vector<uint32_t> vrodid32tile;
  std::vector<uint32_t> vrodid32ros; //  for virtual  lar ros in ROBs
  std::vector<uint32_t> vrodid32tros; // for virtual tile ros in ROBs

  TrigRoiDescriptor tmproi(true);
  // TTEM
  m_regionSelector->DetROBIDListUint(TTEM,-1,tmproi,vrodid32em);
  // TTHEC
  m_regionSelector->DetROBIDListUint(TTHEC,0,tmproi,vrodid32hec0);
  m_regionSelector->DetROBIDListUint(TTHEC,1,tmproi,vrodid32hec1);
  m_regionSelector->DetROBIDListUint(TTHEC,2,tmproi,vrodid32hec2);
  m_regionSelector->DetROBIDListUint(TTHEC,3,tmproi,vrodid32hec3);
  // FCALHAD
  m_regionSelector->DetROBIDListUint(FCALHAD,-1,tmproi,vrodid32fcalhad);
  m_regionSelector->DetROBIDListUint(FCALEM,-1,tmproi,vrodid32fcalem);
  vrodid32lar.insert(vrodid32lar.end(),vrodid32em.begin(),vrodid32em.end());
  vrodid32hec.insert(vrodid32hec.end(),vrodid32hec0.begin(),vrodid32hec0.end());
  vrodid32lar.insert(vrodid32lar.end(),vrodid32hec.begin(),vrodid32hec.end());
  vrodid32lar.insert(vrodid32lar.end(),vrodid32fcalhad.begin(),vrodid32fcalhad.end());
  vrodid32lar.insert(vrodid32lar.end(),vrodid32fcalem.begin(),vrodid32fcalem.end());
  m_vrodid32fullDet.insert(m_vrodid32fullDet.end(), vrodid32lar.begin(), vrodid32lar.end() );

  unsigned int nFebs=70;
  unsigned int high_granu = (unsigned int)ceilf(m_vrodid32fullDet.size()/((float)nFebs) );
  unsigned int jj=0;
  unsigned int kk=0;
  m_vrodid32fullDetHG.resize(high_granu);
  for( unsigned int ii=0; ii<m_vrodid32fullDet.size();ii++){
	if ( kk >= nFebs ) {
	   kk-=nFebs;
	   jj++;
	}
	std::vector<uint32_t> & vec = m_vrodid32fullDetHG.at(jj);
	vec.push_back(m_vrodid32fullDet[ii]);
	kk++;
  }
  const CaloDetDescrManager* theCaloDDM = CaloDetDescrManager::instance();
  const CaloCell_ID* theCaloCCIDM = theCaloDDM->getCaloCell_ID();
  unsigned int hashMax = theCaloCCIDM->calo_cell_hash_max();

  // Prepare cache containers to be used for LAr unpacking.
  for ( size_t slot  = 0; slot <  m_nSlots; ++ slot ) {
  EventContext ec;
  ec.setSlot( slot );
  HLTCaloEventCache *cache = m_larcell.get( ec );
  cache->container = new LArCellCont();
  CHECK( cache->container->initialize( m_applyOffsetCorrection ) );
  std::vector<CaloCell*> local_cell_copy;
  local_cell_copy.reserve(200000);
  LArCellCont* larcell = cache->container;
  cache->lastFSEvent = 0xFFFFFFFF;
  CaloCellContainer* cachefullcont = new CaloCellContainer(SG::VIEW_ELEMENTS);
  cachefullcont->reserve(190000);
  for(unsigned int lcidx=0; lcidx < larcell->size(); lcidx++){
          LArCellCollection* lcc = larcell->at(lcidx);
          unsigned int lccsize = lcc->size();
          for(unsigned int lccidx=0; lccidx<lccsize; lccidx++){
                  CaloCell* cell = ((*lcc).at(lccidx));
                  if ( cell && cell->caloDDE() ) local_cell_copy.push_back( cell );
          } // end of loop over cells
    } // end of loop over collection

// This should stay here as this will be enabled when tile is ready to be decoded as well
/*
    for(unsigned int lcidx=0; lcidx < m_tilecell->size(); lcidx++){
          TileCellCollection* lcc = m_tilecell->at(lcidx);
          unsigned int lccsize = lcc->size();
          for(unsigned int lccidx=0; lccidx<lccsize; lccidx++){
                  CaloCell* cell = ((*lcc).at(lccidx));
                  if ( cell ) local_cell_copy.push_back( cell );
          } // end of loop over cells
    } // end of loop over collection
*/

  // For the moment the container has to be completed by hand (again, because of tile)
  for(unsigned int i=0;i<hashMax;i++){
        cachefullcont->push_back_fast(nullptr);
  }

  unsigned int localcellMax = local_cell_copy.size();
  for(unsigned int i=0;i<localcellMax;i++){
        unsigned int j = local_cell_copy.at(i)->caloDDE()->calo_hash();
        if ( j < hashMax ) {
                cachefullcont->at(j) = local_cell_copy.at(i);
        }
  }
  for(unsigned int i=0;i<hashMax;i++)
        if ( cachefullcont->at(i) == nullptr ){
                Identifier id = theCaloCCIDM->cell_id(i);
                if ( id!=0 ){
                  const CaloDetDescrElement* el = theCaloDDM->get_element(id);
                  cachefullcont->at(i) = (new CaloCell(el,0,0,0,(CaloGain::CaloGain)0) );
                }
        }

  cachefullcont->setHasCalo(CaloCell_ID::LAREM);
  cachefullcont->setHasCalo(CaloCell_ID::LARHEC);
  cachefullcont->setHasCalo(CaloCell_ID::LARFCAL);
  cachefullcont->setHasCalo(CaloCell_ID::TILE); //lying... But this needs to be checked later

  // make sure this "map" container has a good hashID
  cachefullcont->order();

  if ( hashMax != cachefullcont->size() )
    ATH_MSG_ERROR("Problem in the size of the full container");
  cachefullcont->setIsOrdered(true);
  cachefullcont->setIsOrderedAndComplete(true);
  cache->fullcont = cachefullcont;

  local_cell_copy.clear();
  }
  m_lateInitDone = true;
  return StatusCode::SUCCESS;
}

TrigCaloDataAccessSvc::Status 
TrigCaloDataAccessSvc::convertROBs( const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& robFrags, LArCellCont* larcell ) {

  Status status;
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
	} else { // End of if small size
	  //TB the converter has state
	  m_larDecoder->setRobFrag( rob );
	  m_larDecoder->fillCollectionHLT( roddata, roddatasize, *coll );

	  // Accumulates inferior byte from ROD Decoder
	  // TB the converter has state
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
					      const int sampling,
					      DETID detector ) {

  // If the full event was already unpacked, don't need to unpack RoI
  if ( !m_lateInitDone && lateInit().isFailure() ) {
    return Status::InternalError;
  }
  HLTCaloEventCache* cache = m_larcell.get( context );
  if ( cache->lastFSEvent == context.evt() ) return Status(0);

  std::vector<uint32_t> requestROBs;

  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> robFrags;
  {
    std::lock_guard<std::mutex> dataPrepLock { m_dataPrepMutex };
    m_regionSelector->DetROBIDListUint( detector, sampling, roi, requestROBs ); // we know that the RegSelSvc is thread safe   
    m_robDataProvider->addROBData( requestROBs );
    m_robDataProvider->getROBData( context, requestROBs, robFrags );
  }
  if ( robFrags.empty() ) {
    return Status();
  }

  auto lockTime = Monitored::MonitoredTimer::declare ( "TIME_locking_LAr_RoI" );  
  std::lock_guard<std::mutex> collectionLock { cache->mutex };  
  lockTime.stop();

  // TB, what would happen from now inside the collection unapcking
  // if this event number is different than the one for each collection the unpacking will happen, 
  // if it is the same the unpacking will not be repeated
  // same in prepareLArFullCollections
  cache->container->eventNumber( context.evt() );
  
  Status status = convertROBs( robFrags, ( cache->container ) );

  if ( requestROBs.size() != robFrags.size() ) {
    ATH_MSG_DEBUG( "Missing ROBs, requested " << requestROBs.size() << " obtained " << robFrags.size() );
    status.addError( Status::MissingROB );
    clearMissing( requestROBs, robFrags, ( cache->container ) );
  }
  auto roiROBs = Monitored::MonitoredScalar::declare( "roiROBs_LAr", robFrags.size() );
  auto roiEta = Monitored::MonitoredScalar::declare( "roiEta_LAr", roi.eta() );
  auto roiPhi = Monitored::MonitoredScalar::declare( "roiPhi_LAr", roi.phi() );

  Monitored::MonitoredScope::declare( m_monTool, lockTime, roiEta, roiPhi, roiROBs );
  return status;
}

