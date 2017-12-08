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
  m_onlineID(0),
  m_cablingService("LArCablingService")
{
  declareInterface<ILArAutoCorrDecoderTool>(this);
  declareProperty("KeyAutoCorr",m_keyAutoCorr="LArAutoCorr");
  declareProperty("DecodeMode", m_decodemode=0);
  declareProperty("UseAlwaysHighGain", m_alwaysHighGain=false);
}


LArAutoCorrDecoderTool::~LArAutoCorrDecoderTool() 
{}

StatusCode LArAutoCorrDecoderTool::initialize() 
{
  ATH_MSG_DEBUG("LArAutoCorrDecoderTool initialize() begin");
  
  StatusCode sc = detStore()->retrieve(m_onlineID,"LArOnlineID");
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Unable to retrieve  LArOnlineID from DetectorStore" << endreq;
    return StatusCode::FAILURE;
  }

  if(m_cablingService.retrieve().isFailure()){
    msg(MSG::ERROR) << "Unable to get CablingService " << endreq;
    return StatusCode::FAILURE;
  }


  sc=detStore()->regHandle(m_autoCorr,m_keyAutoCorr);
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to register Datahandle<ILArAutoCorr> to SG key " << m_keyAutoCorr << endreq;
    return sc;
  }


  if (m_alwaysHighGain)
    msg(MSG::INFO) << "Will always return HIGH gain autocorrelation matrix for EM calo, MEDIUM for HEC and FCAL" << endreq;

  ATH_MSG_DEBUG("LArAutoCorrDecoderTool initialize() end");
  return StatusCode::SUCCESS;
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


const Eigen::MatrixXd LArAutoCorrDecoderTool::ACDiagonal( const HWIdentifier&  CellID, int gain, unsigned nSamples=5 ) const {

  if (m_alwaysHighGain) {
    if (m_onlineID->isFCALchannel(CellID) ||m_onlineID->isHECchannel(CellID))
      gain=1;
    else
      gain=0;
  }

  Eigen::MatrixXd AutoCorrMatrix=Eigen::MatrixXd::Zero(nSamples,nSamples);

  if ( m_autoCorr ) { // LArAutoCorrComplete is loaded in DetStore

    ILArAutoCorr::AutoCorrRef_t dbcorr = m_autoCorr->autoCorr(CellID,gain);

    if ( dbcorr.size()== 0 ) { // empty AutoCorr for given channel
      msg(MSG::WARNING) << "Empty AutoCorr vector for channel " <<  m_onlineID->channel_name(CellID) << " in Gain = " << gain<< endreq;
      nSamples=0;
    }
    else if (dbcorr.size() < nSamples-1 ) {
      msg(MSG::WARNING) << "Not enough samples in AutoCorr vector for channel " <<  m_onlineID->channel_name(CellID) << " in Gain = " << gain<< endreq;
      nSamples=1+dbcorr.size(); //The remaining values of the eigen matrix are left to 0.0
    } 
  
    // fill diagonal matrix with vector 
    for (unsigned i=0;i<nSamples;i++) {
      AutoCorrMatrix(i,i)= 1 ;
      for (unsigned j=i+1;j<nSamples;j++) {
	AutoCorrMatrix(i,j) = AutoCorrMatrix(j,i) = dbcorr[j-i-1];      
      }
    }
  }//else if m_autoCorr
  else { // no LArAutoCorrComplete loaded in DetStore (e.g. DB problem) :-(
    msg(MSG::WARNING) << "No valid AutoCorr object loaded from DetStore" << endreq;
  }

  ATH_MSG_DEBUG("AutoCorr matrix for channel " <<  m_onlineID->channel_name(CellID) 
		<< " in Gain = " << gain
		<< ":\n" << AutoCorrMatrix);

  return AutoCorrMatrix;
  
}

const Eigen::MatrixXd LArAutoCorrDecoderTool::ACPhysics( const HWIdentifier&  CellID, int gain, unsigned nSamples=5 ) const {


  if (m_alwaysHighGain) {
    if (m_onlineID->isFCALchannel(CellID) ||m_onlineID->isHECchannel(CellID))
      gain=1;
    else
      gain=0;
  }

  Eigen::MatrixXd AutoCorrMatrix=Eigen::MatrixXd::Identity(nSamples,nSamples);

  if ( m_autoCorr ) { // LArAutoCorrComplete is loaded in DetStore

    ILArAutoCorr::AutoCorrRef_t corrdb = m_autoCorr->autoCorr(CellID,gain);
   
    if ( corrdb.size()== 0 ) { // empty AutoCorr for given channel
      msg(MSG::WARNING) << "Empty AutoCorr vector for channel " << m_onlineID->channel_name(CellID) << " in Gain = " << gain<< endreq;
      nSamples=0; //return all-zero matrix
    }
    else  if ( corrdb.size() < nSamples*(nSamples+1)/2 ) {
      msg(MSG::WARNING) << "Not enough samples in AutoCorr vector for channel " <<  m_onlineID->channel_name(CellID) 
			<< "in Gain = " << gain << " for AC Physics mode"<< endreq;
      nSamples=0;//return all-zero matrix 
    } 

    // Corr size could be bigger, then it's asked now, need remapping:
    const unsigned int nsamples_AC = (-1+((int)(sqrt(1+8*corrdb.size()))))/2;
    unsigned int k=0;
    for (unsigned i=0;i<nSamples;i++) {
      for (unsigned j=i;j<nSamples;j++,k++) {
	if (i<=j) {
	  AutoCorrMatrix(i,j) = AutoCorrMatrix(j,i)= corrdb[k];
	}
      }
      k+=nsamples_AC-nSamples;
    }
  } //end if m_autoCorr 
  else { // no LArAutoCorrComplete loaded in DetStore (e.g. DB problem) :-(
    msg(MSG::WARNING) << "No valid AutoCorr object loaded from DetStore" << endreq;
  }
   
  ATH_MSG_DEBUG("AutoCorr matrix for channel " <<  m_onlineID->channel_name(CellID) 
		<< " in Gain = " << gain
		<< ":\n" << AutoCorrMatrix);

  return AutoCorrMatrix;
  
}
