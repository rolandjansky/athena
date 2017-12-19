/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// General:
#include "TrigHIClusterMaker.h"

// For towers:
#include "NavFourMom/INavigable4MomentumCollection.h"

#include "CaloEvent/CaloTowerSeg.h"
#include "CaloEvent/CaloTower.h" 
#include "CaloEvent/CaloTowerContainer.h" 
#include "CaloUtils/CaloCollectionHelper.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/ListItem.h"
//#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/StatusCode.h"

#include "StoreGate/StoreGateSvc.h"
//#include "AthenaKernel/errorcheck.h"

#include <string>
#include <vector>
#include <iomanip>

// For HIClusters:
#include "CaloEvent/CaloCell.h"
#include "CaloEvent/CaloCellContainer.h"
#include "Navigation/NavigationToken.h"
#include "CaloUtils/CaloClusterStoreHelper.h"
//#include "TrigCaloRec/TrigCaloQuality.h"

/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////

TrigHIClusterMaker::TrigHIClusterMaker(const std::string& name, 
			       ISvcLocator* pSvcLocator) :
   HLT::FexAlgo(name, pSvcLocator)
{
  // for clusters:
  // --------------
  declareProperty("MinimumEnergyForMoments",m_E_min_moment=50.,"> E, cluster given tower coordinates");	// originally 50.
  declareProperty("OutputContainerKey",m_output_key="HLTHICluster");
}

/////////////////////////////////////////////////////////////////////
// DESTRUCTOR:
/////////////////////////////////////////////////////////////////////

TrigHIClusterMaker::~TrigHIClusterMaker()
{  }

/////////////////////////////////////////////////////////////////////
// INITIALIZE:
// The initialize method will create all the required algorithm objects
// Note that it is NOT NECESSARY to run the initialize of individual
// sub-algorithms.  The framework takes care of it.
/////////////////////////////////////////////////////////////////////

HLT::ErrorCode TrigHIClusterMaker::hltInitialize()
{

  ATH_MSG_DEBUG ("Initialization completed successfully");
  return HLT::OK;

}

/////////////////////////////////////////////////////////////////////
// EXECUTE:
// The execute method will execute the individual sub-algorithms 
// created in the initialize method.
/////////////////////////////////////////////////////////////////////

HLT::ErrorCode TrigHIClusterMaker::hltExecute(const HLT::TriggerElement* inputTE,
                            HLT::TriggerElement* outputTE)
{
  ATH_MSG_DEBUG("outputTE->getId(): " << outputTE->getId());
  ATH_MSG_DEBUG("inputTE->getId(): " << inputTE->getId());
  

  // We retrieve the TowerContainer from the Trigger Element...
  // -------------------------------------------------------------
  std::vector<const CaloTowerContainer*> vectorOfTowerContainers;

  if ( getFeatures(outputTE, vectorOfTowerContainers, "") != HLT::OK ) {
    msg() << MSG::ERROR << "Failed to get TrigTowers" << endmsg;
    return HLT::ERROR;
  }

  msg() << MSG::DEBUG << "Got vector with " << vectorOfTowerContainers.size() << " TowerContainers" << endmsg;

  // if no containers were found, just leave the vector empty and leave
  if ( vectorOfTowerContainers.size() < 1) {
    msg() << MSG::ERROR << "No towers to analyse, leaving!" << endmsg;
    return HLT::ERROR;
  }

  // get last ccontainer to be put in vector (should also be the only one)
  const CaloTowerContainer* theCont = vectorOfTowerContainers.back();

  // All this only to rebuild the key :     
  std::string towCollKey ;
  getStoreGateKey( theCont, towCollKey );
  
  //if(msgLvl() <= MSG::WARNING) {
    msg() << MSG::DEBUG << " REGTEST: Retrieved the tower container: " << towCollKey << endmsg;
    msg() << MSG::DEBUG << " REGTEST: Retrieved a tower container of size = " << theCont->size() << endmsg;
  //}

  
  // We retrieve the cell Container from the Trigger Element...
  // -------------------------------------------------------------
  std::vector<const CaloCellContainer*> vectorOfCellContainers;
   
  HLT::ErrorCode sc = getFeatures(outputTE, vectorOfCellContainers, "");
  if (sc != HLT::OK) {
    if(msgLvl() <= MSG::WARNING) {
      msg() << MSG::WARNING << "Failed to get TrigCells" << endmsg;
    }

    // return OK anyways, for some reason...
    return HLT::OK;
  }
    
  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << "Got vector with " << vectorOfCellContainers.size()
          << " CellContainers" << endmsg;
  }
    
  // if no containers were found, just leave the vector empty and leave
  if ( vectorOfCellContainers.size() < 1) {
    if(msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "No cells to analyse, leaving!" << endmsg;
    return HLT::OK;
  }
    
  // Get the last container in the vector. Should be th one produced by the previous TrigCaloCellMaker.
  const CaloCellContainer* theCellCont = vectorOfCellContainers.back();
  if(!theCellCont){
    if(msgLvl() <= MSG::WARNING) msg() << MSG::WARNING << "No Cells found in the RoI" << endmsg;
    return HLT::OK;
  }

  // All this only to rebuild the key :     
  std::string cellCollKey ;
  getStoreGateKey( theCellCont, cellCollKey );
  
  //if(msgLvl() <= MSG::WARNING) {
    msg() << MSG::DEBUG << " REGTEST: Retrieved the cell container: " << cellCollKey << endmsg;
    msg() << MSG::DEBUG << " REGTEST: Retrieved a Cell Container of size = " << theCellCont->size() << endmsg;
  //}
 
  // Create a cluster collection
  // -----------------------------
  xAOD::CaloClusterContainer* cl_container  = new xAOD::CaloClusterContainer();

  std::string clusterCollKey = "";

  /*
  pTrigCaloQuality = 0;
  std::string persKey = "";
  std::string persKeyLink = "";
  if (store()->retrieve( pTrigCaloQuality, cellCollKey ).isFailure()) {
    pTrigCaloQuality=0;
    msg() << MSG::WARNING << "cannot retireve TrigCaloQuality with key=" << cellCollKey << endmsg;
  } else {
    persKey     = (pTrigCaloQuality->getPersistencyFlag() ? name() : "TrigHIClusterMaker");
    persKeyLink = persKey + "_Link";
  }
  msg() << MSG::DEBUG << "CaloClusterContainer is stored with key  = " << persKey << endmsg;
  msg() << MSG::DEBUG << "CaloCellLinkContainer is stored with key = " << persKeyLink << endmsg;

  //  msg() << MSG::DEBUG << store()->dump() << endmsg;
  sc = getUniqueKey( cl_container, clusterCollKey, persKey );
  if (sc != HLT::OK) {
    msg() << MSG::DEBUG << "Could not retrieve the cluster collection key" << endmsg;
    return sc;
  }
	
  if (store()->record (cl_container, clusterCollKey).isFailure()) {
    msg() << MSG::ERROR << "recording CaloClusterContainer with key <" << clusterCollKey << "> failed" << endmsg;
    delete cl_container;
    return HLT::TOOL_FAILURE;
  } 
  */

  if (store()->overwrite(cl_container, m_output_key).isFailure()) {	// overwrite <- record
    msg() << MSG::ERROR << "recording CaloClusterContainer with key <" << m_output_key << "> failed" << endmsg;
    delete cl_container;
    return HLT::ERROR;
  }
  else {
    msg() << MSG::DEBUG << "recording CaloClusterContainer with key <" << m_output_key << "> succeeded" << endmsg;
  }


  /*
  xAOD::CaloClusterAuxContainer* cl_container_aux = new xAOD::CaloClusterAuxContainer();

  if (store()->overwrite(cl_container_aux, m_output_key+"Aux").isFailure()) {
    msg() << MSG::ERROR << "recording CaloClusterAuxContainer with key <" << m_output_key+"Aux" << "> failed" << endmsg;
    delete cl_container_aux;
    return HLT::ERROR;
  }
  else {
    msg() << MSG::DEBUG << "recording CaloClusterAuxContainer with key <" << m_output_key+"Aux" << "> succeeded" << endmsg;
  }

  cl_container->setStore( cl_container_aux );
  */

  xAOD::CaloClusterAuxContainer cl_container_aux = xAOD::CaloClusterAuxContainer();
  cl_container->setStore( &cl_container_aux );

  
  // make clusters
  // ---------------------------------------------------

  const INavigable4MomentumCollection* theNav4Coll = (INavigable4MomentumCollection*) theCont;

  for(INavigable4MomentumCollection::const_iterator towerItr=theNav4Coll->begin();
      towerItr!=theNav4Coll->end(); towerItr++)
  {

    //initialize variables
    float E_cl=0;
    float eta_cl=0;
    float phi_cl=0;
    float time_cl=0;
    float R_cl=0;

    // std::vector<float> E_sampling(NUMSAMPLES,0);
    // std::vector<float> eta_sampling(NUMSAMPLES,0);
    // std::vector<float> phi_sampling(NUMSAMPLES,0);

    // //Make the cluster:
    //xAOD::CaloCluster* cl=CaloClusterStoreHelper::makeCluster(theCellCont);

    xAOD::CaloCluster* cl=CaloClusterStoreHelper::makeCluster(cl_container,theCellCont);

    //xAOD::CaloCluster* cl = new xAOD::CaloCluster();


    //navigate back to cells
    //Default is to sort the cells by either pointer values leading to irreproducible output
    //CaloCellIDFcn ensures cells are ordered by their IDs
    NavigationToken<CaloCell,double,CaloCellIDFcn> cellToken;
    (*towerItr)->fillToken(cellToken,double(1.));
    if ( cellToken.size() == 0 ) continue;
    for(NavigationToken<CaloCell,double,CaloCellIDFcn>::const_iterator cellItr = cellToken.begin();
	cellItr != cellToken.end(); cellItr++ )
    {
      //Bad cell policy
      //if(m_bad_cell_tool->SkipCell(*cellItr))
      //{
      //if(m_skipBadCells && (*cellItr)->badcell()) continue;
      //}
      
      double geoWeight = cellToken.getParameter(*cellItr); 
      double cell_E_w=(*cellItr)->energy()*geoWeight;

      IdentifierHash hashid =(*cellItr)->caloDDE()->calo_hash();
      //size_t iCell=cellColl->findIndex(hashid);
      size_t iCell=theCellCont->findIndex(hashid);
      cl->addCell(iCell,geoWeight);

      E_cl+=cell_E_w;
      eta_cl+=cell_E_w*(*cellItr)->eta();
      phi_cl+=cell_E_w*(*cellItr)->phi();

      time_cl+=cell_E_w*(*cellItr)->time();
      R_cl+=cell_E_w*(*cellItr)->caloDDE()->r();

      // unsigned int isample=0;
      // E_sampling[isample]+=cell_E_w;
      // eta_sampling[isample] =cell_E_w*(*cellItr)->eta();
      // phi_sampling[isample]+=cell_E_w*(*cellItr)->phi();

    }//end cell loop
    if (E_cl < m_E_min_moment)
    {
      eta_cl=(*towerItr)->eta();
      phi_cl=(*towerItr)->phi();
      //set time to zero?
    }
    else
    {
      eta_cl/=E_cl;
      phi_cl/=E_cl;
      time_cl/=E_cl;
      R_cl/=E_cl;
    }

    //phi moment does not respect wrap-around
    phi_cl=TVector2::Phi_mpi_pi(phi_cl);

    //set initial tower position
    cl->setEta0((*towerItr)->eta());
    cl->setPhi0((*towerItr)->phi());

    //set initial kinematics to be the same for all signal states
    //update upstream
    cl->setRawE(E_cl);
    cl->setRawEta(eta_cl);
    cl->setRawPhi(phi_cl);
    cl->setRawM(0);

    cl->setAltE(E_cl);
    cl->setAltEta(eta_cl);
    cl->setAltPhi(phi_cl);
    cl->setAltM(0);

    cl->setCalE(E_cl);
    cl->setCalEta(eta_cl);
    cl->setCalPhi(phi_cl);
    cl->setCalM(0);


    //extra info
    cl->insertMoment(xAOD::CaloCluster::CENTER_MAG,R_cl);
    cl->setTime(time_cl);

    //cl_container->push_back(cl);

    ATH_MSG_VERBOSE("PUSHING CLUSTER: " << cl->e() << ", " << cl->eta() << ", " << cl->phi() );

  } //end tower loop


  // Attaching the outputTE
  // ------------------------

  StatusCode status = CaloClusterStoreHelper::finalizeClusters( store(), cl_container,
                                                                m_output_key, msg());

  if ( status.isFailure() ) {
    msg() << MSG::ERROR << "recording CaloClusterContainer with key <" << m_output_key << "> failed" << endmsg;
    return HLT::ERROR;
  } else {
      msg() << MSG::DEBUG << " REGTEST: The cluster container finalized " << endmsg;
  }


  // Build the "uses" relation for the outputTE to the cell container
  std::string aliasKey = "HIClusters";
  if ( reAttachFeature(outputTE, cl_container, aliasKey, m_output_key ) != HLT::OK ) {
    msg() << MSG::ERROR << "Write of Cluster Container into outputTE failed" << endmsg;
    return HLT::ERROR;
  } 
  else {
    msg() << MSG::DEBUG << "ClusterContainer reattached with aliasKey" << aliasKey << endmsg;
  }


  // get a pointer to caloclusterLink
  aliasKey = "HIClustersLinks";
  const CaloClusterCellLinkContainer* pCaloCellLinkContainer = 0;
  if (store()->retrieve( pCaloCellLinkContainer, m_output_key+"_links").isFailure()) {
    msg() << MSG::WARNING << "cannot get CaloClusterCellLinkContainer (not return FAILURE) " << endmsg;
  }
  else {
    if ( reAttachFeature(outputTE, pCaloCellLinkContainer, aliasKey, m_output_key+"_Link" ) != HLT::OK ) {
      msg() << MSG::ERROR << "Write of RoI CellLink Container into outputTE failed" << endmsg;
    }
    else msg() << MSG::DEBUG << "Writing succesfully the CellLink Container with aliasKey" << aliasKey << endmsg;
  }

  //msg() << MSG::INFO << store()->dump() << endmsg;

  return HLT::OK;
}


/////////////////////////////////////////////////////////////////////
// FINALIZE:
/////////////////////////////////////////////////////////////////////

HLT::ErrorCode TrigHIClusterMaker::hltFinalize()
{
  ATH_MSG_DEBUG ("Finalizing " << name() << "..." );
  return HLT::OK;
}


HLT::ErrorCode TrigHIClusterMaker::attachClusterCollection(HLT::TriggerElement* outputTE,
                                   const xAOD::CaloClusterContainer* cl_container){

  
  std::string key = "";

  HLT::ErrorCode hltStatus = reAttachFeature(outputTE, cl_container, key, "TrigHIClusterMaker");

  if (hltStatus != HLT::OK) {
    ATH_MSG_ERROR("Failed to attach xAOD::CaloClusterContainer into outputTE with key = " << key
                  << hltStatus);
  } else {
    ATH_MSG_DEBUG("Attached heavy ion xAOD::CaloClusterContainer into outputTE with key = " << key);
  }

  return hltStatus;
}






