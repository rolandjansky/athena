/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// MdtRdoToPrepDataTool.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "MdtRdoToPrepDataTool.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/PropertyMgr.h"

#include "StoreGate/StoreGateSvc.h"

#include "MuonIdHelpers/MdtIdHelper.h"
#include "MuonIdHelpers/MuonIdHelperTool.h"
#include "MuonReadoutGeometry/MuonStation.h"
#include "MuonReadoutGeometry/MdtReadoutElement.h"
#include "MuonRDO/MdtCsmContainer.h"
#include "MdtRDO_Decoder.h"

#include "MuonCalibEvent/MdtCalibHit.h"
#include "MdtCalibSvc/MdtCalibrationSvc.h"
#include "MdtCalibSvc/MdtCalibrationSvcSettings.h"
#include "MdtCalibSvc/MdtCalibrationSvcInput.h"

#include "MuonMDT_Cabling/MuonMDT_CablingSvc.h"

// BS access
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
#include "MuonCnvToolInterfaces/IMuonRawDataProviderTool.h"
#include "MuonMDT_CnvTools/IMDT_RDO_Decoder.h"
#include "MuonContainerManager/MuonRdoContainerAccess.h"

#include "MuonPrepRawData/MdtTwinPrepData.h"    // TWIN TUBES

using namespace MuonGM;
using namespace Trk;
using namespace Muon;

Muon::MdtRdoToPrepDataTool::MdtRdoToPrepDataTool(const std::string& t,
              const std::string& n,
              const IInterface*  p )
  :
  AthAlgTool(t,n,p),
  m_sgSvc(0),
  m_muonMgr(0),
  m_mdtHelper(0),
  m_calibrationSvc(0),
  m_mdtCalibSvcSettings(new MdtCalibrationSvcSettings() ),
  m_calibHit( 0 ),
  m_invSpeed(1./299.792458),
  m_mdtPrepDataContainer(0),
  m_doIdentifierCorrection(false),
  m_calibratePrepData(true),
  //m_robDataProvider ("ROBDataProviderSvc",n),
  m_rawDataProviderTool("Muon::MDT_RawDataProviderTool/MDT_RawDataProviderTool"),
  m_mdtDecoder("Muon::MdtRDO_Decoder/MdtRDO_Decoder"),
  m_idHelper("Muon::MuonIdHelperTool/MuonIdHelperTool"),
  m_fullEventDone(false),
  m_isTestBeam(false),
  m_BMEpresent(false),
  m_BMGpresent(false),
  m_BMEid(-1),
  m_BMGid(-1)
{
  declareInterface<Muon::IMuonRdoToPrepDataTool>(this);

  // tools
  declareProperty ("RawDataProviderTool",      m_rawDataProviderTool);
  
  //  template for property decalration
  declareProperty("DoRomeBOGCorrection", m_doIdentifierCorrection);
  declareProperty("OutputCollection",    m_outputCollectionLocation = "MDT_DriftCircles" );
  declareProperty("CalibratePrepData",   m_calibratePrepData = true );
  declareProperty("DecodeData",          m_decodeData = true ); 
  
  declareProperty("useBStoRdoTool",      m_useBStoRdoTool=false );
  declareProperty("TestBeam",            m_isTestBeam=false );
  
  // + TWIN TUBE
  declareProperty("UseTwin",                 m_useTwin = true);
  declareProperty("UseAllBOLTwin",           m_useAllBOLTwin = false);        
  declareProperty("Use1DPrepDataTwin",       m_use1DPrepDataTwin = false);
  declareProperty("TwinCorrectSlewing",      m_twinCorrectSlewing = false);
  declareProperty("DiscardSecondaryHitTwin", m_discardSecondaryHitTwin = false);
  // - TWIN TUBE
  declareProperty("TimeWindowLowerBound",   m_mdtCalibSvcSettings->windowLowerBound );
  declareProperty("TimeWindowUpperBound",   m_mdtCalibSvcSettings->windowUpperBound );
  declareProperty("TimeWindowSetting",      m_mdtCalibSvcSettings->windowSetting = 2 );
  declareProperty("DoTofCorrection",        m_mdtCalibSvcSettings->doTof  = true );
  declareProperty("DoPropagationCorrection",m_mdtCalibSvcSettings->doProp = false );
}


Muon::MdtRdoToPrepDataTool::~MdtRdoToPrepDataTool()
{

}

StatusCode Muon::MdtRdoToPrepDataTool::initialize()
{  
  ATH_MSG_DEBUG(" in initialize()");

 
  if (StatusCode::SUCCESS != serviceLocator()->service("MuonMDT_CablingSvc", m_mdtCabling)) {
    ATH_MSG_ERROR(" Can't get MuonMDT_CablingSvc ");
    return StatusCode::FAILURE;
  }

  /// Store Gate active store
  StatusCode sc = serviceLocator()->service("StoreGateSvc", m_sgSvc);
  if (sc != StatusCode::SUCCESS ) {
    ATH_MSG_ERROR(" Cannot get StoreGateSvc ");
    return sc ;
  }
  
  /// get the detector descriptor manager
  StoreGateSvc* detStore=0;
  sc = serviceLocator()->service("DetectorStore", detStore);
  
  if (sc.isSuccess()) {
    sc = detStore->retrieve( m_muonMgr );
    if (sc.isFailure()) {
      ATH_MSG_FATAL(" Cannot retrieve MuonReadoutGeometry ");
      return sc;
    }
  } else {
    ATH_MSG_ERROR("DetectorStore not found ");
    return sc;
  }
  
  // get MDT Calibration service
  sc = serviceLocator()->service("MdtCalibrationSvc", m_calibrationSvc);
  if (!sc.isSuccess() || 0 == m_calibrationSvc) {
    ATH_MSG_ERROR(" Could not initialize MDT Calibration service Service");
    return sc;
  }
  
  //   // Get ROBDataProviderSvc
  //   if (m_robDataProvider.retrieve().isFailure()) {
  //      *m_log << MSG::FATAL << "Failed to retrieve serive " << m_robDataProvider << endreq;
  //     return StatusCode::FAILURE;
  //   } else
  //      *m_log << MSG::INFO << "Retrieved service " << m_robDataProvider << endreq;
  
  // Get MdtRawDataProviderTool
  if (m_rawDataProviderTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve tool ");
    return StatusCode::FAILURE;
  } else
    ATH_MSG_INFO("Retrieved Tool ");
  
  
  /// create an empty MDT PrepData container for filling
  m_mdtHelper = m_muonMgr->mdtIdHelper();
  try {
    m_mdtPrepDataContainer = new MdtPrepDataContainer(m_mdtHelper->module_hash_max());
  } catch(std::bad_alloc) {
    ATH_MSG_FATAL("Could not create a new MDT PrepRawData container!");
    return StatusCode::FAILURE;
  }
  
  // Retrieve the RDO decoder 
  if (m_mdtDecoder.retrieve().isFailure()) {
    ATH_MSG_FATAL("Failed to retrieve Mdt decoder tool ");
    return StatusCode::FAILURE;
  } else
    ATH_MSG_INFO("Retrieved Mdt Decoder Tool ");
  
  ATH_CHECK(m_idHelper.retrieve());
  // old way before the tool migration
//  m_mdtDecoder = new MdtRDO_Decoder;
//  ATH_MSG_VERBOSE(" MdtRDO_Decoder created");
//  m_mdtDecoder->set(m_mdtHelper, m_mdtCabling);
  
  m_mdtPrepDataContainer->addRef();
  
  if (m_doIdentifierCorrection)
    ATH_MSG_WARNING("Identifier correction turned on! This will shift the Identifiers of BOG MDT tubes to correct for a bug in simulation. This is ONLY needed for files produced before the fix, i.e. produced before release 12.0.0 - using it on more recent files will be a bug in itself.");
  
  // + TWIN TUBES
  // make an array of [multilayer][layer][twin-pair]; 2 multilayers, 3 layer per multilayer, 36 twin-pairs per layer 
  // fill twin_chamber array with unique numbers  
  if(m_useTwin){
    for(int i=0; i<2; i++){
      for(int j=0; j<3; j++){
	for(int k=0; k<36; k++){
	  twin_chamber[i][j][k] = 1000*i + 100*j + k;
	}
      }
    }
    // for secondary hits we need to make a second array with unique numbers 
    // (i+1 is used in the expression, so numbers are always different from twin_chamber array)
    for(int i=0; i<2; i++){
      for(int j=0; j<3; j++){
	for(int k=0; k<36; k++){
	  secondaryHit_twin_chamber[i][j][k] = 10000*(i+1) + 100*j + k;
	}
      }
    }
  }// end if(m_useTwin){
  // - TWIN TUBES

  m_calibHit = new MdtCalibHit();
  m_mdtCalibSvcSettings->initialize();

  // check if the layout includes elevator chambers
  m_BMEpresent = m_mdtHelper->stationNameIndex("BME") != -1;
  if(m_BMEpresent){
    ATH_MSG_INFO("Processing configuration for layouts with BME chambers.");
    m_BMEid = m_mdtHelper->stationNameIndex("BME");
  }
  m_BMGpresent = m_mdtHelper->stationNameIndex("BMG") != -1;
  if(m_BMGpresent){
    ATH_MSG_INFO("Processing configuration for layouts with BMG chambers.");
    m_BMGid = m_mdtHelper->stationNameIndex("BMG");
    for(int phi=6; phi<8; phi++) { // phi sectors
      for(int eta=1; eta<4; eta++) { // eta sectors
        for(int side=-1; side<2; side+=2) { // side
          if( !m_muonMgr->getMuonStation("BMG", side*eta, phi) ) continue;
          for(int roe=1; roe<=( m_muonMgr->getMuonStation("BMG", side*eta, phi) )->nMuonReadoutElements(); roe++) { // iterate on readout elemets
            const MdtReadoutElement* mdtRE =
                  dynamic_cast<const MdtReadoutElement*> ( ( m_muonMgr->getMuonStation("BMG", side*eta, phi) )->getMuonReadoutElement(roe) ); // has to be an MDT
            if(mdtRE) initDeadChannels(mdtRE);
          }
        }
      }
    }
  }

  ATH_MSG_INFO("initialize() successful in " << name());
  return StatusCode::SUCCESS;
}

StatusCode Muon::MdtRdoToPrepDataTool::finalize()
{
  if (0 != m_mdtPrepDataContainer) m_mdtPrepDataContainer->release();
  delete m_calibHit;
  return StatusCode::SUCCESS;
}

StatusCode Muon::MdtRdoToPrepDataTool::decode( const std::vector<uint32_t>& robIds )
{
  const std::vector<IdentifierHash>& chamberHashInRobs = m_mdtCabling->getChamberHashVec(robIds);
  return decode(robIds,chamberHashInRobs);
}

StatusCode Muon::MdtRdoToPrepDataTool::decode( const std::vector<uint32_t>& robIds, const std::vector<IdentifierHash>& chamberHashInRobs )
{
  //left unused, needed by other decode function and further down the code.
  std::vector<IdentifierHash> idWithDataVect;
  
  if(!m_sgSvc->contains<Muon::MdtPrepDataContainer>(m_outputCollectionLocation)){
    m_mdtPrepDataContainer->cleanup();	
    
    StatusCode status = m_sgSvc->record(m_mdtPrepDataContainer,m_outputCollectionLocation);
    if (status.isFailure()) 	{
      ATH_MSG_FATAL("Could not record container of MDT PrepData Container at " << m_outputCollectionLocation);
      return status;
    }
    else 
      {
	ATH_MSG_DEBUG("MDT PrepData Container recorded in StoreGate with key " << m_outputCollectionLocation);
      }
  }
  else 
    {
      ATH_MSG_DEBUG("MDT PrepData Container is already in StoreGate ");
    }
  
  if( !m_decodeData ) {
    ATH_MSG_DEBUG("Stored empty container. Decoding MDT RDO into MDT PrepRawData is switched off");
    return StatusCode::SUCCESS;
  }
  
  if (m_useBStoRdoTool) {
    
    if (m_rawDataProviderTool->convert(robIds).isFailure())
      {
	ATH_MSG_FATAL("BS conversion into RDOs failed");
	return StatusCode::FAILURE;
      }
    ATH_MSG_DEBUG ("BS conversion into RDOs for the selected collections done !");
  }
  
  const MdtCsmContainer* rdoContainer = 0;
  ATH_MSG_DEBUG("Retrieving the MDT RDO container from the store");
  StatusCode status = m_sgSvc->retrieve( rdoContainer, "MDTCSM" );
  if (status.isFailure()) {
    ATH_MSG_WARNING("Retrieval of Mdt RDO container failed !");
    return StatusCode::SUCCESS;                                        
    }                 
  MdtCsmContainer::const_iterator rdoColli;
  const MdtCsm* rdoColl = 0;
  
  std::vector<IdentifierHash>::const_iterator it = chamberHashInRobs.begin();
  int processingDetEl = 1;
  while (it != chamberHashInRobs.end()){
    // if in prep data the chamber already exists ...
    if (m_mdtPrepDataContainer->indexFind(*it) != m_mdtPrepDataContainer->end()) {
      // and it's not the 2nd CSM (means it's the 1st and therefore a duplicate)
      if (processingDetEl != 2) { 
	++it;
	continue;
      }
    }

    //fix this need online hash  
    if (!m_BMEpresent) 
      // the iterator is the module/chamber hash
      // these hashes are also used to register the RDO
      rdoColli = rdoContainer->indexFind(*it);
    else {
      // if there are BMEs the RDOs are registered with the detectorElement hash
      Identifier tmp_id;
      IdContext tmp_context = m_mdtHelper->module_context();
      m_mdtHelper->get_id(*it, tmp_id, &tmp_context);
      // for BMEs the 2 CSMs are registered with the hashes of the 2 multilayers
      Identifier ml_id = m_mdtHelper->multilayerID(tmp_id, processingDetEl);
      IdentifierHash ml_hash;
      m_mdtHelper->get_detectorElement_hash(ml_id, ml_hash);
      rdoColli = rdoContainer->indexFind(ml_hash);
    }

    if(rdoColli == rdoContainer->end()) {
      ++it;
      continue;
    }
    
    rdoColl = *rdoColli;

    bool hasTwin = false;    

    if(m_useTwin){
      Identifier elementId = rdoColl->identify();
      // two chambers in ATLAS are installed with Twin Tubes; in detector coordinates BOL4A13 & BOL4C13; only INNER multilayer(=1) is with twin tubes
      // here no check is done for multilayer=1, because an MdtCsm(rdoColli) contains hits from both multilayers, so this check needs to be done later
      
      std::string stationName =  m_mdtHelper->stationNameString(m_mdtHelper->stationName(elementId));
      // find these two chambers in coordinates given by IdDictMuonSpectrometer
      // used version R.01 for mapping of MuonSpectrometer (19/03/2008)
      // http://atlas-sw.cern.ch/cgi-bin/viewcvs-atlas.cgi/offline/DetectorDescription/IdDictParser/data/IdDictMuonSpectrometer_R.01.xml?revision=1.4&view=markup
      
      bool isBOL = stationName == "BOL";

      // implement twin tube writing to prepData either for all BOL (m_useAllBOLTwin = true) _OR_ only for two chambers really installed   
      if( m_useAllBOLTwin && isBOL ) {
	hasTwin = true;
      }
      else if(isBOL && fabs(m_mdtHelper->stationEta(elementId)) == 4 
	      && m_mdtHelper->stationPhi(elementId) == 7){
	hasTwin = true;
      }
         
    }//ends if (m_useTwin)
    
    if( hasTwin ) {
      processCsmTwin(rdoColl, idWithDataVect).ignore();
    } 
    else{
      processCsm(rdoColl, idWithDataVect).ignore();	
    }
    
    if (m_BMEpresent) {
      Identifier elementId = rdoColl->identify();
      // if there are BMEs it's also required to process there 2nd CSM
      if (m_mdtHelper->stationName(elementId) == m_BMEid) { // is BME chamber
        // do the loop once again with the SAME iterator, but for the 2nd multilayer
	if (processingDetEl == 2) {
	  // reset to CSM/multilayer 1 and go to next chamber
	  processingDetEl = 1;
	  ++it;
	  continue;
	} else {
	  // the 1st CSM was done, do the 2nd in the same chamber
	  processingDetEl = 2;
	  continue;
	}
      }
    }

    ++it;
  }//ends loop over chamberhash
  return StatusCode::SUCCESS;
}//end decode


StatusCode Muon::MdtRdoToPrepDataTool::decode( std::vector<IdentifierHash>& idVect, std::vector<IdentifierHash>& idWithDataVect )
{
  // Conversion code goes in here
  
  int sizeVectorRequested = idVect.size();
  ATH_MSG_DEBUG("decodeMdtRDO for "<<sizeVectorRequested<<" offline collections called");
  
  // clear output vector of selected data collections containing data 
  idWithDataVect.clear();
  
  //bool firstTimeInTheEvent = false;
  if(!m_sgSvc->contains<Muon::MdtPrepDataContainer>(m_outputCollectionLocation))
    {
      //firstTimeInTheEvent = true;
      // this happens the first time in the event !
      /// clean up the PrepRawData container
      m_mdtPrepDataContainer->cleanup();
      
      /// record the container in storeGate
    StatusCode status = m_sgSvc->record(m_mdtPrepDataContainer,m_outputCollectionLocation);
    if (status.isFailure()) {
      ATH_MSG_FATAL("Could not record container of MDT PrepData Container at " << m_outputCollectionLocation);
      return status;
    }
    else ATH_MSG_DEBUG("MDT PrepData Container recorded in StoreGate with key " << m_outputCollectionLocation);
    if (sizeVectorRequested == 0) m_fullEventDone=true;
    else m_fullEventDone=false;
    }
  else 
    {
      ATH_MSG_DEBUG("MDT PrepData Container is already in StoreGate ");
      if (m_fullEventDone) 
        {
      ATH_MSG_DEBUG("Whole event has already been decoded; nothing to do");
      return StatusCode::SUCCESS;
        }
      if (sizeVectorRequested == 0) m_fullEventDone=true;
    }
  
  
  // if MDT decoding is switched off stop here
  if( !m_decodeData ) {
    ATH_MSG_DEBUG("Stored empty container. Decoding MDT RDO into MDT PrepRawData is switched off");
    return StatusCode::SUCCESS;
  }
  
  ATH_MSG_DEBUG("Decoding MDT RDO into MDT PrepRawData");
  
  
  /// MDT data collection context
    IdContext mdtContext = m_mdtHelper->module_context();

    const MdtCsmContainer* rdoContainer = 0;

    if (m_useBStoRdoTool) {
      // ask MdtRawDataProviderTool to decode the list of robs and to fill the rdo IDC
      if (sizeVectorRequested == 0)
    {
      if (m_rawDataProviderTool->convert().isFailure())
        {
          ATH_MSG_FATAL("BS conversion into RDOs failed");
          return StatusCode::FAILURE;
        }
      ATH_MSG_DEBUG("BS conversion into RDOs for the entire event done !");
    }
      else
    {
      if (m_rawDataProviderTool->convert(idVect).isFailure())
        {
          ATH_MSG_FATAL("BS conversion into RDOs failed");
          return StatusCode::FAILURE;
        }
      ATH_MSG_DEBUG ("BS conversion into RDOs for the selected collections done !");
    }
    }


    //  // retrieve the collection of RDO
    //  const MdtCsmContainer* rdoContainer = Muon::MuonRdoContainerAccess::retrieveMdtCsm("MDTCSM");
    //  if(rdoContainer!=0)
    //    {
    //      ATH_MSG_DEBUG("Running on bytestream input and MDTCSM managed by MuonRdoContainerManager");
    //      ATH_MSG_DEBUG("=> Cannot know if the RDO is already produced (must run RobDataProvider + Cablings + RawDataProviderTool)");
    //      
    //    }
    //  else
    //    {
    //      // we come here if the rdo container is already in SG (for example in MC RDO!) => the ContainerManager return NULL
    //  ATH_MSG_DEBUG("Mdt CSM container not registered by "
    //  << "MuonRdoContainerManager; retrieving it " 
    //  << "from the store! ");             

    ATH_MSG_DEBUG("Retrieving the MDT RDO container from the store");
    StatusCode status = m_sgSvc->retrieve( rdoContainer, "MDTCSM" );
    if (status.isFailure()) {
      ATH_MSG_WARNING("Retrieval of Mdt RDO container failed !");
      return StatusCode::SUCCESS;                                        
    }                                                                  
  
    ///////////// here the RDO container is retrieved and filled -whatever input type we start with- => check the size 
      MdtCsmContainer::const_iterator rdoColli;
      if (rdoContainer->begin() == rdoContainer->end()) 
    {
      // empty csm container - no mdt rdo in this event
      ATH_MSG_DEBUG("Empty csm container - no mdt rdo in this event ");
      return StatusCode::SUCCESS;
    }
      ATH_MSG_DEBUG("Not empty csm container in this event ");
  
  
      const MdtCsm* rdoColl;
      int mycounter = 0;
  
      if (sizeVectorRequested != 0) 
    {
      // seeded mode
      std::vector<IdentifierHash>::const_iterator it = idVect.begin();
      int processingDetEl = 1;
      while (it != idVect.end())
        {
          mycounter++;
          // if in prep data the chamber already exists ...
          if (m_mdtPrepDataContainer->indexFind(*it) != m_mdtPrepDataContainer->end()) 
        {
          // and it's not the 2nd CSM (means it's the 1st and therefore a duplicate)
          if (processingDetEl != 2) {
            idWithDataVect.push_back(*it);
            ++it;
            continue;
          }
        }
      
          ATH_MSG_VERBOSE("In the loop - el.# "<<mycounter);
        if (!m_BMEpresent) 
          // the iterator is the module/chamber hash
          // these hashes are also used to register the RDO
          rdoColli = rdoContainer->indexFind(*it);
        else {
          // if there are BMEs the RDOs are registered with the detectorElement hash
          Identifier tmp_id;
          IdContext tmp_context = m_mdtHelper->module_context();
          m_mdtHelper->get_id(*it, tmp_id, &tmp_context);
          // for BMEs the 2 CSMs are registered with the hashes of the 2 multilayers
          Identifier ml_id = m_mdtHelper->multilayerID(tmp_id, processingDetEl);
          IdentifierHash ml_hash;
          m_mdtHelper->get_detectorElement_hash(ml_id, ml_hash);
          rdoColli = rdoContainer->indexFind(ml_hash);
        }
          
          if(rdoColli == rdoContainer->end()) 
        {
          ATH_MSG_VERBOSE("No data found for Csm hash "<<(int)(*it));
          ++it;
          continue;
        }
          rdoColl = *rdoColli;
          ATH_MSG_DEBUG("Csm found in the container for Offline coll id hash = "<<(int)(*it)<<" size is "<<rdoColl->size());
      
          // + TWIN TUBES
          // use twin tubes
          if(m_useTwin){
	    Identifier elementId = rdoColl->identify();
	    // two chambers in ATLAS are installed with Twin Tubes; in detector coordinates BOL4A13 & BOL4C13; only INNER multilayer(=1) is with twin tubes
	    // here no check is done for multilayer=1, because an MdtCsm(rdoColli) contains hits from both multilayers, so this check needs to be done later
	    bool BOL4X13 = false;
            MuonStationIndex::ChIndex chIndex = m_idHelper->chamberIndex(elementId);
	    // find these two chambers in coordinates given by IdDictMuonSpectrometer
	    // used version R.01 for mapping of MuonSpectrometer (19/03/2008)
	    // http://atlas-sw.cern.ch/cgi-bin/viewcvs-atlas.cgi/offline/DetectorDescription/IdDictParser/data/IdDictMuonSpectrometer_R.01.xml?revision=1.4&view=markup
	    //  std::cout << "  ALEX: m_useTwin = " << m_useTwin 
	    // 		      << "   m_useAllBOLTwin = " << m_useAllBOLTwin 
	    // 		      << "   stationName = " << stationName 
	    // 		      << "   eta(4) = " <<  m_mdtHelper->stationEta(elementId)
	    // 		      << "   phi(7) = " << m_mdtHelper->stationPhi(elementId)
	    // 		      << std::endl;
	    if( chIndex == MuonStationIndex::BOL && fabs(m_mdtHelper->stationEta(elementId)) == 4 
	       && m_mdtHelper->stationPhi(elementId) == 7){
	      BOL4X13 = true;
	    }
	    // implement twin tube writing to prepData either for all BOL (m_useAllBOLTwin = true) _OR_ only for two chambers really installed
	  //  std::cout << "  ALEX: BOL4X13 = " << BOL4X13 << std::endl;
	    if( ( m_useAllBOLTwin && chIndex == MuonStationIndex::BOL ) || BOL4X13 ) {
	    //  std::cout << "  ALEX: gonna processCsmTwin " << std::endl;
	      StatusCode sc = processCsmTwin(rdoColl, idWithDataVect);
	      if (sc != StatusCode::SUCCESS)
		{
		  //return sc;
		}
	    } // end if( ( m_useAllBOLTwin && stationName == "BOL" ) || BOL4X13 ) {
	    else{
	      StatusCode sc = processCsm(rdoColl, idWithDataVect);
	      if (sc != StatusCode::SUCCESS)
		{
		  
		  //return sc;
		}
	    }
          } // end if(!m_useTwin)
          else{
	    StatusCode sc = processCsm(rdoColl, idWithDataVect);
	    if (sc != StatusCode::SUCCESS)
	      {
		//return sc;
	      }
          } // end else{ --> if(!m_useTwin)
          // - TWIN TUBES
          if (m_BMEpresent) {
            Identifier elementId = rdoColl->identify();
            // if there are BMEs it's also required to process there 2nd CSM
            if (m_mdtHelper->stationName(elementId) == m_BMEid) { // is BME chamber
            // do the loop once again with the SAME iterator, but for the 2nd multilayer
              if (processingDetEl == 2) {
                // reset to CSM/multilayer 1 and go to next chamber
                processingDetEl = 1;
                ++it;
                continue;
              } else {
                // the 1st CSM was done, do the 2nd in the same chamber
                processingDetEl = 2;
                continue;
              }
            }
          }
          ++it;
        } //ends loop over idVect
    }
      else
    { 
      // unseeded mode
      for (MdtCsmContainer::const_iterator rdoColli = rdoContainer->begin(); rdoColli!=rdoContainer->end(); ++rdoColli)
        {
          mycounter++;
          ATH_MSG_VERBOSE("In the loop - el.# "<<mycounter);
          // loop over all elements of the pad container 
          rdoColl = *rdoColli;
          if (rdoColl->empty()) continue;
          ATH_MSG_DEBUG("A new csm here with " 
                << rdoColl->size() <<" amt hits inside ");
          Identifier elementId = rdoColl->identify();
          ATH_MSG_DEBUG(" --- Identifier is "<<m_mdtHelper->show_to_string(elementId));
          // + TWIN TUBES
          // use twin tubes
          if(m_useTwin){
        Identifier elementId = rdoColl->identify();
        // two chambers in ATLAS are installed with Twin Tubes; in detector coordinates BOL4A13 & BOL4C13; only INNER multilayer(=1) is with twin tubes
        bool BOL4X13 = false;
        MuonStationIndex::ChIndex chIndex = m_idHelper->chamberIndex(elementId);
        // find these two chambers in coordinates given by IdDictMuonSpectrometer
        // used version R.01 for mapping of MuonSpectrometer (19/03/2008)
        // http://atlas-sw.cern.ch/cgi-bin/viewcvs-atlas.cgi/offline/DetectorDescription/IdDictParser/data/IdDictMuonSpectrometer_R.01.xml?revision=1.4&view=markup
	// 	std::cout << "  ALEX: m_useTwin = " << m_useTwin 
	// 		  << "   m_useAllBOLTwin = " << m_useAllBOLTwin 
	// 		  << "   stationName = " << stationName 
	// 		  << "   eta(4) = " <<  m_mdtHelper->stationEta(elementId)
	// 		  << "   phi(7) = " << m_mdtHelper->stationPhi(elementId)
	// 		  << std::endl;
	if(chIndex == MuonStationIndex::BOL && fabs(m_mdtHelper->stationEta(elementId)) == 4 
           && m_mdtHelper->stationPhi(elementId) == 7){ 
          BOL4X13 = true;
        }
        // implement twin tube writing to prepData either for all BOL (m_useAllBOLTwin = true) _OR_ only for two chambers really installed
//	std::cout << "  ALEX: BOL4X13 = " << BOL4X13 << std::endl;
	if( ( m_useAllBOLTwin && chIndex == MuonStationIndex::BOL ) || BOL4X13 ) {
//	  std::cout << "  ALEX: gonna processCsmTwin " << std::endl;
          StatusCode sc = processCsmTwin(rdoColl, idWithDataVect);
          if (sc != StatusCode::SUCCESS)
            {
              //return sc;
            }
        } // end if( ( m_useAllBOLTwin && stationName == "BOL" ) || BOL4X13 ) {
        else{
          StatusCode sc = processCsm(rdoColl, idWithDataVect);
          if (sc != StatusCode::SUCCESS)
            {
              //return sc;
            }
        }
          } // end if(!m_useTwin)
          else{
        StatusCode sc = processCsm(rdoColl, idWithDataVect);
        if (sc != StatusCode::SUCCESS)
          {
            ATH_MSG_DEBUG("processCsm returns a bad StatusCode - keep going for new data collections in this event");
            //return sc;
          }
          } // end else{ --> if(!m_useTwin)
          // - TWIN TUBES
        }// end loop over pads
    }
  
      return status;
}

// dump the RDO in input
void Muon::MdtRdoToPrepDataTool::printInputRdo()
{

  ATH_MSG_INFO("******************************************************************************************");
  ATH_MSG_INFO("***************** Listing MdtCsmContainer collections content ********************************");

  const MdtCsmContainer* rdoContainer;
  StatusCode status = evtStore()->retrieve(rdoContainer,"MDTCSM");
  if (status.isFailure()) {
    ATH_MSG_WARNING("Retrieval of MDT Rdo container for debugging purposes failed");
    return;
  }

  if (rdoContainer->size()==0) ATH_MSG_INFO("MdtCsmContainer is Empty");

  ATH_MSG_INFO("-----------------------------------------------------------------------------");

  MdtCsmContainer::const_iterator it = rdoContainer->begin();

  int ncsm = 0;
  int namt = 0;
  // loop on the MdtCsm collections
  for (  ; it != rdoContainer->end() ; ++it ) {

    ncsm++;
    const MdtCsm* mdtColl = (*it);
    // get the online Id of the collection:
    uint16_t subdet = mdtColl->SubDetId();
    uint16_t mrodid = mdtColl->MrodId();
    uint16_t csmid  = mdtColl->CsmId();

    ATH_MSG_INFO("**** MdtCsm with online Id: subdetector: " << MSG::hex 
         << subdet << MSG::dec << "  mrod: " << MSG::hex << mrodid 
         << MSG::dec << "  csmid: " << MSG::hex << csmid << MSG::dec);
    ATH_MSG_INFO("  number of mdt hits: " << mdtColl->size());
  
    // loop on the hits of the CSM
    MdtCsm::const_iterator it_amt = mdtColl->begin();
    for ( ; it_amt != mdtColl->end() ; ++it_amt ) {

      const MdtAmtHit* amtHit = (*it_amt);
      
      namt++;
      uint16_t tdcId=amtHit->tdcId();
      uint16_t channelId=amtHit->channelId();
      uint16_t fine=amtHit->fine();
      uint16_t coarse=amtHit->coarse();
      uint16_t width=amtHit->width();
      
      ATH_MSG_INFO(">> AmtHit in tdc: " << MSG::hex << tdcId << MSG::dec
           << "  channel: " << MSG::hex << channelId << MSG::dec
           << "  fine time: " << fine
           << "  coarse time: " << coarse
           << "  width: " << width); 
      
    }

  }

  ATH_MSG_INFO("*** Event Summary: csm collections:" << ncsm << "  amt hits: " << namt);

  return;
}

void Muon::MdtRdoToPrepDataTool::printPrepData(  )
{
  // Dump info about PRDs
  ATH_MSG_INFO("******************************************************************************************");
  ATH_MSG_INFO("***************** Listing MdtPrepData collections content ********************************");
  
  if (m_mdtPrepDataContainer->size() <= 0) ATH_MSG_INFO("No MdtPrepRawData collections found");
  int ncoll = 0;
  int nhits = 0;
  ATH_MSG_INFO("--------------------------------------------------------------------------------------------");
  for (IdentifiableContainer<Muon::MdtPrepDataCollection>::const_iterator mdtColli = m_mdtPrepDataContainer->begin();
       mdtColli!=m_mdtPrepDataContainer->end(); ++mdtColli)
    {
      const Muon::MdtPrepDataCollection* mdtColl = *mdtColli;
      
      int nhitcoll = 0;
      if ( mdtColl->size() > 0 ) 
        {            
      ATH_MSG_INFO("PrepData Collection ID "<<m_mdtHelper->show_to_string(mdtColl->identify()));
      MdtPrepDataCollection::const_iterator it_mdtPrepData;
      for (it_mdtPrepData=mdtColl->begin(); it_mdtPrepData != mdtColl->end(); it_mdtPrepData++) {
        nhitcoll++;
        nhits++;
        ATH_MSG_INFO(" in this coll. "<<nhitcoll<<" prepData id = "
             <<m_mdtHelper->show_to_string((*it_mdtPrepData)->identify())
             <<" tdc/adc ="<<(*it_mdtPrepData)->tdc()<<"/"<< (*it_mdtPrepData)->adc());
      }
      ncoll++;
      ATH_MSG_INFO("*** Collection "<<ncoll<<" Summary: N. hits = "<<nhitcoll);
      ATH_MSG_INFO("--------------------------------------------------------------------------------------------");
        }
    }
  ATH_MSG_INFO("*** Event  Summary: "
           <<ncoll <<" Collections / "
           <<nhits<<" hits  ");
  ATH_MSG_INFO("--------------------------------------------------------------------------------------------");
  
}

StatusCode Muon::MdtRdoToPrepDataTool::processCsm(const MdtCsm *rdoColl, std::vector<IdentifierHash>& idWithDataVect)
{
  ATH_MSG_DEBUG(" ***************** Start of processCsm");

  /// MDT hit context
  //IdContext hitContext = m_mdtHelper->channel_context();
  Identifier elementId = m_mdtHelper->parentID(rdoColl->identify());

  // prepare the special case of two chamber connected to the same CSM
  // create objects but do not initialize them
  bool doubleCsmFound = false;
  bool doubleCsm = false;

  Identifier secondElementId;
  MdtPrepDataCollection* secondDriftCircleColl = NULL;
  IdentifierHash secondMdtHashId;

  ATH_MSG_DEBUG(" Number of AmtHit in this Csm "
    << rdoColl->size() <<" CSM id is "<<m_mdtHelper->show_to_string(elementId));

  uint16_t subdetId = rdoColl->SubDetId();
  uint16_t mrodId = rdoColl->MrodId();
  uint16_t csmId = rdoColl->CsmId();
  ATH_MSG_VERBOSE("Identifier = "<<m_mdtHelper->show_to_string(elementId)
    <<" subdetId/ mrodId/ csmId = "<<subdetId<<" / "<<mrodId<<" / "<<csmId);

  // Create prepdata collection
  IdContext mdtContext = m_mdtHelper->module_context();
  IdentifierHash mdtHashId;
  if(m_mdtHelper->get_hash(elementId, mdtHashId, &mdtContext)) {
    ATH_MSG_FATAL("Unable to get MDT hash id from MDT digit collection "
      << "context begin_index = " << mdtContext.begin_index()
      << " context end_index  = " << mdtContext.end_index()
      << " the identifier is ");
    elementId.show();
    return StatusCode::FAILURE;
  }
  ATH_MSG_VERBOSE("HashId = "<<(int)mdtHashId);

  MdtPrepDataCollection * driftCircleColl = NULL;

  if ( m_mdtPrepDataContainer->indexFind(mdtHashId) != m_mdtPrepDataContainer->end() ) {
    // for elevator chambers there are 2 CSMs to be filled in the same collection
    if ( m_mdtHelper->stationName(elementId) == m_BMEid && m_BMEpresent) {
      driftCircleColl = const_cast<MdtPrepDataCollection*>(&(**m_mdtPrepDataContainer->indexFind(mdtHashId)));
      ATH_MSG_DEBUG("In ProcessCSM - collection already contained in IDC, but BME! Taking it.");
    } 
    else {
      ATH_MSG_DEBUG("In ProcessCSM - collection already contained in IDC");
      return StatusCode::FAILURE;
    }
  }
  // create new collection + add it if there's nothing yet (also for the first CSM of a elevator chambers)
  else { 
    driftCircleColl = new MdtPrepDataCollection(mdtHashId);
    idWithDataVect.push_back(mdtHashId);
    driftCircleColl->setIdentifier(elementId);
    ATH_MSG_DEBUG("A new MdtPrepDataCollection here");
    //MdtPrepDataContainer::KEY key = m_mdtPrepDataContainer->key(elementId);

    if (StatusCode::SUCCESS != m_mdtPrepDataContainer->addCollection(driftCircleColl, mdtHashId)) {
      ATH_MSG_DEBUG("In ProcessCSM - Couldn't record in the Container MDT Drift Circle Collection with hashID = "
        << (int)mdtHashId <<" ext. id = "
        << m_mdtHelper->show_to_string(elementId));
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("MdtPrepDataCollection added to the container");
  }

  // for each Csm, loop over AmtHit, converter AmtHit to digit
  // retrieve/create digit collection, and insert digit into collection
  MdtCsm::const_iterator itD   = rdoColl->begin();
  MdtCsm::const_iterator itD_e = rdoColl->end();

  int mc = 0;
  for (; itD!=itD_e; ++itD)
  {
    mc++;
    const MdtAmtHit * amtHit = (*itD);

      // set to false the decision of whether it's a special case of two chambers connected to a 
      // single CSM or not
    doubleCsm = false;

      // FIXME: Still use the digit class.

    if (!(m_isTestBeam && subdetId==99 && mrodId==3 &&
    csmId==5 && amtHit->tdcId()==17)) {

      ATH_MSG_VERBOSE("Amt Hit n. "<<mc<<" tdcId = "<< amtHit->tdcId());

      MdtDigit * newDigit = m_mdtDecoder->getDigit(amtHit,subdetId,
        mrodId,csmId);

      if (newDigit==NULL) {
        ATH_MSG_WARNING("Error in MDT RDO decoder for subdetId/mrodId/csmId "
          <<subdetId<<"/"<<mrodId<<"/"<<csmId<<" amtHit channelId/tdcId ="
          <<amtHit->channelId()<<"/"<<amtHit->tdcId());
        continue;
      }

    // Do something with it
      Identifier     channelId   = newDigit->identify();
      Identifier     parentId    = m_mdtHelper->parentID(channelId);
      if( m_mdtHelper->stationName(parentId) == m_BMGid && m_BMGpresent) {
        std::map<Identifier, std::vector<Identifier> >::iterator myIt = m_DeadChannels.find(m_muonMgr->getMdtReadoutElement(channelId)->identify());
        if( myIt != m_DeadChannels.end() ){
          if( std::find( (myIt->second).begin(), (myIt->second).end(), channelId) != (myIt->second).end() ) {
            ATH_MSG_DEBUG("processCsm : Deleting BMG digit with identifier" << m_mdtHelper->show_to_string(channelId) );
            delete newDigit;
            continue;
          }
        }
      }

    // check if the module ID of this channel is different from the CSM one
    // If it's the first case, create the additional collection

      if ( parentId != elementId ) {

        int stationName = m_mdtHelper->stationName(channelId);
      // check that at least the station name is the same, otherwise it must be an error
        if ( stationName == m_mdtHelper->stationName(elementId) ) {

          doubleCsm = true;

          if ( !doubleCsmFound ) {

            doubleCsmFound = true;
            secondElementId = parentId;

            ATH_MSG_DEBUG("A special case of single CSM reading two chambers has been found");

            ATH_MSG_DEBUG("First station: " << m_mdtHelper->stationNameString(stationName) << " eta: " 
              << m_mdtHelper->stationEta(elementId) << " phi: " << m_mdtHelper->stationPhi(elementId) );

            ATH_MSG_DEBUG("Second station: " << m_mdtHelper->stationNameString(stationName) << " eta: " 
              << m_mdtHelper->stationEta(secondElementId) << " phi: " 
              << m_mdtHelper->stationPhi(secondElementId) );

          //
          // create the collection and initialize the hash Id
          //
            if(m_mdtHelper->get_hash(secondElementId, secondMdtHashId, &mdtContext)) {
              ATH_MSG_FATAL("Unable to get MDT hash id from MDT digit collection "
                << "context begin_index = " << mdtContext.begin_index()
                << " context end_index  = " << mdtContext.end_index()
                << " the identifier is ");
              secondElementId.show();
              return StatusCode::FAILURE;
            }
            ATH_MSG_VERBOSE("HashId = "<<(int) secondMdtHashId);

	    if (m_mdtPrepDataContainer->indexFind(secondMdtHashId)!=m_mdtPrepDataContainer->end()) {
	      ATH_MSG_DEBUG("In ProcessCSM - collection already contained in IDC");
	      return StatusCode::FAILURE;
	    }

            secondDriftCircleColl = new MdtPrepDataCollection(secondMdtHashId);
            idWithDataVect.push_back(secondMdtHashId);
            secondDriftCircleColl->setIdentifier(secondElementId);

            ATH_MSG_DEBUG("A new MdtPrepDataCollection here");

            if (StatusCode::SUCCESS != m_mdtPrepDataContainer->addCollection(secondDriftCircleColl, secondMdtHashId)) {
              ATH_MSG_DEBUG("In ProcessCSM - Couldn't record in the Container MDT Drift Circle Collection with hashID = "
                << (int)secondMdtHashId <<" ext. id = "
                << m_mdtHelper->show_to_string(secondElementId));
              return StatusCode::FAILURE;
            }
          } 
        }
      }

      IdentifierHash channelHash = newDigit->identifyHash();

      ATH_MSG_VERBOSE("got digit with id ext / hash "<<m_mdtHelper->show_to_string(channelId)<<" / "<<(int)channelHash);

    //check to see if ID needs correcting & is BOL (i.e. station name = 10)
    // Q. Is there a better way to do this by hard-coding numbers? Ed.
      if (m_doIdentifierCorrection && 10==m_mdtHelper->stationName(channelId) )
      {
        int tube = m_mdtHelper->tube(channelId);
        ATH_MSG_VERBOSE("Station name: "<<m_mdtHelper->stationName(channelId)
          <<", string="
          <<m_mdtHelper->stationNameString(m_mdtHelper->stationName(channelId)));
        int oldTube=tube;

        if ( (6<tube) && (tube<35) )
          tube--;
        else if (tube>=35)
          tube-=2;
        if (tube!=oldTube)
        {
        // just use same values, apart from modified
          channelId = m_mdtHelper->channelID( m_mdtHelper->stationName(channelId),
            m_mdtHelper->stationEta(channelId),
            m_mdtHelper->stationPhi(channelId),
            m_mdtHelper->multilayer(channelId),
            m_mdtHelper->tubeLayer(channelId),
            tube );
          ATH_MSG_VERBOSE("Changed tube from :"<<oldTube<<" to: "<<tube
            << " Id is: "<<m_mdtHelper->show_to_string(channelId));
        }
      }

      double radius(0.); double errRadius(0.);
      Muon::MdtDriftCircleStatus digitStatus = Muon::MdtStatusDriftTime;

      // do lookup once
      const MdtReadoutElement * descriptor = m_muonMgr->getMdtReadoutElement(channelId);
      if (descriptor == NULL){
        ATH_MSG_WARNING("Detector Element not found for Identifier from the cabling service <"
          <<m_mdtHelper->show_to_string(channelId)<<">  =>>ignore this hit");
        delete newDigit;
        continue;
      }
      if (!descriptor->containsId(channelId)) {
	ATH_MSG_WARNING("Detector Element "<<m_mdtHelper->show_to_string(descriptor->identify())
			<<" does not contains candidate prd Identifier <"
			<<m_mdtHelper->show_to_string(channelId)<<">  =>>ignore this hit");
        delete newDigit;
        continue;
      }
      

      if (newDigit->is_masked()) {
        digitStatus = Muon::MdtStatusMasked;
      }else{
        digitStatus = getMdtDriftRadius(newDigit, radius, errRadius, descriptor);
        if( radius < -999 ) {
          ATH_MSG_WARNING("MDT PrepData with very large, negative radius "
            << " Id is: "<<m_mdtHelper->print_to_string(channelId));
          
        }
      }

      Amg::Vector2D driftRadius(radius,0);
      Amg::MatrixX* cov = new Amg::MatrixX(1,1);
      (*cov)(0,0) = errRadius*errRadius;
    // Create new PrepData

    // Check that the hit does not belong to the second chamber which might be connected to
    // this CSM
      MdtPrepDataCollection* collectionToBeUpdated;
      IdentifierHash hashId;

      if ( (!doubleCsmFound) || (!doubleCsm) ) {
        collectionToBeUpdated = driftCircleColl;
        hashId = mdtHashId;
      }
      else {
        collectionToBeUpdated = secondDriftCircleColl;
        hashId = secondMdtHashId;
      }

      MdtPrepData* newPrepData = new MdtPrepData(channelId,
        hashId,
        driftRadius,
        cov,
        descriptor,
        newDigit->tdc(),
        newDigit->adc(),
        digitStatus);

      newPrepData->setHashAndIndex(collectionToBeUpdated->identifyHash(), collectionToBeUpdated->size());
      collectionToBeUpdated->push_back(newPrepData);

      delete newDigit;

    }
    else 
    {
      ATH_MSG_DEBUG("Skipping some unknown data !!!!!!!!");
    }

  }
  return StatusCode::SUCCESS;
}

StatusCode Muon::MdtRdoToPrepDataTool::processCsmTwin(const MdtCsm *rdoColl, std::vector<IdentifierHash>& idWithDataVect)
{
  ATH_MSG_DEBUG(" ***************** Start of processCsmTwin");
  
  ATH_MSG_DEBUG(" Number of AmtHit in this Csm "
        << rdoColl->size());
  /// MDT hit context
    //IdContext hitContext = m_mdtHelper->channel_context();
    Identifier elementId = m_mdtHelper->parentID(rdoColl->identify());
    
    uint16_t subdetId = rdoColl->SubDetId();
    uint16_t mrodId = rdoColl->MrodId();
    uint16_t csmId = rdoColl->CsmId();
    ATH_MSG_VERBOSE("Identifier = "<<m_mdtHelper->show_to_string(elementId)
            <<" subdetId/ mrodId/ csmId = "<<subdetId<<" / "
            <<mrodId<<" / "<<csmId);
    

    
    // Create prepdata collection
    IdContext mdtContext = m_mdtHelper->module_context();
    IdentifierHash mdtHashId;
    if(m_mdtHelper->get_hash(elementId, mdtHashId, &mdtContext)) {
      ATH_MSG_FATAL("Unable to get MDT hash id from MDT digit collection "
            << "context begin_index = " << mdtContext.begin_index()
            << " context end_index  = " << mdtContext.end_index()
            << " the identifier is ");
      elementId.show();
      return StatusCode::FAILURE;
    }
    ATH_MSG_VERBOSE("HashId = "<<(int)mdtHashId);

    if (m_mdtPrepDataContainer->indexFind(mdtHashId)!=m_mdtPrepDataContainer->end()) {
      ATH_MSG_DEBUG("In ProcessCSM - collection already contained in IDC");
      return StatusCode::FAILURE;
    }

    MdtPrepDataCollection * driftCircleColl = new MdtPrepDataCollection(mdtHashId);
    idWithDataVect.push_back(mdtHashId);
    driftCircleColl->setIdentifier(elementId);
    ATH_MSG_DEBUG("A new MdtPrepDataCollection here");
    //MdtPrepDataContainer::KEY key = m_mdtPrepDataContainer->key(elementId);

    if (StatusCode::SUCCESS != m_mdtPrepDataContainer->addCollection(driftCircleColl, mdtHashId)) {
      ATH_MSG_DEBUG("In ProcessCSMtwin - Couldn't record in the Container MDT Drift Circle Collection with hashID = "
            << (int)mdtHashId <<" ext. id = "
            << m_mdtHelper->show_to_string(elementId));
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("MdtPrepDataCollection added to the container");

    // for each Csm, loop over AmtHit, converter AmtHit to digit
    // retrieve/create digit collection, and insert digit into collection
    MdtCsm::const_iterator itD   = rdoColl->begin();
    MdtCsm::const_iterator itD_e = rdoColl->end();
    
    // make a map to be filled for every twin-pair
    //   std::map<int, std::vector<MdtDigit*> > mdtDigitColl;
    std::map<int, std::pair<MdtDigit*, MdtDigit*> > mdtDigitColl;

    for (; itD!=itD_e; ++itD){
      const MdtAmtHit * amtHit = (*itD);
      
      // FIXME: Still use the digit class.
      if (!(m_isTestBeam && subdetId==99 && mrodId==3 && csmId==5 && amtHit->tdcId()==17)) {
    
    MdtDigit * newDigit = m_mdtDecoder->getDigit(amtHit,subdetId,mrodId,csmId);
    
    if (newDigit==NULL) {
          ATH_MSG_WARNING("Error in MDT RDO decoder for subdetId/mrodId/csmId "<<subdetId<<"/"<<mrodId<<"/"<<csmId<<" amtHit channelId/tdcId ="<<amtHit->channelId()<<"/"<<amtHit->tdcId());
      continue;
    }
    else{
      // make an Identifier
      Identifier channelId = newDigit->identify();
      //IdentifierHash channelHash = newDigit->identifyHash();

      if( m_mdtHelper->stationName(channelId) == m_BMGid && m_BMGpresent) {
        std::map<Identifier, std::vector<Identifier> >::iterator myIt = m_DeadChannels.find(m_muonMgr->getMdtReadoutElement(channelId)->identify());
        if( myIt != m_DeadChannels.end() ){
          if( std::find( (myIt->second).begin(), (myIt->second).end(), channelId) != (myIt->second).end() ) {
            ATH_MSG_DEBUG("processCsm : Deleting BMG digit with identifier" << m_mdtHelper->show_to_string(channelId) );
            delete newDigit;
            continue;
          }
        }
      }

      // get tube params
      int tube = m_mdtHelper->tube(channelId);
      int layer = m_mdtHelper->tubeLayer(channelId);
      int multilayer = m_mdtHelper->multilayer(channelId);
      
      // find the correct twin-pair (tube-1 & tube-3 are twin pair 1, tube-2 & tube-4 are twin pair 2)
      int twinPair = -1;
      if( tube%4 == 1){ twinPair = (tube + 1)/2 ;}
      if( tube%4 == 3){ twinPair = (tube - 1)/2 ;}
      if( tube%4 == 2){ twinPair = (tube + 2)/2 ;}
      if( tube%4 == 0){ twinPair = tube/2 ;}

    

      // fill the digitColl map
      if( mdtDigitColl[ twin_chamber[multilayer-1][layer-1][twinPair-1] ].first == 0){
        mdtDigitColl[ twin_chamber[multilayer-1][layer-1][twinPair-1] ].first = newDigit;
      }
      else if( mdtDigitColl[ twin_chamber[multilayer-1][layer-1][twinPair-1] ].second == 0){
        mdtDigitColl[ twin_chamber[multilayer-1][layer-1][twinPair-1] ].second = newDigit;
      }
      // if a secondary hit appears in a tube add it to mdtDigitColl, unless m_discardSecondaryHitTwin flag is true
      else{
        ATH_MSG_VERBOSE(" TWIN TUBES: found a secondary(not twin) hit in a twin tube");
 
        if(!m_discardSecondaryHitTwin){
          if( mdtDigitColl[ secondaryHit_twin_chamber[multilayer-1][layer-1][twinPair-1] ].first == 0){
        mdtDigitColl[ secondaryHit_twin_chamber[multilayer-1][layer-1][twinPair-1] ].first = newDigit;
          }
          else if( mdtDigitColl[ secondaryHit_twin_chamber[multilayer-1][layer-1][twinPair-1] ].second == 0){
	mdtDigitColl[ secondaryHit_twin_chamber[multilayer-1][layer-1][twinPair-1] ].second = newDigit;
	  }
          else{ ATH_MSG_VERBOSE(" TWIN TUBES: found a tertiary hit in a twin tube in one RdoCollection for "
				<< m_mdtHelper->stationNameString(m_mdtHelper->stationName(channelId))
				<< " eta = " << m_mdtHelper->stationEta(channelId) << " phi = " << m_mdtHelper->stationPhi(channelId)
				<< "  ml =  "  <<m_mdtHelper->multilayer(channelId)  << "  layer = " << m_mdtHelper->tubeLayer(channelId) 
				<< " tube = " <<  m_mdtHelper->tube(channelId) << " with adc  = " << newDigit->adc() << "  tdc = " << newDigit->tdc());
	  }
        } //end --   if(!m_discardSecondaryHitTwin){
        else{
          ATH_MSG_INFO(" TWIN TUBES: discarding secondary(non-twin) hit in a twin tube as flag m_discardSecondaryHitTwin is set to true");
        }
      }
         
    }// end else - if (newDigit==NULL) {
      } // end  if (!(m_mdtCabling->isTestBeamSetup() && subdetId==99 && mrodId==3 && csmId==5 && amtHit->tdcId()==17)) {
    }// end for-loop over rdoColl  
 
    // make iterator over mdtDigitColl map
    std::map<int, std::pair<MdtDigit*, MdtDigit*> >::iterator iter_map;

    //iterate over mdtDigitColl
    for( iter_map = mdtDigitColl.begin(); iter_map != mdtDigitColl.end(); iter_map++ ) {

      // get the twin hits from mdtDigitColl
      MdtDigit* digit = iter_map->second.first;
      MdtDigit* second_digit = iter_map->second.second;

      if (digit==NULL) {
	ATH_MSG_FATAL("A null pointer to a digit ");
	return StatusCode::FAILURE;
      }
      
      // Do something with it
      Identifier     channelId   = digit->identify();
      IdentifierHash channelHash = digit->identifyHash();

      int multilayer = m_mdtHelper->multilayer(channelId);
  
      //check if the hit is in multilayer=1	    
      // two chambers in ATLAS are installed with Twin Tubes; in detector coordinates BOL4A13 & BOL4C13; only INNER multilayer(=1) is with twin tubes
      if(multilayer==1){
	// if no twin hit present in data, use standard PRD making 
	if(second_digit == NULL){
	  
	  
	  ATH_MSG_VERBOSE("got digit with id ext / hash "
			  <<m_mdtHelper->show_to_string(channelId)<<" / "
			  <<(int)channelHash);
	  
	  //check to see if ID needs correcting & is BOG (i.e. station name = 10)
	  // Q. Is there a better way to do this by hard-coding numbers? Ed.
	  if (m_doIdentifierCorrection && 10==m_mdtHelper->stationName(channelId) )
	    {
	      int tube = m_mdtHelper->tube(channelId);
	      ATH_MSG_VERBOSE("Station name: "<<m_mdtHelper->stationName(channelId)
			      <<", string="<<m_mdtHelper->stationNameString(m_mdtHelper->stationName(channelId)));
	      int oldTube=tube;
	      
	      if ( (6<tube) && (tube<35) )
		tube--;
	      else if (tube>=35)
		tube-=2;
	      if (tube!=oldTube)
		{
		  // just use same values, apart from modified
		  channelId = m_mdtHelper->channelID( m_mdtHelper->stationName(channelId),
						      m_mdtHelper->stationEta(channelId),
						      m_mdtHelper->stationPhi(channelId),
						      m_mdtHelper->multilayer(channelId),
						      m_mdtHelper->tubeLayer(channelId),
						      tube );
		  ATH_MSG_VERBOSE("Changed tube from :"<<oldTube<<" to: "<<tube
				  << " Id is: "<<m_mdtHelper->show_to_string(channelId));
		}
	    }
	  
	  double radius(0.); double errRadius(0.);
	  Muon::MdtDriftCircleStatus digitStatus = Muon::MdtStatusDriftTime;
	  
	  // do lookup once
	  const MdtReadoutElement * descriptor = m_muonMgr->getMdtReadoutElement(channelId);
	  if (descriptor == NULL){
	    ATH_MSG_WARNING("Detector Element not found for Identifier from the cabling service <"
			    <<m_mdtHelper->show_to_string(channelId)<<">  =>>ignore this hit");
	    delete digit;
	    continue;
	  }
	  
	  if (digit->is_masked()) {
	    digitStatus = Muon::MdtStatusMasked;
	  }else{
	    digitStatus = getMdtDriftRadius(digit, radius, errRadius, descriptor);
	    if( radius < -999 ) {
	      ATH_MSG_WARNING("MDT PrepData with very large, negative radius "
			      << " Id is: "<<m_mdtHelper->show_to_string(channelId));
	    }
	  }
	  
	  Amg::Vector2D driftRadius(radius,0);
	  Amg::MatrixX* cov = new Amg::MatrixX(1,1);
	  (*cov)(0,0) = errRadius*errRadius;
	  
	  // Create new PrepData
	  MdtPrepData *newPrepData = new MdtPrepData(channelId,
						     mdtHashId,
						     driftRadius,
						     cov,
						     descriptor,
						     digit->tdc(),
						     digit->adc(),
						     digitStatus);
	  
	  newPrepData->setHashAndIndex(driftCircleColl->identifyHash(), driftCircleColl->size());
	  driftCircleColl->push_back(newPrepData);
	  
	  ATH_MSG_DEBUG(" MADE ORIGINAL PREPDATA " << m_mdtHelper->stationNameString(m_mdtHelper->stationName(channelId)) 
			<< "  eta = " << m_mdtHelper->stationEta(channelId)
			<< "  phi = " << m_mdtHelper->stationPhi(channelId)
			<< "  ml = " << m_mdtHelper->multilayer(channelId)
			<< "  layer = " << m_mdtHelper->tubeLayer(channelId) 
			<< "  tube = " <<  m_mdtHelper->tube(channelId) 
			<< "  radius = " << radius << " +- " << errRadius);
	  
	  delete digit;
	}// end if(second_digit == NULL){
	else{
	  
	  // define twin position and error
	  double zTwin(0.); double errZTwin(0.); bool secondHitIsPrompt(false);
	  // define drift-radius and error
	  double radius(0.); double errRadius(0.);
	  Muon::MdtDriftCircleStatus digitStatus = Muon::MdtStatusDriftTime;
	  // call the function to calculate radii and twin coordinate
	  digitStatus = getMdtTwinPosition(digit, second_digit, radius, errRadius, zTwin, errZTwin, secondHitIsPrompt);
	  if( zTwin <-99999 ) {   ATH_MSG_WARNING("MDT Twin PrepData with very large, negative twin coordinate " << zTwin
						  << " Id is: "<<m_mdtHelper->show_to_string(digit->identify())
						  << " Twin Id is: "<<m_mdtHelper->show_to_string(second_digit->identify())); }
	  
	  // set the properties of PrepData-object to the tube that was PROMPT (= hit by the muon)
	  Identifier promptHit_channelId = digit->identify();
	  IdentifierHash promptHit_channelHash = digit->identifyHash();
	  MdtDigit* promptHit_Digit = digit; 
	  Identifier twinHit_channelId = second_digit->identify();
	  IdentifierHash twinHit_channelHash = second_digit->identifyHash();
	  MdtDigit* twinHit_Digit = second_digit; 
	  
	  if(secondHitIsPrompt){
	    promptHit_channelId = second_digit->identify();
	    promptHit_channelHash = second_digit->identifyHash();
	    promptHit_Digit = second_digit;
	    twinHit_channelId = digit->identify();
	    twinHit_channelHash = digit->identifyHash();
	    twinHit_Digit = digit; 
	  }
	  
	  // do lookup once
	  const MdtReadoutElement * descriptor = m_muonMgr->getMdtReadoutElement(promptHit_channelId);
	  if (descriptor == NULL){
	    ATH_MSG_WARNING("Detector Element not found for Identifier from the DetManager <"
			    <<m_mdtHelper->show_to_string(promptHit_channelId)<<">  =>>ignore this hit");
	    delete digit;
	    continue;
	  }
	  
	  // check if digit is masked
	  if (promptHit_Digit->is_masked()) {
	    digitStatus = Muon::MdtStatusMasked;
	  }else if( radius < -999 ) {
	    ATH_MSG_WARNING("MDT Twin PrepData with very large, negative radius "
			    << " Id is: "<<m_mdtHelper->show_to_string(promptHit_channelId));
	  }
	  
          
	  Amg::Vector2D driftRadiusZTwin(radius,zTwin);
	  // make a 2x2 matrix with all values initialized at 0
	  Amg::MatrixX* cov = new Amg::MatrixX(2,2);
	  (*cov)(0,0) = errRadius*errRadius;
	  (*cov)(1,1) = errZTwin*errZTwin;
	  (*cov)(0,1) = 0;
	  (*cov)(1,0) = 0;
          
          
	  // Create new PrepData either w/ or w/o twin hit info depending on m_use1DPrepDataTwin flag
	  if(!m_use1DPrepDataTwin){
	    MdtTwinPrepData *twin_newPrepData = new MdtTwinPrepData(promptHit_channelId,
								    //promptHit_channelHash,
								    mdtHashId,  
								    driftRadiusZTwin,
								    cov,
								    descriptor,
								    promptHit_Digit->tdc(),
								    promptHit_Digit->adc(),
								    twinHit_Digit->tdc(),
								    twinHit_Digit->adc(),
								    digitStatus);
	    
	    ATH_MSG_DEBUG(" MADE A 2D TWINPREPDATA " << m_mdtHelper->stationNameString(m_mdtHelper->stationName(promptHit_channelId))
			  << "  eta = " << m_mdtHelper->stationEta(promptHit_channelId)
			  << "  phi = " << m_mdtHelper->stationPhi(promptHit_channelId)
			  << "  ml = " << m_mdtHelper->multilayer(promptHit_channelId)
			  << "  layer = " << m_mdtHelper->tubeLayer(promptHit_channelId) << " tube = " <<  m_mdtHelper->tube(promptHit_channelId) 
			  << "  zTwin = " << zTwin << " +- " << errZTwin 
			  << "  radius = " << radius << " +- " << errRadius);
	    
	    Amg::Vector3D gpos_centertube = twin_newPrepData->globalPosition();
	    const MdtReadoutElement* detEl = m_muonMgr->getMdtReadoutElement(promptHit_channelId);
	    //      Amg::Vector3D locpos_centertube = detEl->globalToLocalCoords(gpos_centertube, promptHit_channelId);
	    Amg::Vector3D locpos_centertube = Amg::Vector3D(0.,0.,zTwin);
	    Amg::Vector3D gpos_twin = detEl->localToGlobalCoords(locpos_centertube, promptHit_channelId);
	    
	    ATH_MSG_DEBUG(" global pos center tube  x = " << gpos_centertube.x() << " y = " << gpos_centertube.y() << " z = " << gpos_centertube.z());
	    ATH_MSG_DEBUG(" local pos center tube w/ TWIN INFO  x = " << locpos_centertube.x() << " y = " << locpos_centertube.y() << " z = " << locpos_centertube.z());
	    ATH_MSG_DEBUG(" global pos w/ TWIN INFO x = " << gpos_twin.x() << " y = " << gpos_twin.y() << " z = " << gpos_twin.z());
	    
	    twin_newPrepData->setHashAndIndex(driftCircleColl->identifyHash(), driftCircleColl->size());
	    driftCircleColl->push_back(twin_newPrepData);
	    
	    
	  } // end if(!m_use1DPrepDataTwin){
	  else{
	    Amg::Vector2D driftRadius(radius,0);
	    // make a 2x2 matrix with all values initialized at 0
	    Amg::MatrixX* cov = new Amg::MatrixX(1,1);
	    (*cov)(0,0) = errRadius*errRadius;
	    
	    MdtPrepData *twin_newPrepData = new MdtPrepData(promptHit_channelId,
							    promptHit_channelHash,  
							    driftRadius,
							    cov,
							    descriptor,
							    promptHit_Digit->tdc(),
							    promptHit_Digit->adc(),
							    digitStatus);
	    
	    ATH_MSG_DEBUG(" MADE A 1D(=original) PREPDATA OUT OF TWINPAIR "
			  << "   TWIN COORDINATE IS NOT STORED IN PREPDATA "
			  << m_mdtHelper->stationNameString(m_mdtHelper->stationName(promptHit_channelId))
			  << "  eta = " << m_mdtHelper->stationEta(promptHit_channelId)
			  << "  phi = " << m_mdtHelper->stationPhi(promptHit_channelId)
			  << "  ml = " << m_mdtHelper->multilayer(promptHit_channelId)
			  << "  layer = " << m_mdtHelper->tubeLayer(promptHit_channelId) << " tube = " <<  m_mdtHelper->tube(promptHit_channelId) 
			  << "  zTwin = " << zTwin << " +- " << errZTwin 
			  << "  radius = " << radius << " +- " << errRadius);
	    
	    twin_newPrepData->setHashAndIndex(driftCircleColl->identifyHash(), driftCircleColl->size());
	    driftCircleColl->push_back(twin_newPrepData);
	    
	  }// end else --  if(!m_use1DPrepDataTwin){
	  
	  delete digit;
	  delete second_digit;
	  
	}// end else --  if(second_digit == NULL){
      } // end -- if(multilayer==1)
      else if(multilayer==2){
	// if multilayer=2, then treat every hit as a separate hit, no twin hit should be present here as the hardware is not installed
        
  	if(second_digit == NULL){

	  ATH_MSG_VERBOSE("got digit with id ext / hash "
			  <<m_mdtHelper->show_to_string(channelId)<<" / "
			  <<(int)channelHash);
	  
	  //check to see if ID needs correcting & is BOG (i.e. station name = 10)
	  // Q. Is there a better way to do this by hard-coding numbers? Ed.
	  if (m_doIdentifierCorrection && 10==m_mdtHelper->stationName(channelId) )
	    {
	      int tube = m_mdtHelper->tube(channelId);
	      ATH_MSG_VERBOSE("Station name: "<<m_mdtHelper->stationName(channelId)
			      <<", string="<<m_mdtHelper->stationNameString(m_mdtHelper->stationName(channelId)));
	      int oldTube=tube;
	      
	      if ( (6<tube) && (tube<35) )
		tube--;
	      else if (tube>=35)
		tube-=2;
	      if (tube!=oldTube)
		{
		  // just use same values, apart from modified
		  channelId = m_mdtHelper->channelID( m_mdtHelper->stationName(channelId),
						      m_mdtHelper->stationEta(channelId),
						      m_mdtHelper->stationPhi(channelId),
						      m_mdtHelper->multilayer(channelId),
						      m_mdtHelper->tubeLayer(channelId),
						      tube );
		  ATH_MSG_VERBOSE("Changed tube from :"<<oldTube<<" to: "<<tube
				  << " Id is: "<<m_mdtHelper->show_to_string(channelId));
		}
	    }
	  
	  double radius(0.); double errRadius(0.);
	  Muon::MdtDriftCircleStatus digitStatus = Muon::MdtStatusDriftTime;
	  
	  // do lookup once
	  const MdtReadoutElement * descriptor = m_muonMgr->getMdtReadoutElement(channelId);
	  if (descriptor == NULL){
	    ATH_MSG_WARNING("Detector Element not found for Identifier from the cabling service <"
			    <<m_mdtHelper->show_to_string(channelId)<<">  =>>ignore this hit");
	    delete digit;
	    continue;
	  }
	  
	  if (digit->is_masked()) {
	    digitStatus = Muon::MdtStatusMasked;
	  }else{
	    digitStatus = getMdtDriftRadius(digit, radius, errRadius, descriptor);
	    if( radius < -999 ) {
	      ATH_MSG_WARNING("MDT PrepData with very large, negative radius "
			      << " Id is: "<<m_mdtHelper->show_to_string(channelId));
	    }
	  }
	  
	  Amg::Vector2D driftRadius(radius,0);
	  Amg::MatrixX* cov = new Amg::MatrixX(1,1);
	  (*cov)(0,0) = errRadius*errRadius;

	  
	  // Create new PrepData
	  MdtPrepData *newPrepData = new MdtPrepData(channelId,
						     mdtHashId,
						     driftRadius,
						     cov,
						     descriptor,
						     digit->tdc(),
						     digit->adc(),
						     digitStatus);
	  
	  newPrepData->setHashAndIndex(driftCircleColl->identifyHash(), driftCircleColl->size());
	  driftCircleColl->push_back(newPrepData);
	  
	  ATH_MSG_DEBUG(" MADE ORIGINAL PREPDATA " << m_mdtHelper->stationNameString(m_mdtHelper->stationName(channelId)) 
			<< "  eta = " << m_mdtHelper->stationEta(channelId)
			<< "  phi = " << m_mdtHelper->stationPhi(channelId)
			<< "  ml = " << m_mdtHelper->multilayer(channelId)
			<< "  layer = " << m_mdtHelper->tubeLayer(channelId) 
			<< "  tube = " <<  m_mdtHelper->tube(channelId) 
			<< "  radius = " << radius << " +- " << errRadius);
	  
	  delete digit;
	  delete second_digit;
	} //end  --  if(second_digit == NULL){
	else{
	  
	  // Do something with second_digit
	  Identifier     second_channelId   = second_digit->identify();
	  IdentifierHash second_channelHash = second_digit->identifyHash();
	  
	  ATH_MSG_VERBOSE("got digit with id ext / hash "
			  <<m_mdtHelper->show_to_string(channelId)<<" / "
			  <<(int)channelHash);
	  
	  //second_digit
	  ATH_MSG_VERBOSE("got second_digit with id ext / hash "
			  <<m_mdtHelper->show_to_string(second_channelId)<<" / "
			  <<(int)second_channelHash);
	  
	  //check to see if ID needs correcting & is BOG (i.e. station name = 10)
	  // Q. Is there a better way to do this by hard-coding numbers? Ed.
	  if (m_doIdentifierCorrection && 10==m_mdtHelper->stationName(channelId) )
	    {
	      int tube = m_mdtHelper->tube(channelId);
	      ATH_MSG_VERBOSE("Station name: "<<m_mdtHelper->stationName(channelId)
			      <<", string="<<m_mdtHelper->stationNameString(m_mdtHelper->stationName(channelId)));
	      int oldTube=tube;
	      
	      if ( (6<tube) && (tube<35) )
		tube--;
	      else if (tube>=35)
		tube-=2;
	      if (tube!=oldTube)
		{
		  // just use same values, apart from modified
		  channelId = m_mdtHelper->channelID( m_mdtHelper->stationName(channelId),
						      m_mdtHelper->stationEta(channelId),
						      m_mdtHelper->stationPhi(channelId),
						      m_mdtHelper->multilayer(channelId),
						      m_mdtHelper->tubeLayer(channelId),
						      tube );
		  ATH_MSG_VERBOSE("Changed tube from :"<<oldTube<<" to: "<<tube
				  << " Id is: "<<m_mdtHelper->show_to_string(channelId));
		}
	    }

	  //second_digit
	  //check to see if ID needs correcting & is BOG (i.e. station name = 10)
	  // Q. Is there a better way to do this by hard-coding numbers? Ed.
	  if (m_doIdentifierCorrection && 10==m_mdtHelper->stationName(second_channelId) )
	    {
	      int tube = m_mdtHelper->tube(second_channelId);
	      ATH_MSG_VERBOSE("Station name: "<<m_mdtHelper->stationName(second_channelId)
			      <<", string="<<m_mdtHelper->stationNameString(m_mdtHelper->stationName(second_channelId)));
	      int oldTube=tube;
	      
	      if ( (6<tube) && (tube<35) )
		tube--;
	      else if (tube>=35)
		tube-=2;
	      if (tube!=oldTube)
		{
		  // just use same values, apart from modified
		  second_channelId = m_mdtHelper->channelID( m_mdtHelper->stationName(second_channelId),
							     m_mdtHelper->stationEta(second_channelId),
							     m_mdtHelper->stationPhi(second_channelId),
							     m_mdtHelper->multilayer(second_channelId),
							     m_mdtHelper->tubeLayer(second_channelId),
							     tube );
		  ATH_MSG_VERBOSE("Changed tube from :"<<oldTube<<" to: "<<tube
				  << " Id is: "<<m_mdtHelper->show_to_string(second_channelId));
		}
	    }
	  
	  // Calculate radius
	  double radius(0.); double errRadius(0.);
	  Muon::MdtDriftCircleStatus digitStatus = Muon::MdtStatusDriftTime;
	
	  //second_digit
	  double second_radius(0.); double second_errRadius(0.);
	  Muon::MdtDriftCircleStatus second_digitStatus = Muon::MdtStatusDriftTime;
	  
	  // do lookup once
	  const MdtReadoutElement * descriptor = m_muonMgr->getMdtReadoutElement(channelId);
	  if (descriptor == NULL){
	    ATH_MSG_WARNING("Detector Element not found for Identifier from the cabling service <"
			    <<m_mdtHelper->show_to_string(channelId)<<">  =>>ignore this hit");
	    delete digit;
	    continue;
	  }
	  
	  if (digit->is_masked()) {
	    digitStatus = Muon::MdtStatusMasked;
	  }else{
	    digitStatus = getMdtDriftRadius(digit, radius, errRadius, descriptor);
	    if( radius < -999 ) {
	      ATH_MSG_WARNING("MDT PrepData with very large, negative radius "
			      << " Id is: "<<m_mdtHelper->show_to_string(channelId));
	    }
	  }

	  const MdtReadoutElement * second_descriptor = m_muonMgr->getMdtReadoutElement(second_channelId);
	  if (second_descriptor == NULL){
	    ATH_MSG_WARNING("Detector Element not found for Identifier from the cabling service <"
			    <<m_mdtHelper->show_to_string(second_channelId)<<">  =>>ignore this hit");
	    delete second_digit;
	    continue;
	  }
	  
	  //second_digit
	  if (second_digit->is_masked()) {
	    second_digitStatus = Muon::MdtStatusMasked;
	  }else{
	    second_digitStatus = getMdtDriftRadius(second_digit, second_radius, second_errRadius, second_descriptor);
	    if( second_radius < -999 ) {
	      ATH_MSG_WARNING("MDT PrepData with very large, negative radius "
			      << " Id is: "<<m_mdtHelper->show_to_string(second_channelId));
	    }
	  }
	  
	  Amg::Vector2D driftRadius(radius,0);
	  Amg::MatrixX* cov = new Amg::MatrixX(1,1);
	  (*cov)(0,0) = errRadius*errRadius;

	  
	  //second_digit
	  Amg::Vector2D second_driftRadius(second_radius,0);
	  Amg::MatrixX* cov2 = new Amg::MatrixX(1,1);
	  (*cov2)(0,0) = second_errRadius*second_errRadius;

	  // Create new PrepData
	  MdtPrepData *newPrepData = new MdtPrepData(channelId,
						     mdtHashId,
						     driftRadius,
						     cov,
						     descriptor,
						     digit->tdc(),
						     digit->adc(),
						     digitStatus);
	  
	  newPrepData->setHashAndIndex(driftCircleColl->identifyHash(), driftCircleColl->size());
	  driftCircleColl->push_back(newPrepData);
	  
	  //second_digit
	  // Create new PrepData
	  MdtPrepData *second_newPrepData = new MdtPrepData(second_channelId,
							    mdtHashId,
							    second_driftRadius,
							    cov2,
							    second_descriptor,
							    second_digit->tdc(),
							    second_digit->adc(),
							    second_digitStatus);
	  
	  second_newPrepData->setHashAndIndex(driftCircleColl->identifyHash(), driftCircleColl->size());
	  driftCircleColl->push_back(second_newPrepData);
	  
	  ATH_MSG_DEBUG(" MADE ORIGINAL PREPDATA " << m_mdtHelper->stationNameString(m_mdtHelper->stationName(channelId)) 
			<< "  eta = " << m_mdtHelper->stationEta(channelId)
			<< "  phi = " << m_mdtHelper->stationPhi(channelId)
			<< "  ml = " << m_mdtHelper->multilayer(channelId)
			<< "  layer = " << m_mdtHelper->tubeLayer(channelId) 
			<< "  tube = " <<  m_mdtHelper->tube(channelId) 
			<< "  radius = " << radius << " +- " << errRadius);
	  
	  //second_digit
	  ATH_MSG_DEBUG(" MADE ORIGINAL PREPDATA FOR SECOND DIGIT " << m_mdtHelper->stationNameString(m_mdtHelper->stationName(second_channelId))
			<< "  eta = " << m_mdtHelper->stationEta(second_channelId)
			<< "  phi = " << m_mdtHelper->stationPhi(second_channelId)
			<< "  ml = " << m_mdtHelper->multilayer(second_channelId)
			<< "  layer = " << m_mdtHelper->tubeLayer(second_channelId) 
			<< "  tube = " <<  m_mdtHelper->tube(second_channelId) 
			<< "  radius = " << second_radius << " +- " << second_errRadius);
	  
	  delete digit;
	  delete second_digit;
	} //end  --  else -- if(second_digit == NULL){
      }
      else{
	ATH_MSG_DEBUG("Something strange in MdtRdoToPrepDataTool, MDT multilayer (must be 1 or 2)= " << multilayer);
      }
      
      
    } // end for( iter_map = mdtDigitColl.begin(); iter_map != mdtDigitColl.end(); iter_map++ ) {
    
    
    return StatusCode::SUCCESS;
}


MdtDriftCircleStatus MdtRdoToPrepDataTool::getMdtDriftRadius(const MdtDigit * digit,
                                 double& radius, double& errRadius, const MuonGM::MdtReadoutElement * descriptor) {

  ATH_MSG_DEBUG("in getMdtDriftRadius()");

  if( m_calibratePrepData ){
    Identifier channelId = digit->identify();
    //const MdtReadoutElement * descriptor = m_muonMgr->getMdtReadoutElement(channelId);

    // here check validity
    // if invalid, reset flags
    if (!descriptor->containsId(channelId)) {
      radius = -1000.;
      ATH_MSG_WARNING("Identifier from the cabling service <"
                      <<m_mdtHelper->show_to_string(channelId)<<"> inconsistent with the geometry of detector element <"
                      <<m_mdtHelper->show_to_string(descriptor->identify())<<">  =>>ignore this hit");
      return MdtStatusUnDefined;
    }

    m_calibHit->setIdentifier( channelId );
    m_calibHit->setTdc( digit->tdc() );
    m_calibHit->setAdc( digit->adc() );
    m_calibHit->setGeometry(descriptor);      

    // use center (cached) to get the tube position instead of tubepos
    const Amg::Vector3D& position = descriptor->center(channelId);

    MdtCalibrationSvcInput inputData;
    double signedTrackLength = position.mag();
    inputData.tof = signedTrackLength*m_invSpeed;

    // double measured_perp = position.perp();
    // if (descriptor->getStationS() != 0.) {
    //   measured_perp = sqrt(measured_perp*measured_perp-
    //                        descriptor->getStationS()*descriptor->getStationS());
    // }
    // double measured_x = measured_perp * cos( position.phi() );
    // double measured_y = measured_perp * sin( position.phi() );
    // const Amg::Vector3D measured_position(measured_x, measured_y, position.z());
    // ATH_MSG_VERBOSE("Measured position is "<<measured_position);
    // MdtCalibHit calib_hit( channelId, digit->tdc(), digit->adc(), measured_position, descriptor );
    m_calibHit->setGlobalPointOfClosestApproach(position);

    bool drift_ok = m_calibrationSvc->driftRadiusFromTime(*m_calibHit,inputData,*m_mdtCalibSvcSettings,false);
    if (!drift_ok) {
      if( m_calibHit->driftTime() < 0. ) return MdtStatusBeforeSpectrum;
      else                             return MdtStatusAfterSpectrum;
    }
    radius = m_calibHit->driftRadius();
    errRadius = m_calibHit->sigmaDriftRadius();
    ATH_MSG_VERBOSE("Calibrated drift radius is "<<radius<<"+/-"<<errRadius);
  }
  else
    {
      radius = 0.;
      errRadius = 4.215; // 14.6/sqrt(12)
    }
  return MdtStatusDriftTime;
}


MdtDriftCircleStatus MdtRdoToPrepDataTool::getMdtTwinPosition(const MdtDigit * digit, const MdtDigit * second_digit, 
                                  double& radius, double& errRadius,
                                  double& zTwin, double& errZTwin, bool& secondHitIsPrompt) {

  ATH_MSG_DEBUG("in getMdtTwinPosition()");

  //  StatusCode status = StatusCode::SUCCESS;

  if( m_calibratePrepData ){
    
    // start digit
    Identifier channelId = digit->identify();
    //IdentifierHash channelHash = digit->identifyHash();
    const MdtReadoutElement * descriptor = m_muonMgr->getMdtReadoutElement(channelId);

    // here check validity
    // if invalid, reset flags
    if (descriptor == NULL){
      ATH_MSG_WARNING("getMdtTwinPosition(): Detector Element not found for Identifier from the cabling service <"
              <<m_mdtHelper->show_to_string(channelId)<<">  =>>ignore this hit");
      zTwin = -100000.;
      return MdtStatusUnDefined;
    }
    else if (!descriptor->containsId(channelId)){
      zTwin = -100000.;
      ATH_MSG_WARNING("getMdtTwinPosition(): Identifier from the cabling service <"
              <<m_mdtHelper->show_to_string(channelId)<<"> inconsistent with the geometry of detector element <"
              <<m_mdtHelper->show_to_string(descriptor->identify())<<">  =>>ignore this hit");
      return MdtStatusUnDefined;
    }
    
    Amg::Vector3D position = descriptor->tubePos(channelId);
    double measured_perp = position.perp();
    if (descriptor->getStationS() != 0.) {
      measured_perp = sqrt(measured_perp*measured_perp-descriptor->getStationS()*descriptor->getStationS());
    }
    double measured_x = measured_perp * cos( position.phi() );
    double measured_y = measured_perp * sin( position.phi() );
    const Amg::Vector3D measured_position(measured_x, measured_y, position.z());
    MdtCalibHit calib_hit = MdtCalibHit( channelId, digit->tdc(), digit->adc(), measured_position, descriptor);
    calib_hit.setGlobalPointOfClosestApproach(measured_position);
    double signedTrackLength =  measured_position.mag();
     
    // start second digit
    Identifier second_channelId = second_digit->identify();
    //IdentifierHash second_channelHash = second_digit->identifyHash();
    const MdtReadoutElement * second_descriptor = m_muonMgr->getMdtReadoutElement(second_channelId);
    
    // here check validity
    // if invalid, reset flags
    if (second_descriptor == NULL){
      ATH_MSG_WARNING("getMdtTwinPosition(): Detector Element not found for Identifier from the cabling service <"
              <<m_mdtHelper->show_to_string(second_channelId)<<">  =>>ignore this hit");
      zTwin = -100000.;
      return MdtStatusUnDefined;
    }
    else if (!second_descriptor->containsId(second_channelId)){
      zTwin = -100000.;
      ATH_MSG_WARNING("getMdtTwinPosition(): Identifier from the cabling service <"
              <<m_mdtHelper->show_to_string(second_channelId)<<"> inconsistent with the geometry of detector element <"
              <<m_mdtHelper->show_to_string(second_descriptor->identify())<<">  =>>ignore this hit");
      return MdtStatusUnDefined;
    }
    
    Amg::Vector3D second_position = second_descriptor->tubePos(second_channelId);
    double second_measured_perp = second_position.perp();
    if (second_descriptor->getStationS() != 0.) {
      second_measured_perp = sqrt(second_measured_perp*second_measured_perp-second_descriptor->getStationS()*second_descriptor->getStationS());
    }
    double second_measured_x = second_measured_perp * cos( second_position.phi() );
    double second_measured_y = second_measured_perp * sin( second_position.phi() );
    const Amg::Vector3D second_measured_position(second_measured_x, second_measured_y, second_position.z());
    MdtCalibHit second_calib_hit = MdtCalibHit( second_channelId, second_digit->tdc(), second_digit->adc(),
                                                second_measured_position,  second_descriptor );
    double second_signedTrackLength =  second_measured_position.mag();

    // calculate and calibrate radius for both hits and calculate twin position
    second_calib_hit.setGlobalPointOfClosestApproach(second_measured_position);


    bool second_ok = m_calibrationSvc->twinPositionFromTwinHits(calib_hit, second_calib_hit, signedTrackLength, second_signedTrackLength, secondHitIsPrompt);
    if (!second_ok){
      if( calib_hit.driftTime() < 0. || second_calib_hit.driftTime() < 0. ) return MdtStatusBeforeSpectrum;
      else                             return MdtStatusAfterSpectrum;
    } 
    
    // set radius and error to the prompt tube (tube that was actually hit by the muon)
    radius = calib_hit.driftRadius();
    errRadius = calib_hit.sigmaDriftRadius();
    if(secondHitIsPrompt){
      radius = second_calib_hit.driftRadius();
      errRadius = second_calib_hit.sigmaDriftRadius();
    }

    zTwin = second_calib_hit.xtwin();
    errZTwin = second_calib_hit.sigmaXtwin();

    ATH_MSG_VERBOSE(" Calibrated drift radius of prompt hit (of twin pair) is " << radius << "+/-" << errRadius);
    ATH_MSG_VERBOSE(" Calibrated twin coordinate = " << zTwin << "+/-" << errZTwin);

  } // end if(m_calibratePrepData)    
  else{
    radius = 0.;
    Identifier channelId = digit->identify();
    errRadius = m_muonMgr->getMdtReadoutElement(channelId)->innerTubeRadius()/sqrt(12); // 14.6/sqrt(12)
    zTwin = 0.;
    double tubelength = m_muonMgr->getMdtReadoutElement(channelId)->getTubeLength(m_mdtHelper->tubeLayer(channelId),m_mdtHelper->tube(channelId));
    errZTwin = tubelength/2.;
  }
  
  return MdtStatusDriftTime;
}

void MdtRdoToPrepDataTool::initDeadChannels(const MuonGM::MdtReadoutElement* mydetEl) {
  PVConstLink cv = mydetEl->getMaterialGeom(); // it is "Multilayer"
  int nGrandchildren = cv->getNChildVols();
  if(nGrandchildren <= 0) return;

  Identifier detElId = mydetEl->identify();

  int name = m_mdtHelper->stationName(detElId);
  int eta = m_mdtHelper->stationEta(detElId);
  int phi = m_mdtHelper->stationPhi(detElId);
  int ml = m_mdtHelper->multilayer(detElId);
  std::vector<Identifier> deadTubes;

  for(int layer = 1; layer <= mydetEl->getNLayers(); layer++){
    for(int tube = 1; tube <= mydetEl->getNtubesperlayer(); tube++){
      bool tubefound = false;
      for(unsigned int kk=0; kk < cv->getNChildVols(); kk++) {
        int tubegeo = cv->getIdOfChildVol(kk) % 100;
        int layergeo = ( cv->getIdOfChildVol(kk) - tubegeo ) / 100;
        if( tubegeo == tube && layergeo == layer ) {
          tubefound=true;
          break;
        }
        if( layergeo > layer ) break; // don't loop any longer if you cannot find tube anyway anymore
      }
      if(!tubefound) {
        Identifier deadTubeId = m_mdtHelper->channelID( name, eta, phi, ml, layer, tube );
        deadTubes.push_back( deadTubeId );
        ATH_MSG_VERBOSE("adding dead tube (" << tube  << "), layer(" <<  layer
                        << "), phi(" << phi << "), eta(" << eta << "), name(" << name
                        << "), multilayerId(" << ml << ") and identifier " << deadTubeId <<" .");
      }
    }
  }
  std::sort(deadTubes.begin(), deadTubes.end());
  m_DeadChannels[detElId] = deadTubes;
  return;
}
