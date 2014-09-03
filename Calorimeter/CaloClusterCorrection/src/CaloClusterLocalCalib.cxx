/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CaloClusterCorrection/CaloClusterLocalCalib.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ListItem.h"
#include "StoreGate/StoreGateSvc.h"


/// Standard AlgTool constructor
using xAOD::CaloCluster;
CaloClusterLocalCalib::CaloClusterLocalCalib(const std::string& type,
					     const std::string& name,
					     const IInterface* parent)
  :CaloClusterProcessor(type, name, parent),
   m_classificationTool(this),
   m_calibTools(this)
{
  m_recoStatus.resize(0);
  declareProperty ("ClusterClassificationTool",m_classificationTool);
  declareProperty ("LocalCalibTools",m_calibTools) ;
  declareProperty ("ClusterRecoStatus",m_recoStatus) ;
}


StatusCode  CaloClusterLocalCalib::initialize() {

  if (m_classificationTool.size()>1) {
    msg(MSG::ERROR) << "More than one classification tool specified!" << endreq;
    return StatusCode::FAILURE;
  }
  //Retrieve classification tool (if necessary)
  if (m_classificationTool.size()>0) {
    StatusCode sc=m_classificationTool[0].retrieve();
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Failed to retrieve classification tool " << m_classificationTool[0] << endreq;
      return sc;
    }
    msg(MSG::INFO) << "Found classification tool " << m_classificationTool[0] << endreq;
  }

  //Retrieve calibration tools
  for (auto tool : m_calibTools) {
    if(tool.retrieve().isFailure()) {
      msg(MSG::ERROR) << "Failed to retrieve calibration tool " << m_classificationTool << endreq;
      return StatusCode::FAILURE;
    }
  }
  msg(MSG::INFO) <<"Loaded "<< m_calibTools.size() <<" hadronic calibration tools"<<endreq;
  
  // check that at least one reco status is defined
  if ( m_recoStatus.size() == 0 ) {
    msg(MSG::ERROR) << "Please specify at least one valid reco status with ClusterRecoStatus = [...] for this calib tool " << endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode  CaloClusterLocalCalib::execute(CaloCluster* theCluster) {

  CaloRecoStatus& recoStatus=theCluster->recoStatus();
  // call classification tool
  if (!m_classificationTool.empty()) {
    recoStatus=m_classificationTool[0]->classify(theCluster);
  }
  // check desired reco status
  bool isSelected (false);
  for (unsigned int i=0;!isSelected && i<m_recoStatus.size();i++ )
    isSelected = recoStatus.checkStatus(CaloRecoStatus::StatusIndicator(m_recoStatus[i]));
  if ( isSelected ) {
    //report <<MSG::VERBOSE<<"Cluster is selected for local calibration"<<endreq;
    for (auto tool : m_calibTools) {
      if (tool->weight(theCluster).isFailure())
	msg(MSG::ERROR) << " failed to weight cluster " << endreq;
    }
    //report <<MSG::DEBUG<<"new cluster energy="<<theCluster->e()<<endreq;
  }//end of "has correct reco status" if statement

  // PL add calibration method bits to reco status
  recoStatus.setStatus(CaloRecoStatus::CALIBRATEDLHC);

  return StatusCode::SUCCESS;
}



