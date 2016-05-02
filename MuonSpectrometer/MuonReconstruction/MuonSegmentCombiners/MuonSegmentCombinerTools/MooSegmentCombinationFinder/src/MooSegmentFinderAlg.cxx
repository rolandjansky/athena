/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MooSegmentFinderAlg.h"
#include "MuonSegmentCombinerToolInterfaces/IMooSegmentCombinationFinder.h"

#include "MuonRecToolInterfaces/IMuonPatternSegmentAssociationTool.h"
#include "MuonSegmentMakerToolInterfaces/IMuonClusterSegmentFinder.h"

#include "MuonSegment/MuonSegment.h"
#include "MuonSegment/MuonSegmentCombination.h"

#include "MuonSegment/MuonSegmentCombinationCollection.h"
#include "MuonPrepRawData/MuonPrepDataContainer.h"

#include "StoreGate/StoreGateSvc.h"

MooSegmentFinderAlg::MooSegmentFinderAlg(const std::string& name, ISvcLocator* pSvcLocator):
  AthAlgorithm(name,pSvcLocator), 
  m_segmentFinder("Muon::MooSegmentCombinationFinder/MooSegmentCombinationFinder"),
  m_assocTool("Muon::MuonPatternSegmentAssociationTool/MuonPatternSegmentAssociationTool"),
  m_clusterSegMaker("Muon::MuonClusterSegmentFinder/MuonClusterSegmentFinder")
{
  declareProperty("UseRPC",m_useRpc = true);
  declareProperty("UseTGC",m_useTgc = true);
  declareProperty("UseTGCPriorBC",m_useTgcPriorBC = false);
  declareProperty("UseTGCNextBC",m_useTgcNextBC = false);
  declareProperty("UseCSC",m_useCsc = true);
  declareProperty("UseMDT",m_useMdt = true);

  declareProperty("doTGCClust",m_doTGCClust = false);
  declareProperty("doRPCClust",m_doRPCClust = false);


  declareProperty("CscPrepDataContainer", m_keyCsc = "CSC_Clusters");
  declareProperty("MdtPrepDataContainer", m_keyMdt = "MDT_DriftCircles");
  declareProperty("RpcPrepDataContainer", m_keyRpc = "RPC_Measurements");
  declareProperty("TgcPrepDataContainer", m_keyTgc = "TGC_Measurements");
  declareProperty("TgcPrepDataContainerPriorBC", m_keyTgcPriorBC = "TGC_MeasurementsPriorBC");
  declareProperty("TgcPrepDataContainerNextBC", m_keyTgcNextBC  = "TGC_MeasurementsNextBC");

  declareProperty("MuonPatternCombinationLocation",      m_patternCombiLocation = "MuonHoughPatternCombinations" );
  declareProperty("MuonSegmentOutputLocation",           m_segmentLocation      = "MooreSegments");
  declareProperty("MuonSegmentCombinationOutputLocation",m_segmentCombiLocation = "MooreSegmentCombinations");

  declareProperty("SegmentFinder", m_segmentFinder );
  declareProperty("MuonClusterSegmentFinderTool",m_clusterSegMaker);
}

MooSegmentFinderAlg::~MooSegmentFinderAlg()
{
}

StatusCode MooSegmentFinderAlg::initialize()
{
  if (AthAlgorithm::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }
  
  if( m_segmentFinder.retrieve().isFailure() ){
    ATH_MSG_FATAL("Could not get " << m_segmentFinder); 
    return StatusCode::FAILURE;
  }
  
  if( m_clusterSegMaker.retrieve().isFailure() ){
    ATH_MSG_FATAL("Could not get " << m_clusterSegMaker);
    return StatusCode::FAILURE;
  }

  if (m_assocTool.retrieve().isFailure()){
    ATH_MSG_FATAL("Could not get " << m_assocTool); 
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS; 
}

StatusCode MooSegmentFinderAlg::execute()
{

  m_assocTool->reset(); // clear all prior associations.

  std::vector<const Muon::MdtPrepDataCollection*> mdtCols;
  std::vector<const Muon::CscPrepDataCollection*> cscCols;
  std::vector<const Muon::TgcPrepDataCollection*> tgcCols; 
  std::vector<const Muon::RpcPrepDataCollection*> rpcCols;
  if( m_useMdt ) retrieveCollections(mdtCols,m_keyMdt);
  if( m_useCsc ) retrieveCollections(cscCols,m_keyCsc);
  if( m_useTgc ) retrieveCollections(tgcCols,m_keyTgc);
  if( m_useTgcPriorBC ) retrieveCollections(tgcCols,m_keyTgcPriorBC);
  if( m_useTgcNextBC )  retrieveCollections(tgcCols,m_keyTgcNextBC);
  if( m_useRpc ) retrieveCollections(rpcCols,m_keyRpc);

  Muon::IMooSegmentCombinationFinder::Output* output = m_segmentFinder->findSegments( mdtCols, cscCols, tgcCols, rpcCols );

  //do cluster based segment finding
  std::vector<const Muon::MuonSegment*>* segs(NULL);
  if (m_doTGCClust || m_doRPCClust) segs = m_clusterSegMaker->getClusterSegments(m_doTGCClust,m_doRPCClust);

  const MuonSegmentCombinationCollection* segmentCombinations = output ? output->segmentCombinations : 0;
  if( !segmentCombinations ) segmentCombinations = new MuonSegmentCombinationCollection();

  if( segmentCombinations ) {
    if( evtStore()->record(segmentCombinations,m_segmentCombiLocation).isSuccess() ){
      ATH_MSG_VERBOSE("stored MuonSegmentCombinationCollection " << segmentCombinations->size() 
			     << " at " << m_segmentCombiLocation);
    }else{
      ATH_MSG_ERROR("Failed to store MuonSegmentCombinationCollection at " << m_segmentCombiLocation);
    }
  }
  
  Trk::SegmentCollection* segmentCollection = output ? const_cast<Trk::SegmentCollection*>(output->segmentCollection) : 0;
  if( !segmentCollection ) segmentCollection = new Trk::SegmentCollection();

  //add the cluster segments to the segmentCollection
  if (segs){ 
    for( std::vector<const Muon::MuonSegment*>::iterator sit = segs->begin(); sit!=segs->end();++sit ){
      Muon::MuonSegment* nonConstIt = const_cast<Muon::MuonSegment*>(*sit);
      Trk::Segment* trkSeg = nonConstIt;
      segmentCollection->push_back(trkSeg);
    }
    delete segs;
  }
 
  if (evtStore()->record(segmentCollection,m_segmentLocation).isSuccess() ){
    ATH_MSG_VERBOSE("stored MuonSegmentCollection at " <<  m_segmentLocation 
			   << " size " << segmentCollection->size());
  }else{
    ATH_MSG_ERROR("Failed to store MuonSegmentCollection ");
  }

  const MuonPatternCombinationCollection* patternCombinations = output ? output->patternCombinations : 0;
  if( !patternCombinations ) patternCombinations = new MuonPatternCombinationCollection();

  if( evtStore()->record(patternCombinations,m_patternCombiLocation).isSuccess() ){
    ATH_MSG_VERBOSE("stored MuonPatternCombinationCollection " << patternCombinations->size() 
						 << " at " << m_patternCombiLocation);
  }else{
    ATH_MSG_ERROR("Failed to store MuonPatternCombinationCollection at " << m_patternCombiLocation);
  }
  
  delete output;

  return StatusCode::SUCCESS;
} // execute

StatusCode MooSegmentFinderAlg::finalize()
{
  
  return AthAlgorithm::finalize();
}

void MooSegmentFinderAlg::retrieveCollections( std::vector<const Muon::RpcPrepDataCollection*>& cols, std::string key ) {

  const Muon::RpcPrepDataContainer* rpcPrds = 0;      
  StatusCode sc =  evtStore()->retrieve(rpcPrds,key);
  if (sc.isFailure()) {
    ATH_MSG_VERBOSE("Cannot retrieve RpcPrepDataContainer " << key << " accessing via collections ");

    // Access by Collection
    const DataHandle<Muon::RpcPrepDataCollection> collection;
    const DataHandle<Muon::RpcPrepDataCollection> lastColl;
	    
    if (evtStore()->retrieve(collection,lastColl).isSuccess()) {
      ATH_MSG_VERBOSE("collections retrieved");
      for ( ; collection != lastColl ; ++collection ) {
	if( collection->empty() ) continue;
	cols.push_back( &*collection );
      }
    }else{
      ATH_MSG_VERBOSE("Cannot retrieve RpcPrepDataCollections using DataHandle");
    }
  }else{
      
    
    Muon::RpcPrepDataContainer::const_iterator it = rpcPrds->begin();
    Muon::RpcPrepDataContainer::const_iterator it_end = rpcPrds->end();
    for( ; it!=it_end; ++it ) {
      // skip empty collections
      if( (*it)->empty() ) continue;
      cols.push_back( *it ) ;
    }
    ATH_MSG_VERBOSE("Retrieved RpcPrepDataContainer " <<  cols.size());
  }
}

void MooSegmentFinderAlg::retrieveCollections( std::vector<const Muon::TgcPrepDataCollection*>& cols, std::string key ) {

  const Muon::TgcPrepDataContainer* tgcPrds = 0;      
  StatusCode sc =  evtStore()->retrieve(tgcPrds,key);

  if (sc.isFailure()) {
    ATH_MSG_VERBOSE("Cannot retrieve TgcPrepDataContainer " << key << " accessing via collections ");

    // Access by Collection
    const DataHandle<Muon::TgcPrepDataCollection> collection;
    const DataHandle<Muon::TgcPrepDataCollection> lastColl;
	    
    if (evtStore()->retrieve(collection,lastColl).isSuccess()) {
      ATH_MSG_VERBOSE("collections retrieved");
      for ( ; collection != lastColl ; ++collection ) {
	if( collection->empty() ) continue;
	cols.push_back( &*collection );

      }
    }else{
      ATH_MSG_VERBOSE("Cannot retrieve TgcPrepDataCollections using DataHandle");
    }
  }else{
      
	
    Muon::TgcPrepDataContainer::const_iterator it = tgcPrds->begin();
    Muon::TgcPrepDataContainer::const_iterator it_end = tgcPrds->end();
    for( ; it!=it_end; ++it ) {
      // skip empty collections
      if( (*it)->empty() ) continue;
      cols.push_back( *it );
    }

    ATH_MSG_VERBOSE("Retrieved TgcPrepDataContainer " <<  cols.size());

  }
}


void MooSegmentFinderAlg::retrieveCollections( std::vector<const Muon::MdtPrepDataCollection*>& cols, std::string key ) {

  const Muon::MdtPrepDataContainer* mdtPrds = 0;      
  StatusCode sc = evtStore()->retrieve(mdtPrds,key);

    
  if (sc.isFailure()) {
    ATH_MSG_VERBOSE("Cannot retrieve MdtPrepDataContainer " << key << " accessing via collections ");

    // Access by Collection
    const DataHandle<Muon::MdtPrepDataCollection> collection;
    const DataHandle<Muon::MdtPrepDataCollection> lastColl;
    
    if (evtStore()->retrieve(collection,lastColl).isSuccess()) {
      ATH_MSG_VERBOSE("collections retrieved");
      for ( ; collection != lastColl ; ++collection ) {
	if( collection->empty() ) continue;
	cols.push_back( &*collection );
      }
    }else{
      ATH_MSG_VERBOSE("Cannot retrieve MdtPrepDataCollections using DataHandle");
    }
  }else{
    
    
    Muon::MdtPrepDataContainer::const_iterator it = mdtPrds->begin();
    Muon::MdtPrepDataContainer::const_iterator it_end = mdtPrds->end();
    for( ; it!=it_end; ++it ) {
      // skip empty collections
      if( (*it)->empty() ) continue;
      cols.push_back( *it );
    }
    ATH_MSG_VERBOSE("Retrieved MdtPrepDataContainer " <<  cols.size());

  }
}


void MooSegmentFinderAlg::retrieveCollections( std::vector<const Muon::CscPrepDataCollection*>& cols, std::string key ) {
  const Muon::CscPrepDataContainer* cscPrds = 0;      
  StatusCode sc =  evtStore()->retrieve(cscPrds,key);
  if (sc.isFailure()) {
    ATH_MSG_VERBOSE("Cannot retrieve CscPrepDataContainer " << key << " accessing via collections ");

    // Access by Collection
    const DataHandle<Muon::CscPrepDataCollection> collection;
    const DataHandle<Muon::CscPrepDataCollection> lastColl;
      
    if (evtStore()->retrieve(collection,lastColl).isSuccess()) {
      ATH_MSG_VERBOSE("collections retrieved");
      for ( ; collection != lastColl ; ++collection ) {
	if( collection->empty() ) continue;
	cols.push_back( &*collection );
      }
    }else{
      ATH_MSG_VERBOSE("Cannot retrieve CscPrepDataCollections using DataHandle");
    }
  }else{
      
      
    Muon::CscPrepDataContainer::const_iterator it = cscPrds->begin();
    Muon::CscPrepDataContainer::const_iterator it_end = cscPrds->end();
    for( ; it!=it_end; ++it ) {
      // skip empty collections
      if( (*it)->empty() ) continue;
      cols.push_back( *it );
    }
    ATH_MSG_VERBOSE("Retrieved CscPrepDataContainer " <<  cols.size());
  }
}
