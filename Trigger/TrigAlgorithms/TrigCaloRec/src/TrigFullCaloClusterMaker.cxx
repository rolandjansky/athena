/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigFullCaloClusterMaker.cxx
// PACKAGE:  Trigger/TrigAlgorithms/TrigCaloRec
//
// AUTHOR:   P. Urquijo
//           This is an AllTE Hlt algorithm that builds offline clusters
//           and produces one RoI per cluster.
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
//
#include "TrigT1Interfaces/TrigT1Interfaces_ClassDEF.h"
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
//
#include "CaloEvent/CaloCellContainer.h"
#include "xAODCaloEvent/CaloClusterContainer.h"
//
#include "CaloUtils/CaloClusterStoreHelper.h"
//
#include "EventKernel/INavigable4Momentum.h"
#include "NavFourMom/INavigable4MomentumCollection.h"
//
#include "CaloUtils/CaloCollectionHelper.h"
//
#include "CaloRec/CaloClusterCollectionProcessor.h"
#include "CaloRec/CaloClusterProcessor.h"

#include "CaloEvent/CaloTowerContainer.h"
#include "CaloEvent/CaloShowerContainer.h"
#include "CaloEvent/CaloCellLinkContainer.h"
//#include "xAODCaloEvent/CaloClusterAuxContainer.h"
#include "xAODTrigCalo/CaloClusterTrigAuxContainer.h"
//

#include "TrigCaloRec/TrigFullCaloClusterMaker.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"
//
#include <math.h>
//
class ISvcLocator;


/////////////////////////////////////////////////////////////////////
// CONSTRUCTOR:
/////////////////////////////////////////////////////////////////////
//
TrigFullCaloClusterMaker::TrigFullCaloClusterMaker(const std::string& name, ISvcLocator* pSvcLocator)
  : HLT::AllTEAlgo(name, pSvcLocator), 
    m_pCaloClusterContainer(NULL),
    m_AllTECaloClusterContainer(NULL),
    m_useCachedResult(false), m_roiEtaLimit(4.8)
{

  // Eta and Phi size of the RoI window...

  // Name(s) of Cluster Maker Tools
  declareProperty("ClusterMakerTools",m_clusterMakerNames);

  // Name(s) of Cluster Correction Tools
  declareProperty("ClusterCorrectionTools",m_clusterCorrectionNames);


  // not needed since the key2key
  declareProperty("ClustersOutputName",m_clustersOutputName ="TriggerClusters");

  declareProperty("roiEtaLimit",m_roiEtaLimit=4.8);

  declareMonitoredVariable("ClusterContainerSize", m_ClusterContainerSize);

  declareMonitoredStdContainer("L2Eta", m_L2Eta);
  declareMonitoredStdContainer("L2Phi", m_L2Phi);

  declareMonitoredStdContainer("Eta",   m_Eta);
  declareMonitoredStdContainer("Phi",   m_Phi);
  declareMonitoredStdContainer("Et",    m_Et);

}

/////////////////////////////////////////////////////////////////////
    // DESTRUCTOR:
    /////////////////////////////////////////////////////////////////////
    //
TrigFullCaloClusterMaker::~TrigFullCaloClusterMaker()
{  }

/////////////////////////////////////////////////////////////////////
    // INITIALIZE:
    // The initialize method will create all the required algorithm objects
    // Note that it is NOT NECESSARY to run the initialize of individual
    // sub-algorithms.  The framework takes care of it.
    /////////////////////////////////////////////////////////////////////
    //

HLT::ErrorCode TrigFullCaloClusterMaker::hltInitialize()
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

  std::vector<std::string>::iterator itrName;
  std::vector<std::string>::iterator endName;

  for (int iC=1;iC<3;++iC){ 
    if (iC==1) {
      itrName = m_clusterMakerNames.begin();
      endName = m_clusterMakerNames.end();
    } else if (iC==2) {
      itrName = m_clusterCorrectionNames.begin();
      endName = m_clusterCorrectionNames.end();
    }

    for (; itrName!=endName; ++itrName) {

      ListItem theItem(*itrName);
      IAlgTool* algtool;

      if( toolSvc->retrieveTool(theItem.type(), theItem.name(), algtool,this).isFailure() ) {
	msg() << MSG::FATAL << "Unable to find tool for " << (*itrName) << endmsg;
	return HLT::BAD_JOB_SETUP;
      } else {
	msg() << MSG::DEBUG << (*itrName) << " successfully retrieved" << endmsg;
	if(iC==1) {
	  m_clusterMakerPointers.push_back(dynamic_cast<CaloClusterCollectionProcessor*>(algtool) );
	} else if (iC==2) {
	  m_clusterCorrectionPointers.push_back(dynamic_cast<CaloClusterProcessor*>(algtool) );
	}
        if(timerSvc() ) m_timer.push_back(addTimer("TCClF_"+theItem.name())); // One timer per tool
      }
    }
  }
 
  // end of helpers...
 
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG
	  << "Initialization of TrigFullCaloClusterMaker completed successfully"
	  << endmsg;

  return HLT::OK;
}


HLT::ErrorCode TrigFullCaloClusterMaker::hltFinalize()
{
  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "in finalize()" << endmsg;

  return HLT::OK;
}


//HLT::ErrorCode TrigFullCaloClusterMaker::hltExecute(const HLT::TriggerElement* inputTE,
//						HLT::TriggerElement* outputTE)
//{

HLT::ErrorCode TrigFullCaloClusterMaker::hltExecute( std::vector<std::vector<HLT::TriggerElement*> >& tes_in,
                              unsigned int type_out) {



  // Time total TrigFullCaloClusterMaker execution time.
  if (timerSvc()) m_timer[0]->start();

  if (msgLvl() <= MSG::DEBUG)
    msg() << MSG::DEBUG << "in execute()" << endmsg;

  // Monitoring initialization...
  m_ClusterContainerSize = 0.;
  m_L2Eta.clear();
  m_L2Phi.clear();
  m_Eta.clear();
  m_Phi.clear();
  m_Et.clear();


  if (m_useCachedResult){
    
    std::multimap <unsigned int, HLT::TriggerElement*>::iterator cachedTEList_it = m_cachedTEList.begin();    
    std::multimap <unsigned int, HLT::TriggerElement*>::iterator cachedTEList_end = m_cachedTEList.end();

    //    for (itrclus; itrclus != endclus; ++itrclus) {    
    for (;  cachedTEList_it!=cachedTEList_end ; ++cachedTEList_it) {    
     
      const IRoiDescriptor* roiDescriptor   = 0;

      const TrigRoiDescriptor* tmproi   = 0;
 
      HLT::TriggerElement* AllTE_cached_outputTE = 0;
      
      HLT::ErrorCode sc = getFeature(cachedTEList_it->second, tmproi);
      
      if (sc != HLT::OK || tmproi==0 ) {
	if (msgLvl() <= MSG::ERROR)
	  msg() << MSG::ERROR << "Could not find Feature to be attached" << endmsg;
	return sc;
      }
      
      roiDescriptor = tmproi;
      /// copy the RoI since this is being attached to a new TE
      /// WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING 
      /// NB:  if this is a composite RoI, it probably won't perform a deep copy of 
      ///      the RoI constituents, and may caus a crash in the event cleanup
      ///      or sooner. So this is probably NOT what you want to do
      /// WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
      if ( !tmproi->composite() ) {
	TrigRoiDescriptor* tmproi2 = new TrigRoiDescriptor( *tmproi );
	AllTE_cached_outputTE = addRoI(type_out,tmproi2);
      }
      else { 
	/// so try a deep copy here ...
	TrigRoiDescriptor* tmproi2 = new TrigRoiDescriptor( true );
	for ( unsigned i=tmproi->size() ; i-- ; ) tmproi2->push_back( new TrigRoiDescriptor( *dynamic_cast<const TrigRoiDescriptor*>(tmproi->at(i)) ) );
	tmproi2->setComposite(true);
	tmproi2->manageConstituents(true);
	AllTE_cached_outputTE = addRoI(type_out,tmproi2);
      }
      
      ///     what is this roi about if it is for the full calorimeter??
      //      float roiwidth=0.1;
      //      TrigRoiDescriptor *tmproi = new
      //      TrigRoiDescriptor(roiDescriptor->eta(), roiDescriptor->eta()-roiwidth,roiDescriptor->eta()+roiwidth,
      //		        roiDescriptor->phi(), roiDescriptor->phi()-roiwidth,roiDescriptor->phi()+roiwidth);
      
      AllTE_cached_outputTE->setActiveState(true);
      m_config->getNavigation()->copyAllFeatures(cachedTEList_it->second, AllTE_cached_outputTE);
      if (msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << "in cached mode ( RoI id ) " <<roiDescriptor->roiId() << "eta " <<roiDescriptor->eta() << "phi  " <<roiDescriptor->phi() << endmsg;
    }
    return HLT::OK;
  }

  beforeExecMonitors().ignore();


  //get all input TEs
  std::string cellcontname;
  std::string towercontname;

  //HLT::TEVec allTEs;

  std::vector<const CaloCellContainer*> vectorOfCellContainers;
  std::vector<const CaloTowerContainer*> vectorOfTowerContainers;
  std::vector<const CaloCellLinkContainer*> vectorOfCellLinkContainers;
  std::vector<const CaloShowerContainer*> vectorOfShowerContainers;


  for (std::vector<HLT::TEVec>::const_iterator it = tes_in.begin(); it != tes_in.end(); ++it) {
    
    for (HLT::TEVec::const_iterator inner_it = (*it).begin(); inner_it != (*it).end(); ++inner_it) {
      if (msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << "TE1  feature size "<< (*inner_it)->getFeatureAccessHelpers().size() << endmsg;

      //allTEs.push_back(*inner_it);
      const TrigRoiDescriptor* roiDescriptor = 0;
      HLT::ErrorCode sc = getFeature(*inner_it, roiDescriptor, "");
      if (sc != HLT::OK) return sc;
      if (msgLvl() <= MSG::DEBUG) {
        msg() << MSG::DEBUG << " REGTEST: RoI id " << roiDescriptor->roiId()
              << " located at   phi = " <<  roiDescriptor->phi()
              << ", eta = " << roiDescriptor->eta() << endmsg;
        msg() << MSG::DEBUG << " REGTEST: RoI " << *roiDescriptor << endmsg;
      }
      //get the cluster and tower container names - there should only be one of each


      cellcontname  = retrieveCellContName(*inner_it);
      towercontname = retrieveTowerContName(*inner_it);

      m_L2Eta.push_back(roiDescriptor->eta());
      m_L2Phi.push_back(roiDescriptor->phi());

      //get the cell container from the previous te to add to the new output tes
      sc = getFeatures((*inner_it), vectorOfCellContainers, "");
      if( sc != HLT::OK ){
	msg() << MSG::ERROR << "Failed to get TrigCells" << endmsg;    
	return sc;
      }

      sc = getFeatures((*inner_it), vectorOfTowerContainers, "");
      if( sc != HLT::OK) {
	msg() << MSG::ERROR << "Failed to get TrigTowers" << endmsg;    
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


  // We now take care of the Cluster Making... 
  // This container will not be recorded, so, better only view
  // the contents, so that we can delete later
  m_pCaloClusterContainer  = new xAOD::CaloClusterContainer(SG::VIEW_ELEMENTS);
  std::vector<std::string> vProp;
  
 
  // Looping over cluster maker tools... 
  
  if (timerSvc()) m_timer[1]->start(); // cluster makers time.

  int index=0;
  for (CaloClusterCollectionProcessor* clproc : m_clusterMakerPointers) {
    // We need to set the properties of the offline tools. this way of doing is ugly...
    // Abusing of harcoding?? Yes...
    if(m_clusterMakerNames[index].find("CaloTopoClusterMaker") != std::string::npos){
      //      vProp.push_back(retrieveCellContName(outputTE));
      vProp.push_back(cellcontname);
      AlgTool* algtool = dynamic_cast<AlgTool*> (clproc);
      if(!algtool || algtool->setProperty( StringArrayProperty("CellsNames",vProp) ).isFailure()) {
        msg() << MSG::ERROR << "ERROR setting the CaloCellContainer name in the offline tool" << endmsg;
	vProp.clear();
        return HLT::TOOL_FAILURE;
      }
      vProp.clear();

      //} else if(m_clusterMakerNames[index]=="CaloClusterBuilderSW/SWCluster"){
    } else if(m_clusterMakerNames[index].find("trigslw") != std::string::npos){
      //      if((*itrclus)->setProperty( StringProperty("TowerContainer",retrieveTowerContName(outputTE))).isFailure()) {
      AlgTool* algtool = dynamic_cast<AlgTool*> (clproc);
      if(!algtool || algtool->setProperty( StringProperty("TowerContainer",towercontname)).isFailure()) {
        msg() << MSG::ERROR << "ERROR setting the CaloCellContainer name in the offline tool" << endmsg;
        return HLT::TOOL_FAILURE;
      }
    }


    if (timerSvc()) m_timer[3+index]->start();
    if ( clproc->execute(m_pCaloClusterContainer).isFailure() ) {
      msg() << MSG::ERROR << "Error executing tool " << m_clusterMakerNames[index] << endmsg;
    } else {
      msg() << MSG::DEBUG << "Executed tool " << m_clusterMakerNames[index] << endmsg;
    }
    if (timerSvc()) m_timer[3+index]->stop();

    ++index;
  }
  if (timerSvc()) m_timer[1]->stop(); // cluster makers time.

  // Looping over cluster correction tools... 
  
  if (timerSvc()) m_timer[2]->start(); // cluster corrections time.
  std::vector<CaloClusterProcessor*>::const_iterator itrcct = m_clusterCorrectionPointers.begin();
  std::vector<CaloClusterProcessor*>::const_iterator endcct = m_clusterCorrectionPointers.end();
    
  index=0;
  for (; itrcct!=endcct; ++itrcct) {

    ISetCaloCellContainerName* setter =
      dynamic_cast<ISetCaloCellContainerName*> (*itrcct);
    if (setter) {
      if(setter->setCaloCellContainerName(cellcontname) .isFailure()) {
        msg() << MSG::ERROR << "ERROR setting the CaloCellContainer name in the offline tool" << endmsg;
        return HLT::BAD_JOB_SETUP;
      }
    }

    if (timerSvc()) m_timer[3+index+m_clusterMakerPointers.size()]->start(); // cluster corrections time.
    for (xAOD::CaloCluster* cl : *m_pCaloClusterContainer) {
      if ( (*itrcct)->execute(cl).isFailure() ) {
        msg() << MSG::ERROR << "Error executing correction tool " <<  m_clusterCorrectionNames[index] << endmsg;
        return HLT::TOOL_FAILURE;
      } else {
	msg() << MSG::DEBUG << "Executed correction tool " << m_clusterCorrectionNames[index] << endmsg;
      }
    }
    if (timerSvc()) m_timer[3+index+m_clusterMakerPointers.size()]->stop();
    ++index;
  }
  if (timerSvc()) m_timer[2]->stop(); // cluster corrections time.
  
  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG << " REGTEST: Produced a Cluster Container of Size= " << m_pCaloClusterContainer->size() << endmsg;
    if(!m_pCaloClusterContainer->empty()) {
      msg() << MSG::DEBUG << " REGTEST: Last Cluster Et  = " << (m_pCaloClusterContainer->back())->et() << endmsg;
      msg() << MSG::DEBUG << " REGTEST: Last Cluster eta = " << (m_pCaloClusterContainer->back())->eta() << endmsg;
      msg() << MSG::DEBUG << " REGTEST: Last Cluster phi = " << (m_pCaloClusterContainer->back())->phi() << endmsg;
    }
  }
  

  //loop through the clusters of pCaloClusterContainer do the same as above but make an RoI
  //for each one of the clusters

  int counter;
  counter=0;
  HLT::ErrorCode sc;

  for (xAOD::CaloCluster* cl : *m_pCaloClusterContainer) {
    if ( counter > 150 ) continue; // Don't accumulate too many clusters - won't work anyway
    msg() << MSG::DEBUG << "Creating one RoI per CaloCluster: type_out "<< type_out << endmsg;
    
    //may need to change this to use an roidescriptor such that the width of the cluster region is configured 
    //by this algorithm, rather than taking the default roi width. Depends on tracking configuration.

    double eta0 = cl->eta();
    double phi0 = cl->phi();

    //sanity check

    //Make sure phi of cluster is inside allowed region
    while ( phi0 >  M_PI   ) phi0 -= 2. * M_PI;
    while ( phi0 <  -M_PI  ) phi0 += 2. * M_PI;
    //Make sure eta of cluster is inside calorimeter
    if( fabs(eta0) > m_roiEtaLimit) continue;

    m_Et.push_back(cl->et());
    m_Eta.push_back(cl->eta());
    m_Phi.push_back(cl->phi());

    float roiwidth=0.1;
    TrigRoiDescriptor *tmproi = new
      TrigRoiDescriptor(eta0, eta0-roiwidth,eta0+roiwidth,
			phi0, phi0-roiwidth,phi0+roiwidth);

    HLT::TriggerElement* AllTEoutputTE = addRoI(type_out,tmproi);
    AllTEoutputTE->setActiveState(true);
    m_config->getNavigation()->copyAllFeatures( (tes_in.front()).front(), AllTEoutputTE);
   
    //add the calo cell container
    const CaloCellContainer* AllTECaloCellContainer   = vectorOfCellContainers.back();
    std::string AllTEcellCollKey;
    sc = reAttachFeature(AllTEoutputTE, AllTECaloCellContainer, cellcontname, "TrigCaloCellMaker");
    if (sc != HLT::OK) {
      msg() << MSG::ERROR << "Could not record a cell container in the TE with key " << cellcontname << endmsg;
      return sc;
    } else {
      if (msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << " REGTEST: Recorded the cell container in the TE " << endmsg;
    }

    //TrigCaloClusterMaker method
    //if ( store()->record(AllTECaloCellContainer, AllTEcellCollKey).isFailure() ) {
    //  msg() << MSG::ERROR << "Could not record a cell container in the RoI with key " << AllTEcellCollKey << endmsg;
    //}
    
    
    //Add the calo tower container. Doesn't seem to be required if copyAllFeatures is used, but will add to make sure.
    const CaloTowerContainer* AllTECaloTowerContainer = vectorOfTowerContainers.back();
    std::string AllTEtowerCollKey;
    sc = reAttachFeature(AllTEoutputTE, AllTECaloTowerContainer, towercontname, "TrigCaloTowerMaker");
    
    if (sc != HLT::OK) {
      msg() << MSG::ERROR << "Could not record a tower container in the TE with key " << towercontname << endmsg;
      return sc;
    } else {
      if (msgLvl() <= MSG::DEBUG)
	msg() << MSG::DEBUG << " REGTEST: Recorded the tower container in the TE " << endmsg;
    }
    
    
    /* Method for adding the tower collection as defined in TrigCaloTowerMaker.cxx, does not seem to be needed.
       const CaloTowerContainer* AllTECaloTowerContainer    = new CaloTowerContainer();          
       sc = getUniqueKey( AllTECaloTowerContainer, AllTEtowerCollKey, "TrigCaloTowerMaker" );
       
       if (sc != HLT::OK) {
       msg() << MSG::ERROR << "Could not record a tower container in the RoI with key " <<  AllTEtowerCollKey << endmsg;
       return sc;
       } else {
       if(msgLvl() <= MSG::DEBUG)
       msg() << MSG::DEBUG << " REGTEST: Recorded the Tower container in SG " << endmsg;
       const INavigable4MomentumCollection* theNav4Coll = 0;
       
       if ((store()->setConst(AllTECaloTowerContainer)).isSuccess()) {
       if((store()->symLink(AllTECaloTowerContainer,theNav4Coll)).isFailure()) {
       msg() << MSG::ERROR << "Could not link the Tower Container to the INavigable4MomentumCollection " << endmsg;
       return HLT::NAV_ERROR;
       }
       } else {
       msg() << MSG::ERROR << "Could not link the Tower Container to the INavigable4MomentumCollection " << endmsg;
       return HLT::NAV_ERROR;
       }
       }
    */
    
    //add the calo cluster container
    m_AllTECaloClusterContainer  = new xAOD::CaloClusterContainer();
    std::string AllTEclusterCollKey = "";

    sc = getUniqueKey(  m_pCaloClusterContainer, AllTEclusterCollKey, "TrigCaloClusterMaker" );
    if (sc != HLT::OK) {
      msg() << MSG::DEBUG << "Could not retrieve the cluster collection key" << endmsg;
      return sc;
    }
    
    if (store()->record (m_AllTECaloClusterContainer, AllTEclusterCollKey).isFailure()) {
      msg() << MSG::ERROR << "recording CaloClusterContainer with key <" << AllTEclusterCollKey << "> failed" << endmsg;
      delete m_AllTECaloClusterContainer;
      return HLT::TOOL_FAILURE;
    }

    xAOD::CaloClusterTrigAuxContainer* aux = new xAOD::CaloClusterTrigAuxContainer();
    if (store()->record (aux, AllTEclusterCollKey + "Aux.").isFailure()) {
      msg() << MSG::ERROR << "recording CaloClusterTrigAuxContainer with key <" << AllTEclusterCollKey << "Aux.> failed" << endmsg;
      delete aux;
      return HLT::TOOL_FAILURE;
    }
    m_AllTECaloClusterContainer->setStore (aux);

    m_AllTECaloClusterContainer->push_back(cl);
    
    const TrigRoiDescriptor* outputRoi;
    getFeature(AllTEoutputTE,outputRoi);
    counter++;
    msg() << MSG::DEBUG << "Event RoI number " << counter << " REGTEST: RoI id " << outputRoi->roiId()
	  << " located at   phi = " <<  outputRoi->phi()
	  << ", eta = " << outputRoi->eta() << endmsg;


#if 0    
    //m_AllTECaloClusterContainer->ROI_ID(outputRoi->roiId);
    std::stringstream strm; strm << outputRoi->roiId();
    m_AllTECaloClusterContainer->setROIAuthor(m_clustersOutputName + "_" + strm.str());
#endif
    
    if ( !CaloClusterStoreHelper::finalizeClusters( store(), m_AllTECaloClusterContainer,
                                                    AllTEclusterCollKey, msg() ).isSuccess() ) {
      msg() << MSG::ERROR << "recording CaloClusterContainer with key <" << AllTEclusterCollKey << "> failed" << endmsg;
      return HLT::TOOL_FAILURE;
    }

    // Build the "uses" relation for the outputTE to the cell container
    std::string aliasKey = "";
    if ( reAttachFeature(AllTEoutputTE, m_AllTECaloClusterContainer, aliasKey, "TrigCaloClusterMaker") != HLT::OK ) {
       msg() << MSG::ERROR
	    << "Write of RoI Cluster Container into outputTE failed"
	    << endmsg;
      return HLT::NAV_ERROR;
    }

    //caching
    m_cachedTEList.insert(std::pair<unsigned int , HLT::TriggerElement*> (outputRoi->roiId(),AllTEoutputTE));

  }
  m_useCachedResult=true;
  m_ClusterContainerSize = (float)m_pCaloClusterContainer->size();  
    
  
  // Time total TrigFullCaloClusterMaker execution time.
  if (timerSvc()){
    m_timer[0]->stop();
  }
  // avoid mem leak
  delete m_pCaloClusterContainer;
  m_pCaloClusterContainer=NULL;
  afterExecMonitors().ignore();
  return HLT::OK; 
}


std::string TrigFullCaloClusterMaker::retrieveCellContName( HLT::TriggerElement* outputTE )
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


std::string TrigFullCaloClusterMaker::retrieveTowerContName(HLT::TriggerElement* outputTE)
{
  // We retrieve the TowerContainer from the Trigger Element...
  std::vector<const CaloTowerContainer*> vectorOfTowerContainers;

  if ( getFeatures(outputTE, vectorOfTowerContainers, "") != HLT::OK ) {
    msg() << MSG::ERROR << "Failed to get TrigTowers" << endmsg;
    return "";
  }
  
  msg() << MSG::DEBUG << "Got vector with " << vectorOfTowerContainers.size() << " TowerContainers" << endmsg;
  
  // if no containers were found, just leave the vector empty and leave
  if ( vectorOfTowerContainers.size() < 1) {
    msg() << MSG::ERROR << "No towers to analyse, leaving!" << endmsg;
    return "";
  }

  // get last ccontainer to be put in vector (should also be the only one)
  const CaloTowerContainer* theCont = vectorOfTowerContainers.back();

  // All this only to rebuild the key : 
  std::string towerCollKey;
  if ( getStoreGateKey( theCont, towerCollKey) != HLT::OK) {
    msg() << MSG::ERROR << "Failed to get key for TrigTowers" << endmsg;    
    return "";
  }

  if(msgLvl() <= MSG::DEBUG) {
    msg() << MSG::DEBUG  << " REGTEST: Retrieved the Tower container in the RoI " << endmsg;
    msg() << MSG::DEBUG << " REGTEST: Retrieved a Tower Container of Size= " << theCont->size() << endmsg;
  }

  //std::cout << "@@ Got key " << towerCollKey << std::endl;
  return towerCollKey;
}
