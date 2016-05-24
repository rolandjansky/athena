/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

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
  m_LVL1Collection(nullptr),
  m_BCIDCollection(nullptr)
{
  declareProperty ("RDOKey"      , m_RDO_Key = "SCT_RDOs");
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
  return StatusCode::SUCCESS;
}

/// --------------------------------------------------------------------
/// Execute

StatusCode SCTRawDataProvider::execute() {
  SCT_RDO_Container *container = new SCT_RDO_Container(m_sct_id->wafer_hash_max()); 
  if (evtStore()->record(container, m_RDO_Key).isFailure()) {
    msg(MSG::FATAL) << "Unable to record SCT RDO Container." << endreq;
    return StatusCode::FAILURE;
  }
  
  //// do we need this??  rdoIdc->cleanup();

  /** ask ROBDataProviderSvc for the vector of ROBFragment for all SCT ROBIDs */
  std::vector<const ROBFragment*> listOfRobf;
  std::vector<boost::uint32_t> rodList;
  m_cabling->getAllRods(rodList);
  m_robDataProvider->getROBData( rodList , listOfRobf);

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Number of ROB fragments " << listOfRobf.size() << endreq;

  m_LVL1Collection = new InDetTimeCollection();
  m_LVL1Collection->reserve(listOfRobf.size());
  m_BCIDCollection = new InDetTimeCollection();
  m_BCIDCollection->reserve(listOfRobf.size());  

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
    m_LVL1Collection->push_back(lvl1Pair) ;
    
    unsigned int bcid = (*rob_it)->rod_bc_id();  
    std::pair<uint32_t, unsigned int>* bcidPair = new std::pair<uint32_t, unsigned int>(std::make_pair(robid,bcid));
    m_BCIDCollection->push_back(bcidPair);
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) <<"Stored LVL1ID "<<lvl1id<<" and BCID "<<bcid<<" in InDetTimeCollections"<<endreq;
    
  }

  
  StatusCode sc = evtStore()->record(m_LVL1Collection,"SCT_LVL1ID");  
  if (sc.isFailure() ) {   
    msg(MSG::ERROR) << "failed to record LVL1ID TimeCollection" << endreq;   
    return sc;   
  }
  sc = evtStore()->record(m_BCIDCollection,"SCT_BCID");  
  if (sc.isFailure() ) {   
    msg(MSG::ERROR) << "failed to record BCID TimeCollection" << endreq;   
    return sc;   
  }
  

  /** ask SCTRawDataProviderTool to decode it and to fill the IDC */
  if (m_rawDataTool->convert(listOfRobf,container)==StatusCode::FAILURE)
    msg(MSG::ERROR) << "BS conversion into RDOs failed" << endreq;
  
  return StatusCode::SUCCESS;
}

