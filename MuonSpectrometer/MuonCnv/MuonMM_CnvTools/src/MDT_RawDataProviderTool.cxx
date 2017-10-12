/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "MDT_RawDataProviderTool.h"
#include "MuonRDO/MdtCsmContainer.h"
#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "MuonMDT_Cabling/MuonMDT_CablingSvc.h"


// using namespace OFFLINE_FRAGMENTS_NAMESPACE;
Muon::MDT_RawDataProviderTool::MDT_RawDataProviderTool(const std::string& t,
						       const std::string& n,
						       const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  //m_rdoContainer(0),
  //m_lastLvl1ID(0),
  m_decoder("MdtROD_Decoder"),
  m_rdoContainerKey("MDTCSM"),
  m_muonMgr(0),
  m_mdtCabling(0),
  m_robDataProvider ("ROBDataProviderSvc",n)
{
  declareInterface<Muon::IMuonRawDataProviderTool>(this);
  
  //  template for property declaration
  declareProperty ("Decoder", m_decoder);
  declareProperty ("RdoLocation", m_rdoContainerKey);  
}


Muon::MDT_RawDataProviderTool::~MDT_RawDataProviderTool()
{}

StatusCode Muon::MDT_RawDataProviderTool::initialize()
{    
    ATH_MSG_VERBOSE("Starting init");
  StatusCode sc = service("ActiveStoreSvc", m_activeStore);
  if ( !sc.isSuccess() ) {
    ATH_MSG_FATAL("Could not get active store service");
    return sc;
  }

  ATH_MSG_VERBOSE("Getting cabling");  
  
  // get MDT cablingSvc
  StatusCode status = service("MuonMDT_CablingSvc", m_mdtCabling);
  
  if (status.isFailure()) {
    ATH_MSG_FATAL("Could not get MuonMDT_CablingSvc !");
    m_mdtCabling = 0;
    return StatusCode::FAILURE;
  }
  else {
    ATH_MSG_DEBUG(" Found the MuonMDT_CablingSvc. ");
  }
  
  ATH_MSG_VERBOSE("Getting m_robDataProvider");  
  
  // Get ROBDataProviderSvc
  if (m_robDataProvider.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve serive " << m_robDataProvider);
    return StatusCode::FAILURE;
  } else
    ATH_MSG_INFO("Retrieved service " << m_robDataProvider);
  
  ATH_MSG_VERBOSE("Getting MuonDetectorManager");  
  
  if (detStore()->retrieve(m_muonMgr).isFailure())
    {
      ATH_MSG_ERROR("Cannot retrieve MuonDetectorManager");
      return sc;
    }
  
    ATH_MSG_VERBOSE("Getting m_decoder");  
  
  // Retrieve decoder
  if (m_decoder.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool " << m_decoder);
    return StatusCode::FAILURE;
  } else 
    ATH_MSG_INFO("Retrieved tool " << m_decoder);
  
  
  // Check if EventSelector has the ByteStreamCnvSvc
  bool has_bytestream = false;
  IJobOptionsSvc* jobOptionsSvc;
  sc = service("JobOptionsSvc", jobOptionsSvc, false);
  if (sc.isFailure()) {
    ATH_MSG_DEBUG("Could not find JobOptionsSvc");
    jobOptionsSvc = 0;
  } else {
    IService* svc = dynamic_cast<IService*>(jobOptionsSvc);
    if(svc != 0 ) {
      ATH_MSG_INFO(" Tool = " << name() 
		   << " is connected to JobOptionsSvc Service = "
		   << svc->name());
    } else return StatusCode::FAILURE;
  }
  
  IJobOptionsSvc* TrigConfSvc;
  sc = service("TrigConf::HLTJobOptionsSvc", TrigConfSvc, false);
  if (sc.isFailure()) {
    ATH_MSG_DEBUG("Could not find TrigConf::HLTJobOptionsSvc");
    TrigConfSvc = 0;
  } else {
    IService* svc = dynamic_cast<IService*>(TrigConfSvc);
    if(svc != 0 ) {
      ATH_MSG_INFO(" Tool = " << name() 
		   << " is connected to JobOptionsSvc Service = "
		   << svc->name());
    } else return StatusCode::FAILURE;
  }
  
  if(jobOptionsSvc==0 && TrigConfSvc==0)
    {
      ATH_MSG_FATAL("Bad job configuration");
      return StatusCode::FAILURE;  
    }
  const std::vector<const Property*>* byteStreamNavProps
    = (jobOptionsSvc)?  jobOptionsSvc->getProperties("ByteStreamNavigationProviderSvc") : 0;
  
  
  const std::vector<const Property*>* dataFlowProps 
    = (jobOptionsSvc)?  jobOptionsSvc->getProperties("DataFlowConfig") : 0;
  
  const std::vector<const Property*>* eventSelProps 
    = (jobOptionsSvc)? jobOptionsSvc->getProperties("EventSelector") :
    TrigConfSvc->getProperties("EventSelector");	
  
  if     ( dataFlowProps != 0 ) has_bytestream = true;
  if( byteStreamNavProps != 0 ) has_bytestream = true;
  else if( eventSelProps != 0 )
    {
      for (std::vector<const Property*>::const_iterator 
	     cur  = eventSelProps->begin();
	   cur != eventSelProps->end(); cur++) {
	
        if( (*cur)->name() == "ByteStreamInputSvc" ) has_bytestream = true;
      }
    } 
  else has_bytestream = true;
  //{
  //    m_log << MSG::FATAL << "Cannot get the job configuration" << endmsg;
  //    return StatusCode::FAILURE;
  //}
  
  
  // register the container only when the imput from ByteStream is set up     
  m_activeStore->setStore( &*evtStore() ); 

  m_useContainer = (has_bytestream || m_rdoContainerKey.key() != "MDTCSM") && !m_rdoContainerKey.key().empty();

  if(!m_useContainer)
  {
      ATH_MSG_DEBUG("ByteStream conversion service not found.");
      ATH_MSG_DEBUG("MDT CSM container not registered.");
  }
  else{
      m_maxhashtoUse = m_muonMgr->mdtIdHelper()->stationNameIndex("BME") != -1 ?
             m_muonMgr->mdtIdHelper()->detectorElement_hash_max() : m_muonMgr->mdtIdHelper()->module_hash_max();
  }

  ATH_CHECK( m_rdoContainerKey.initialize() );
  
  ATH_MSG_INFO("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

StatusCode Muon::MDT_RawDataProviderTool::finalize()
{
  return StatusCode::SUCCESS;
}

// the new one 
StatusCode Muon::MDT_RawDataProviderTool::convert() //call decoding function using list of all detector ROBId's
{
  return convert(m_mdtCabling->getAllROBId());
 }

StatusCode Muon::MDT_RawDataProviderTool::convert(const std::vector<IdentifierHash>& HashVec)
{
 return convert(m_mdtCabling->getROBId(HashVec));
}

StatusCode Muon::MDT_RawDataProviderTool::convert(const std::vector<uint32_t>& robIds)
{
  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecOfRobf;
  m_robDataProvider->getROBData( robIds, vecOfRobf);
   return convert(vecOfRobf); // using the old one
 }
StatusCode Muon::MDT_RawDataProviderTool::convert( const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs,
 const std::vector<IdentifierHash>&)
{
  return convert(vecRobs);
}

StatusCode Muon::MDT_RawDataProviderTool::convert( const std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>& vecRobs)
{
  ATH_MSG_VERBOSE("convert(): " << vecRobs.size()<<" ROBFragments.");
  
  // retrieve the container through the MuonRdoContainerManager becasue
  // if the MuonByteStream CNV has to be used, the container must have been
  // registered there!
  m_activeStore->setStore( &*evtStore() ); 
  
  if(m_useContainer==false)
    {
      ATH_MSG_DEBUG("Container " << m_rdoContainerKey.key() 
		    << " for bytestream conversion not available.");
      ATH_MSG_DEBUG("Try retrieving it from the Store");
      
      return StatusCode::SUCCESS; // Maybe it should be false to stop the job
      // because the convert method should not
      // have been called .... but this depends
      // on the user experience
    }

  SG::WriteHandle<MdtCsmContainer> handle(m_rdoContainerKey);
  if (handle.isPresent())
    return StatusCode::SUCCESS;
  auto csm = std::make_unique<MdtCsmContainer>(m_maxhashtoUse);

  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*>::const_iterator itFrag;
  
  for (itFrag = vecRobs.begin(); itFrag != vecRobs.end(); itFrag++)
    {
      //convert only if data payload is delivered
      if ( (**itFrag).rod_ndata()!=0 )
	{
	  //std::vector<IdentifierHash> coll =
	  //                          to_be_converted(**itFrag,collections);
	  
	  if (m_decoder->fillCollections(**itFrag, *csm).isFailure())
            {
	      // store the error conditions into the StatusCode and continue
            }
	}
      else
	{
	  if(msgLvl(MSG::DEBUG))
	    {
	      uint32_t sourceId= (**itFrag).source_id();
	      msg(MSG::DEBUG) << " ROB " << MSG::hex << sourceId
			    << " is delivered with an empty payload" << MSG::dec;
	    }
	  // store the error condition into the StatusCode and continue
	}
    }
  //in presence of errors return FAILURE
  ATH_MSG_DEBUG("After processing numColls="<<csm->numberOfCollections());
  ATH_CHECK( handle.record (std::move (csm)) );
  return StatusCode::SUCCESS;
}

