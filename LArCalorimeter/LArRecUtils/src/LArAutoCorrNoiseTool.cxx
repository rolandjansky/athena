/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRecUtils/LArAutoCorrNoiseTool.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "LArElecCalib/LArConditionsException.h"
#include "StoreGate/StoreGateSvc.h"

#include <cmath>

/////////////////////////////////////////////////////////////////////////////

LArAutoCorrNoiseTool::LArAutoCorrNoiseTool(const std::string& type, 
					   const std::string& name, 
					   const IInterface* parent) 
  : AthAlgTool(type, name, parent),
    m_Nsampl(5),m_MCSym(true), 
    m_lar_on_id(nullptr),
    m_lar_scon_id(nullptr),
    m_cablingService("LArCablingService"),
    m_larmcsym("LArMCSymTool"),
    m_keyAutoCorr("LArAutoCorr"),
    m_cacheValid(false),m_loadAtBegin(true),
    m_isSC(false)

{
  declareInterface<ILArAutoCorrNoiseTool>(this);

  declareProperty("keyAutoCorr",m_keyAutoCorr);
  declareProperty("NSamples",m_Nsampl);
  declareProperty("MCSym",m_MCSym);
  declareProperty("LoadAtBegin",m_loadAtBegin);
  declareProperty("SuperCell",m_isSC);

}

/////////////////////////////////////////////////////////////////////////////

StatusCode LArAutoCorrNoiseTool::initialize()
{
  MsgStream log( msgSvc(), name() );
  
  log << MSG::DEBUG << "LArAutoCorrNoiseTool initialize() begin" << endreq;
  
  if (StatusCode::SUCCESS==detStore()->regFcn(&ILArAutoCorrNoiseTool::LoadCalibration,
                                              dynamic_cast<ILArAutoCorrNoiseTool*>(this),m_dd_autocorr,m_keyAutoCorr)) {
    log << MSG::INFO << "Registered callback for key: " << m_keyAutoCorr << endreq;
  } else {
    log << MSG::ERROR << "Cannot register testCallback function for key " << m_keyAutoCorr << endreq;
  }
  
  if ( m_isSC ){
    StatusCode sc = detStore()->retrieve(m_lar_scon_id,"LArOnline_SuperCellID");
    if (sc.isFailure()) {
      log  << MSG::ERROR << "Unable to retrieve  LArOnline_SuperCellID from DetectorStore" 
           << endreq;
      return StatusCode::FAILURE;
    }
  
    if(m_cablingSCService.retrieve().isFailure())
    {
      log << MSG::ERROR << "Unable to get SCCablingService " << endreq;
      return StatusCode::FAILURE;
    }


  } // m_isSC
  else {

    StatusCode sc = detStore()->retrieve(m_lar_on_id,"LArOnlineID");
    if (sc.isFailure()) {
      log  << MSG::ERROR << "Unable to retrieve  LArOnlineID from DetectorStore"
           << endreq;
      return StatusCode::FAILURE;
    }

    if(m_cablingService.retrieve().isFailure())
    {
      log << MSG::ERROR << "Unable to get CablingService " << endreq;
      return StatusCode::FAILURE;
    }
    if (m_MCSym) {
      if (m_larmcsym.retrieve().isFailure()){
        log << MSG::ERROR << "Unable to get LArMCSym Tool " << endreq;
        return StatusCode::FAILURE;
      }
    }

  }
  
  if (m_loadAtBegin) {
    log << MSG::DEBUG << "Setting callback function to load calibration at begin of run" << endreq;
    // Incident Service: 
    IIncidentSvc* incSvc;
    StatusCode sc = service("IncidentSvc", incSvc);
    if (sc.isFailure()) {
      log << MSG::ERROR << "Unable to retrieve pointer to IncidentSvc "
	  << endreq;
      return sc;
    }
    
    //start listening to "BeginRun". The incident should be fired AFTER the IOV callbacks and only once.
    const long priority=std::numeric_limits<long>::min(); //Very low priority
    incSvc->addListener(this, "BeginRun", priority ,false,true); //single-shot incident
  }
  
  log << MSG::DEBUG << "LArAutoCorrNoiseTool initialize() end" << endreq;
  
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////
StatusCode LArAutoCorrNoiseTool::finalize()
{
  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////

// *** retrieves needed data from the DB *** 
StatusCode LArAutoCorrNoiseTool::LoadCalibration(IOVSVC_CALLBACK_ARGS)
{
  
#if 0
  MsgStream log( msgSvc(), name() );
  log << MSG::DEBUG << "in LoadCalibration " << endreq;
  
  log << MSG::DEBUG << "Callback invoked for " << keys.size() << " keys" << endreq;
#endif
  
  m_cacheValid = false; 


  return StatusCode::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////

// *** compute some terms of the calculation of the autocorr function *** 
StatusCode LArAutoCorrNoiseTool::getTerms()
{
  
#ifndef NDEBUG
  MsgStream log( msgSvc(), name() );

  log << MSG::DEBUG << "in getAutoCorrNoise::getTerms" << endreq;
#endif

  // get HWIdentifier iterator
  std::vector<HWIdentifier>::const_iterator it  ; 
  std::vector<HWIdentifier>::const_iterator it_e;
  LArCablingBase* cable(0);
  unsigned int ngains(3);
  if ( m_isSC ){
  it   =m_lar_scon_id->channel_begin();
  it_e =m_lar_scon_id->channel_end();
  LArSuperCellCablingTool* tcable = &(*m_cablingSCService);
  cable = (LArCablingBase*)tcable;
  ngains=1;
  } else {
  it   =m_lar_on_id->channel_begin();
  it_e =m_lar_on_id->channel_end();
  LArCablingService* tcable = &(*m_cablingService);
  cable = (LArCablingService*)tcable;
  }
  
  // resize vector to #(gain) = 3
  m_terms.resize(ngains);
  
  int count = 0;
  int count2 = 0;
  // loop over em Identifiers
#ifndef NDEBUG
  log << MSG::DEBUG << "start loop over cells in getAutoCorrNoise" << endreq;
#endif
  for(;it!=it_e;++it)
  {    
    count ++;
    const HWIdentifier id = *it;
    unsigned int id32     = id.get_identifier32().get_compact();

    if(cable->isOnlineConnected(id)) 
    {    
      if(!m_isSC && m_MCSym) {
        HWIdentifier id2 = m_larmcsym->symOnline(id);
        if (id2 != id) continue;
      }
      count2++;
      // the Shape is a function of gain
      for(unsigned int igain=0;igain<ngains;igain++)
      {    
	ILArAutoCorr::AutoCorrRef_t AC =  m_dd_autocorr->autoCorr(id,igain);

	std::vector<float> vTerms;
        int size = m_Nsampl*m_Nsampl;
        vTerms.resize(size);

// compute sqrt to covariance matrix
        for (int j=0;j<m_Nsampl;j++) {
// a) diagonal terms
            double ck=0.;
            for (int k=0;k<=j-1;k++) {
                int index=j*m_Nsampl+k;
                ck=ck+vTerms[index]*vTerms[index];
            }
//     diagonal term of autocorr matrix = 1
            vTerms[j*m_Nsampl+j]=sqrt(std::fabs(1.-ck));
// b) off-diagonal terms
            for (int i=j+1;i<m_Nsampl;i++) {
                double ck=0;
                for(int k=0;k<=j-1;k++) {
                 int index1=i*m_Nsampl+k; 
                 int index2=j*m_Nsampl+k; 
                 ck=ck+vTerms[index1]*vTerms[index2];
                }
                int index=i*m_Nsampl+j;
                int index2=j*m_Nsampl+j;
                unsigned int index3=(i-j)-1;
                double AutoCorr=0.;
                if (index3<(AC.size()))  AutoCorr=AC[index3];
                vTerms[index]=(AutoCorr-ck)/vTerms[index2];
            }
        }

	m_terms[igain][id32] = vTerms;

      }//(loop on gains)

    } 
    else//unconnected
      for(unsigned int igain=0;igain<ngains;igain++) 
      {
	m_terms[igain][id32] = std::vector<float>(1, 0.);
      }

  }
#ifndef NDEBUG
  log << MSG::INFO  << "LArAutoCorrNoise Ncell " << count << endreq;
  log << MSG::INFO  << "LArAutoCorrNoise Nsymcell " << count2 << endreq;
  log << MSG::DEBUG << "end of loop over cells " << endreq;
#endif

  m_cacheValid = true;
  return StatusCode::SUCCESS;
}
 
/////////////////////////////////////////////////////////////////////////////

const std::vector<float> &
LArAutoCorrNoiseTool::autoCorrSqrt(const HWIdentifier& CellID,
				    int gain, int Nsampl) 
{

  if(!m_cacheValid){
    StatusCode  sc = this->getTerms();
    if (sc.isFailure()) 
      {
	MsgStream log( msgSvc(), name() );
	log << MSG::ERROR
	    << "getTerms failed "
	    << endreq;
	throw LArConditionsException("Could not compute in LArAutoCorrNoiseTool::autoCorrSqrt");
      }
  }
// recompute everything if number of samples is different from initial ones
  if (Nsampl>0 && Nsampl != m_Nsampl) {
    m_Nsampl = Nsampl;
    if ( (this->getTerms()).isFailure() ){
	MsgStream log( msgSvc(), name() ); 
	log << MSG::ERROR << "getTerms failed" << endreq; 
    }
  }

  HWIdentifier id;
  if (!m_isSC && m_MCSym) 
    id = m_larmcsym->symOnline(CellID);
  else 
    id = CellID;
  unsigned int id32 = id.get_identifier32().get_compact();
  
  MAP::const_iterator it = (m_terms[gain]).find(id32) ; 
  if(it == (m_terms[gain]).end()){
     MsgStream log( msgSvc(), name() );
    log << MSG::ERROR
	<< "Unable to find ID = " << CellID << " in m_terms" 
	<< endreq;
    static std::vector<float> empty; 
    return empty; 
  }
  
  return ((*it).second);  
}

/////////////////////////////////////////////////////////////////////////////

const std::vector<float> &
LArAutoCorrNoiseTool::autoCorrSqrt(const Identifier& CellID,
                                   int gain, int /*Nsampl*/) 
{

  if(!m_cacheValid){
    StatusCode  sc = this->getTerms();
    if (sc.isFailure()) 
      {
	MsgStream log( msgSvc(), name() );
	log << MSG::ERROR
	    << "getTerms failed "
	    << endreq;
	throw LArConditionsException("Could not compute in LArAutoCorrNoiseTool::autoCorrSqrt");
      }
  }
  HWIdentifier id;
  if (!m_isSC && m_MCSym)
    id = m_larmcsym->symOnline(CellID);
  else 
    id = m_cablingService->createSignalChannelID(CellID);
  unsigned int id32 = id.get_identifier32().get_compact();

  MAP::const_iterator it = (m_terms[gain]).find(id32) ; 
  if(it == (m_terms[gain]).end()){
     MsgStream log( msgSvc(), name() );
    log << MSG::ERROR
        << "Unable to find ID = " << CellID << " in m_terms" 
        << endreq; 
    static std::vector<float> empty;  
    return empty;  
  }
  
  return ((*it).second); 

}


void LArAutoCorrNoiseTool::handle(const Incident&) {
     MsgStream log( msgSvc(), name() );
     log << MSG::DEBUG << "In Incident-handle" << endreq;

     if(!m_cacheValid){
       StatusCode  sc = this->getTerms();
       if (sc.isFailure()) 
	 {
	   MsgStream log( msgSvc(), name() );
	   log << MSG::ERROR
	       << "getTerms failed "
	       << endreq;
	   throw LArConditionsException("Could not getTerms in LArAutoCorrNoiseTool::handle");
	 }
     }
     
}
