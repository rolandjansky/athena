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
  declareProperty("ShowerBuilderTool", m_showerBuilder, "Handle of instance of EMShowerBuilder");
  declareProperty("FourMomBuilderTool", m_fourMomBuilder, "Handle of instance of EMFourBuilder");
  m_caloCellDetPos = new CaloCellDetPos();

  //Monitor collections
  declareMonitoredStdContainer("EnergyBE0",m_EBE0);
  declareMonitoredStdContainer("EnergyBE1",m_EBE1);
  declareMonitoredStdContainer("EnergyBE2",m_EBE2);
  declareMonitoredStdContainer("EnergyBE3",m_EBE3);
  declareMonitoredStdContainer("Eta",m_Eta);
  declareMonitoredStdContainer("EtaCalo",m_EtaCalo);
  declareMonitoredStdContainer("PhiCalo",m_PhiCalo);
  declareMonitoredStdContainer("E",m_E);
  declareMonitoredStdContainer("ECalib",m_ECalib);
  declareMonitoredStdContainer("ERes",m_ERes);
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
      m_MVACalibTool = 0;
      return HLT::BAD_JOB_SETUP;
  }
  else {
      ATH_MSG_DEBUG("Retrieved tool " << m_MVACalibTool);   
  }
  
  if (m_showerBuilder.empty()) {
      ATH_MSG_INFO("ShowerBuilder is empty");
      return HLT::BAD_JOB_SETUP;
  }
  if(m_showerBuilder.retrieve().isFailure()) {
      ATH_MSG_ERROR("Unable to retrieve "<<m_showerBuilder);
      return HLT::BAD_JOB_SETUP; 
  }
  else ATH_MSG_DEBUG("Retrieved Tool " << m_showerBuilder);
  
  if (m_fourMomBuilder.empty()) {
      ATH_MSG_INFO("FourMomBuilder is empty");
      return HLT::BAD_JOB_SETUP;
  }
  if(m_fourMomBuilder.retrieve().isFailure()) {
      ATH_MSG_ERROR("Unable to retrieve "<<m_fourMomBuilder);
      return HLT::BAD_JOB_SETUP; 
  }
  else ATH_MSG_DEBUG("Retrieved Tool " << m_fourMomBuilder);
  ATH_MSG_DEBUG("Initialization of TrigEFCaloHypo completed successfully");
  ATH_MSG_DEBUG("Initialization successful");
  return HLT::OK;
}


HLT::ErrorCode TrigEFCaloCalibFex::hltFinalize()
{
  if ( msgLvl() <= MSG::INFO )
    msg() << MSG::INFO << "in finalize()" << endreq;

  delete m_caloCellDetPos;
  return HLT::OK;
}


HLT::ErrorCode TrigEFCaloCalibFex::hltExecute(const HLT::TriggerElement* inputTE,
        HLT::TriggerElement* outputTE) {
    HLT::ErrorCode stat = HLT::OK;

    //clear the monitoring vectors
    m_EBE0.clear();
    m_EBE1.clear();
    m_EBE2.clear();
    m_EBE3.clear();
    m_Eta.clear();
    m_EtaCalo.clear();
    m_PhiCalo.clear();
    m_E.clear();
    m_ECalib.clear();
    m_ERes.clear();


    //Set the container to 0
    m_pCaloClusterContainer = 0;
    // Retrieve cluster container
    // get pointer to CaloClusterContainer from the trigger element
    std::vector<const xAOD::CaloClusterContainer*> vectorClusterContainer;
    stat = getFeatures(inputTE, vectorClusterContainer);
    if ( stat!= HLT::OK ) {
        msg() << MSG::ERROR << " REGTEST: No CaloClusterContainers retrieved for the trigger element" << endreq;
        return HLT::OK;
    }
    //debug message
    if ( msgLvl() <= MSG::VERBOSE)
        msg() << MSG::VERBOSE << " REGTEST: Got " << vectorClusterContainer.size()
            << " CaloClusterContainers associated to the TE " << endreq;

    // Check that there is only one ClusterContainer in the RoI
    if (vectorClusterContainer.size() != 1){
        if ( msgLvl() <= MSG::ERROR )
            msg() << MSG::ERROR
                << "REGTEST: Size of vectorClusterContainer is not 1, it is: "
                << vectorClusterContainer.size()
                << endreq;
        //return HLT::BAD_JOB_SETUP;
        return HLT::OK;
    }

    // Get the last ClusterContainer
    const xAOD::CaloClusterContainer* clusContainer = vectorClusterContainer.back();
    if(!clusContainer){
        if ( msgLvl() <= MSG::ERROR )
            msg() << MSG::ERROR
                << " REGTEST: Retrieval of CaloClusterContainer from vector failed"
                << endreq;
        //return HLT::BAD_JOB_SETUP;
        return HLT::OK;
    }

    if(msgLvl() <= MSG::DEBUG) msg() << MSG::DEBUG
        << clusContainer->size() << " calo clusters in container" << endreq;

    if(clusContainer->size() < 1){
        return HLT::OK;
    }
    //==============================================================================================
    m_pCaloClusterContainer  = new xAOD::CaloClusterContainer();
    std::string clusterCollKey = "";

    pTrigCaloQuality = 0;
    std::string persKey = "";
    std::string persKeyLink = "";
    std::string cells_name = retrieveCellContName (inputTE);
    if (store()->retrieve( pTrigCaloQuality, cells_name ).isFailure()) {
        pTrigCaloQuality=0;
        msg() << MSG::WARNING << "cannot retireve TrigCaloQuality with key=" << cells_name << endreq;
    } else {
        persKey     = (pTrigCaloQuality->getPersistencyFlag() ? name() : "TrigEFCaloCalibFex");
        persKeyLink = persKey + "_Link";
    }
    msg() << MSG::DEBUG << "CaloClusterContainer is stored with key  = " << persKey << endreq;
    msg() << MSG::DEBUG << "CaloCellLinkContainer is stored with key = " << persKeyLink << endreq;

    //  msg() << MSG::DEBUG << store()->dump() << endreq;
    HLT::ErrorCode sc = getUniqueKey( m_pCaloClusterContainer, clusterCollKey, persKey );
    if (sc != HLT::OK) {
        msg() << MSG::DEBUG << "Could not retrieve the cluster collection key" << endreq;
        return sc;
    }

    if (store()->record (m_pCaloClusterContainer, clusterCollKey).isFailure()) {
        msg() << MSG::ERROR << "recording CaloClusterContainer with key <" << clusterCollKey << "> failed" << endreq;
        delete m_pCaloClusterContainer;
        return HLT::TOOL_FAILURE;
    }

    xAOD::CaloClusterAuxContainer* aux = new xAOD::CaloClusterAuxContainer();
    if (store()->record (aux, clusterCollKey + "Aux.").isFailure()) {
        msg() << MSG::ERROR << "recording CaloClusterAuxContainer with key <" << clusterCollKey << "Aux.> failed" << endreq;
        delete aux;
        return HLT::TOOL_FAILURE;
    }
    m_pCaloClusterContainer->setStore (aux);

   
    // Now setup the Photon container for selection at EFCaloHypo
    //
    m_egContainer = new xAOD::PhotonContainer();
    xAOD::PhotonAuxContainer *egAux = new xAOD::PhotonAuxContainer();
    std::string egKey = "TrigEFCaloCalibFex"; 
    std::string egContSGKey = "";
    sc = getUniqueKey( m_egContainer, egContSGKey, egKey);
    if (sc != HLT::OK) { 
        msg() << MSG::DEBUG << "Could not retrieve the photon container key" << endreq;
        return sc;                                                                       
    } 
    std::string egContSGName = egContSGKey; //if you put no empty string here you'll get an alias in SG
    
    if ( store()->record(m_egContainer, egContSGKey).isFailure()) {
        msg() << MSG::ERROR << "REGTEST: trigger PhotonContainer registration failed" << endreq;
        return HLT::ERROR;
    }
    if ( store()->record(egAux, egContSGKey + "Aux.").isFailure() ) {
        msg() << MSG::ERROR << "REGTEST: trigger PhotonAuxContainer registration failed" << endreq;
        delete egAux;
        return HLT::ERROR;
    }
    m_egContainer->setStore(egAux);
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
                << endreq;
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
        
        //newClus->addCellLink(new CaloClusterCellLink(clus->getCellLinks()->getCellContainer())); //Ensure the cell links are there
        ATH_MSG_DEBUG("Copied cluster Energy, eta, phi " << newClus->e() << " " << newClus->eta() << " " << newClus->phi());
        ATH_MSG_DEBUG("Copied cluster raw E, eta, phi " << newClus->rawE() << " " << newClus->rawEta() << " " << newClus->rawPhi()); 
        ATH_MSG_DEBUG("Copied cluster cal E, eta, phi " << newClus->calE() << " " << newClus->calEta() << " " << newClus->calPhi()); 
        ATH_MSG_DEBUG("Copied cluster raw E in layers " << newClus->energyBE(0) << " " << newClus->energyBE(1) << " " << newClus->energyBE(2) << " " <<newClus->energyBE(3));
        ATH_MSG_DEBUG("Copied cluster " << newClus->eta0() << " " << newClus->clusterSize() << " " << newClus->eta0() << " " << newClus->phi0() <<
                newClus->etaBE(0) << " " << newClus->etaBE(2));
        
        //Now fill position in calo-frame
        fillPositionsInCalo(newClus);
         //Now set the calibrated energy from MVA
        if(m_applyMVACalib){
            ATH_MSG_DEBUG("Applying MVA Calib");
            if(m_MVACalibTool->hltexecute(newClus,m_egType).isFailure())
                ATH_MSG_DEBUG("MVACalib Fails");
        }
        ATH_MSG_DEBUG("MVA Calibrated Cluster Energy, eta, phi " << newClus->e() << " " << newClus->eta() << " " << newClus->phi());

        //Setting link to new cluster for temporary studies
        const ElementLink<xAOD::CaloClusterContainer> linkToOriginal(*clusContainer,iclus);
        newClus->auxdata< ElementLink< xAOD::CaloClusterContainer > >( "originalCaloCluster" ) = linkToOriginal;
        // linkToOriginal.toPersistent(); // Not compiling, is this still needed or persistency done behind the scenes???
       
        // Now creating the photon objects (dummy EDM for Calo selection at EF)
        const ElementLink<xAOD::CaloClusterContainer> clusterLink(*m_pCaloClusterContainer,iclus);
        std::vector< ElementLink<xAOD::CaloClusterContainer> > clLinks;
        clLinks.push_back(clusterLink);

        xAOD::Photon *eg = new xAOD::Photon();
        m_egContainer->push_back(eg);
        eg->setCaloClusterLinks(clLinks);

        // Run the tools for each object
        ATH_MSG_DEBUG("REGTEST: Run the tools on eg object");
        if(m_fourMomBuilder->hltExecute(eg,0));
        else ATH_MSG_DEBUG("Problem with FourMomBuilder");
        if(m_showerBuilder->recoExecute(eg,pCaloCellContainer));
        else ATH_MSG_DEBUG("Problem with ShowerBuilder");
        ATH_MSG_DEBUG("REGTEST: e " << eg->e());
        ATH_MSG_DEBUG("REGTEST: eta " << eg->eta());
        ATH_MSG_DEBUG("REGTEST: phi " << eg->phi());

        // Monitoring
        m_EBE0.push_back(newClus->energyBE(0));
        m_EBE0.push_back(newClus->energyBE(1));
        m_EBE0.push_back(newClus->energyBE(2));
        m_EBE0.push_back(newClus->energyBE(3));
        m_Eta.push_back(newClus->eta());
        double tmpeta = -999.;
        newClus->retrieveMoment(xAOD::CaloCluster::ETACALOFRAME,tmpeta);
        double tmpphi = -999.;
        newClus->retrieveMoment(xAOD::CaloCluster::ETACALOFRAME,tmpphi);
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
                << endreq;
        }
    } else {
        pass = false;
        if ( msgLvl() <= MSG::DEBUG ) {
            msg() << MSG::DEBUG << "AcceptAll property not set: applying selection"
                << endreq;
        }
    }

    bool status = CaloClusterStoreHelper::finalizeClusters( store(), m_pCaloClusterContainer,
            clusterCollKey, msg());

    if ( !status ) { 
        msg() << MSG::ERROR << "recording CaloClusterContainer with key <" << clusterCollKey << "> failed" << endreq;
        return HLT::TOOL_FAILURE;
    } else {
        if(msgLvl() <= MSG::DEBUG)
            msg() << MSG::DEBUG << " REGTEST: Recorded the cluster container in the RoI " << endreq;
    }
    
    // Build the "uses" relation for the outputTE to the cell container
    std::string aliasKey = "";
    stat = reAttachFeature(outputTE, m_pCaloClusterContainer, aliasKey, persKey );

    if (stat != HLT::OK) {
        msg() << MSG::ERROR
            << "Write of RoI Cluster Container into outputTE failed"
            << endreq;
        return HLT::NAV_ERROR;
    }
    
    // get a pointer to caloclusterLink
    const CaloClusterCellLinkContainer* pCaloCellLinkContainer = 0;
    if (store()->retrieve( pCaloCellLinkContainer, clusterCollKey+"_links").isFailure()) {
        msg() << MSG::WARNING << "cannot get CaloClusterCellLinkContainer (not return FAILURE) " << endreq;
    }
    else {
        stat = reAttachFeature(outputTE, pCaloCellLinkContainer, aliasKey, persKeyLink );
        if (stat != HLT::OK) {
            msg() << MSG::ERROR
                << "Write of RoI CellLink Container into outputTE failed"
                << endreq;
        }
    }

    stat = reAttachFeature(outputTE, pCaloCellContainer, aliasKey, cells_name );
    if (stat != HLT::OK) {
        msg() << MSG::ERROR
            << "Write of RoI CellLink Container into outputTE failed"
            << endreq;
    }

    // attach photon container to the TE
    stat = reAttachFeature( outputTE, m_egContainer, egContSGName, egKey);
    if (stat != HLT::OK){
        ATH_MSG_ERROR("REGTEST: trigger xAOD::PhotonContainer attach to TE and record into StoreGate failed");
        return status;
    } else{
        ATH_MSG_DEBUG( "egKey for photon container: " << egKey);
        ATH_MSG_DEBUG( "REGTEST: xAOD::PhotonContainer created and attached to TE: " <<egContSGName 
                << " with alias: " << egContSGKey);
    }

    // lock the photon collection
    if ( (store()->setConst(m_egContainer)).isFailure() ) {
        ATH_MSG_ERROR(" REGTEST: cannot set m_photon_container to const ");
        return HLT::OK;
    } else {
        // debug message
        ATH_MSG_DEBUG("photon container locked with key: " << egContSGName 
                << " and address: " << m_egContainer);
    }
    // set output TriggerElement unless acceptAll is set
    if (!m_acceptAll) pass = result;

    if ( msgLvl() <= MSG::DEBUG ) msg() << MSG::DEBUG << "acceptInput = " << pass << endreq;

    return HLT::OK;
}

std::string TrigEFCaloCalibFex::retrieveCellContName( const HLT::TriggerElement* inputTE )
{
    // We retrieve the CellContainer from the Trigger Element...
    std::vector<const CaloCellContainer*> vectorOfCellContainers;

    if( getFeatures(inputTE, vectorOfCellContainers, "") != HLT::OK) {
        msg() << MSG::ERROR << "Failed to get TrigCells" << endreq;   
        return "";
    }

    msg() << MSG::DEBUG << "Got vector with " << vectorOfCellContainers.size() << " CellContainers" << endreq;

    // if no containers were found, just leave the vector empty and leave
    if ( vectorOfCellContainers.size() < 1) {
        msg() << MSG::ERROR << "No cells to analyse, leaving!" << endreq;
        return "";
    }

    // get last ccontainer to be put in vector (should also be the only one)
    const CaloCellContainer* theCellCont = vectorOfCellContainers.back();

    // All this only to retrieve the key :
    std::string cellCollKey;
    if ( getStoreGateKey( theCellCont, cellCollKey) != HLT::OK) {
        msg() << MSG::ERROR << "Failed to get key for TrigCells" << endreq;   
        return "";
    }

    if(msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG << " REGTEST: Retrieved the cell container in the RoI " << endreq;
        msg() << MSG::DEBUG << " REGTEST: Retrieved a Cell Container of Size= " << theCellCont->size() << endreq;
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
