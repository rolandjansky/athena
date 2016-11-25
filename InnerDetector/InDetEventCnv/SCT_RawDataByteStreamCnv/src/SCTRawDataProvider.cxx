/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <memory>
#include "SCTRawDataProvider.h"

#include "SCT_RawDataByteStreamCnv/ISCTRawDataProviderTool.h"
#include "SCT_Cabling/ISCT_CablingSvc.h"
#include "InDetIdentifier/SCT_ID.h"

/// --------------------------------------------------------------------
/// Constructor

SCTRawDataProvider::SCTRawDataProvider(const std::string& name,
				       ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_robDataProvider ("ROBDataProviderSvc",name),
  m_rawDataTool     ("SCTRawDataProviderTool",this),
  m_cabling         ("SCT_CablingSvc",name),
  m_sct_id(nullptr),
  m_rdoContainerKey(""),
  m_lvl1CollectionKey(""),
  m_bcidCollectionKey("")
{
  declareProperty("RDOKey", m_rdoContainerKey = std::string("SCT_RDOs"));
  declareProperty("LVL1IDKey", m_lvl1CollectionKey = std::string("SCT_LVL1ID"));
  declareProperty("BCIDKey", m_bcidCollectionKey = std::string("SCT_BCID"));
  declareProperty ("ProviderTool", m_rawDataTool);
  declareProperty ("CablingSvc",   m_cabling);
}

/// --------------------------------------------------------------------
/// Initialize

StatusCode SCTRawDataProvider::initialize() {
  /** Get ROBDataProviderSvc */
  ATH_CHECK(m_robDataProvider.retrieve());
  /** Get SCTRawDataProviderTool */
  ATH_CHECK(m_rawDataTool.retrieve());
  /** Get the SCT ID helper **/
  ATH_CHECK(detStore()->retrieve(m_sct_id,"SCT_ID"));
  /** Retrieve Cabling service */ 
  ATH_CHECK(m_cabling.retrieve());
  //Initialize 
  ATH_CHECK( m_rdoContainerKey.initialize() );
  ATH_CHECK( m_lvl1CollectionKey.initialize() );
  ATH_CHECK( m_bcidCollectionKey.initialize() );
  return StatusCode::SUCCESS;
}

/// --------------------------------------------------------------------
/// Execute

StatusCode SCTRawDataProvider::execute() {

  SG::WriteHandle<SCT_RDO_Container> rdoContainer(m_rdoContainerKey);
  rdoContainer = std::make_unique<SCT_RDO_Container>(m_sct_id->wafer_hash_max()); 
  ATH_CHECK(rdoContainer.isValid());
  
  //// do we need this??  rdoIdc->cleanup();

  /** ask ROBDataProviderSvc for the vector of ROBFragment for all SCT ROBIDs */
  std::vector<const ROBFragment*> listOfRobf;
  std::vector<boost::uint32_t> rodList;
  m_cabling->getAllRods(rodList);
  m_robDataProvider->getROBData( rodList , listOfRobf);

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Number of ROB fragments " << listOfRobf.size() << endmsg;

  SG::WriteHandle<InDetTimeCollection> lvl1Collection(m_lvl1CollectionKey);
  lvl1Collection = std::make_unique<InDetTimeCollection>(listOfRobf.size()); 
  ATH_CHECK(lvl1Collection.isValid());

  SG::WriteHandle<InDetTimeCollection> bcidCollection(m_bcidCollectionKey);
  bcidCollection = std::make_unique<InDetTimeCollection>(listOfRobf.size()); 
  ATH_CHECK(bcidCollection.isValid());

  std::vector<const ROBFragment*>::const_iterator rob_it = listOfRobf.begin();  
  for(; rob_it!=listOfRobf.end(); ++rob_it) {
    
    uint32_t robid = (*rob_it)->rod_source_id();
    /**
     * Store LVL1ID and BCID information in InDetTimeCollection 
     * to be stored in StoreGate at the end of the loop.
     * We want to store a pair<ROBID, LVL1ID> for each ROD, once per event.
     **/
    
    unsigned int lvl1id = (*rob_it)->rod_lvl1_id();
    std::pair<uint32_t, unsigned int>* lvl1Pair = new std::pair<uint32_t, unsigned int>(std::make_pair(robid,lvl1id));
    lvl1Collection->push_back(lvl1Pair) ;
    
    unsigned int bcid = (*rob_it)->rod_bc_id();  
    std::pair<uint32_t, unsigned int>* bcidPair = new std::pair<uint32_t, unsigned int>(std::make_pair(robid,bcid));
    bcidCollection->push_back(bcidPair);
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"Stored LVL1ID "<<lvl1id<<" and BCID "<<bcid<<" in InDetTimeCollections"<<endmsg;
    
  }

  /** ask SCTRawDataProviderTool to decode it and to fill the IDC */
  if (m_rawDataTool->convert(listOfRobf,&(*rdoContainer))==StatusCode::FAILURE)
    msg(MSG::ERROR) << "BS conversion into RDOs failed" << endmsg;
  
  return StatusCode::SUCCESS;
}

