/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// TGC_RawDataProviderTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "TGC_RawDataProviderTool.h"

#include "MuonTGC_CnvTools/ITGC_RodDecoder.h"
#include "MuonRDO/TgcRdoContainer.h"

#include "MuonReadoutGeometry/MuonDetectorManager.h"
#include "MuonContainerManager/MuonRdoContainerAccess.h"

#include "TGCcablingInterface/ITGCcablingServerSvc.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

#include "GaudiKernel/ServiceHandle.h"
#include "GaudiKernel/IJobOptionsSvc.h"

//================ Constructor =================================================

Muon::TGC_RawDataProviderTool::TGC_RawDataProviderTool(
						       const std::string& t,
						       const std::string& n,
						       const IInterface*  p) :
  AthAlgTool(t, n, p),
  m_muonMgr(0),
  m_decoder("Muon__TGC_RodDecoderReadout"),
  //m_rdoContainer(0),
  m_rdoContainerKey("TGCRDO"), 
  m_activeStore(0),
  m_cabling(0),
  m_robDataProvider("ROBDataProviderSvc",n) 
{
  declareInterface<IMuonRawDataProviderTool>(this);
  declareProperty("Decoder",     m_decoder);
  declareProperty("RdoLocation", m_rdoContainerKey);
}

//================ Destructor =================================================

Muon::TGC_RawDataProviderTool::~TGC_RawDataProviderTool()
{}

//================ Initialisation =================================================

StatusCode Muon::TGC_RawDataProviderTool::initialize()
{
  StatusCode sc = AthAlgTool::initialize();

  if(sc.isFailure()) return sc;

  sc = service("ActiveStoreSvc", m_activeStore);
  if(!sc.isSuccess()) {
    msg(sc.isFailure() ? MSG::FATAL : MSG::ERROR) << "Could not get active store service" << endmsg;
    return sc;
  }

  if(detStore()->retrieve(m_muonMgr).isFailure()) {
    ATH_MSG_WARNING( "Cannot retrieve MuonDetectorManager" );
    return StatusCode::SUCCESS;
  }

  if(m_decoder.retrieve().isFailure()) {
    ATH_MSG_FATAL( "Failed to retrieve tool " << m_decoder );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO( "Retrieved tool " << m_decoder );
  }

  // Get ROBDataProviderSvc
  if(m_robDataProvider.retrieve().isFailure()) {
    ATH_MSG_FATAL( "Failed to retrieve serive " << m_robDataProvider );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_INFO( "Retrieved service " << m_robDataProvider );
  }

  // Check if EventSelector has the ByteStreamCnvSvc
  bool has_bytestream = false;
  IJobOptionsSvc* jobOptionsSvc;
  sc = service("JobOptionsSvc", jobOptionsSvc, false);
  if(sc.isFailure()) {
    ATH_MSG_DEBUG( "Could not find JobOptionsSvc" );
    jobOptionsSvc = 0;
  } else {
    IService* svc = dynamic_cast<IService*>(jobOptionsSvc);
    if(svc != 0) {
      ATH_MSG_INFO( " Tool = " << name() 
		    << " is connected to JobOptionsSvc Service = "
		    << svc->name() );
    } else {
      return StatusCode::FAILURE;
    }
  }
  
  IJobOptionsSvc* TrigConfSvc;
  sc = service("TrigConf::HLTJobOptionsSvc", TrigConfSvc, false);
  if(sc.isFailure()) {
    ATH_MSG_DEBUG( "Could not find TrigConf::HLTJobOptionsSvc" );
    TrigConfSvc = 0;
  } else {
    IService* svc = dynamic_cast<IService*>(TrigConfSvc);
    if(svc != 0) {
      ATH_MSG_INFO( " Tool = " << name() 
		    << " is connected to JobOptionsSvc Service = "
		    << svc->name() );
    } else {
      return StatusCode::FAILURE;
    }
  }
    
  if(jobOptionsSvc==0 && TrigConfSvc==0) {
    ATH_MSG_FATAL( "Bad job configuration" );
    return StatusCode::FAILURE;  
  }
    
  const std::vector<const Property*>* byteStreamNavProps
    = (jobOptionsSvc)?  jobOptionsSvc->getProperties("ByteStreamNavigationProviderSvc") : 0;
 
  
  const std::vector<const Property*>* dataFlowProps 
    = (jobOptionsSvc)?  jobOptionsSvc->getProperties("DataFlowConfig") : 0;

  const std::vector<const Property*>* eventSelProps 
    = (jobOptionsSvc)? jobOptionsSvc->getProperties("EventSelector") :
    TrigConfSvc->getProperties("EventSelector");	
    
    
  if(dataFlowProps != 0) has_bytestream = true;

  if(byteStreamNavProps != 0) {
    has_bytestream = true;
  } else if(eventSelProps != 0) {
    std::vector<const Property*>::const_iterator cur   = eventSelProps->begin();
    std::vector<const Property*>::const_iterator cur_e = eventSelProps->end();
    for(; cur!=cur_e; cur++) {
      if((*cur)->name()=="ByteStreamInputSvc") has_bytestream = true;
    }
  } else {
    has_bytestream = true;
  }
    
  // register the container only when the imput from ByteStream is set up     
  m_activeStore->setStore( &*evtStore() );
  if(has_bytestream || m_rdoContainerKey != "TGCRDO") {   
    TgcRdoContainer* m_container = Muon::MuonRdoContainerAccess::retrieveTgcRdo(m_rdoContainerKey);

    // create and register the container only once
    if(m_container==0) {
      try {
	m_container = 
	  new TgcRdoContainer(m_muonMgr->tgcIdHelper()->module_hash_max());
      } catch(std::bad_alloc) {
	ATH_MSG_FATAL( "Could not create a new TGC RDO container!" );
	return StatusCode::FAILURE;
      }
	
      // record the container for being used by the convert method

      sc = Muon::MuonRdoContainerAccess::record(m_container,
						m_rdoContainerKey,
						serviceLocator(),
						msg(),
						&*evtStore());
      if(sc.isFailure()) {
	ATH_MSG_FATAL( "Recording of container " 
		       << m_rdoContainerKey
		       << " into MuonRdoContainerManager has failed" );
	return StatusCode::FAILURE;
      }
    }
  } else {
    ATH_MSG_DEBUG( "ByteStream conversion service not found." );
    ATH_MSG_DEBUG( "TGC RDO container not registered." );
  }
  
  
  //try to configure the cabling service
  sc = getCabling();
  if(sc.isFailure()) {
      ATH_MSG_INFO( "TGCcablingServerSvc not yet configured; postone TGCcabling initialization at first event. " );
  }
  
  ATH_MSG_INFO( "initialize() successful in " << name() );
  return StatusCode::SUCCESS;
}

//================ Finalisation =================================================

StatusCode Muon::TGC_RawDataProviderTool::finalize()
{
  StatusCode sc = AthAlgTool::finalize();
  return sc;
}

//============================================================================================

StatusCode Muon::TGC_RawDataProviderTool::convert(const ROBFragmentList& vecRobs) 
{    
  // retrieve the container through the MuonRdoContainerManager becasue
  // if the MuonByteStream CNV has to be used, the container must have been
  // registered there!
  m_activeStore->setStore( &*evtStore() );
  TgcRdoContainer* TGC = Muon::MuonRdoContainerAccess::retrieveTgcRdo(m_rdoContainerKey);
      
  if(TGC==0) {
    ATH_MSG_DEBUG( "Container " << m_rdoContainerKey 
		   << " for bytestream conversion not available." );
    ATH_MSG_DEBUG( "Try retrieving it from the Store" );
    
    return StatusCode::SUCCESS; // Maybe it should be false to stop the job
    // because the convert method should not
    // have been called .... but this depends
    // on the user experience
  }
  
  static int DecodeErrCount = 0;

  ROBFragmentList::const_iterator itFrag   = vecRobs.begin();
  ROBFragmentList::const_iterator itFrag_e = vecRobs.end();
  for(; itFrag!=itFrag_e; itFrag++) {
    if(m_decoder->fillCollection(**itFrag, *TGC).isFailure()) {
      if(DecodeErrCount < 100) {
	ATH_MSG_INFO( "Problem with TGC ByteStream Decoding!" );
	DecodeErrCount++;
      } else if(100 == DecodeErrCount) {
	ATH_MSG_INFO( "Too many Problems with TGC Decoding messages. Turning message off." );
	DecodeErrCount++;
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode  Muon::TGC_RawDataProviderTool::convert(const ROBFragmentList& vecRobs,
						   const std::vector<IdentifierHash>&) 
{
  return convert(vecRobs);
}

StatusCode  Muon::TGC_RawDataProviderTool::convert()
{
  if(!m_cabling) {
    StatusCode sc = getCabling();
    if(sc.isFailure()) return sc;
  }

  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecOfRobf;
  const std::vector<uint32_t>& robIds = m_hid2re.allRobIds();

  m_robDataProvider->getROBData(robIds, vecOfRobf);

  return convert(vecOfRobf); 
}

StatusCode  Muon::TGC_RawDataProviderTool::convert(const std::vector<IdentifierHash>& rdoIdhVect)
{
  if(!m_cabling) {
    StatusCode sc = getCabling();
    if(sc.isFailure()) return sc;
  }

  const TgcIdHelper* idHelper = m_muonMgr->tgcIdHelper();
  IdContext tgcContext = idHelper->module_context();

  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vecOfRobf;
  std::vector<uint32_t> robIds;

  unsigned int size = rdoIdhVect.size();
  for(unsigned int i=0; i<size; ++i) {
    Identifier Id;
    if(idHelper->get_id(rdoIdhVect[i], Id, &tgcContext)) {
      ATH_MSG_WARNING( "Unable to get TGC Identifier from collection hash id " );
      continue;
    }
    const Identifier tgcId = Id;
    uint32_t rodId = m_hid2re.getRodID(tgcId);
    uint32_t robId = m_hid2re.getRobID(rodId);
    std::vector<uint32_t>::iterator it_robId = std::find(robIds.begin(), robIds.end(), robId); 
    if(it_robId==robIds.end()) {
      robIds.push_back(robId);
    }
  }
  m_robDataProvider->getROBData(robIds, vecOfRobf);
  ATH_MSG_VERBOSE( "Number of ROB fragments " << vecOfRobf.size() );

  return convert(vecOfRobf, rdoIdhVect);
}

StatusCode  Muon::TGC_RawDataProviderTool::getCabling() {
  const ITGCcablingServerSvc* TgcCabGet = 0;
  StatusCode sc = service("TGCcablingServerSvc", TgcCabGet, true);
  if(sc.isFailure()) {
    ATH_MSG_FATAL( "Could not get TGCcablingServerSvc !" );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_VERBOSE( " TGCcablingServerSvc retrieved" );
  } 

  if(!TgcCabGet->isConfigured()) {
      ATH_MSG_DEBUG( "TGCcablingServer not yet configured!" );
      return StatusCode::FAILURE;
  }

  sc = TgcCabGet->giveCabling(m_cabling);
  if(sc.isFailure()) {
    ATH_MSG_FATAL( "Could not get ITGCcablingSvc from the Server !" );
    m_cabling = 0;
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_VERBOSE( "ITGCcablingSvc obtained" );
  }    
  
  m_hid2re.set(m_cabling); 
  
  return StatusCode::SUCCESS;
}
