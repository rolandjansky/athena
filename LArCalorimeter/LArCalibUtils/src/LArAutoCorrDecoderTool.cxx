/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArAutoCorrDecoderTool.h"

#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "AthenaKernel/errorcheck.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "StoreGate/StoreGateSvc.h"

#include "LArElecCalib/LArConditionsException.h"


LArAutoCorrDecoderTool::LArAutoCorrDecoderTool(const std::string& type, 
					       const std::string& name, 
					       const IInterface* parent) 
  : 
  AthAlgTool(type, name, parent),
  m_cablingService("LArCablingService"),
  m_cacheValid(false)
{
  declareInterface<ILArAutoCorrDecoderTool>(this);
  declareProperty("KeyAutoCorr",m_keyAutoCorr="LArAutoCorr");
  declareProperty("LoadAtBegin",m_loadAtBegin=true);
  declareProperty("DecodeMode", m_decodemode=0);
}


LArAutoCorrDecoderTool::~LArAutoCorrDecoderTool() 
{}

StatusCode LArAutoCorrDecoderTool::initialize() 
{
  ATH_MSG_DEBUG("LArAutoCorrDecoderTool initialize() begin");
  
  StatusCode sc = detStore()->retrieve(m_lar_on_id,"LArOnlineID");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to retrieve  LArOnlineID from DetectorStore" << endreq;
    return StatusCode::FAILURE;
  }

  if(m_cablingService.retrieve().isFailure()){
    msg(MSG::ERROR) << "Unable to get CablingService " << endreq;
    return StatusCode::FAILURE;
  }


  // CallBack function for AutoCorr
  sc = detStore()->regFcn(&ILArAutoCorrDecoderTool::LoadAutoCorr,dynamic_cast<ILArAutoCorrDecoderTool*>(this),
                        m_AutoCorr,m_keyAutoCorr);
  if(sc.isSuccess()){
    msg(MSG::INFO) << "Registered callback for key: " << m_keyAutoCorr << endreq;
  } else {
    msg(MSG::ERROR) << "Cannot register testCallback function for key " << m_keyAutoCorr << endreq;
  }


  if (m_loadAtBegin) {
    ATH_MSG_DEBUG("Setting callback function to load AutoCorr at begin of run");
    // Incident Service:
    IIncidentSvc* incSvc;
    sc = service("IncidentSvc", incSvc);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Unable to retrieve pointer to IncidentSvc" << endreq;
      return sc;
    }

    //start listening to "BeginRun". The incident should be fired AFTER the IOV callbacks and only once.
    const long priority=std::numeric_limits<long>::min(); //Very low priority
    incSvc->addListener(this, "BeginRun", priority ,false,true); //single-shot incident
  }

  ATH_MSG_DEBUG("LArAutoCorrDecoderTool initialize() end");
  return StatusCode::SUCCESS;
}


StatusCode LArAutoCorrDecoderTool::LoadAutoCorr(IOVSVC_CALLBACK_ARGS) 
{
  msg(MSG::DEBUG) << "In LoadAutoCorr" << endreq;
  m_cacheValid=false;
  return StatusCode::SUCCESS;
}

void LArAutoCorrDecoderTool::handle(const Incident&) 
{
  ATH_MSG_DEBUG("In Incident-handle");
  if (m_cacheValid) {
  ATH_MSG_DEBUG("Cached data already computed.");
    return;
  }

  //Retrieve AutoCorr from StoreGate
  StatusCode sc = detStore()->retrieve(m_AutoCorr,m_keyAutoCorr);

  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Cannot load LArAutoCorr object with key " << m_keyAutoCorr << endreq;
    m_cacheValid=false;
  } else {
    m_cacheValid=true;
  }

}

const Eigen::MatrixXd LArAutoCorrDecoderTool::AutoCorr( const HWIdentifier&  CellID, int gain, unsigned nSamples=5 ) const
{
  if (m_decodemode==1)
    return ACPhysics(CellID,gain,nSamples);
  else
    return ACDiagonal(CellID,gain,nSamples);
}

const Eigen::MatrixXd LArAutoCorrDecoderTool::AutoCorr( const Identifier&  CellID, int gain, unsigned nSamples=5 ) const
{
  const HWIdentifier OnId = m_cablingService->createSignalChannelID(CellID);
  return AutoCorr(OnId,gain,nSamples);
}


const Eigen::MatrixXd LArAutoCorrDecoderTool::ACDiagonal( const HWIdentifier&  CellID, int gain, unsigned nSamples=5 ) const
{

  std::vector<float> Corr;

  if ( m_AutoCorr ) { // LArAutoCorrComplete is loaded in DetStore

    ILArAutoCorr::AutoCorrRef_t dbcorr = m_AutoCorr->autoCorr(CellID,gain);
    Corr.assign(dbcorr.begin(),dbcorr.end());

    if ( Corr.size()== 0 ) { // empty AutoCorr for given channel
      msg(MSG::WARNING) << "Empty AutoCorr vector for channel 0x" << std::hex << CellID.get_compact() << "in Gain = " << gain<< endreq;
      Corr.resize(nSamples-1,0.);
    }

    if ( Corr.size() != 0 && Corr.size() < nSamples-1 ) {
      msg(MSG::WARNING) << "Not enough samples in AutoCorr vector for channel 0x" << std::hex << CellID.get_compact() << "in Gain = " << gain<< endreq;
      Corr.resize(nSamples-1,0.);       
    } 
   
  } else { // no LArAutoCorrComplete loaded in DetStore (e.g. DB problem) :-(

    Corr.resize(nSamples-1,0.);

  }

  // fill diagonal matrix with vector
  Eigen::MatrixXd AutoCorrMatrix(nSamples,nSamples);

  for (unsigned i=0;i<nSamples;i++) {
    AutoCorrMatrix(i,i)= 1 ;
    for (unsigned j=i+1;j<nSamples;j++) {
      AutoCorrMatrix(i,j) = AutoCorrMatrix(j,i) = Corr[j-i-1];      
    }
  }

  ATH_MSG_DEBUG("AutoCorr matrix for channel 0x" << std::hex << CellID.get_compact() << std::dec
		<< " in Gain = " << gain
		//	   << " is: 1 " << Corr << endreq ;
		<< ":\n" << AutoCorrMatrix);

  return AutoCorrMatrix;
  
}

const Eigen::MatrixXd LArAutoCorrDecoderTool::ACPhysics( const HWIdentifier&  CellID, int gain, unsigned nSamples=5 ) const
{
  std::vector<float> Corr;

  if ( m_AutoCorr ) { // LArAutoCorrComplete is loaded in DetStore

    ILArAutoCorr::AutoCorrRef_t corrdb = m_AutoCorr->autoCorr(CellID,gain);
    Corr.assign(corrdb.begin(),corrdb.end());

    if ( Corr.size()== 0 ) { // empty AutoCorr for given channel
      msg(MSG::WARNING) << "Empty AutoCorr vector for channel 0x" << std::hex << CellID.get_compact() << "in Gain = " << gain<< endreq;
      return ACDiagonal(CellID,gain,nSamples);
    }

    if ( Corr.size() != 0 && Corr.size() < nSamples*(nSamples+1)/2 ) {
      msg(MSG::WARNING) << "Not enough samples in AutoCorr vector for channel 0x" << std::hex << CellID.get_compact() << "in Gain = " << gain << " for AC Physiocs mode"<< endreq;
      return ACDiagonal(CellID,gain,nSamples);
    } 
   
  } else { // no LArAutoCorrComplete loaded in DetStore (e.g. DB problem) :-(

    return ACDiagonal(CellID,gain,nSamples);

  }

  // fill diagonal matrix with vector
  Eigen::MatrixXd AutoCorrMatrix (nSamples,nSamples);  
  
  unsigned int k=0;
  for (unsigned i=0;i<nSamples;i++) {  
    for (unsigned j=i;j<nSamples;j++,k++) {
      if (i<=j) {
	AutoCorrMatrix(i,j) = AutoCorrMatrix(j,i)= Corr[k];
      }
    }
  }

  ATH_MSG_DEBUG("AutoCorr matrix for channel 0x" << std::hex << CellID.get_compact() << std::dec
		<< " in Gain = " << gain
		//	   << " is: 1 " << Corr << endreq ;
		<< ":\n" << AutoCorrMatrix);

  return AutoCorrMatrix;
  
}
