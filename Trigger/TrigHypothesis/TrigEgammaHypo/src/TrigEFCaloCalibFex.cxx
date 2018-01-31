// -*- C++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/**************************************************************************
 **
 **   File: Trigger/TrigHypothesis/TrigEgammaHypo/TrigEFCaloCalibFex.cxx
 **
 **   Description: FEX Algorithm to apply MVA calibration for HLT CaloClusters 
 **                
 **
 **   Author: Ryan Mackenzie White 
 **            
 **
 **   Created:      October 2014 
 **************************************************************************/

#include "TrigEgammaHypo/TrigEFCaloCalibFex.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
#include "TrigCaloRec/TrigCaloQuality.h"
#include "egammaMVACalib/IegammaMVATool.h"
#include "xAODEgamma/EgammaEnums.h"

class ISvcLocator;


TrigEFCaloCalibFex::TrigEFCaloCalibFex(const std::string & name, ISvcLocator* pSvcLocator)
  : HLT::FexAlgo(name, pSvcLocator)
{
  // Read cuts - should probably get these from an xml file
  declareProperty( "AcceptAll",            m_acceptAll  = false );
  declareProperty( "egType", m_egType = "Electron");
  declareProperty("MVACalibTool", m_MVACalibTool);
  declareProperty("ApplyMVACalib", m_applyMVACalib=true);
  declareProperty("ClusterContainerKey", m_persKey="TrigEFCaloCalibFex");
  m_caloCellDetPos = new CaloCellDetPos();

  //Monitor collections
  declareMonitoredStdContainer("EnergyBE0",m_EBE0,AutoClear);
  declareMonitoredStdContainer("EnergyBE1",m_EBE1,AutoClear);
  declareMonitoredStdContainer("EnergyBE2",m_EBE2,AutoClear);
  declareMonitoredStdContainer("EnergyBE3",m_EBE3,AutoClear);
  declareMonitoredStdContainer("Eta",m_Eta,AutoClear);
  declareMonitoredStdContainer("Phi",m_Phi,AutoClear);
  declareMonitoredStdContainer("EtaCalo",m_EtaCalo,AutoClear);
  declareMonitoredStdContainer("PhiCalo",m_PhiCalo,AutoClear);
  declareMonitoredStdContainer("E",m_E,AutoClear);
  declareMonitoredStdContainer("ECalib",m_ECalib,AutoClear);
  declareMonitoredStdContainer("ERes",m_ERes,AutoClear);
  
  //Initialize pointers
  m_totalTimer=nullptr;
  m_toolTimer=nullptr;
  pTrigCaloQuality=nullptr;
  m_pCaloClusterContainer=nullptr;
}


TrigEFCaloCalibFex::~TrigEFCaloCalibFex()
{}


HLT::ErrorCode TrigEFCaloCalibFex::hltInitialize()
{
  ATH_MSG_DEBUG("Initialization:");

  // print out settings
  ATH_MSG_DEBUG("Initialization completed successfully:");
  ATH_MSG_DEBUG("AcceptAll            = "	<< (m_acceptAll==true ? "True" : "False") ); 
  // Get the cluster correction tool
  
  if(m_MVACalibTool.retrieve().isFailure()) {
      ATH_MSG_ERROR("Failed to retrieve " << m_MVACalibTool);
      m_applyMVACalib = false;
      return HLT::BAD_JOB_SETUP;
  }
  else {
      ATH_MSG_DEBUG("Retrieved tool " << m_MVACalibTool);   
  }
  
  
  if (timerSvc()){
   m_totalTimer  = addTimer("TrigEFCaloCalibFexTot");
   m_toolTimer = addTimer("MVACalibTool");
  }
  
  ATH_MSG_DEBUG("Initialization of TrigEFCaloHypo completed successfully");
  return HLT::OK;
}


HLT::ErrorCode TrigEFCaloCalibFex::hltFinalize()
{
    ATH_MSG_INFO("in finalize()");

  delete m_caloCellDetPos;
  return HLT::OK;
}


HLT::ErrorCode TrigEFCaloCalibFex::hltExecute(const HLT::TriggerElement* inputTE,
        HLT::TriggerElement* outputTE) {
    HLT::ErrorCode stat = HLT::OK;

    // Time total TrigEFCaloHypo execution time.
    // -------------------------------------
    if (timerSvc()) m_totalTimer->start();    
    //Set the container to 0
    m_pCaloClusterContainer = 0;
    // Retrieve cluster container
    // get pointer to CaloClusterContainer from the trigger element
    std::vector<const xAOD::CaloClusterContainer*> vectorClusterContainer;
    stat = getFeatures(inputTE, vectorClusterContainer);
    if ( stat!= HLT::OK ) {
        msg() << MSG::ERROR << " REGTEST: No CaloClusterContainers retrieved for the trigger element" << endmsg;
        return HLT::OK;
    }
    //debug message
    if ( msgLvl() <= MSG::VERBOSE)
        msg() << MSG::VERBOSE << " REGTEST: Got " << vectorClusterContainer.size()
            << " CaloClusterContainers associated to the TE " << endmsg;

    // Check that there is only one ClusterContainer in the RoI
    if (vectorClusterContainer.size() != 1){
        if ( msgLvl() <= MSG::ERROR )
            msg() << MSG::ERROR
                << "REGTEST: Size of vectorClusterContainer is not 1, it is: "
                << vectorClusterContainer.size()
                << endmsg;
        //return HLT::BAD_JOB_SETUP;
        return HLT::OK;
    }

    // Get the last ClusterContainer
    const xAOD::CaloClusterContainer* clusContainer = vectorClusterContainer.back();
    if(!clusContainer){
        if ( msgLvl() <= MSG::ERROR )
            msg() << MSG::ERROR
                << " REGTEST: Retrieval of CaloClusterContainer from vector failed"
                << endmsg;
        //return HLT::BAD_JOB_SETUP;
        return HLT::OK;
    }

    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG
        << clusContainer->size() << " calo clusters in container" << endmsg;

    if(clusContainer->size() < 1){
        return HLT::OK;
    }
    size_t coll_size = clusContainer->size();
    //==============================================================================================
    m_pCaloClusterContainer  = new xAOD::CaloClusterContainer();
    std::string clusterCollKey = "";

    msg() << MSG::DEBUG << "CaloClusterContainer is stored with key  = " << m_persKey << endmsg;

    //  msg() << MSG::DEBUG << store()->dump() << endmsg;
    HLT::ErrorCode sc = getUniqueKey( m_pCaloClusterContainer, clusterCollKey, m_persKey );
    if (sc != HLT::OK) {
        msg() << MSG::DEBUG << "Could not retrieve the cluster collection key" << endmsg;
        return sc;
    }

    if (store()->record (m_pCaloClusterContainer, clusterCollKey).isFailure()) {
        msg() << MSG::ERROR << "recording CaloClusterContainer with key <" << clusterCollKey << "> failed" << endmsg;
        delete m_pCaloClusterContainer;
        return HLT::TOOL_FAILURE;
    }

    xAOD::CaloClusterTrigAuxContainer aux;
    m_pCaloClusterContainer->setStore (&aux);
    m_pCaloClusterContainer->reserve(coll_size);
   
    //==============================================================================================
    
    
    // Retrieve CaloCellContainers from InputTE and reattach to outputTE
    const CaloCellContainer* pCaloCellContainer = 0;
    // Get vector of pointers to all CaloCellContainers from TE
    std::vector<const CaloCellContainer*> vectorCellContainer;

    stat = getFeatures(inputTE, vectorCellContainer);
    if ( stat != HLT::OK ) {
        ATH_MSG_ERROR("REGTEST: No CaloCellContainers retrieved for the trigger element");
    } else{
        ATH_MSG_DEBUG(" REGTEST: Got " << vectorCellContainer.size() << " CaloCellContainers associated to the TE "); 
        // Check that there is only one CellContainer in the RoI
        if (vectorCellContainer.size() != 1){
            msg() << MSG::ERROR
                << "REGTEST: Size of calo cell container vector is not 1 but " << vectorCellContainer.size()
                << endmsg;
            return HLT::ERROR;
        }
        else{
            // Get the last CellContainer if ShoweShapeTool is going to be run
            pCaloCellContainer = vectorCellContainer.back();
            if(!pCaloCellContainer){
                ATH_MSG_ERROR("Retrieval of CaloCellContainer from vector failed, m_showerBuilderToExec: ");
                return HLT::ERROR;
                //May need to add ERROR codes for online debugging
            } 
        }
    }

    // Need a counter to know position in container
    // Rather use auto
    unsigned int iclus=0;
    
    for(const auto *clus : *clusContainer){
        
	// To avoid clusters formed from cosmics or noise bursts
        if ( (clus->eta() < -998.0) && (clus->phi() < -998.0) ) continue;

        if (!clus->inBarrel() && !clus->inEndcap() )
        {
            ATH_MSG_ERROR("Cluster neither in barrel nor in endcap, Skipping cluster");
            continue;
        }

        ATH_MSG_DEBUG("Found cluster with state " << clus->signalState());
        ATH_MSG_DEBUG("Cluster Energy, eta, phi " << clus->e() << " " << clus->eta() << " " << clus->phi()); 
        ATH_MSG_DEBUG("Cluster raw E" << clus->energyBE(0) << " " << clus->energyBE(1) << " " << clus->energyBE(2) << " " <<clus->energyBE(3)); 
        //Make a copy of the original cluster
        xAOD::CaloCluster *newClus = new xAOD::CaloCluster(*clus);
        m_pCaloClusterContainer->push_back(newClus);
        //*newClus = *clus; //This causes an FPE overflow in TrigEgammaRec
        
        ATH_MSG_DEBUG("Copied cluster Energy, eta, phi " << newClus->e() << " " << newClus->eta() << " " << newClus->phi());
        ATH_MSG_DEBUG("Copied cluster raw E, eta, phi " << newClus->rawE() << " " << newClus->rawEta() << " " << newClus->rawPhi()); 
        ATH_MSG_DEBUG("Copied cluster cal E, eta, phi " << newClus->calE() << " " << newClus->calEta() << " " << newClus->calPhi()); 
        ATH_MSG_DEBUG("Copied cluster raw E in layers " << newClus->energyBE(0) << " " << newClus->energyBE(1) << " " << newClus->energyBE(2) << " " <<newClus->energyBE(3));
        ATH_MSG_DEBUG("Copied cluster " << newClus->eta0() << " " << newClus->clusterSize() << " " << newClus->eta0() << " " << newClus->phi0() <<
                newClus->etaBE(0) << " " << newClus->etaBE(2));
        
        //Now fill position in calo-frame
        fillPositionsInCalo(newClus);
        double tmpeta = -999.;
        newClus->retrieveMoment(xAOD::CaloCluster::ETACALOFRAME,tmpeta);
        double tmpphi = -999.;
        newClus->retrieveMoment(xAOD::CaloCluster::PHICALOFRAME,tmpphi);
        ATH_MSG_DEBUG("Calo frame vars eta, phi = " << tmpeta << " " << tmpphi);
        //Now set the calibrated energy from MVA
        if(m_applyMVACalib){
            ATH_MSG_DEBUG("Applying MVA Calib");

            if (timerSvc()) m_toolTimer->start();    
            if(m_MVACalibTool->hltexecute(newClus,m_egType).isFailure())
                ATH_MSG_DEBUG("MVACalib Fails");
            if (timerSvc()) m_toolTimer->stop();    
        }
        ATH_MSG_DEBUG("MVA Calibrated Cluster Energy, eta, phi " << newClus->e() << " " << newClus->eta() << " " << newClus->phi());

        //Setting link to new cluster for temporary studies
        const ElementLink<xAOD::CaloClusterContainer> linkToOriginal(*clusContainer,iclus);
        newClus->auxdata< ElementLink< xAOD::CaloClusterContainer > >( "originalCaloCluster" ) = linkToOriginal;
       


        // Monitoring
        m_EBE0.push_back(newClus->energyBE(0));
        m_EBE1.push_back(newClus->energyBE(1));
        m_EBE2.push_back(newClus->energyBE(2));
        m_EBE3.push_back(newClus->energyBE(3));
        m_Eta.push_back(newClus->eta());
        m_Phi.push_back(newClus->phi());
        m_EtaCalo.push_back(tmpeta);
        m_PhiCalo.push_back(tmpphi);
        m_E.push_back(clus->e());
        m_ECalib.push_back(newClus->e());
        m_ERes.push_back(clus->e()-newClus->e());

        // Increment counter to keep position in container
        iclus++;
    }

    bool pass = false;
    bool result = false;
    // Accept-All mode: temporary patch; should be done with force-accept
    if (m_acceptAll) {
        pass = true;
        if ( msgLvl() <= MSG::DEBUG ) {
            msg() << MSG::DEBUG << "AcceptAll property is set: taking all events"
                << endmsg;
        }
    } else {
        pass = false;
        if ( msgLvl() <= MSG::DEBUG ) {
            msg() << MSG::DEBUG << "AcceptAll property not set: applying selection"
                << endmsg;
        }
    }

    bool status = CaloClusterStoreHelper::finalizeClusters( store(), m_pCaloClusterContainer,
            clusterCollKey, msg());

    if ( !status ) { 
        msg() << MSG::ERROR << "recording CaloClusterContainer with key <" << clusterCollKey << "> failed" << endmsg;
        return HLT::TOOL_FAILURE;
    } else {
        if(msgLvl() <= MSG::DEBUG)
            msg() << MSG::DEBUG << " REGTEST: Recorded the cluster container in the RoI " << endmsg;
    }
    
    std::string aliasKey = "";
    stat = reAttachFeature(outputTE, m_pCaloClusterContainer, aliasKey, m_persKey );

    if (stat != HLT::OK) {
        msg() << MSG::ERROR
            << "Write of RoI Cluster Container into outputTE failed"
            << endmsg;
        return HLT::NAV_ERROR;
    }

    // set output TriggerElement unless acceptAll is set
    if (!m_acceptAll) pass = result;

    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "acceptInput = " << pass << endmsg;

    // Time total TrigEFCaloHypo execution time.
    // -------------------------------------
    if (timerSvc()) m_totalTimer->stop();    
    return HLT::OK;
}

std::string TrigEFCaloCalibFex::retrieveCellContName( const HLT::TriggerElement* inputTE )
{
    // We retrieve the CellContainer from the Trigger Element...
    std::vector<const CaloCellContainer*> vectorOfCellContainers;

    if( getFeatures(inputTE, vectorOfCellContainers, "") != HLT::OK) {
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
        msg() << MSG::DEBUG << " REGTEST: Retrieved the cell container in the RoI " << endmsg;
        msg() << MSG::DEBUG << " REGTEST: Retrieved a Cell Container of Size= " << theCellCont->size() << endmsg;
    }

    return cellCollKey;
}

void TrigEFCaloCalibFex::fillPositionsInCalo(xAOD::CaloCluster* cluster){
    ATH_MSG_DEBUG("Fill positions in Calo");
    CaloCell_ID::CaloSample sample = cluster->inBarrel() ? CaloCell_ID::EMB2 : CaloCell_ID::EME2;
    // eta and phi of the cluster in the calorimeter frame
    double eta, phi;
    m_caloCellDetPos->getDetPosition(sample, cluster->eta(), cluster->phi(), eta, phi);

    cluster->insertMoment(xAOD::CaloCluster::ETACALOFRAME,eta);
    cluster->insertMoment(xAOD::CaloCluster::PHICALOFRAME,phi);

    //  eta in the second sampling
    m_caloCellDetPos->getDetPosition(sample, cluster->etaBE(2), cluster->phiBE(2), eta, phi);
    cluster->insertMoment(xAOD::CaloCluster::ETA2CALOFRAME,eta);
    cluster->insertMoment(xAOD::CaloCluster::PHI2CALOFRAME,phi);
    //  eta in the first sampling
    sample = cluster->inBarrel() ? CaloCell_ID::EMB1 : CaloCell_ID::EME1;
    m_caloCellDetPos->getDetPosition(sample, cluster->etaBE(1), cluster->phiBE(1),eta, phi);
    cluster->insertMoment(xAOD::CaloCluster::ETA1CALOFRAME,eta);
    cluster->insertMoment(xAOD::CaloCluster::PHI1CALOFRAME,phi);

}
