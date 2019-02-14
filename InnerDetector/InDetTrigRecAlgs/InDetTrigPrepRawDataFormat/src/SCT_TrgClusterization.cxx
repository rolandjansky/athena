/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

//***************************************************************************
// 
// Implementation for SCT_TrgClusterization.
// (see class definition)
//
//****************************************************************************

#include "InDetTrigPrepRawDataFormat/SCT_TrgClusterization.h"
#include "InDetPrepRawData/SiClusterContainer.h"
#include "SiClusterizationTool/ISCT_ClusteringTool.h"
#include "InDetTrigToolInterfaces/ITrigRawDataProviderTool.h"


#include "InDetIdentifier/PixelID.h"
#include "InDetIdentifier/SCT_ID.h"

#include "InDetRawData/SCT_RDORawData.h"
#include "InDetRawData/InDetRawDataCLASS_DEF.h"

#include "InDetReadoutGeometry/PixelDetectorManager.h"
#include "Identifier/Identifier.h"
#include "AtlasDetDescr/AtlasDetectorID.h"    


//Gaudi includes
#include "AthenaKernel/Timeout.h"

//Trigger
#include "TrigSteeringEvent/TrigRoiDescriptor.h"
#include "IRegionSelector/IRegSelSvc.h"
#include "IRegionSelector/IRoiDescriptor.h"
#include "TrigTimeAlgs/TrigTimerSvc.h"

#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"

namespace InDet{

  using namespace InDet;
  static const std::string maxRDOsReached("SCT_TrgClusterization: Exceeds max RDOs");

  //----------------------------------  
  //           Constructor:
  //----------------------------------------------------------------------------
  SCT_TrgClusterization::SCT_TrgClusterization(const std::string &name, 
					       ISvcLocator *pSvcLocator) :
    //----------------------------------------------------------------------------
    HLT::FexAlgo(name, pSvcLocator),
    m_rawDataProvider("InDet::TrigSCTRawDataProvider"),
    m_clusteringTool("InDet::SCT_ClusteringTool"),
    m_clustersName("SCT_TrigClusters"),
    m_flaggedCondDataName("SCT_FlaggedCondData_TRIG"),
    m_idHelper(0),
    m_clusterContainer(nullptr),
    m_regionSelector("RegSelSvc", name),
    m_doFullScan(false),
    m_etaHalfWidth(0.1),
    m_phiHalfWidth(0.1),
    m_sctRDOContainerName("SCT_RDOs"),
    m_robDataProvider("ROBDataProviderSvc", name),
    m_checkBadModules(true),
    m_maxRDOs(0),
    m_doTimeOutChecks(true),
    m_timerSGate(0),
    m_timerCluster(0),
    m_timerRegSel(0),
    m_timerDecoder(0)
  {  
    // Get parameter values from jobOptions file
    declareProperty("SCT_RDOContainerName",m_sctRDOContainerName);
    declareProperty("clusteringTool",      m_clusteringTool);
    declareProperty("ClustersName",        m_clustersName);
    declareProperty("FlaggedCondDataName", m_flaggedCondDataName);
    declareProperty("RegionSelectorTool",  m_regionSelector );
    declareProperty("doFullScan",          m_doFullScan );

    declareProperty("EtaHalfWidth",        m_etaHalfWidth);
    declareProperty("PhiHalfWidth",        m_phiHalfWidth);
    declareProperty("RawDataProvider",     m_rawDataProvider);

    declareProperty("checkBadModules",      m_checkBadModules);
    declareProperty("maxRDOs",              m_maxRDOs);
    declareProperty("doTimeOutChecks",      m_doTimeOutChecks);

    declareMonitoredVariable("numSctClusters", m_numSctClusters    );
    declareMonitoredVariable("numSctIds", m_numSctIds    );
    declareMonitoredStdContainer("SctHashId", m_ClusHashId);
    declareMonitoredStdContainer("SctBSErr",  m_SctBSErr);
    declareMonitoredStdContainer("SctOccupancyHashId",  m_occupancyHashId);
    

    m_clusterCollection = NULL;

    // error strategy
    //
    // 0 : broad errors (cluster width/sqrt(12) )
    //     as in old clustering code (release 6 and 7)
    // 1 : narrow errors (strip pitch/sqrt(12.) )
    //     DEFAULT - should be more accurate, and still conservative
    //declareProperty("ErrorStrategy",m_errorStrategy);
  }


  //----------------------------------  
  //          Initialize method:
  //----------------------------------------------------------------------------
  HLT::ErrorCode SCT_TrgClusterization::hltInitialize() {
    //----------------------------------------------------------------------------
 
    ATH_MSG_DEBUG( "SCT_TrgClusterization::initialize()" );
    
    
    // get the clusteringTool
    if ( m_clusteringTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL( m_clusteringTool.propertyName() << ": Failed to retrieve tool " << m_clusteringTool.type() );
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
    else {
      ATH_MSG_INFO( m_clusteringTool.propertyName() << ": Retrieved tool " << m_clusteringTool.type() );
    }
    
    // Retrieving Region Selector Tool:
    if ( m_regionSelector.retrieve().isFailure() ) {
      ATH_MSG_FATAL( m_regionSelector.propertyName()
		     << " : Unable to retrieve RegionSelector tool "  
		     << m_regionSelector.type() );
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
    
    const SCT_ID * IdHelper(0);
    if (detStore()->retrieve(IdHelper, "SCT_ID").isFailure()) {
      ATH_MSG_FATAL( "Could not get SCT ID helper" );
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
    m_idHelper = IdHelper;
    

    // register the IdentifiableContainer into StoreGate
    // ------------------------------------------------------
    if(!store()->transientContains<SCT_ClusterContainer>(m_clustersName)){
      
      // declare the container, starting the keys after the maximum pixel key
      m_clusterContainer = new SCT_ClusterContainer(m_idHelper->wafer_hash_max());

      if (store()->record(m_clusterContainer, m_clustersName).isFailure()) {
	ATH_MSG_WARNING( " Container " << m_clustersName
			 << " could not be recorded in StoreGate !" );
      } 
      else {
	ATH_MSG_INFO( "Container " << m_clustersName 
		      << " registered  in StoreGate" );  
      }
    }
    else {    
      StatusCode sc = store()->retrieve(m_clusterContainer, m_clustersName);

      if (sc.isFailure()) {
	ATH_MSG_ERROR( "Failed to get Cluster Container" );
	return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
      }
      else { 
	ATH_MSG_INFO( "Got Cluster Container from TDS: " << m_clustersName );
      }
    }
    m_clusterContainer->addRef();

    //symlink the collection
    const SiClusterContainer* symSiContainer(0);
    StatusCode sc = store()->symLink(m_clusterContainer, symSiContainer);
    if (sc.isFailure()) {
      ATH_MSG_WARNING( "SCT clusters could not be symlinked in StoreGate !" );
    } 
    else{
      ATH_MSG_DEBUG( "SCT clusters '" << m_clustersName 
		     << "' symlinked in StoreGate" );
    } 

    //retrieve rob data provider service
    if (m_robDataProvider.retrieve().isFailure()) {
      ATH_MSG_FATAL( "Failed to retrieve " << m_robDataProvider );
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    } else
      ATH_MSG_INFO( "Retrieved service " << m_robDataProvider << " in SCT_trgClusterization. " );

    //decoding tool
    if (m_rawDataProvider.retrieve().isFailure()){
      ATH_MSG_ERROR( "Raw data provider not available" );
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }

    if (m_SCTDetEleCollKey.initialize().isFailure()) {
      ATH_MSG_ERROR( m_SCTDetEleCollKey.fullKey() << " not available" );
      return HLT::ErrorCode(HLT::Action::ABORT_JOB, HLT::Reason::BAD_JOB_SETUP);
    }
 
    if (m_checkBadModules){
      if (m_pSummaryTool.retrieve().isFailure()){
	ATH_MSG_ERROR( "Could not retrieve " << m_pSummaryTool );
      } else {
	ATH_MSG_INFO( "Using " << m_pSummaryTool << " in clusterization" );
      }
    } else {
      m_pSummaryTool.disable();
    }

    ATH_MSG_INFO( "SCT_TrgClusterization configured with "
                  << "PhiHalfWidth: " << m_phiHalfWidth << " EtaHalfWidth: "<< m_etaHalfWidth );
    if (m_doFullScan) ATH_MSG_INFO( "FullScan mode" );
    ATH_MSG_INFO( "will be driven by RoI objects" );
    
    m_timerSGate   = addTimer("SGate");
    m_timerCluster  = addTimer("Cluster");
    m_timerRegSel = addTimer("RegSel");
    m_timerDecoder    = addTimer("Decoder");

    return HLT::OK;
  }
  
  //--------------------------------
  //         Execute method:
  //----------------------------------------------------------------------------
  HLT::ErrorCode SCT_TrgClusterization::hltExecute(const HLT::TriggerElement*, 
						   HLT::TriggerElement* outputTE) {
    //----------------------------------------------------------------------------


    ATH_MSG_DEBUG( "SCT_TrgClusterization::hltExecute()" );

    //initialisation of monitored quantities
    m_numSctIds = 0;
    m_numSctClusters = 0;
    m_clusterCollection = NULL;
    m_listOfSctIds.clear();
    m_ClusHashId.clear();
    m_SctBSErr.clear();
    m_occupancyHashId.clear();

    StatusCode sc;

    if(doTiming()) m_timerSGate->resume();

    // register the IdentifiableContainer into StoreGate
    // ------------------------------------------------------
    if(!store()->transientContains<SCT_ClusterContainer>(m_clustersName)){
      
      // The cluster container is cleant up only at the begining of the event.
      m_clusterContainer->cleanup();
      
      if (store()->record(m_clusterContainer,
                          m_clustersName, false).isFailure()) {
        ATH_MSG_WARNING( " Container " << m_clustersName
			 << " could not be recorded in StoreGate !" );
      } 
      else {
	ATH_MSG_DEBUG( "REGTEST: Container " << m_clustersName 
		       << " registered  in StoreGate" );  
      }
      
      // ------------------------------------------------------
    }
    else {    
      ATH_MSG_DEBUG( "Container '" << m_clustersName 
		     << "' existed already  in StoreGate. " );
    }
    
    // Prepare SCT_FlaggedCondData
    SCT_FlaggedCondData* flaggedCondData{nullptr};
    if (!store()->transientContains<SCT_FlaggedCondData>(m_flaggedCondDataName)) {
      flaggedCondData = new SCT_FlaggedCondData{};

      if (store()->record(flaggedCondData, m_flaggedCondDataName, true, true).isFailure()) {
        ATH_MSG_WARNING(" Container " << m_flaggedCondDataName << " could not be recorded in StoreGate !");
      }
      else {
	ATH_MSG_DEBUG( "REGTEST: Container " << m_flaggedCondDataName << " registered  in StoreGate" );
      }
    }
    else {
      ATH_MSG_DEBUG( "Container '" << m_flaggedCondDataName << "' existed already  in StoreGate. " );
      if (!store()->retrieve(flaggedCondData, m_flaggedCondDataName)) {
        ATH_MSG_WARNING(m_flaggedCondDataName <<" could not be retrieved from StoreGate !" );
        return HLT::OK;
      }
    }

    if(doTiming()) m_timerSGate->pause();

    //---------------------------
    // Trigger specific part:
    //---------------------------
    //   Get from the Trigger Element the RoI
    //   Ask the Region Selector for the list of hash indexes for this ROI.
    //   Transform those indexes into a RDO collection.
    //-------------------------------------------------------------------------

    //handling of decoding problems
    StatusCode scdec = StatusCode::SUCCESS;


    // Get RoiDescriptor
    const TrigRoiDescriptor* roi;
    if ( HLT::OK != getFeature(outputTE, roi) ) {
      ATH_MSG_WARNING( "Can't get RoI" );
      return HLT::NAV_ERROR;
    }
    
    if (!roi){
      ATH_MSG_WARNING( "Received NULL RoI" );
      return HLT::NAV_ERROR;
    }
    
    double RoiEtaWidth = roi->etaPlus() - roi->etaMinus();
    double RoiPhiWidth = roi->phiPlus() - roi->phiMinus();
    const double M_2PI = 2*M_PI;
    const float  M_PIF = float(M_PI);
    while ( RoiPhiWidth> M_PIF ) RoiPhiWidth -= M_2PI;
    while ( RoiPhiWidth<-M_PIF ) RoiPhiWidth += M_2PI;
    if ( RoiPhiWidth> M_PI ) RoiPhiWidth -= 1e-7;
    if ( RoiPhiWidth<-M_PI ) RoiPhiWidth += 1e-7;

    if (!roi->isFullscan()){
      if( fabs(RoiEtaWidth/2. - m_etaHalfWidth) > 0.02) {
	ATH_MSG_DEBUG( "ROI range is different from configuration: " );
	ATH_MSG_DEBUG( "eta width = " << RoiEtaWidth << "; with etaPlus = " << roi->etaPlus() << "; etaMinus = " << roi->etaMinus() );
	ATH_MSG_DEBUG( "etaHalfWidth from config: " << m_etaHalfWidth );
      }
      
    } 
    else {
      if (m_etaHalfWidth<2.5 || m_phiHalfWidth<3.) {
	ATH_MSG_WARNING( "FullScan RoI and etaHalfWidth from config: " << m_etaHalfWidth );
	ATH_MSG_WARNING( "FullScan RoI and phiHalfWidth from config: " << m_phiHalfWidth );
      }
    }


    ATH_MSG_DEBUG( "REGTEST:" << *roi );

      
    //   Get the SCT RDO's:
    //     - First get the SCT ID's using the RegionSelector
    //     - Retrieve from SG the RDOContainer: 
    //       Identifiable Container that contains pointers to all the RDO 
    //        collections (one collection per detector)
    //     - Retrieve from StoreGate the RDO collections.
    //       (the ByteStreamConvertors are called here).

    if(doTiming()) m_timerDecoder->start();
    scdec = m_rawDataProvider->decode(roi);
    if(doTiming()) m_timerDecoder->stop();

    if (scdec.isSuccess()){
      //check for recoverable errors

    } else {
      ATH_MSG_DEBUG( " m_rawDataProvider->decode failed" );
    }


    if (!(roi->isFullscan())){
      if(doTiming()) m_timerRegSel->start();
      m_regionSelector->DetHashIDList(SCT, *roi, m_listOfSctIds );
      if(doTiming()) m_timerRegSel->stop();
      
      m_numSctIds = m_listOfSctIds.size();
      
      

      ATH_MSG_DEBUG( "REGTEST: SCT : Roi contains " 
		     << m_numSctIds << " det. Elements" );
    }


    if(doTiming()) m_timerSGate->resume();

    const SCT_RDO_Container* p_sctRDOContainer;
    sc = store()->retrieve(p_sctRDOContainer,  m_sctRDOContainerName);

    if (sc.isFailure() ) {
      ATH_MSG_WARNING( "Could not find the SCT_RDO_Container " 
		       << m_sctRDOContainerName );

      // Activate the TriggerElement anyhow.
      // (FEX algorithms should not cut and it could be that in the Pixel/TRT 
      // tracks can anyhow be reconstructed) 
      return HLT::OK;
    } 
    else {
      ATH_MSG_DEBUG( " Found the SCT_RDO_Container " 
		     << m_sctRDOContainerName );
    }

    if(doTiming()) m_timerSGate->pause();
    
    if(!(roi->isFullscan())){

      if(doTiming()) m_timerCluster->resume();

      for (unsigned int i=0; i<m_listOfSctIds.size(); i++) {

	SCT_RDO_Container::const_iterator 
	  RDO_collection_iter = p_sctRDOContainer->indexFind(m_listOfSctIds[i]); 
	
	if (RDO_collection_iter == p_sctRDOContainer->end()) continue;

	if (m_doTimeOutChecks && Athena::Timeout::instance().reached() ) {
	  ATH_MSG_WARNING( "Timeout reached. Aborting sequence." );
	  return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::TIMEOUT);
	}

    
	const InDetRawDataCollection<SCT_RDORawData>* RDO_Collection (*RDO_collection_iter);
               
	if (!RDO_Collection) continue;

	//optionally check for bad modules
	bool goodModule=true;

	if (m_checkBadModules && m_pSummaryTool){
	  goodModule = m_pSummaryTool->isGood(RDO_Collection->identifyHash());
	  if (!goodModule)
	    ATH_MSG_DEBUG( "Module not good: " << RDO_Collection->identifyHash() );
	}
	
	
	const size_t rdosize = RDO_Collection->size();
	if (m_maxRDOs >0 && rdosize>m_maxRDOs){
          const int hid = RDO_Collection->identifyHash();
          flaggedCondData->insert(std::make_pair(hid, maxRDOsReached));
          m_flaggedModules.insert(hid);
          m_occupancyHashId.push_back(hid);
	  continue;
	}
	
	if (rdosize>0 && goodModule) {
	  // Use one of the specific clustering AlgTools to make clusters
	  
	  m_clusterCollection =  m_clusteringTool->clusterize(*RDO_Collection,
							      *m_idHelper);
	  
	  // -me- fix test
	  if (m_clusterCollection->size()!=0) {
	    
	    m_numSctClusters+= m_clusterCollection->size();
	    sc = m_clusterContainer->addCollection(m_clusterCollection,
						   m_clusterCollection->identifyHash());
	    m_ClusHashId.push_back(m_clusterCollection->identifyHash());
	    
	    if (!sc.isSuccess()) {
	      ATH_MSG_VERBOSE( "Failed to add Cluster collection : "
			       << m_clusterCollection->identifyHash() );
	      delete m_clusterCollection;
	    }
	  }
	  else{
 
	    delete m_clusterCollection;
	    ATH_MSG_DEBUG( "Clustering algorithm found no clusters" );
	  
	  }
	}
      }
      if(doTiming()) m_timerCluster->pause();
    }

    else{   //fullScan

      //p_sctRDOContainer->clID(); 	// anything to dereference the DataHandle
      // will trigger the converter

      SCT_RDO_Container::const_iterator rdoCollections    = p_sctRDOContainer->begin();
      SCT_RDO_Container::const_iterator rdoCollectionsEnd = p_sctRDOContainer->end();
      
      AtlasDetectorID detType;
      
      if(doTiming()) m_timerCluster->resume();

      for(;rdoCollections!=rdoCollectionsEnd;++rdoCollections){
	const InDetRawDataCollection<SCT_RDORawData>* rd(*rdoCollections);
	
	if (m_doTimeOutChecks && Athena::Timeout::instance().reached() ) {
	  ATH_MSG_WARNING( "Timeout reached. Aborting sequence." );
	  return HLT::ErrorCode(HLT::Action::ABORT_CHAIN, HLT::Reason::TIMEOUT);
	}
	
	const size_t rdosize = rd->size();

	// if (!rd) continue;
	ATH_MSG_VERBOSE( "RDO collection size=" 
			 << rdosize 
			 << ", ID=" << rd->identify() );
        
	//optionally check for bad modules
	bool goodModule=true;
	if (m_checkBadModules && m_pSummaryTool){
	  goodModule = m_pSummaryTool->isGood(rd->identifyHash());
	  if (!goodModule)
	    ATH_MSG_DEBUG( "Module not good: " << rd->identifyHash() );
	}
	
	
	if (m_maxRDOs >0 && rdosize>m_maxRDOs){
          const int hid = rd->identifyHash();
          flaggedCondData->insert(std::make_pair(hid, maxRDOsReached));
          m_flaggedModules.insert(hid);
          m_occupancyHashId.push_back(hid);
	  continue;
	}

	if (rdosize>0 && goodModule){
	  // Use one of the specific clustering AlgTools to make clusters
	  m_clusterCollection = m_clusteringTool->clusterize(*rd,
							     *m_idHelper);


	  if (m_clusterCollection) {          
	    if (m_clusterCollection->size() != 0) {
	      // -me- new IDC does no longer register in Storegate if hash is used !
	      //sc = m_clusterContainer->addCollection(clusterCollection, clusterCollection->identify());
	      m_numSctClusters+= m_clusterCollection->size();
	      sc = m_clusterContainer->addCollection(m_clusterCollection, 
						     m_clusterCollection->identifyHash());
	      m_ClusHashId.push_back(m_clusterCollection->identifyHash());
	      
	      if (sc.isFailure()){
		ATH_MSG_VERBOSE( "Clusters could not be added to container !" );
		delete m_clusterCollection;
	      } 
	      else{
		ATH_MSG_VERBOSE( "Clusters with key '" << m_clusterCollection->identify() 
				 << "' added to Container" );
	      } 
	    } 
	    else{
	      ATH_MSG_DEBUG( "Don't write empty collections" );
	      // -me- clean up memory
	      delete m_clusterCollection;
	    }
	  }
	  else{
	    ATH_MSG_DEBUG( "Clustering algorithm found no clusters" );
	  }
	}
      }
      if(doTiming()) m_timerCluster->pause();
    }
    
    if( doTiming() ){
      m_timerSGate->stop();
      m_timerCluster->stop();
    }

    ATH_MSG_DEBUG( "REGTEST: Number of reconstructed clusters = " << m_numSctClusters );

    return HLT::OK;
  }

  //-----------------------------------
  //          Finalize method:
  //----------------------------------------------------------------------------
  HLT::ErrorCode SCT_TrgClusterization::hltFinalize() 
  //----------------------------------------------------------------------------
  {
    // Get the messaging service, print where you are
    ATH_MSG_INFO( "SCT_TrgClusterization::hltFinalize()" );

    m_clusterContainer->cleanup();

    //delete m_globalPosAlg;  
    //delete m_clusterContainer;
    
    //igb -fix me- it crashes
    m_clusterContainer->release();

    //printout statistics on flagged modules
    if (m_maxRDOs>0){
      ATH_MSG_INFO( "REGTEST: " << m_flaggedModules.size() << " modules flagged as noisy" );
      std::set<IdentifierHash>::const_iterator itr(m_flaggedModules.begin());
      std::set<IdentifierHash>::const_iterator end(m_flaggedModules.end());
      for (int num(0); (itr != end) && (num < 10) ; ++itr, ++num) {
        ATH_MSG_INFO( "Noisy: "
		      << m_idHelper->print_to_string(m_idHelper->wafer_id(*itr)));
      }

    }
    return HLT::OK;
  }

  //-------------------------------------------------------------------------
  HLT::ErrorCode SCT_TrgClusterization::prepareRobRequests(const HLT::TriggerElement* inputTE ) {

    ATH_MSG_DEBUG( "Running prepareRobRequests in SCT_trgClusterization. " );
 
    // Calculate ROBs needed  - this code should be shared with hltExecute to avoid slightly different requests
    const TrigRoiDescriptor* roi = 0;

    if (getFeature(inputTE, roi) != HLT::OK || roi == 0) {
      ATH_MSG_WARNING( "REGTEST / Failed to find RoiDescriptor " );
      return HLT::NAV_ERROR;
    }
 

    ATH_MSG_DEBUG( "REGTEST prepareROBs / event"
		   << *roi);

    //const TrigRoiDescriptor fs(true);

    std::vector<unsigned int> uIntListOfRobs;
    m_regionSelector->DetROBIDListUint( SCT, *roi, uIntListOfRobs );
    //m_regionSelector->DetROBIDListUint( SCT, fs, uIntListOfRobs );


    ATH_MSG_DEBUG( "list of pre-registered ROB ID in SCT: " );
      for(uint i_lid(0); i_lid<uIntListOfRobs.size(); i_lid++)
	ATH_MSG_DEBUG( uIntListOfRobs.at(i_lid) );
    

    //m_robDataProvider->addROBData( uIntListOfRobs );
    config()->robRequestInfo()->addRequestScheduledRobIDs( uIntListOfRobs );
    uIntListOfRobs.clear();

    return HLT::OK;

  }



}
//---------------------------------------------------------------------------
