/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ClusterCreator.cxx 585681 2014-02-28 14:23:10Z wlampl $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#define private public
#define protected public
#include "CaloEvent/CaloClusterContainer.h"
#undef private
#undef protected
#include "xAODCaloEvent/CaloClusterContainer.h"
#include "xAODCaloEvent/CaloClusterAuxContainer.h"

// Local include(s):
#include "ClusterCreator.h"

ClusterCreator::ClusterCreator( const std::string& name,
				ISvcLocator* svcLoc )
  : AthAlgorithm( name, svcLoc ),
    m_keySet(false){
  declareProperty( "AODContainerNames",m_aodContainerNames);
  declareProperty( "xAODContainerNames",m_xaodContainerNames);
}

StatusCode ClusterCreator::initialize() {

  ATH_MSG_INFO( "Initializing - Package version: " << PACKAGE_VERSION );

  if (m_xaodContainerNames.size()==0) //Assume same key in AOD and xAOD if to explicitly set
    m_xaodContainerNames=m_aodContainerNames;

  if (m_aodContainerNames.size()!=m_xaodContainerNames.size()) {
    msg(MSG::ERROR) << "Job properties 'AODContainerNames' and 'xAODContainerNames' have unequal lenght! (" 
		    << m_aodContainerNames.size() << " vs " << m_xaodContainerNames.size() << ")" << endreq;
    return StatusCode::FAILURE;
  }

  if (m_aodContainerNames.size()==0 && m_xaodContainerNames.size()==0) {
    ATH_MSG_INFO( "Will convert all CaloClusterContainers in the event");
    m_keySet=false;
  }
  else {
    m_keySet=true;
    const size_t nKeys=m_aodContainerNames.size();
    for (size_t i=0;i<nKeys;++i) {
      msg(MSG::INFO) << "Will convert CaloClusterContainer#" << m_aodContainerNames[i] << " to xAOD::CaloClusterContainer#" << m_xaodContainerNames[i] << endreq;
    }
  }

  //ATH_MSG_INFO( "AODContainerName  = " << m_aodContainerName );
  //ATH_MSG_INFO( "xAODContainerName = " << m_xaodContainerName );

  // Return gracefully:
  return StatusCode::SUCCESS;
}

StatusCode ClusterCreator::execute() {

  if (!m_keySet) {
    evtStore()->keys<CaloClusterContainer>(m_aodContainerNames,false,false);
    m_xaodContainerNames=m_aodContainerNames; //Use same container key for input and output
  
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << "Found the following CaloClusterContainers: ";
      for (const std::string& k : m_aodContainerNames) {
	msg(MSG::DEBUG) << k << ", ";
      }
      msg(MSG::DEBUG) << endreq;
    }
  }

  const size_t nKeys=m_aodContainerNames.size();

  //Loop over container keys
  for (size_t iKey=0;iKey<nKeys;++iKey) {
    const std::string& aodName=m_aodContainerNames[iKey];
    const std::string& xaodName=m_xaodContainerNames[iKey];

    if (!evtStore()->contains<CaloClusterContainer>(aodName)) {
      ATH_MSG_DEBUG("AOD cluster container " << aodName << " not found. Ignored.");
      continue;
    }

    if (evtStore()->contains<xAOD::CaloClusterContainer>(xaodName)) {
      ATH_MSG_DEBUG("xAOD::CaloCluster with key " << xaodName << " exists already. No conversion done");
      continue;
    }

    // Retrieve the AOD clusters:
    const CaloClusterContainer* aod = 0;
    CHECK( evtStore()->retrieve( aod, aodName ) );
    ATH_MSG_DEBUG( "Retrieved clusters with key: " << aodName );
  
    
    //Create new Cell-Link container
    const std::string ccclc_name=xaodName+"_links";
    CaloClusterCellLinkContainer* ccclc=new CaloClusterCellLinkContainer();
    CHECK(evtStore()->record(ccclc,ccclc_name));


    // Create the xAOD container and its auxiliary store:
    xAOD::CaloClusterContainer* xaod = new xAOD::CaloClusterContainer();
    CHECK( evtStore()->record( xaod, xaodName ) );
    xAOD::CaloClusterAuxContainer* aux = new xAOD::CaloClusterAuxContainer();
    CHECK( evtStore()->record( aux, xaodName + "Aux." ) );
    xaod->setStore( aux );
    ATH_MSG_DEBUG( "Recorded xAOD clusters with key: " << xaodName );
  
    // Create the xAOD objects:
    CaloClusterContainer::const_iterator itr = aod->begin();
    CaloClusterContainer::const_iterator end = aod->end();
    for(unsigned i = 0 ; itr != end; ++itr, ++i ) {

      ATH_MSG_VERBOSE( "Copying cluster #" << i );

      // Create the xAOD object:
      xAOD::CaloCluster* cluster = new xAOD::CaloCluster();
      xaod->push_back( cluster );

      if ((*itr)->isCellLinkValid()) {
	ATH_MSG_DEBUG("Found cluster with valid cell link");
	CaloCluster::cell_iterator cit=(*itr)->begin();
	CaloCluster::cell_iterator cit_e=(*itr)->end();
	if (cit!=cit_e) { //Protect against empty-cluster case
	  //Get underlying cell container
	  const CaloCellContainer* ccc=(*itr)->getContainer(cit);
	  //std::cout << "Got a cell container with size " << ccc->size() << std::endl;
	  if (ccc && ccc->size()>0) { 
	    CaloClusterCellLink* cccl=new CaloClusterCellLink(ccc);
	    cluster->addCellLink(cccl);
	    for (;cit!=cit_e;++cit) {
	      cluster->addCell(cit.getElement().index(),cit.getParameter());
	      //std::cout << cit.getParameter() << "/" << cit.getElement().index() << std::endl;
	    }//end loop over cells of a cluster
	  cluster->setLink(ccclc); //Move cell links to external cell-link container
	  } //end cell container ptr not NULL
	  else
	    msg(MSG::INFO) << "Got NULL ptr to CaloCellContainer" << endreq;
	}//end check cluster not-empty
	else
	  ATH_MSG_DEBUG("Found cluster with empty list of constituents");
      }//end if isCellLink valid
      else
	ATH_MSG_DEBUG("Found cluster without valid cell link");



      // Cache signal state:
      const  P4SignalState::State oldState=(*itr)->signalState();  
      //Set basic quantities:
      cluster->setSamplingPattern((*itr)->m_samplingPattern);
      cluster->setClusterSize((xAOD::CaloCluster::ClusterSize)(*itr)->getClusterSize());
      cluster->setEta0((double)(*itr)->eta0() );
      cluster->setPhi0((double)(*itr)->phi0() );
      cluster->setTime((*itr)->getTime() );
    
      (*itr)->setSignalState( P4SignalState::CALIBRATED);
      cluster->setCalE((*itr)->e());
      cluster->setCalEta((*itr)->eta());
      cluster->setCalPhi((*itr)->phi());	           	 
      cluster->setCalM((*itr)->m());
      
      (*itr)->setSignalState( P4SignalState::UNCALIBRATED);
      cluster->setRawE((*itr)->e());
      cluster->setRawEta((*itr)->eta());
      cluster->setRawPhi((*itr)->phi());	           	 
      cluster->setRawM((*itr)->m());
    
      (*itr)->setSignalState( P4SignalState::ALTCALIBRATED);
      cluster->setAltE((*itr)->e() );	           		 
      cluster->setAltEta((*itr)->eta());
      cluster->setAltPhi((*itr)->phi());	           	 
      cluster->setAltM((*itr)->m());  
    
      //reset signal state:
      (*itr)->setSignalState( oldState );
    
     

      for (unsigned iSamp=0;iSamp<CaloSampling::Unknown;++iSamp) {
	xAOD::CaloCluster_v1::CaloSample sNew=(xAOD::CaloCluster_v1::CaloSample)iSamp;
	CaloSampling:: CaloSample sOld=( CaloSampling:: CaloSample)iSamp;
	if ((*itr)->hasSampling(sOld)) {
	  cluster->setEta(sNew,(*itr)->getVariable(CaloVariableType::ETA,sOld,true));
	  cluster->setPhi(sNew,(*itr)->getVariable(CaloVariableType::PHI,sOld,true));
	  cluster->setEnergy(sNew,(*itr)->getVariable(CaloVariableType::ENERGY,sOld,true));
	}
      }

      // Copy the moments:
      CaloCluster::moment_iterator mom_itr = ( *itr )->beginMoment();
      CaloCluster::moment_iterator mom_end = ( *itr )->endMoment();
      for( ; mom_itr != mom_end; ++mom_itr ) {
	// Forcibly convert between the two types:
	const CaloCluster::moment_type aod_type = mom_itr.getMomentType();
	const xAOD::CaloCluster_v1::MomentType xaod_type =
	  static_cast< xAOD::CaloCluster_v1::MomentType >( aod_type );
	// Insert the moment:
	cluster->insertMoment( xaod_type,
			       mom_itr.getMoment().getValue() );
	ATH_MSG_VERBOSE( "Copied moment " << aod_type );
      }
    
      // const  SG::auxid_set_t& auxIds=aux->getDynamicAuxIDs();
      // for (auto ai: auxIds) {
      //   std::cout << "Auxid=" << ai << std::endl;
      // }
      //cluster->getAllMoments();
      ATH_MSG_VERBOSE( "Number of dynamic auxiliary variables: "
		     << aux->getDynamicAuxIDs().size() );
    }//end loop over clusters
  }// end loop over cluster containers

  // Return gracefully:
  return StatusCode::SUCCESS;
}

