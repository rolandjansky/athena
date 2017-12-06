/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigLArNoisyROAlg.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigCaloRec
//
// AUTHOR:   D.O. Damazio
//
// ********************************************************************
//
#include <sstream>

//
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/ListItem.h"
#include "CaloInterface/ISetCaloCellContainerName.h"
#include "CaloEvent/CaloClusterContainer.h"
//
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
//
#include "CaloEvent/CaloCellContainer.h"
//
#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
//
#include "CaloUtils/CaloCollectionHelper.h"

#include "TrigCaloRec/TrigLArNoisyROAlg.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
//
#include "LArIdentifier/LArOnlineID.h" 
#include "LArCabling/LArCablingService.h"
//#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODTrigCalo/CaloClusterTrigAuxContainer.h"

#include <math.h>
//
class ISvcLocator;


TrigLArNoisyROAlg::TrigLArNoisyROAlg(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::AllTEAlgo(name, pSvcLocator), 
    m_useCachedResult(false), m_roiEtaLimit(4.8), m_onlineID(0),
    m_cablingService("LArCablingService"),
    m_AllTECaloClusterContainer(NULL)
{

  declareProperty("roiEtaLimit",m_roiEtaLimit=4.8);
  declareProperty( "BadChanPerFEB", m_BadChanPerFEB=30 );
  declareProperty( "CellQualityCut", m_CellQualityCut=4000 );

}

TrigLArNoisyROAlg::~TrigLArNoisyROAlg()
{  }

HLT::ErrorCode TrigLArNoisyROAlg::hltInitialize()
{
  msg() << MSG::DEBUG << "in initialize()" << endmsg;
  
  // Global timers...
  if (timerSvc()) {
    m_timer.push_back(addTimer("TCClF_TimerTot"));
    m_timer.push_back(addTimer("TCClF_TimeClustMaker"));
    m_timer.push_back(addTimer("TCClF_TimeClustCorr"));
  }

  // Cache pointer to ToolSvc
  IToolSvc* toolSvc = 0;// Pointer to Tool Service
  if (service("ToolSvc", toolSvc).isFailure()) {
    msg() << MSG::FATAL << " Tool Service not found " << endmsg;
    return HLT::BAD_JOB_SETUP;
  }

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG
	  << "Initialization of TrigLArNoisyROAlg completed successfully"
	  << endmsg;

  if ( geoInit().isFailure() ) 
    return HLT::BAD_JOB_SETUP; 

  return HLT::OK;
}

StatusCode TrigLArNoisyROAlg::geoInit(){
/*
	const  CaloIdManager* caloIdMgr;
	if ( (detStore()->retrieve(caloIdMgr)).isFailure() ){
	  msg(MSG::ERROR) << "Unable to retrieve CaloIdMgr" << endmsg;
	  return StatusCode::FAILURE;
	}
*/
	StoreGateSvc* detStore(0);
        if ( service("DetectorStore", detStore).isFailure() ){
	   msg(MSG::ERROR) << "Unable to retrieve detStore " << endmsg;
           return StatusCode::FAILURE;
	}
	if ( (detStore->retrieve(m_onlineID, "LArOnlineID")).isFailure() ){
	   msg(MSG::ERROR) << "Unable to retrieve LArOnlineID " << endmsg;
           return StatusCode::FAILURE;
	}
	if ( m_cablingService.retrieve().isFailure() ){
	   msg(MSG::ERROR) << "Unable to retrieve LArCablingService " << endmsg;
           return StatusCode::FAILURE;
	}
	return StatusCode::SUCCESS;
}


HLT::ErrorCode TrigLArNoisyROAlg::hltFinalize()
{
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "in finalize()" << endmsg;

  return HLT::OK;
}

HLT::ErrorCode TrigLArNoisyROAlg::hltExecute( std::vector<std::vector<HLT::TriggerElement*> >& tes_in,
                              unsigned int type_out) {



  // Time total TrigLArNoisyROAlg execution time.
  if (timerSvc()) m_timer[0]->start();

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "in execute()" << endmsg;

  // Monitoring initialization...

  beforeExecMonitors().ignore();


  //get all input TEs
  std::string cellcontname;

  std::vector<const CaloCellContainer*> vectorOfCellContainers;
  for (std::vector<HLT::TEVec>::const_iterator it = tes_in.begin(); it != tes_in.end(); ++it) {
    
    for (HLT::TEVec::const_iterator inner_it = (*it).begin(); inner_it != (*it).end(); ++inner_it) {
      if (msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << "TE1  feature size "<< (*inner_it)->getFeatureAccessHelpers().size() << endmsg;

      const TrigRoiDescriptor* roiDescriptor = 0;
      HLT::ErrorCode sc = getFeature(*inner_it, roiDescriptor, "");
      if (sc != HLT::OK) return sc;
      if (msgLvl() <= MSG::DEBUG)
        msg() << MSG::DEBUG << " REGTEST: RoI id " << roiDescriptor->roiId()
              << " located at   phi = " <<  roiDescriptor->phi()
              << ", eta = " << roiDescriptor->eta() << endmsg;
      //get the cluster and tower container names - there should only be one of each
      cellcontname  = retrieveCellContName(*inner_it);

      //get the cell container from the previous te to add to the new output tes
      sc = getFeatures((*inner_it), vectorOfCellContainers, "");
      if( sc != HLT::OK ){
	msg() << MSG::ERROR << "Failed to get TrigCells" << endmsg;    
	return sc;
      }

      unsigned int n_features1 = 0;
      if (msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << "TE  feature size "<< (*inner_it)->getFeatureAccessHelpers().size() << endmsg;
      for ( std::vector< HLT::TriggerElement::FeatureAccessHelper >::const_iterator itc = (*inner_it)->getFeatureAccessHelpers().begin();
	    itc != (*inner_it)->getFeatureAccessHelpers().end(); ++itc ) {
	if (msgLvl() <= MSG::DEBUG)
	  msg() << MSG::DEBUG << "TE  feature clid "<< itc->getCLID() << " feature: "<< n_features1 << endmsg;
	n_features1++;
      }

      
    }
  }

  m_FEB_BadChanCount.clear();
  const CaloCellContainer* AllTECaloCellContainer   = vectorOfCellContainers.back();
  CaloCellContainer::const_iterator cellItr    = AllTECaloCellContainer->begin();
  CaloCellContainer::const_iterator cellItrEnd = AllTECaloCellContainer->end();
  for ( ; cellItr != cellItrEnd; ++cellItr ) {
       const CaloCell* cell = (*cellItr);
       if (!cell) continue;
       if ( cell->quality() < m_CellQualityCut ) continue;
       if ( (cell->caloDDE()->getSubCalo()) != CaloCell_Base_ID::LAREM ) continue;

       Identifier id = cell->ID();
       HWIdentifier hwid = m_cablingService->createSignalChannelID(id);
       HWIdentifier febid = m_onlineID->feb_Id(hwid);
       unsigned int channel = m_onlineID->channel(hwid);
       if ( msgLvl() <= MSG::DEBUG ) {
           msg() << MSG::DEBUG;
           msg() << "cell et: " << cell->e();
           msg() << "; eta: " << cell->eta();
           msg() << "; phi: " << cell->phi();
           msg() << "; qual: " << cell->quality();
           msg() << "; Sub: " << cell->caloDDE()->getSubCalo();
           msg() << "; chan: " << channel;
           msg() << std::hex;
           msg() << "; FEB: 0x" << febid.get_identifier32().get_compact();
           msg() << std::dec;
           msg() << endmsg;
       }
       if ( m_FEB_BadChanCount.find(febid) == m_FEB_BadChanCount.end() ){
	  // This does not exist in the map
	  m_FEB_BadChanCount[febid] = 1;
       } else {
	  m_FEB_BadChanCount[febid]++;
       }
     
  }
  std::map<HWIdentifier, unsigned int>::const_iterator itr = m_FEB_BadChanCount.begin();
  std::map<HWIdentifier, unsigned int>::const_iterator itrE = m_FEB_BadChanCount.end();
  if ( msgLvl() <= MSG::DEBUG ) {
  for(; itr!=itrE; ++itr){
	msg(MSG::DEBUG) << "In FEB ID : " << std::hex 
	<< ((*itr).first).get_identifier32().get_compact()
	<< std::dec << " there are : " << (*itr).second
	<< " bad cells" << endmsg;
  }
  }

  unsigned int NBadFEBEMECA = 0;
  unsigned int NBadFEBEMECC = 0;
  unsigned int NBadFEBEMBA = 0;
  unsigned int NBadFEBEMBC = 0;
  for(itr = m_FEB_BadChanCount.begin(); itr!=itrE; ++itr){
	if ( (*itr).second > m_BadChanPerFEB ){
	   HWIdentifier chanID = m_onlineID->channel_Id((*itr).first,0);
	   if ( m_onlineID->isEMBchannel(chanID) ) {
	      if ( m_onlineID->pos_neg(chanID) == 1 ) NBadFEBEMBA++;
	      else NBadFEBEMBC++;
           } else if ( m_onlineID->isEMECchannel(chanID) ) {
	      if ( m_onlineID->pos_neg(chanID) == 1 ) NBadFEBEMECA++;
              else  NBadFEBEMECC++;
	   }
	}
  } // end of loop over FEBs
  if ( msgLvl() <= MSG::DEBUG ) {
       msg() << MSG::DEBUG;
       msg() << "NBadFEBEMBA : " << NBadFEBEMBA;
       msg() << "; NBadFEBEMBC : " << NBadFEBEMBC;
       msg() << "; NBadFEBEMECA : " << NBadFEBEMECA;
       msg() << "; NBadFEBEMECC : " << NBadFEBEMECC;
       msg() << endmsg;
  }

  m_AllTECaloClusterContainer  = new xAOD::CaloClusterContainer();
  xAOD::CaloCluster * pCaloCluster = new xAOD::CaloCluster();
  //xAOD::CaloCluster * pCaloCluster = new xAOD::CaloCluster(0,0);
  pCaloCluster->setE(NBadFEBEMBA);
  pCaloCluster->setM(NBadFEBEMBC);
  pCaloCluster->setRawE(NBadFEBEMECA);
  pCaloCluster->setTime(NBadFEBEMECC);
  m_AllTECaloClusterContainer->push_back(pCaloCluster);
  HLT::TriggerElement* AllTEoutputTE = addRoI(type_out,new TrigRoiDescriptor(0., -0.1, 0.1, 0., -0.1, 0.1));
  AllTEoutputTE->setActiveState(true);
  m_config->getNavigation()->copyAllFeatures( (tes_in.front()).front(), AllTEoutputTE);
  std::string AllTEclusterCollKey = "";
  
  xAOD::CaloClusterTrigAuxContainer* aux = new xAOD::CaloClusterTrigAuxContainer();
  m_AllTECaloClusterContainer->setStore(aux);
  
  // Build the "uses" relation for the outputTE to the cell container
  std::string aliasKey = "";
  if ( recordAndAttachFeature(AllTEoutputTE, m_AllTECaloClusterContainer, aliasKey, "TrigCaloClusterMaker_slw") != HLT::OK ) {
    msg() << MSG::ERROR
          << "Write of RoI Cluster Container into outputTE failed"
          << endmsg;
    return HLT::NAV_ERROR;
  }
  if (timerSvc()){
    m_timer[0]->stop();
  }
  // avoid mem leak
  afterExecMonitors().ignore();
  return HLT::OK; 
}


std::string TrigLArNoisyROAlg::retrieveCellContName( HLT::TriggerElement* outputTE )
{
  // We retrieve the CellContainer from the Trigger Element...
  std::vector<const CaloCellContainer*> vectorOfCellContainers;

  if( getFeatures(outputTE, vectorOfCellContainers, "") != HLT::OK) {
    msg() << MSG::ERROR << "Failed to get TrigCells" << endmsg;    
    return "";
  }
  
  msg() << MSG::DEBUG << "Got vector with " << vectorOfCellContainers.size() << " CellContainers" << endmsg;
  
  // if no containers were found, just leave the vector empty and leave
  if ( vectorOfCellContainers.size() < 1) {
    msg() << MSG::ERROR << "No cells to analyse, leaving!" << endmsg;
    return "";
  }

  // get last ccontainer to be put in vector (should also be the only one)
  const CaloCellContainer* theCellCont = vectorOfCellContainers.back();

  // All this only to retrieve the key : 
  std::string cellCollKey;
  if ( getStoreGateKey( theCellCont, cellCollKey) != HLT::OK) {
    msg() << MSG::ERROR << "Failed to get key for TrigCells" << endmsg;    
    return "";
  }

  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG  << " REGTEST: Retrieved the cell container in the RoI " << endmsg;
    msg() << MSG::DEBUG << " REGTEST: Retrieved a Cell Container of Size= " << theCellCont->size() << endmsg;
  }

  return cellCollKey;
}

