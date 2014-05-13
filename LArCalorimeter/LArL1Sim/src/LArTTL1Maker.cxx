/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// +======================================================================+
// +                                                                      +
// + Author ........: F. Ledroit                                          +
// + Institut ......: ISN Grenoble                                        +
// + Creation date .: 09/01/2003                                          +
// +                                                                      +
// +======================================================================+
// 
// ........ includes
//
#include "LArL1Sim/LArTTL1Maker.h"
// .......... utilities
//
#include <math.h>
#include <fstream>
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
#include "CLHEP/Random/RandomEngine.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include "LArRawEvent/LArTTL1.h"
#include "LArRawEvent/LArTTL1Container.h"
#include "LArSimEvent/LArHitContainer.h"
#include "LArDigitization/LArHitList.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArID.h"
#include "CaloIdentifier/CaloID_Exception.h"
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "LArIdentifier/LArIdManager.h"
#include "LArTools/LArCablingService.h"
#include "CaloTriggerTool/CaloTriggerTowerService.h"
//
// ........ Event Header Files:
//
//#include "EventInfo/EventID.h"
//#include "EventInfo/EventInfo.h"
#include "EventInfo/PileUpTimeEventIndex.h"
//
// ........ Gaudi needed includes
//
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ListItem.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "StoreGate/StoreGateSvc.h"
#include "PathResolver/PathResolver.h"

// Pile up
#include "PileUpTools/PileUpMergeSvc.h"
// trigger time
#include "AthenaKernel/ITriggerTime.h"

//


using CLHEP::RandGaussZiggurat;


LArTTL1Maker::LArTTL1Maker(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator)
  , m_storeGateSvc("StoreGateSvc",name)
  , m_atRndmGenSvc("AtRndmGenSvc",name)
  , m_rndmEngineName("LArTTL1Maker")
  , m_rndmEngine(0)
  , m_hitmap(0)
  , m_fSamplKey("LArfSampl")
// + -------------------------------------------------------------------- +
// + Author ........: F. Ledroit                                          +
// + Creation date .: 09/01/2003                                          +
// + Subject: TTL1 Maker constructor                                     +
// + -------------------------------------------------------------------- +
{
//
// ........ default values of private data
//
  m_detectorStore         = 0;
  //m_storeGateSvc          = 0;
  m_chronSvc              = 0;
  m_mergeSvc              = 0;
  m_useTriggerTime        = false;
  m_triggerTimeToolName   = "CosmicTriggerTimeTool";
  p_triggerTimeTool       = 0;
  m_useMapfromStore = true;

  m_BeginRunPriority      = 100;

  m_cablingSvc            = 0;
  m_ttSvc                 = 0;
  m_lvl1Helper            = 0;
  m_emHelper              = 0;
  m_hecHelper             = 0;
  m_fcalHelper            = 0;

  m_SubDetectors             = "LAr_All";
  m_EmTTL1ContainerName      = "LArTTL1EM";
  m_HadTTL1ContainerName     = "LArTTL1HAD";

  m_EmBarrelHitContainerName = "LArHitEMB";
  m_EmEndCapHitContainerName = "LArHitEMEC";
  m_HecHitContainerName      = "LArHitHEC";
  m_ForWardHitContainerName  = "LArHitFCAL";

  m_NoiseOnOff               = true;
  m_PileUp                   = false;
  m_noEmCalibMode            = false;
  m_noHadCalibMode           = false;
  m_chronoTest               = false;
  m_debugThresh              = 5000.;

  m_calibCoeffEmb.resize(s_NBETABINS);
  m_calibCoeffEmec.resize(s_NBETABINS);
  m_calibCoeffHec.resize(s_NBETABINS);
  for(int ieta=0 ; ieta<s_NBETABINS ; ieta++) {
    m_calibCoeffEmb[ieta]=1.;
    m_calibCoeffEmec[ieta]=1.;
    m_calibCoeffHec[ieta]=1.;
  }
  const int nEta = 4;
  m_calibCoeffFcalEm.resize(nEta);
  m_calibCoeffFcalHad.resize(nEta);
  for(int ieta=0 ; ieta<nEta ; ieta++) {
    m_calibCoeffFcalEm[ieta]=.03;
    m_calibCoeffFcalHad[ieta]=.03;
  }

  //
  // ........ declare the private data as properties
  //

  declareProperty("EventStore",m_storeGateSvc,"StoreGate Service");
  declareProperty("SubDetectors",m_SubDetectors);
  declareProperty("RndmSvc", m_atRndmGenSvc);

  declareProperty("EmBarrelHitContainerName",m_EmBarrelHitContainerName);
  declareProperty("EmEndCapHitContainerName",m_EmEndCapHitContainerName);
  declareProperty("HecHitContainerName",m_HecHitContainerName);
  declareProperty("ForWardHitContainerName",m_ForWardHitContainerName);

  declareProperty("EmTTL1ContainerName",m_EmTTL1ContainerName);
  declareProperty("HadTTL1ContainerName",m_HadTTL1ContainerName);

  declareProperty("NoiseOnOff",m_NoiseOnOff);

  declareProperty("PileUp",m_PileUp);
  declareProperty("UseTriggerTime",m_useTriggerTime);
  declareProperty("TriggerTimeToolName",m_triggerTimeToolName);

  declareProperty("EmBarrelCalibrationCoeffs",m_calibCoeffEmb);
  declareProperty("EmEndCapCalibrationCoeffs",m_calibCoeffEmec);
  declareProperty("HECCalibrationCoeffs",m_calibCoeffHec);
  declareProperty("EmFcalCalibrationCoeffs",m_calibCoeffFcalEm);
  declareProperty("HadFcalCalibrationCoeffs",m_calibCoeffFcalHad);

  declareProperty("NoEmCalibrationMode",m_noEmCalibMode);
  declareProperty("NoHadCalibrationMode",m_noHadCalibMode);
  declareProperty("ChronoTest",m_chronoTest);
  declareProperty("DebugThreshold",m_debugThresh);

  declareProperty("useMapFromStore",m_useMapfromStore,"Use LArHitEMap already filled from detector store");

//
return;
}


LArTTL1Maker::~LArTTL1Maker()
{  
  /**  TTL1 Maker destructor                                      */

return;
}


StatusCode LArTTL1Maker::initialize()
{
// +======================================================================+
// +                                                                      +
// + Author ........: F. Ledroit                                          +
// + Creation date .: 09/01/2003                                          +
// +                                                                      +
// +======================================================================+
//
// ......... declaration
//
  MsgStream  msglog(messageService(),name());
  int outputLevel = msgSvc()->outputLevel( name() );

  m_chronSvc = chronoSvc();

  msglog << MSG::INFO 	
	 << "***********************************************"
	 << endreq;
  msglog << MSG::INFO 	
	 << "* Steering options for LArTTL1Maker algorithm *" 
	 << endreq;
  msglog << MSG::INFO 	
	 << "***********************************************"
	 << endreq;
  //
  // ......... print the noise flag
  //
  if ( m_NoiseOnOff )
  {
    msglog << MSG::INFO 
	   << "Electronic noise will be added in each TT for selected sub-detectors." 
	   << endreq;
  }
  else
  {
    msglog << MSG::INFO 
	   << "No electronic noise added." 
	   << endreq;
  }

//
// ......... print the pile-up flag
//
  if (m_PileUp)
  {
     msglog << MSG::INFO
            << "take events from PileUp service"
            << endreq;
  }
  else
  {
    msglog << MSG::INFO
           << "no pile up"
           << endreq;
  }

//
// ......... print the trigger time flag
//
  if (m_useTriggerTime) 
  {
     msglog << MSG::INFO
            << "use Trigger Time service " <<  m_triggerTimeToolName   
            << endreq;
  }
  else
  {
    msglog << MSG::INFO
           << "no Trigger Time used"
           << endreq;
  }

//
// ......... print the calibration flags
//
// currently the calib modes are not used anymore -> turning INFO logs into DEBUG
  if (m_noEmCalibMode)
  {
     msglog << MSG::DEBUG
            << "NO calibration mode chosen for EM towers "
            << " == technical option. Should not be used for physics !!! "
            << endreq;
  }
      else 
  {
    msglog << MSG::DEBUG
	   << "standard calibration chosen for EM towers"
	   << endreq;
  }

  if (m_noHadCalibMode)
  {
     msglog << MSG::DEBUG
            << "NO calibration mode chosen for HEC towers "
            << " == technical option. Should not be used for physics !!! "
            << endreq;
  }
  else {
     msglog << MSG::DEBUG
            << "standard calibration mode chosen for HEC towers "
            << endreq;
  }

//
// ....... Access Event StoreGate
//
  //StatusCode sc = service ( "StoreGateSvc" , m_storeGateSvc ) ;
  StatusCode sc = m_storeGateSvc.retrieve();
  if (sc.isFailure()) 
  {
    msglog << MSG::ERROR
	   << "Unable to access pointer to StoreGate"
	   << endreq;
    return StatusCode::FAILURE;
  }

//
// locate the PileUpMergeSvc and initialize our local ptr
//
  if (m_PileUp) {
    if (!(service("PileUpMergeSvc", m_mergeSvc)).isSuccess() ||	0 == m_mergeSvc) {
      msglog << MSG::ERROR 
	     << "Could not find PileUpMergeSvc" 
	     << endreq;
      return StatusCode::FAILURE;
    }
    else
      {
	msglog << MSG::DEBUG << "PileUpMergeSvc successfully initialized" << endreq;
      }
  }     
  
//
// .........retrieve tool computing trigger time if requested
//
  if (m_useTriggerTime && m_PileUp) {
       msglog << MSG::INFO 
        << " In case of pileup, the trigger time subtraction is done in PileUpSvc " 
        << endreq;
       msglog << MSG::INFO
        << "  => LArTTL1Maker will not apply Trigger Time " << endreq;
       m_useTriggerTime = false;
  }

  if (m_useTriggerTime) {
    IToolSvc* p_toolSvc = 0;
    if (!(service("ToolSvc", p_toolSvc)).isSuccess() ||	0 == p_toolSvc) {
      msglog << MSG::ERROR 
	     << "Could not find ToolSvc" 
	     << endreq;
      return StatusCode::FAILURE;
    }

    IAlgTool* algtool(0);
    ListItem theTool(m_triggerTimeToolName.value());
    sc = p_toolSvc->retrieveTool(theTool.type(), theTool.name(),algtool);
    if (sc.isFailure()) {
      msglog << MSG::ERROR
	     << "Unable to find tool for " << m_triggerTimeToolName.value() << endreq;
      p_triggerTimeTool = 0;
    }
     else {
       p_triggerTimeTool=dynamic_cast<ITriggerTime*>(algtool);
       msglog << MSG::DEBUG << "retrieved TriggerTime tool: " 
	      << m_triggerTimeToolName.value() << endreq;
     }
  }

  //
  // ..... need LAr and CaloIdManager to retrieve all needed helpers
  //

  const CaloIdManager* caloMgr;
  const LArIdManager*	 larMgr;
    //
    // ....... Access Detector StoreGate
    //
  sc = service( "DetectorStore", m_detectorStore );
  if ( sc.isSuccess( ) ) {
    sc = m_detectorStore->retrieve(caloMgr);
    if (sc.isFailure()) {
      msglog << MSG::ERROR << "Unable to retrieve CaloIdManager from DetectorStore" << endreq;
      return StatusCode::FAILURE;
    } else {
      if (outputLevel <= MSG::DEBUG) {
	msglog << MSG::DEBUG << "Successfully retrieved CaloIdManager from DetectorStore" << endreq;
      }
    }	
    sc = m_detectorStore->retrieve(larMgr);
    if (sc.isFailure()) {
      msglog << MSG::ERROR << "Unable to retrieve LArIdManager from DetectorStore" << endreq;
      return StatusCode::FAILURE;
    } else {
      if (outputLevel <= MSG::DEBUG) {
	msglog << MSG::DEBUG << "Successfully retrieved LArIdManager from DetectorStore" << endreq;
      }
    }	
  }   else {
    msglog << MSG::ERROR << "Could not locate DetectorStore" << endreq;
    return StatusCode::FAILURE;
  }
  
  //
  //..... need of course the LVL1 helper
  //
  m_lvl1Helper = caloMgr->getLVL1_ID();
  if (!m_lvl1Helper) {
    msglog << MSG::ERROR << "Could not access CaloLVL1_ID helper" << endreq;
    return StatusCode::FAILURE;
  } else {
    if (outputLevel <= MSG::DEBUG) {
      msglog << MSG::DEBUG << "Successfully accessed CaloLVL1_ID helper" << endreq;
    }
  }
  
  //
  //..... also need the LArEM helper (e.g. to deal with the barrel end part)
  //
  sc = m_detectorStore->retrieve(m_emHelper);
  if (sc.isFailure()) {
    msglog << MSG::ERROR << "Unable to retrieve LArEM helper from DetectorStore" << endreq;
    return StatusCode::FAILURE;
  } else {
    if (outputLevel <= MSG::DEBUG) {
      msglog << MSG::DEBUG << "Successfully retrieved LArEM helper from DetectorStore" << endreq;
    }
  }	
  
  //
  //..... also need the LArHEC helper to avoid adding up energy from 4th compartment
  //
  sc = m_detectorStore->retrieve(m_hecHelper);
  if (sc.isFailure()) {
    msglog << MSG::ERROR << "Unable to retrieve LArHEC helper from DetectorStore" << endreq;
    return StatusCode::FAILURE;
  } else {
    if (outputLevel <= MSG::DEBUG) {
      msglog << MSG::DEBUG << "Successfully retrieved LArHEC helper from DetectorStore" << endreq;
    }
  }	
  //
  //..... also need the LArFCAL helper to use hash ids to store all gains
  //
  sc = m_detectorStore->retrieve(m_fcalHelper);
  if (sc.isFailure()) {
    msglog << MSG::ERROR << "Unable to retrieve LArFCAL helper from DetectorStore" << endreq;
    return StatusCode::FAILURE;
  } else {
    if (outputLevel <= MSG::DEBUG) {
      msglog << MSG::DEBUG << "Successfully retrieved LArFCAL helper from DetectorStore" << endreq;
    }
  }	
  
  //
  // ..... need cabling services, to get channels associated to each TT
  //
  IToolSvc* toolSvc;
  StatusCode status   = service( "ToolSvc",toolSvc  );
  if(status.isSuccess()) {
    sc = toolSvc->retrieveTool("LArCablingService",m_cablingSvc);
    if(sc.isFailure()) {
      msglog << MSG::ERROR << "Could not retrieve LArCablingService"<< endreq;
      return(StatusCode::FAILURE);
    }
    sc = toolSvc->retrieveTool("CaloTriggerTowerService",m_ttSvc);
    if(sc.isFailure()) {
      msglog << MSG::ERROR << "Could not retrieve CaloTriggerTowerService"<< endreq;
      return(StatusCode::FAILURE);
    }
  } else      {
    msglog << MSG::ERROR << "Could not get ToolSvc"<< endreq;
    return(StatusCode::FAILURE);
  }
  
  // Incident Service: 
  IIncidentSvc* incSvc;
  sc = service("IncidentSvc", incSvc);
  if (sc.isFailure()) 
  {
    msglog << MSG::ERROR
	   << "Unable to retrieve pointer to DetectorStore "
	   << endreq;
    return StatusCode::FAILURE;
  }
  //start listening to "BeginRun"
  incSvc->addListener(this, "BeginRun",  m_BeginRunPriority);

  if (outputLevel <= MSG::DEBUG) {
    msglog << MSG::DEBUG 
	   << "Initialization completed successfully" 
	   << endreq;
  }

  sc = m_atRndmGenSvc.retrieve();
  if(sc.isFailure()) {
    msglog << MSG::ERROR << "Could not initialize random number service." << endreq;
    return sc;
  }
  m_rndmEngine = m_atRndmGenSvc->GetEngine(m_rndmEngineName);
  if(!m_rndmEngine) {
    msglog << MSG::ERROR << "Could not find RndmEngine : " << m_rndmEngineName << endreq;
    return StatusCode::FAILURE ;
  }

  return StatusCode::SUCCESS;

}



void LArTTL1Maker::handle(const Incident& /* inc*/ )
{
  MsgStream msglog( msgSvc(), name() );
  msglog << MSG::DEBUG << "LArTTL1Maker handle()" << endreq;

  StatusCode sc = this->retrieveDatabase();
  if (sc.isFailure()) {
    msglog << MSG::ERROR << " Error from retrieveDatabase " << endreq;
  }


  //
  // ...... init hit map 
  //
  if ( this->initHitMap() == StatusCode::FAILURE ) {
    msglog << MSG::ERROR << " Error from initHitMap() " << endreq;
  }

  //
  // ...... Read auxiliary data files
  //
  if ( this->readAuxiliary() == StatusCode::FAILURE ) {

    msglog << MSG::ERROR << " Error from readAuxiliary() " << endreq;

    }


  return;
}

StatusCode LArTTL1Maker::retrieveDatabase()
{
// can not do all that in initialize with the new calibration classes/Iov service

  MsgStream msglog( msgSvc(), name() );
  msglog << MSG::DEBUG << "LArTTL1Maker retrieveDatabase()" << endreq;
  StatusCode sc;

  sc = m_detectorStore->retrieve(m_dd_fSampl,m_fSamplKey);
  if (sc.isFailure() || !m_dd_fSampl)  {
    msglog << MSG::ERROR
	   << "Unable to retrieve ILArfSampl from DetectorStore"
	   << endreq;
    return StatusCode::FAILURE;
  } 

  return StatusCode::SUCCESS;
}



StatusCode LArTTL1Maker::execute()
{
// +======================================================================+
// +                                                                      +
// + Author: F. Ledroit                                                   +
// + Creation date: 2003/01/13                                            +
// + Subject: Make the TTL1s and put them into the container             +
// +                                                                      +
// +======================================================================+
  //
  // ......... declarations
  //
  MsgStream  msglog(messageService(),name());
  int outputLevel = msgSvc()->outputLevel( name() );

  if (outputLevel <= MSG::DEBUG) {
    msglog << MSG::DEBUG 
	   << "Begining of execution" 
	   << endreq;
  }

  //
  // ....... fill the LArHitEMap
  //
  if(m_chronoTest) {
    m_chronSvc->chronoStart( "fill LArHitEMap " );
  }

  int totHit=0;
  if ( this->fillEMap(totHit) == StatusCode::FAILURE ) return StatusCode::FAILURE;
  if (outputLevel <= MSG::DEBUG) {
    msglog << MSG::DEBUG << "total number of hits with |E|> 1.e-06 found = " << totHit << endreq;
  }
  

  if(m_chronoTest) {
    m_chronSvc->chronoStop ( "fill LArHitEMap " );
    m_chronSvc->chronoPrint( "fill LArHitEMap " );
  }

  if (!m_useMapfromStore && totHit==0) {
    msglog << MSG::WARNING << " No LAr hit in the event "  << endreq;
  }

  //
  // .....get the trigger time if requested
  //
  double trigtime=0;
  if (m_useTriggerTime && p_triggerTimeTool) {
     trigtime = p_triggerTimeTool->time();
  }
  if (outputLevel <= MSG::DEBUG) {
    msglog << MSG::DEBUG << "Trigger time used : " << trigtime << endreq;
  }

  //
  // ....... create the LAr TTL1 Containers
  //
  LArTTL1Container *ttL1ContainerEm = new LArTTL1Container();
  if ( ttL1ContainerEm == 0 ){
    msglog << MSG::ERROR 
	   << "Could not allocate a new LArTTL1Container" 
	   << endreq;
    return StatusCode::FAILURE;	  
  }
  LArTTL1Container *ttL1ContainerHad = new LArTTL1Container();
  if ( ttL1ContainerHad == 0 ){
    msglog << MSG::ERROR 
	   << "Could not allocate a second new LArTTL1Container" 
	   << endreq;
    return StatusCode::FAILURE;	  
  }

  //
  // ...... register the TTL1 containers into the TES 
  //
  StatusCode sc = m_storeGateSvc->record( ttL1ContainerEm ,  m_EmTTL1ContainerName) ;
  if( sc.isFailure() )
  {
    msglog << MSG::ERROR 
	   << "Could not record new LArTTL1Container in TES : " 
	   << m_EmTTL1ContainerName 
	   << endreq;
    return StatusCode::FAILURE;	  
  }
  sc = m_storeGateSvc->record( ttL1ContainerHad ,  m_HadTTL1ContainerName) ;
  if( sc.isFailure() )
  {
    msglog << MSG::ERROR 
	   << "Could not record second new LArTTL1Container in TES : " 
	   << m_HadTTL1ContainerName 
	   << endreq;
    return StatusCode::FAILURE;	  
  }

  //
  // ... initialise vectors for sums of energy in each TT
  //
  unsigned int nbTT = (unsigned int)m_lvl1Helper->tower_hash_max() ;
  if (outputLevel <= MSG::DEBUG) {
    msglog << MSG::DEBUG
  	   << "Max number of LAr Trigger Towers= " << nbTT 
	   << endreq;
  }
  std::vector<std::vector<float> > sumEnergy ;   // inner index = time slot (from 0 to visEvecSize-1)
  std::vector<std::vector<float> > sumEnergy2 ;  // to allow barrel/endcap separation in 15th TT + FCAL2/3
  sumEnergy.resize(nbTT);
  sumEnergy2.resize(nbTT);
  std::vector<float> ttSumE;
  int ttSumEvecSize=0;
  int refTime = 0;
  if(!m_PileUp) {
    ttSumEvecSize=2*25-1;
    refTime = 25-1;
  } else {
    ttSumEvecSize=s_MAXSAMPLES+s_NBSAMPLES-1;
    refTime = s_MAXSAMPLES-1;
  }
  if (outputLevel <= MSG::DEBUG) {
    msglog << MSG::DEBUG
	   << "Number of time slots considered= " << ttSumEvecSize
	   << " reference time= " << refTime
	   << endreq;
  }
  ttSumE.resize(ttSumEvecSize);
  for(unsigned int iTT=0;iTT<nbTT;iTT++){
    sumEnergy[iTT]=ttSumE;
    sumEnergy2[iTT]=ttSumE;
  }

  m_chronSvc->chronoStart( "LArTTL1Mk hit loop " );

  int it = 0;
  int it_end = m_hitmap->GetNbCells();
  msglog << MSG::DEBUG
	 << "Size of the hit map= " << it_end
	 << endreq;
  
  //
  // .... loop on hit lists in the map
  // .... and fill sumEnergy vector according to hit membership to TT
  // .... (one sum per time slot for each TT)
  //
  float outOfTimeE=0.;
  int nOutOfTime=0;
  float inTimeE=0.;
  int nInTime=0;
  float printEthresh=20.;
  int nMissingGain=0;
  for( ; it!=it_end;++it) {
    LArHitList * hitlist = m_hitmap->GetCell(it);
    if (hitlist != 0 ) {
      
      const std::vector<std::pair<float,float> >* timeE = hitlist->getData();
      if (timeE->size() > 0 ) {
	Identifier  cellId = hitlist->getIdentifier();

	int specialCase=0;
	bool skipCell=false;
	//
	// .... skip disconnected cells (normally not in the TT list) 
	//
	if(m_lvl1Helper->is_lar_em(cellId)) {
	  if(!m_emHelper->is_connected(cellId)) skipCell=true;
	} 
	else if(m_lvl1Helper->is_lar_hec(cellId)) {
	  if(!m_hecHelper->is_connected(cellId)) skipCell=true;
	}
	else if(m_lvl1Helper->is_lar_fcal(cellId)) {
	  if(!m_fcalHelper->is_connected(cellId)) skipCell=true;
	}
	//
	// ...  skip cells not summed up in LVL1 (end of barrel PS and 4th compartment of HEC)
	//
	if(!m_ttSvc->is_in_lvl1(cellId)) skipCell=true;

	if(!skipCell) {
	  //
	  // ...determine to which TT this channel belongs
	  //
	  Identifier ttId = m_ttSvc->whichTTID(cellId);

	  if(m_chronoTest) {
	    m_chronSvc->chronoStart( "retrieve RG " );
	  }
	  
	  //
	  // ....... determine the sampling fraction
	  //........ and the relative (layer) gains for this cellId
	  //	
	  float cellSampFraction = m_dd_fSampl->FSAMPL(cellId);
	  //	  std::cout << "cellid, SF= " << m_lvl1Helper->show_to_string(cellId) << " " << cellSampFraction << std::endl;
	  float relGain = 0.;
	  float sinTheta = 0.;

 	  int region = m_lvl1Helper->region(ttId);
 	  int eta = m_lvl1Helper->eta(ttId);
 	  int Ieta = decodeInverse(region,eta);
	  
	  // case EM cell
	  if(m_lvl1Helper->is_lar_em(cellId)) {
	    bool barrelEnd = m_lvl1Helper->is_barrel_end(ttId);
	    std::vector<float> vecRG ;
	    if(m_emHelper->is_em_barrel(cellId)) {
	      // Barrel
	      sinTheta = m_sinThetaEmb[Ieta-1];  // Ieta starts at 1
	    } else {
	      // End-Cap
	      if(!barrelEnd) {
		sinTheta = m_sinThetaEmec[Ieta-1];			
	      } else {
		// patching the fact that TT offline ID is ambiguous
                // decodeInverse(eta,region) returns 15 (ok barrel) instead of 1 (ec value).
		sinTheta = m_sinThetaEmec[0];			
		specialCase = 1;
	      msglog << MSG::VERBOSE << " special case " 
		     << m_emHelper->show_to_string(ttId) << endreq ;
	      }
	    }
	    relGain = 1.;  // no relative gain for EMB and EMEC
	  }
	  
	  // case HEC cell
	  else if(m_lvl1Helper->is_lar_hec(cellId)) {
	    sinTheta = m_sinThetaHec[Ieta-1];  // Ieta starts at 1
	    relGain = 1.;
	  }
	  
	  // case FCAL cell
	  else if(m_lvl1Helper->is_lar_fcal(cellId)) {
	    IdentifierHash fcalHash = m_fcalHelper->channel_hash(cellId);
	    relGain = m_cellRelGainFcal [fcalHash] ;
	    if(relGain < 0.001)  {
	      nMissingGain++;
	      msglog << MSG::WARNING << " No relative gain value found for FCAL cell " 
		     << m_emHelper->show_to_string(cellId) << " (index " << fcalHash  
		     << "), setting default value 1. " << endreq ;
	      relGain = 1. ;
	    }
	    sinTheta = 1.;   // this factor is included in the relative gain
	  }
	  if(m_chronoTest) {
	    m_chronSvc->chronoStop( "retrieve RG " );
	    m_chronSvc->chronoPrint( "retrieve RG " );
	  }
	  
 	  IdentifierHash ttHash = m_lvl1Helper->tower_hash(ttId) ;
	  
	  //
	  // .... loop on hits in hit list
	  //
	  std::vector<std::pair<float,float> >::const_iterator first = timeE->begin();
	  std::vector<std::pair<float,float> >::const_iterator last  = timeE->end();
	  while (first != last) {
	    float hitEnergy = (*first).first;
	    float hitTime   = (*first).second - trigtime;
	    if(hitTime>99000.) {
	      if(hitEnergy > printEthresh) {
		msglog << MSG::WARNING 
		       << " Found pathological hit time, cellId= " << m_emHelper->show_to_string(cellId) 
		       << "  time= " << hitTime 
		       << "  energy= " << hitEnergy 
		       << endreq;
	      }
	    //  provisionnaly fix a bug in PileUpEvent. Normally not needed starting from 10.5.0
	      hitTime-=100000.;
	    }
	    // remove pathological energies (found in some Grid simulated DC2/Rome samples)
	    if (fabs(hitEnergy)>1e+9) {
	      msglog << MSG::WARNING << " Pathological energy ignored cellId= "
		     << m_emHelper->show_to_string(cellId) << "  energy= " << hitEnergy << endreq;
	      hitEnergy = 0.;
	      hitTime = 0.;
	    }
	    //
	    // ....determine time with respect to ref shape 
	    //
	    int iShift = 0;
	    if(!m_PileUp) {
	      // keep 1ns granularity
	      //iShift = (int)(hitTime+0.5);
	      iShift = static_cast<int>(floor(hitTime+0.5));
	    } else {
	      // round to 25ns
	      //iShift = (int)(hitTime/25.+0.5);
	      iShift = static_cast<int>(floor(hitTime/25.+0.5));
	    }
	    //
	    // ....make time positive to allow using it as an index
	    //
	    int iTime = iShift + refTime ;
	    //
	    // .... keep only hits in the timing window
	    //
	    if(iTime>=0 && iTime < ttSumEvecSize ) {
	      
	      if(!specialCase) {
		// standard case
		// ....... make the energy sum
		ttSumE = sumEnergy[ttHash];
		ttSumE[iTime]         += hitEnergy / cellSampFraction * sinTheta * relGain;
		sumEnergy[ttHash] = ttSumE;
	      }
	      else  {
		// ec part of barrel-end or FCAL3
		// ....... make the energy sum
		ttSumE = sumEnergy2[ttHash];
		ttSumE[iTime]          += hitEnergy / cellSampFraction * sinTheta * relGain;
		sumEnergy2[ttHash] = ttSumE;
	      }
	      //	      msglog << MSG::VERBOSE << "applied relative layer gain " << relGain 
	      //		     << " to a hit of cell " << m_emHelper->show_to_string(cellId) << endreq; 
	      inTimeE+=hitEnergy;
	      nInTime++;
	    } // only hits in timing window
	    else {
	      outOfTimeE+=hitEnergy;
	      nOutOfTime++;
	      if(hitEnergy >  printEthresh) {
		if(!m_PileUp) {
		  msglog << MSG::DEBUG
			 << "Found a hit out of the timing window, hitTime= " << hitTime
			 << " with more than " << printEthresh << " MeV: hitEnergy= " << hitEnergy << " MeV"
			 << endreq;
		}
		else {
		  if (outputLevel <= MSG::VERBOSE) {
		    msglog << MSG::VERBOSE
			   << "Found a hit out of the timing window, hitTime= " << hitTime
			   << " with more than " << printEthresh << " MeV: hitEnergy= " << hitEnergy << " MeV"
			   << endreq;
		  }
		}
	      }
	    }
	    ++first;
	  } // end of loop on hits in the list    
	} // skip cell condition
      } // check timeE->size() > 0
    } // check hitlist > 0
  } // end of loop on hit lists
  
    msglog << MSG::DEBUG
	   << "Number of missing relative FCAL gains for this event = "
	   <<  nMissingGain
	   << endreq;
  if (outputLevel <= MSG::VERBOSE) {
    if (inTimeE == 0 || nInTime == 0)
      msglog << MSG::VERBOSE << "No in time energy" << endreq;
    else
      msglog << MSG::VERBOSE
             << "Out of time energy = " << outOfTimeE << " MeV"
             << " represents " <<  100.* outOfTimeE/inTimeE
             << " % of in time energy for " << nOutOfTime << " (" << 100.*nOutOfTime/nInTime << " %) hits"
             << endreq;
  }
  if(outOfTimeE > 0.02*inTimeE) {
    msglog << MSG::WARNING
	   << "Out of time energy = " << outOfTimeE << " MeV"
	   << " larger than 2% of in time energy = " << inTimeE << " MeV; nb of out of time hits = " 
	   << nInTime << " (" 
           <<  (nInTime > 0 ? 100.*nOutOfTime/nInTime : 0)
           << " %)"
	   << endreq;
  }

  if(m_chronoTest) {
    m_chronSvc->chronoStop( "LArTTL1Mk hit loop " );
    m_chronSvc->chronoPrint( "LArTTL1Mk hit loop " );
    m_chronSvc->chronoStart( "LArTTL1Mk TT loop " );
  }

  std::vector<std::string> emHadString(2);
  emHadString[0]="ElectroMagnetic";
  emHadString[1]="Hadronic";
	      
  std::vector<Identifier>::const_iterator itTt  = m_lvl1Helper->tower_begin() ;
  std::vector<Identifier>::const_iterator itEnd = m_lvl1Helper->tower_end() ;

  //
  // ....... loop on Trigger Towers
  // ....... and build signals using pulse shape and noise for each TT
  //
  if (outputLevel <= MSG::DEBUG) {
    msglog << MSG::DEBUG
	   << " Starting loop on Trigger Towers " 
	   << endreq;
  }
  for(; itTt!=itEnd;++itTt){
	    
    Identifier towerId = (*itTt) ;
    
    //
    // ........ skip Tile cal
    //
    if(!m_lvl1Helper->is_tile(towerId)){  

      int emHad = m_lvl1Helper->sampling(towerId); 
      // convert offline id to hash id
      IdentifierHash ttHash = m_lvl1Helper->tower_hash(towerId) ;
      int region = m_lvl1Helper->region(towerId);
      int eta = m_lvl1Helper->eta(towerId);
      int Ieta = decodeInverse(region,eta);
      
      //
      // .... compute the signal for current trigger tower
      //
      if(m_chronoTest) {
	m_chronSvc->chronoStart( "compute signal " );
      }
      std::vector<float> analogSum (s_NBSAMPLES) ;
      std::vector<float> analogSum2(s_NBSAMPLES) ;
      std::vector<float> sumTTE	 = sumEnergy[ttHash];
      std::vector<float> sumTTE2 = sumEnergy2[ttHash];
      int nSpecialCase=0;
	

      bool hasE = false; 
      for ( unsigned int i=0; i<sumTTE.size();++i){
	if (fabs(sumTTE[i]) > 0.){
	  hasE=true;
	  break;
	}
      }

      bool hasE2 = false; 
      for ( unsigned int i=0; i<sumTTE2.size();++i){
	if (fabs(sumTTE2[i]) > 0.){
	  hasE2=true;
	  break;
	}
      }


      //if ( fabs(sumTTE[refTime]) > 0.|| fabs(sumTTE2[refTime]) > 0. ) {
      if ( hasE || hasE2 ) {
	
	//if(fabs(sumTTE[refTime]) > 0.) {
	if(  hasE ) {
	  analogSum = computeSignal(towerId,Ieta,0,sumTTE,refTime) ;
	}
	
	// treat special case of ec part of the "barrel end" and special case of FCAL2/FCAL3
	// fix me !! this way, we double the saturation energy in this tower...(because it is cut into 2 pieces)
	//if(fabs(sumTTE2[refTime]) > 0.) {
	if(  hasE2) {
	  nSpecialCase+=1;
	  if(nSpecialCase>1) {
	    msglog << MSG::WARNING
		   << " more than 1 special case, current Trigger Tower is " << emHadString[emHad] << ": "
		   << m_emHelper->show_to_string(towerId) << " " 
		   << endreq;
	  }
	  analogSum2 = computeSignal(towerId,Ieta,1,sumTTE2,refTime) ;
	  for( int isamp=0 ; isamp < s_NBSAMPLES ; isamp++) {
	    analogSum[isamp] += analogSum2[isamp] ;
	  }
	}

	if ( sumTTE[refTime] > m_debugThresh) {
	  if (outputLevel <= MSG::DEBUG) {
	    msglog << MSG::DEBUG
		   << " current Trigger Tower is " << emHadString[emHad] << ": "
		   << m_emHelper->show_to_string(towerId) << " " 
		   << endreq;
	    msglog << MSG::DEBUG
		   << " transverse E (i.e. sum E / sampling fraction * sin_theta * rel gain)= (at ref. time, before calib)" 
		   << sumTTE[refTime] << " + " << sumTTE2[refTime] << " (special cases) " 
		   << endreq;
	  }
	}
	else if ( sumTTE[refTime] > 0.) {
	  if (outputLevel <= MSG::VERBOSE) {
	    msglog << MSG::VERBOSE
		   << " current Trigger Tower is "  << emHadString[emHad] << ": "
		   << m_emHelper->show_to_string(towerId) << " " 
		   << endreq;
	    msglog << MSG::VERBOSE
		   << " [very low] transverse E (i.e. sum E / sampling fraction * sin_theta * rel gain)= (at ref. time, before calib)" 
		   << sumTTE[refTime] << " + " << sumTTE2[refTime] << " (special cases) " 
		   << endreq;
	  }
	}
	
      }
      if(m_chronoTest) {
	m_chronSvc->chronoStop ( "compute signal " );
	m_chronSvc->chronoPrint( "compute signal " );
      }
      
      //
      // ........ add the noise
      //
      if(m_chronoTest) {
	m_chronSvc->chronoStart( "adding noise " );
      }
      std::vector<float> fullSignal(s_NBSAMPLES) ;
      if(m_NoiseOnOff) {
	fullSignal = computeNoise(towerId,Ieta,analogSum);
      } else {
	fullSignal  = analogSum;
      }

      if(m_chronoTest) {
	m_chronSvc->chronoStop ( "adding noise " );
	m_chronSvc->chronoPrint ( "adding noise " );
      }
      
      if(sumTTE[refTime] > m_debugThresh) {
	if (outputLevel <= MSG::DEBUG) {
	  msglog << MSG::DEBUG
		 << " uncalibrated amplitudes around peak (+-3 time slots): " 
		 << sumTTE[refTime-3]  << ", "
		 << sumTTE[refTime-2]  << ", "
		 << sumTTE[refTime-1]  << ", "
		 << sumTTE[refTime]  << ", "
		 << sumTTE[refTime+1]  << ", "
		 << sumTTE[refTime+2]  << ", "
		 << sumTTE[refTime+3]
		 << endreq;
	  msglog << MSG::DEBUG
		 << " calibrated signal is " 
		 << analogSum[0] << ", "
		 << analogSum[1] << ", "
		 << analogSum[2] << ", "
		 << analogSum[3] << ", "
		 << analogSum[4] << ", "
		 << analogSum[5] << ", "
		 << analogSum[6] 
		 << endreq;
	  msglog << MSG::DEBUG
		 << " shape of calibrated signal is " 
		 << analogSum[0]/analogSum[3] << ", "
		 << analogSum[1]/analogSum[3] << ", "
		 << analogSum[2]/analogSum[3] << ", "
		 << analogSum[3]/analogSum[3] << ", "
		 << analogSum[4]/analogSum[3] << ", "
		 << analogSum[5]/analogSum[3] << ", "
		 << analogSum[6]/analogSum[3] 
		 << endreq;
	  msglog << MSG::DEBUG
		 << " after adding noise, full signal is " 
		 << fullSignal[0] << ", "
		 << fullSignal[1] << ", "
		 << fullSignal[2] << ", "
		 << fullSignal[3] << ", "
		 << fullSignal[4] << ", "
		 << fullSignal[5] << ", "
		 << fullSignal[6] 
		 << endreq;
	}
	if (outputLevel <= MSG::VERBOSE) {
	  for (unsigned int iTime=0; iTime<sumTTE.size(); iTime++) {
	    msglog << MSG::VERBOSE
		   << " iTime [range=0-28 or 0-299] = " << iTime
		   << " hit energy = " << sumTTE[iTime] 
		   << endreq;
	  }
	}
      } else if(sumTTE[refTime] > 0.) {
	if (outputLevel <= MSG::VERBOSE) {
	  msglog << MSG::VERBOSE
		 << " uncalibrated amplitudes around peak (+-3 time slots): " 
		 << sumTTE[refTime-3]  << ", "
		 << sumTTE[refTime-2]  << ", "
		 << sumTTE[refTime-1]  << ", "
		 << sumTTE[refTime]  << ", "
		 << sumTTE[refTime+1]  << ", "
		 << sumTTE[refTime+2]  << ", "
		 << sumTTE[refTime+3]
		 << endreq;
	  msglog << MSG::VERBOSE
		 << " calibrated signal is " 
		 << analogSum[0] << ", "
		 << analogSum[1] << ", "
		 << analogSum[2] << ", "
		 << analogSum[3] << ", "
		 << analogSum[4] << ", "
		 << analogSum[5] << ", "
		 << analogSum[6] 
		 << endreq;
	  msglog << MSG::VERBOSE
		 << " shape of calibrated signal is " 
		 << analogSum[0]/analogSum[3] << ", "
		 << analogSum[1]/analogSum[3] << ", "
		 << analogSum[2]/analogSum[3] << ", "
		 << analogSum[3]/analogSum[3] << ", "
		 << analogSum[4]/analogSum[3] << ", "
		 << analogSum[5]/analogSum[3] << ", "
		 << analogSum[6]/analogSum[3] 
		 << endreq;
	  msglog << MSG::VERBOSE
		 << " after adding noise, full signal is " 
		 << fullSignal[0] << ", "
		 << fullSignal[1] << ", "
		 << fullSignal[2] << ", "
		 << fullSignal[3] << ", "
		 << fullSignal[4] << ", "
		 << fullSignal[5] << ", "
		 << fullSignal[6] 
		 << endreq;
	}
      }
      
      if(fabs(fullSignal[s_PEAKPOS]) > 0.) {
	//
	// ...... create the LArTTL1 and push it into the appropriate TTL1 container
	//
	LArTTL1 * ttL1 ;
	HWIdentifier ttChannel;
	ttL1 = new LArTTL1(ttChannel,towerId,fullSignal);
	if (emHad) { ttL1ContainerHad->push_back(ttL1); }
	else       { ttL1ContainerEm->push_back(ttL1); }
      }
      
    }  // end excluding Tile cal
  } // end of for loop on TT
  if(m_chronoTest) {
    m_chronSvc->chronoStop ( "LArTTL1Mk TT loop " );
    m_chronSvc->chronoPrint ( "LArTTL1Mk TT loop " );
  }
  
  msglog << MSG::DEBUG << "number of created TTL1s (Em, Had) = " 
	 << ttL1ContainerEm->size() 
	 << " , "
	 << ttL1ContainerHad->size() 
	 << endreq;

  //
  // ...... lock the TTL1 containers
  //
  sc = m_storeGateSvc->setConst( ttL1ContainerEm ) ;
  if( sc.isFailure() )
  {
    msglog << MSG::ERROR 
	   << "Could not lock em LArTTL1Container : " 
	   << m_EmTTL1ContainerName 
	   << endreq;
    return StatusCode::FAILURE;	  
  }
  sc = m_storeGateSvc->setConst( ttL1ContainerHad ) ;
  if( sc.isFailure() )
  {
    msglog << MSG::ERROR 
	   << "Could not lock had LArTTL1Container : " 
	   << m_HadTTL1ContainerName 
	   << endreq;
    return StatusCode::FAILURE;	  
  }


  return StatusCode::SUCCESS;

}


StatusCode LArTTL1Maker::finalize()
{
// +======================================================================+
// +                                                                      +
// + Author: F. Ledroit                                                   +
// + Creation date: 2003/01/13                                            +
// +                                                                      +
// +======================================================================+
//
// ......... declaration
//
  MsgStream  msglog(messageService(),name());

  msglog << MSG::INFO << " LArTTL1Maker finalize completed successfully" 
	 << endreq;

  m_chronSvc->chronoPrint( "LArTTL1Mk hit loop " );
  m_chronSvc->chronoPrint( "LArTTL1Mk TT loop " );
  
  return StatusCode::SUCCESS;

}


std::vector<float> LArTTL1Maker::computeSignal(const Identifier towerId, const int Ieta, const int specialCase,
					       std::vector<float> ttSumEnergy, const int refTime) const

{
// +======================================================================+
// +                                                                      +
// + Author: F. Ledroit                                                  +
// + Creation date: 2003/01/13                                            +
// +                                                                      +
// +======================================================================+
//

  MsgStream  msglog(messageService(),name());
  int outputLevel = msgSvc()->outputLevel( name() );

  std::vector<float> bareSignal(s_NBSAMPLES) ;
  std::vector<float> pulseShape(s_MAXSAMPLES) ;
  std::vector<float> pulseShapeDer(s_MAXSAMPLES) ;

  bool emb = m_lvl1Helper->is_emb(towerId);
  bool barrelEnd = m_lvl1Helper->is_barrel_end(towerId);
  bool emec= m_lvl1Helper->is_emec(towerId);

  unsigned int visEvecSize=ttSumEnergy.size();
  if (outputLevel <= MSG::VERBOSE) {
    msglog << MSG::VERBOSE << "computeSignal: special case = " 
	   << specialCase  << endreq ;
  }

  //
  // ..... case EM
  //
  if (emb || barrelEnd || emec) {
    //
    // ..... retrieve calib coeffs for the tower
    //
    float calibCoeff=0.;
    if (emb) {
      calibCoeff = m_calibCoeffEmb[Ieta-1];
    }
    else if (emec) {
      calibCoeff = m_calibCoeffEmec[Ieta-1];
    }
    else {  // barrelEnd
      if(!specialCase) {
	calibCoeff = m_calibCoeffEmb[14];
      }
      else {
	calibCoeff = m_calibCoeffEmec[0];
      }
    }
    if(calibCoeff < 0.001) {
      msglog << MSG::WARNING << " No calibration coefficient value found for tower " 
	     << m_emHelper->show_to_string(towerId) 
	     << " setting default value 6. "  
	     << endreq ;
      calibCoeff = 6.;
    }

    //
    // ... loop on time samples 
    //
    for (unsigned int iTime=0; iTime<visEvecSize; iTime++) {
      if(fabs(ttSumEnergy[iTime]) > 0.) {
	if(!m_noEmCalibMode) {
	  // apply calibration coefficient
	  ttSumEnergy[iTime] *= calibCoeff;
	  if (outputLevel <= MSG::VERBOSE) {
	    msglog << MSG::VERBOSE << " ComputeSignal: applied EM calibration coefficient (iTime) " 
		   << calibCoeff << " (" << iTime << ") " << endreq;
	  }
	}
	
	// with respect to saturation
	// to compute the amplitude
	float theEnergy = ttSumEnergy[iTime] ;
	if( ttSumEnergy[iTime] > m_refEnergyEm [0]) {
	  theEnergy = m_refEnergyEm [0] ;    
	}
	// to determine the shape
	if( ttSumEnergy[iTime] > m_refEnergyEm [s_NBENERGIES-1]) {
	  // don't know the shape after highest energy point -> stick to 'last' shape
	  ttSumEnergy[iTime] = m_refEnergyEm [s_NBENERGIES-1] ;
	}
	
	//
	// ... determine regime: linear(iene=0) or saturation
	//
	for (int iene = 0 ; iene < s_NBENERGIES ; iene++) {
	  if ( ttSumEnergy[iTime] <= m_refEnergyEm[iene] ) {
	    pulseShape = m_pulseShapeEm[iene] ;  
	    pulseShapeDer = m_pulseShapeDerEm[iene] ;  

	    // make samples
	    for( int iSamp=0 ; iSamp < s_NBSAMPLES ; iSamp++) {
	      if(!m_PileUp) {
		// go back to signed value of time
		int hitTime = iTime - refTime;
		// go to 25 ns sampling
		//int time = (int)(hitTime/25.+0.5) ;
		int time = static_cast<int>(floor(hitTime/25.+0.5));
		// ....determine fine shift within 25ns 
		float dTime = (float)(hitTime - 25*time);
		int j= iSamp - time ;
		if(j >=0 && j < s_MAXSAMPLES) {
		  bareSignal[iSamp] += (pulseShape[j] - pulseShapeDer[j]*dTime) * theEnergy ;
		}
	      }
	      else {
		int j= iSamp - iTime + refTime ;
		if(j >=0 && j < s_MAXSAMPLES) {
		  bareSignal[iSamp] += pulseShape[j] * theEnergy ;
		}
	      }
	    } // end loop on samples
	  
	    break ;
	  }
	} // end of loop on reference energies
      } // end condition visE>0
    } // end loop on times
  } // end case EM
  //
  // ..... case HEC
  //
  else if (m_lvl1Helper->is_hec(towerId)) {
    pulseShape = m_pulseShapeHec ;
    pulseShapeDer = m_pulseShapeDerHec ;
    //
    // ... loop on time samples 
    //
    for (unsigned int iTime=0; iTime<visEvecSize; iTime++) {
      float theEnergy = ttSumEnergy[iTime];
      if(!m_noHadCalibMode) {
	// apply calibration coefficient
	theEnergy *= m_calibCoeffHec[Ieta-1];
      }

      float satEt = m_satEnergyHec[Ieta-1];  
      for( int iSamp=0 ; iSamp < s_NBSAMPLES ; iSamp++) {
	if(!m_PileUp) {
	  // go back to signed value of time
	  int hitTime = iTime - refTime;
	  // go to 25 ns sampling
	  //int time = (int)(hitTime/25.+0.5) ;
	  int time = static_cast<int>(floor(hitTime/25.+0.5));
	  // ....determine fine shift within 25ns 
	  float dTime = (float)(hitTime - 25*time);
	  int j= iSamp - time ;
	  if(j >=0 && j < s_MAXSAMPLES) {
	    bareSignal[iSamp] += (pulseShape[j] - pulseShapeDer[j]*dTime) * theEnergy ;
	  }
	}
	else {
	  int j= iSamp - iTime + refTime ;
	  if(j >=0 && j < s_MAXSAMPLES) {
	    bareSignal[iSamp] += pulseShape[j] * theEnergy ;
	  }
	}
	// saturation
	if ( bareSignal[iSamp] > satEt) {
	  bareSignal[iSamp] = satEt;
	}
      }
    }
  }
  //
  // ..... case FCAL
  //
  else if (m_lvl1Helper->is_fcal(towerId)) {

    int emOrHad = m_lvl1Helper->sampling(towerId) ;
    int module = emOrHad + 1 ;
    std::vector<float> calibCoeff2= m_calibCoeffFcal[module-1];
    if( emOrHad && (Ieta==3 || Ieta==4) ) {
      module+=1; // FCAL3
    }
    pulseShape = m_pulseShapeFcal[module-1] ;  
    pulseShapeDer = m_pulseShapeDerFcal[module-1] ;  
    
    //
    // ... loop on time samples (only one if no pile-up)
    //
    for (unsigned int iTime=0; iTime<visEvecSize; iTime++) {
      float theEnergy = ttSumEnergy[iTime];
      if((!m_noEmCalibMode&&module==1)||(!m_noHadCalibMode&&module>1)) {
	// apply calibration coefficient
	theEnergy *= calibCoeff2[Ieta-1];
      }
      
      // make samples
      for( int iSamp=0 ; iSamp < s_NBSAMPLES ; iSamp++) {
	if(!m_PileUp) {
	  // go back to signed value of time
	  int hitTime = iTime - refTime;
	  // go to 25 ns sampling
	  //int time = (int)(hitTime/25.+0.5) ;
	  int time = static_cast<int>(floor(hitTime/25.+0.5));
	  // ....determine fine shift within 25ns 
	  float dTime = (float)(hitTime - 25*time);
	  int j= iSamp - time ;
	  if(j >=0 && j < s_MAXSAMPLES) {
	    bareSignal[iSamp] += (pulseShape[j] - pulseShapeDer[j]*dTime) * theEnergy ;
	  }
	}
	else {
	  int j= iSamp - iTime + refTime ;
	  if(j >=0 && j < s_MAXSAMPLES) {
	    bareSignal[iSamp] += pulseShape[j] * theEnergy ;
	  }
	}
      }
    }
  }
  else {  
    msglog << MSG::WARNING << " LArTTL1Maker: computeSignal unknown towerId " 
	   << m_lvl1Helper->show_to_string(towerId)
	   << endreq;
    return  bareSignal ;
  }

  return  bareSignal ;

}


std::vector<float> LArTTL1Maker::computeNoise(const Identifier towerId, const int Ieta,  
					      std::vector<float>& inputV) 
{

// +======================================================================+
// +                                                                      +
// + Author: F. Ledroit                                                   +
// + Creation date: 2003/01/13                                            +
// +                                                                      +
// +======================================================================+

  MsgStream  msglog(messageService(),name());

  std::vector<float> outputV (s_NBSAMPLES) ;

  //
  // .... first retrieve auto-correlation matrix and noise rms
  //

  float sigmaNoise  = 0; 
  std::vector<float>* autoC = 0; 
  std::vector<float> noiseRms(4) ;

  bool emb = m_lvl1Helper->is_emb(towerId);
  bool barrelEnd = m_lvl1Helper->is_barrel_end(towerId);
  bool emec= m_lvl1Helper->is_emec(towerId);
  //
  // ..... case EM
  //
  if (emb || barrelEnd || emec) {
    //
    // ..... retrieve noise info for the tower
    //
    autoC = &(m_autoCorrEm);
    if (emb) {
      sigmaNoise = m_noiseRmsEmb[Ieta-1];
    }
    else if (emec) {
      sigmaNoise = m_noiseRmsEmec[Ieta-1];
    }
    else {  // barrelEnd
      sigmaNoise = sqrt(m_noiseRmsEmb[14]*m_noiseRmsEmb[14] + m_noiseRmsEmec[0]*m_noiseRmsEmec[0]);
      msglog << MSG::VERBOSE << " LArTTL1Maker: barrelEnd noise rms" 
	   << sigmaNoise
	   << endreq;
    }
  } // end case EM
  //
  // ..... case HEC
  //
  else if (m_lvl1Helper->is_hec(towerId)) {

    autoC = &(m_autoCorrHec[Ieta-1]);
    sigmaNoise = m_noiseRmsHec[Ieta-1];
  }
  //
  // ..... case FCAL
  //
  else if (m_lvl1Helper->is_fcal(towerId)) {

    int emOrHad = m_lvl1Helper->sampling(towerId) ;
    int module = emOrHad + 1 ;
    if( emOrHad && (Ieta==3 || Ieta==4) ) {
      module+=1; // FCAL3
    }
    autoC = &m_autoCorrFcal;
    //    sigmaNoise = m_noiseRmsFcal[module-1];
    noiseRms = m_noiseRmsFcal[module-1];
    sigmaNoise = noiseRms[Ieta-1];
    msglog << MSG::VERBOSE << " LArTTL1Maker: noise FCAL= " << sigmaNoise
	   << " module= " << module << "Ieta= " << Ieta << endreq;
  }
  else {  
    msglog << MSG::WARNING << " LArTTL1Maker: computeNoise unknown towerId " 
	   << m_lvl1Helper->show_to_string(towerId)
	   << endreq;
    return  inputV ;
  }

  if(fabs((*autoC)[0]) < 0.001) {
    msglog << MSG::WARNING << " No autocorrelation matrix found for tower " 
	   << m_emHelper->show_to_string(towerId) << " " 
	   << "setting default values 1.00   0.10  -0.30  -0.20  -0.05  -0.01  -0.01 "
	   << endreq ;
    (*autoC)[0] = 1.00;
    (*autoC)[1] = 0.10;
    (*autoC)[2] =-0.30;
    (*autoC)[3] =-0.20;
    (*autoC)[4] =-0.05;
    (*autoC)[5] =-0.01;
    (*autoC)[6] =-0.01;
  }
  if(sigmaNoise < 0.001) {
    msglog << MSG::WARNING << " No noise rms value found for tower " 
	   << m_emHelper->show_to_string(towerId) << " " 
	   << "setting default value 300 MeV "
	   << endreq ;
    sigmaNoise = 300.;
  }

  //
  // .... now compute the noise 'signal'
  //

  float c11,c21,c31,c41,c51,c22,c32,c33,c42,c43,c44,c52,c53,c54,c55;
  float c61,c62,c63,c64,c65,c66,c71,c72,c73,c74,c75,c76,c77;

  c11 =  sigmaNoise*(*autoC)[0];
  c21 =  sigmaNoise*(*autoC)[1];
  c31 =  sigmaNoise*(*autoC)[2];
  c41 =  sigmaNoise*(*autoC)[3];
  c51 =  sigmaNoise*(*autoC)[4];
  c61 =  sigmaNoise*(*autoC)[5];
  c71 =  sigmaNoise*(*autoC)[6];
//
  c22 = sqrt(c11*c11-c21*c21);
  c32 = (c21*c11-c21*c31)/c22;
  c33 = sqrt(c11*c11-c31*c31-c32*c32);
  c42 = (c31*c11-c21*c41)/c22;
  c43 = (c21*c11-c31*c41-c32*c42)/c33;
  c44 = sqrt(c11*c11-c41*c41-c42*c42-c43*c43);
  c52 = (c41*c11-c21*c51)/c22;
  c53 = (c31*c11-c31*c51-c32*c52)/c33;
  c54 = (c21*c11-c41*c51-c42*c52-c43*c53)/c44;
  c55 = sqrt(c11*c11-c51*c51-c52*c52-c53*c53-c54*c54);
  c62 = (c51*c11-c21*c61)/c22;
  c63 = (c41*c11-c31*c61-c32*c62)/c33;
  c64 = (c31*c11-c41*c61-c42*c62-c43*c63)/c44;
  c65 = (c21*c11-c51*c61-c52*c62-c53*c63-c54*c64)/c55;
  c66 = sqrt(c11*c11-c61*c61-c62*c62-c63*c63-c64*c64-c65*c65);
  c72 = (c61*c11-c21*c71)/c22;
  c73 = (c51*c11-c31*c71-c32*c72)/c33;
  c74 = (c41*c11-c41*c71-c42*c72-c43*c73)/c44;
  c75 = (c31*c11-c51*c71-c52*c72-c53*c73-c54*c74)/c55;
  c76 = (c21*c11-c61*c71-c62*c72-c63*c73-c64*c74-c65*c75)/c66;
  c77 = sqrt(c11*c11-c71*c71-c72*c72-c73*c73-c74*c74-c75*c75-c76*c76);
  
  double rndm[s_NBSAMPLES];
  RandGaussZiggurat::shootArray(m_rndmEngine,(int)s_NBSAMPLES,rndm,0.,1.);
  outputV[0] = inputV[0] + c11*rndm[0];
  outputV[1] = inputV[1] + c21*rndm[0] + c22*rndm[1];
  outputV[2] = inputV[2] + c31*rndm[0] + c32*rndm[1] + c33*rndm[2];
  outputV[3] = inputV[3] + c41*rndm[0] + c42*rndm[1] + c43*rndm[2] + c44*rndm[3];
  outputV[4] = inputV[4] + c51*rndm[0] + c52*rndm[1] + c53*rndm[2] + c54*rndm[3] + c55*rndm[4];
  outputV[5] = inputV[5] + c61*rndm[0] + c62*rndm[1] + c63*rndm[2] + c64*rndm[3] + c65*rndm[4] + c66*rndm[5];
  outputV[6] = inputV[6] + c71*rndm[0] + c72*rndm[1] + c73*rndm[2] + c74*rndm[3] + c75*rndm[4] + c76*rndm[5] + c77*rndm[6];

  return outputV ;

}


StatusCode LArTTL1Maker::fillEMap(int& totHit) 

{
// +======================================================================+
// +                                                                      +
// + Author: F. Ledroit                                                   +
// + Creation date: 2003/01/14                                            +
// +                                                                      +
// +======================================================================+

  MsgStream  msglog(messageService(),name());
  int outputLevel = msgSvc()->outputLevel( name() );

//
// ........ reset the map Energies
//
  if (m_useMapfromStore) {
     StatusCode sc = m_detectorStore->retrieve(m_hitmap,"LArHitEMap");
     if (sc.isFailure()) {
        msglog << " cannot retrieve LArHitEMap from detector Store " << endreq;
        return sc;
     }
  }

  else {

    m_hitmap->EnergyReset();
    if (outputLevel <= MSG::DEBUG) {
      msglog << MSG::DEBUG 
  	     << "Execute: energy reset done"
	     << endreq;
    }


    Identifier cellId;
    float hitEnergy;
    float hitTime;
    int skipHit=0;

    //
    // ............ loop over the wanted hit containers (one per sub-detector)
    //
    
    for (unsigned int iHitContainer=0;iHitContainer<m_HitContainer.size();iHitContainer++)
    {
      //
      // ..... Get the pointer to the Hit Container from StoreGate
      //
      if (outputLevel <= MSG::VERBOSE) {
	msglog << MSG::VERBOSE << " asking for: " 
	       << m_HitContainer[iHitContainer] 
	       << endreq;
      }
      
      if (!m_PileUp) {
	const LArHitContainer* hit_container ;
	StatusCode sc = m_storeGateSvc->retrieve( hit_container, 
						  m_HitContainer[iHitContainer] ) ;
	
	if ( sc.isFailure() || !hit_container) 	  {
	  msglog << MSG::ERROR << "Could not retrieve a LArHitContainer " 
		 << m_HitContainer[iHitContainer] 
		 << endreq;
	  return StatusCode::FAILURE;
	}
	
	if (outputLevel <= MSG::VERBOSE) {
	  msglog << MSG::VERBOSE 
		 << "number of hits: " << hit_container->size() 
		 << endreq;
	}
	
	//
	// ....... loop over hits and get informations
	//
	
	LArHitContainer::const_iterator hititer;
	for(hititer=hit_container->begin();hititer != hit_container->end();hititer++) {
	  cellId = (*hititer)->cellID();
	  hitEnergy = (float)(*hititer)->energy();
	  hitTime = (float)(*hititer)->time();
	    
	  //
	  // ....... fill the Map ; don't keep hits with ridiculously small energy
	  //
	  if (fabs(hitEnergy) > 1.e-06) {
	    if ( !m_hitmap->AddEnergy(cellId,hitEnergy,hitTime) ) {
	      msglog << MSG::FATAL << " Cell " << cellId.getString()
		     << " could not add the energy= " << hitEnergy  << " (MeV)"
		     << endreq;
	      return(StatusCode::FAILURE);
	    }
	    ++totHit;
	  }
	  else {
	    ++skipHit;
	  }
	  
	} // .... end of loop over hits
      } // ... end of NO pile-up condition
      else {
	
	typedef PileUpMergeSvc::TimedList<LArHitContainer>::type TimedHitContList;
	TimedHitContList hitContList;
	//
	// ...retrieve list of pairs (time,container) from PileUp service
	//
	
	if (!(m_mergeSvc->retrieveSubEvtsData(m_HitContainer[iHitContainer]
					      ,hitContList).isSuccess()) && hitContList.size()==0) {
	  msglog << MSG::ERROR 
		 << "Could not fill TimedHitContList" 
		 << endreq;
	  return StatusCode::FAILURE;
	}
	
	if (outputLevel <= MSG::DEBUG) {
	  msglog << MSG::DEBUG 
		 << "number of containers in the list: " << hitContList.size() 
		 << endreq;
	}
	
	//
	// ...loop over this list
	//
	TimedHitContList::iterator iFirstCont(hitContList.begin());
	TimedHitContList::iterator iEndCont(hitContList.end());
	double SubEvtTimOffset;

	while (iFirstCont != iEndCont) {
	  // get time for this subevent
	  // new coding of time information (January 05)
	  const PileUpTimeEventIndex* time_evt = &(iFirstCont->first);
	  SubEvtTimOffset = time_evt->time();
	  //	  SubEvtTimOffset = iFirstCont->first;

	  // get LArHitContainer for this subevent
	  const LArHitContainer& firstCont = *(iFirstCont->second);
	  if (outputLevel <= MSG::DEBUG) {
	    msglog << MSG::DEBUG 
		   << "number of hits in container: " << firstCont.size() 
		   << ", first five are:"
		   << endreq;
	  }
	
	  int numHit=0;
	  // Loop over cells in this LArHitContainer
	  LArHitContainer::const_iterator f_cell=firstCont.begin();
	  LArHitContainer::const_iterator l_cell=firstCont.end();
	  while (f_cell != l_cell) {
	    hitEnergy = (float) (*f_cell)->energy();
	    hitTime = (float) SubEvtTimOffset;
	    cellId = (*f_cell)->cellID();
            ++f_cell;

	    if(numHit<5) {
	      msglog << MSG::DEBUG
		     << "cellId " << m_lvl1Helper->show_to_string(cellId) 
		     << ", energy= " << hitEnergy
		     << ", time= " << hitTime
		     << endreq;
	    }
	    ++numHit;
	    ++totHit;
	    
	    //
	    // ....... fill the Map
	    //
	    if ( !m_hitmap->AddEnergy(cellId,hitEnergy,hitTime) )
	      {
		msglog << MSG::FATAL << " Cell " << cellId.getString()
		       << " could not add the energy= " << hitEnergy  << " (?eV)"
		       << endreq;
		return(StatusCode::FAILURE);
	      }
	    
	  } // ... end of while loop on hits
	  
	  ++iFirstCont;
	} // ... end of while loop on containers
      
      } // ... end of pile-up condition
    } // .... end of loop over containers
  
    if (outputLevel <= MSG::DEBUG) {
      msglog << MSG::DEBUG
  	     << "skipped " << skipHit  << " hits with abs(energy) less than 1.e-06 " << endreq;
    }

  }

  return StatusCode::SUCCESS;
}


StatusCode LArTTL1Maker::readAuxiliary()
{
  //
  // ...... Read auxiliary data file for EM (barrel and EC)
  //

  MsgStream msglog( msgSvc(), name() );
  msglog << MSG::DEBUG << "executing readAuxiliary()" << endreq;

  float refEnergy;
  std::vector<float> pulseShape(s_MAXSAMPLES) ;
  std::vector<float> pulseShapeDer(s_MAXSAMPLES) ;
  std::string barrel_endcap ;
  int Ieta ;
  float sinTheta=0. ;
  std::vector<float> layerRelGain(s_NBDEPTHS) ;
  std::vector<float> noiseRms(3) ;
  std::vector<float> noiseRms4(4) ;
  std::vector<float> autoCorr(s_NBSAMPLES) ;


  std::string pulsedataname=PathResolver::find_file ("LArEmLvl1.data", "DATAPATH");
  if (pulsedataname == "") {
    msglog << MSG::ERROR << "Could not locate LArEmLvl1.data file" <<  endreq;
    return StatusCode::FAILURE;
  }
  const char * pulsedatafile= pulsedataname.c_str() ;
  std::ifstream infile (pulsedatafile) ;

  if(!infile) {
    msglog << MSG::ERROR
	   << " cannot open EM file "
	   << endreq;
    return StatusCode::FAILURE;
  } else {
    msglog << MSG::DEBUG
	   << " EM file opened "
	   << endreq;
  }

  // first read the pulse shape for all energies 
  // valid for both barrel and endcap (from Xavier de la Broise)
  for(int iene=0 ; iene<s_NBENERGIES ; iene++) {
    infile >> refEnergy 
	   >> pulseShape[0] 
	   >> pulseShape[1] 
	   >> pulseShape[2] 
	   >> pulseShape[3] 
	   >> pulseShape[4] 
	   >> pulseShape[5] 
	   >> pulseShape[6] 
	   >> pulseShape[7] 
	   >> pulseShape[8] 
	   >> pulseShape[9] 
	   >> pulseShape[10] 
	   >> pulseShape[11] 
	   >> pulseShape[12] 
	   >> pulseShape[13] 
	   >> pulseShape[14] 
	   >> pulseShape[15] 
	   >> pulseShape[16] 
	   >> pulseShape[17] 
	   >> pulseShape[18] 
	   >> pulseShape[19] 
	   >> pulseShape[20] 
	   >> pulseShape[21] 
	   >> pulseShape[22]
	   >> pulseShape[23];
    m_refEnergyEm.push_back(refEnergy);
    m_pulseShapeEm.push_back(pulseShape);
    infile >> pulseShapeDer[0] 
	   >> pulseShapeDer[1] 
	   >> pulseShapeDer[2] 
	   >> pulseShapeDer[3] 
	   >> pulseShapeDer[4] 
	   >> pulseShapeDer[5] 
	   >> pulseShapeDer[6] 
	   >> pulseShapeDer[7] 
	   >> pulseShapeDer[8] 
	   >> pulseShapeDer[9] 
	   >> pulseShapeDer[10] 
	   >> pulseShapeDer[11] 
	   >> pulseShapeDer[12] 
	   >> pulseShapeDer[13] 
	   >> pulseShapeDer[14] 
	   >> pulseShapeDer[15] 
	   >> pulseShapeDer[16] 
	   >> pulseShapeDer[17] 
	   >> pulseShapeDer[18] 
	   >> pulseShapeDer[19] 
	   >> pulseShapeDer[20] 
	   >> pulseShapeDer[21] 
	   >> pulseShapeDer[22]
	   >> pulseShapeDer[23];
    m_pulseShapeDerEm.push_back(pulseShapeDer);
  }

  // then read autocorrelation function valid for barrel and endcap
  // ("default" auto-correlation function)
  m_autoCorrEm.resize(s_NBSAMPLES);
  infile >> m_autoCorrEm[0]
         >> m_autoCorrEm[1]
         >> m_autoCorrEm[2]
         >> m_autoCorrEm[3]
         >> m_autoCorrEm[4]
         >> m_autoCorrEm[5]
         >> m_autoCorrEm[6] ;

  // then read the separator for barrel
  infile >> barrel_endcap;

  for(int ieta=0 ; ieta<s_NBETABINS ; ieta++) {
    // then read the calibration factor (from MC) 
    // and the noise rms (from Bill Cleland) for each eta bin, in barrel
    infile >> Ieta
	   >> sinTheta;
    m_sinThetaEmb.push_back(sinTheta);
    infile >> noiseRms[0] 
	   >> noiseRms[1] 
	   >> noiseRms[2] ;
    m_noiseRmsEmb.push_back(noiseRms[2]);
    // if later we want to disentangle between pre-sum and summing electronic noise...
    //    m_noiseRmsEmb.push_back(noiseRms);
  }

  // now read the separator for endcap 
  infile >> barrel_endcap;

  for(int ieta=0 ; ieta<s_NBETABINS ; ieta++) {
    // then read the calibration coefficient (from MC) 
    // and the noise rms (from Bill Cleland) for each eta bin, in barrel
    infile >> Ieta
	   >> sinTheta;
    m_sinThetaEmec.push_back(sinTheta);
    infile >> noiseRms[0] 
	   >> noiseRms[1] 
	   >> noiseRms[2] ;
    m_noiseRmsEmec.push_back(noiseRms[2]);
    // if later we want to disentangle between pre-sum and summing electronic noise...
    //    m_noiseRmsEmec.push_back(noiseRms);

  }
  
  infile.close() ;
  msglog << MSG::DEBUG
	 << " EM file closed "
	 << endreq;

  msglog << MSG::INFO
	 << " 1 pulse shape per energy for EMB+EMEC : "
	 << endreq;
  for(int iene=0;iene<s_NBENERGIES;iene++) {
    msglog << MSG::INFO
	   << m_refEnergyEm[iene] << " MeV: "
	   << (m_pulseShapeEm[iene])[0] << ", "
	   << (m_pulseShapeEm[iene])[1] << ", "
	   << (m_pulseShapeEm[iene])[2] << ", "
	   << (m_pulseShapeEm[iene])[3] << ", "
	   << (m_pulseShapeEm[iene])[4] << ", "
	   << (m_pulseShapeEm[iene])[5] << ", "
	   << (m_pulseShapeEm[iene])[6] << ", "
	   << (m_pulseShapeEm[iene])[7] << ", "
	   << (m_pulseShapeEm[iene])[8] << ", "
	   << (m_pulseShapeEm[iene])[9] << ", "
	   << (m_pulseShapeEm[iene])[10] << ", "
	   << (m_pulseShapeEm[iene])[11] << ", "
	   << (m_pulseShapeEm[iene])[12] << ", "
	   << (m_pulseShapeEm[iene])[13] << ", "
	   << (m_pulseShapeEm[iene])[14] << ", "
	   << (m_pulseShapeEm[iene])[15] << ", "
	   << (m_pulseShapeEm[iene])[16] << ", "
	   << (m_pulseShapeEm[iene])[17] << ", "
	   << (m_pulseShapeEm[iene])[18] << ", "
	   << (m_pulseShapeEm[iene])[19] << ", "
	   << (m_pulseShapeEm[iene])[20] << ", "
	   << (m_pulseShapeEm[iene])[21] << ", "
	   << (m_pulseShapeEm[iene])[22] << ", "
	   << (m_pulseShapeEm[iene])[23] << ", "
	   << endreq;
  }
  msglog << MSG::INFO
	 << " 1 auto-corr matrix for EMB+EMEC : "
	 << m_autoCorrEm[0] << ", "
	 << m_autoCorrEm[1] << ", "
	 << m_autoCorrEm[2] << ", "
	 << m_autoCorrEm[3] << ", "
	 << m_autoCorrEm[4] << ", "
	 << m_autoCorrEm[5] << ", "
	 << m_autoCorrEm[6] 
	 << endreq;

  msglog << MSG::DEBUG
	 << " Finished reading 1 calib coeff + 1 noise value per eta bin for EM: "
	 << endreq;
  for(int ieta=0 ; ieta<s_NBETABINS ; ieta++) {
// currently the calib coeffs are all set to 1 -> turning INFO logs into DEBUG
    msglog << MSG::DEBUG
	   << "Ieta= " << ieta+1
	   << ", calib coeff EMB:  " << m_calibCoeffEmb[ieta]
	   << ", calib coeff EMEC: " << m_calibCoeffEmec[ieta]
	   << endreq;
    msglog << MSG::INFO
	   << "Ieta= " << ieta+1
	   << ", noise rms   EMB:  " << m_noiseRmsEmb[ieta] << " MeV"
	   << ", noise rms   EMEC: " << m_noiseRmsEmec[ieta] << " MeV"
	   << endreq;
  }

  //
  // ...... Read auxiliary data file for HEC
  //

  pulsedataname=PathResolver::find_file ("LArHecLvl1.data", "DATAPATH");
  if (pulsedataname == "") {
    msglog << MSG::ERROR << "Could not locate LArHecLvl1.data file" <<  endreq;
    return StatusCode::FAILURE;
  }
  pulsedatafile= pulsedataname.c_str() ;
  infile.open(pulsedatafile) ;

  if(!infile) {
    msglog << MSG::ERROR
	   << " cannot open HEC file "
	   << endreq;
    return StatusCode::FAILURE;
  } else {
    msglog << MSG::DEBUG
	   << " HEC file opened "
	   << endreq;
  }

  // first read the pulse shape for each eta bin (from Leonid Kurchaninov)
  m_pulseShapeHec.resize(s_MAXSAMPLES);
  infile >> m_pulseShapeHec [0]
         >> m_pulseShapeHec [1]
	 >> m_pulseShapeHec [2]
	 >> m_pulseShapeHec [3]
	 >> m_pulseShapeHec [4]
	 >> m_pulseShapeHec [5]
	 >> m_pulseShapeHec [6]
	 >> m_pulseShapeHec [7]
	 >> m_pulseShapeHec [8]
	 >> m_pulseShapeHec [9]
	 >> m_pulseShapeHec [10]
	 >> m_pulseShapeHec [11]
	 >> m_pulseShapeHec [12]
	 >> m_pulseShapeHec [13]
	 >> m_pulseShapeHec [14]
	 >> m_pulseShapeHec [15]
	 >> m_pulseShapeHec [16]
	 >> m_pulseShapeHec [17]
	 >> m_pulseShapeHec [18]
	 >> m_pulseShapeHec [19]
	 >> m_pulseShapeHec [20]
	 >> m_pulseShapeHec [21]
	 >> m_pulseShapeHec [22]
	 >> m_pulseShapeHec [23] ;
  
  m_pulseShapeDerHec.resize(s_MAXSAMPLES);
  infile >> m_pulseShapeDerHec [0]
         >> m_pulseShapeDerHec [1]
	 >> m_pulseShapeDerHec [2]
	 >> m_pulseShapeDerHec [3]
	 >> m_pulseShapeDerHec [4]
	 >> m_pulseShapeDerHec [5]
	 >> m_pulseShapeDerHec [6]
	 >> m_pulseShapeDerHec [7]
	 >> m_pulseShapeDerHec [8]
	 >> m_pulseShapeDerHec [9]
	 >> m_pulseShapeDerHec [10]
	 >> m_pulseShapeDerHec [11]
	 >> m_pulseShapeDerHec [12]
	 >> m_pulseShapeDerHec [13]
	 >> m_pulseShapeDerHec [14]
	 >> m_pulseShapeDerHec [15]
	 >> m_pulseShapeDerHec [16]
	 >> m_pulseShapeDerHec [17]
	 >> m_pulseShapeDerHec [18]
	 >> m_pulseShapeDerHec [19]
	 >> m_pulseShapeDerHec [20]
	 >> m_pulseShapeDerHec [21]
	 >> m_pulseShapeDerHec [22]
	 >> m_pulseShapeDerHec [23] ;
  
  m_satEnergyHec.resize(s_NBETABINS);
  m_sinThetaHec.resize(s_NBETABINS);
  m_noiseRmsHec.resize(s_NBETABINS);
  m_autoCorrHec.push_back(autoCorr);
  for(int ieta=1 ; ieta<s_NBETABINS ; ieta++) {
    // now read  for each eta bin:
    // - the calibration factor (determined from MC)
    // - the transverse saturating energies (same in all eta bins but Ieta=15)
    // - the value of sin_theta 
    // - the noise rms
    infile >> Ieta
           >> m_satEnergyHec[ieta]
	   >> m_sinThetaHec[ieta]
           >> m_noiseRmsHec[ieta];

  // and the autocorrelation function for each eta bin (from Leonid Kurchaninov)
    infile >> autoCorr[0]
	   >> autoCorr[1]
	   >> autoCorr[2]
	   >> autoCorr[3]
	   >> autoCorr[4]
	   >> autoCorr[5]
	   >> autoCorr[6] ;
    m_autoCorrHec.push_back(autoCorr);
  }
  infile.close() ;
  msglog << MSG::DEBUG
	 << " HEC file closed "
	 << endreq;
  msglog << MSG::INFO
	 << " 1 pulse shape for HEC : "
	 << m_pulseShapeHec[0] << ", "
	 << m_pulseShapeHec[1] << ", "
	 << m_pulseShapeHec[2] << ", "
	 << m_pulseShapeHec[3] << ", "
	 << m_pulseShapeHec[4] << ", "
	 << m_pulseShapeHec[5] << ", "
	 << m_pulseShapeHec[5] << ", "
	 << m_pulseShapeHec[6] << ", "
	 << m_pulseShapeHec[7] << ", "
	 << m_pulseShapeHec[8] << ", "
	 << m_pulseShapeHec[9] << ", "
	 << m_pulseShapeHec[10] << ", "
	 << m_pulseShapeHec[11] << ", "
	 << m_pulseShapeHec[12] << ", "
	 << m_pulseShapeHec[13] << ", "
	 << m_pulseShapeHec[14] << ", "
	 << m_pulseShapeHec[15] << ", "
	 << m_pulseShapeHec[16] << ", "
	 << m_pulseShapeHec[17] << ", "
	 << m_pulseShapeHec[18] << ", "
	 << m_pulseShapeHec[19] << ", "
	 << m_pulseShapeHec[20] << ", "
	 << m_pulseShapeHec[21] << ", "
	 << m_pulseShapeHec[22] << ", "
	 << m_pulseShapeHec[23] 
	 << endreq;
    msglog << MSG::DEBUG
	 << "Finished reading calib coeff, noise rms, sat ene, auto corr for each eta bin for HEC: " 
	 << endreq;
    for(int ieta=1 ; ieta<s_NBETABINS ; ieta++) {
// currently the calib coeffs are all set to 1 -> turning INFO logs into DEBUG
      msglog << MSG::DEBUG
	     << " Ieta= " << ieta+1
	     << ", calib coeff HEC: " << m_calibCoeffHec[ieta]
	     << endreq;
      msglog << MSG::INFO
	     << " Ieta= " << ieta+1
	     << ", noise rms HEC: " << m_noiseRmsHec[ieta] << " MeV"
	     << ", sat ene HEC: " << m_satEnergyHec[ieta] << " MeV"
	     << endreq;
      msglog << MSG::INFO
	     << " Ieta= " << ieta+1
	     << ", auto corr HEC: "
	     << (m_autoCorrHec[ieta])[0] << ", "
	     << (m_autoCorrHec[ieta])[1] << ", "
	     << (m_autoCorrHec[ieta])[2] << ", "
	     << (m_autoCorrHec[ieta])[3] << ", "
	     << (m_autoCorrHec[ieta])[4] << ", "
	     << (m_autoCorrHec[ieta])[5] << ", "
	     << (m_autoCorrHec[ieta])[6] << ", "
	     << endreq;
    }

  //
  // ...... Read auxiliary data files for FCAL
  //

  pulsedataname=PathResolver::find_file ("LArFcalLvl1.data", "DATAPATH");
  if (pulsedataname == "") {
    msglog << MSG::ERROR << "Could not locate LArFcalLvl1.data file" <<  endreq;
    return StatusCode::FAILURE;
  }
  pulsedatafile= pulsedataname.c_str() ;
  infile.open(pulsedatafile) ;

  if(!infile) {
    msglog << MSG::ERROR
	   << " cannot open FCAL file "
	   << endreq;
    return StatusCode::FAILURE;
  } else {
    msglog << MSG::DEBUG
	   << " FCAL file opened "
	   << endreq;
  }

  const int nMod = 3;
  //  m_noiseRmsFcal.resize(nMod);
  int imod=0;

  for(int iMod=0; iMod < nMod; iMod++) {

    // first read the module number + noise rms (from J. Rutherfoord)
    //    infile >> imod >> m_noiseRmsFcal[iMod] ;
    infile >> imod 
	   >> noiseRms4[0]
	   >> noiseRms4[1]
	   >> noiseRms4[2]
	   >> noiseRms4[3] ;
    m_noiseRmsFcal.push_back(noiseRms4);

    // read the pulse shape for this module (from John Rutherfoord)
    infile >> pulseShape [0]
	   >> pulseShape [1]
	   >> pulseShape [2]
	   >> pulseShape [3]
	   >> pulseShape [4]
	   >> pulseShape [5]
	   >> pulseShape [6]
	   >> pulseShape [7]
	   >> pulseShape [8]
	   >> pulseShape [9]
	   >> pulseShape [10]
	   >> pulseShape [11]
	   >> pulseShape [12]
	   >> pulseShape [13]
	   >> pulseShape [14]
	   >> pulseShape [15]
	   >> pulseShape [16]
	   >> pulseShape [17]
	   >> pulseShape [18]
	   >> pulseShape [19]
	   >> pulseShape [20]
	   >> pulseShape [21]
	   >> pulseShape [22]
	   >> pulseShape [23] ;
    m_pulseShapeFcal.push_back(pulseShape);

    // read the pulse shape derivative
    infile >> pulseShapeDer [0]
	   >> pulseShapeDer [1]
	   >> pulseShapeDer [2]
	   >> pulseShapeDer [3]
	   >> pulseShapeDer [4]
	   >> pulseShapeDer [5]
	   >> pulseShapeDer [6]
	   >> pulseShapeDer [7]
	   >> pulseShapeDer [8]
	   >> pulseShapeDer [9]
	   >> pulseShapeDer [10]
	   >> pulseShapeDer [11]
	   >> pulseShapeDer [12]
	   >> pulseShapeDer [13]
	   >> pulseShapeDer [14]
	   >> pulseShapeDer [15]
	   >> pulseShapeDer [16]
	   >> pulseShapeDer [17]
	   >> pulseShapeDer [18]
	   >> pulseShapeDer [19]
	   >> pulseShapeDer [20]
	   >> pulseShapeDer [21]
	   >> pulseShapeDer [22]
	   >> pulseShapeDer [23] ;
    m_pulseShapeDerFcal.push_back(pulseShapeDer);
  }

  m_autoCorrFcal.resize(s_NBSAMPLES);
  // finally read standard autocorrelation matrix
  infile >> m_autoCorrFcal[0]
	 >> m_autoCorrFcal[1]
	 >> m_autoCorrFcal[2]
	 >> m_autoCorrFcal[3]
	 >> m_autoCorrFcal[4]
	 >> m_autoCorrFcal[5]
	 >> m_autoCorrFcal[6] ;
  
  infile.close() ;
  msglog << MSG::DEBUG
	 << " FCAL file closed "
	 << endreq;

  std::vector<float> auxV(3);
  m_calibCoeffFcal.push_back(m_calibCoeffFcalEm);
  m_calibCoeffFcal.push_back(m_calibCoeffFcalHad);

  msglog << MSG::DEBUG
	 << "Finished reading noise, calib coeff and pulse shape for each module for FCAL: "
	 << endreq;
  for(int iMod=0; iMod < nMod; iMod++) {

    msglog << MSG::INFO
	   << " iMod= " << iMod
	   << ", noise rms FCAL (eta bin 1,2,3,4): " 
	   << m_noiseRmsFcal[iMod] << " (transverse) MeV "
	   << endreq;
    if(iMod < 2){
      msglog << MSG::INFO
	     << " iMod= " << iMod
	     << ", calib coeff FCAL (eta bin 1,2,3,4): " 
	     << (m_calibCoeffFcal[iMod])[0] << ", "
	     << (m_calibCoeffFcal[iMod])[1] << ", "
	     << (m_calibCoeffFcal[iMod])[2] << ", "
	     << (m_calibCoeffFcal[iMod])[3]
	     << endreq;
    }
    msglog << MSG::INFO
	   << " iMod= " << iMod
	   << ", pulse shape FCAL: " 
	   << (m_pulseShapeFcal[iMod])[0] << ", "
	   << (m_pulseShapeFcal[iMod])[1] << ", "
	   << (m_pulseShapeFcal[iMod])[2] << ", "
	   << (m_pulseShapeFcal[iMod])[3] << ", "
	   << (m_pulseShapeFcal[iMod])[4] << ", "
	   << (m_pulseShapeFcal[iMod])[5] << ", "
	   << (m_pulseShapeFcal[iMod])[6] << ", "
	   << (m_pulseShapeFcal[iMod])[7] << ", "
	   << (m_pulseShapeFcal[iMod])[8] << ", "
	   << (m_pulseShapeFcal[iMod])[9] << ", "
	   << (m_pulseShapeFcal[iMod])[10] << ", "
	   << (m_pulseShapeFcal[iMod])[11] << ", "
	   << (m_pulseShapeFcal[iMod])[12] << ", "
	   << (m_pulseShapeFcal[iMod])[13] << ", "
	   << (m_pulseShapeFcal[iMod])[14] << ", "
	   << (m_pulseShapeFcal[iMod])[15] << ", "
	   << (m_pulseShapeFcal[iMod])[16] << ", "
	   << (m_pulseShapeFcal[iMod])[17] << ", "
	   << (m_pulseShapeFcal[iMod])[18] << ", "
	   << (m_pulseShapeFcal[iMod])[19] << ", "
	   << (m_pulseShapeFcal[iMod])[20] << ", "
	   << (m_pulseShapeFcal[iMod])[21] << ", "
	   << (m_pulseShapeFcal[iMod])[22] << ", "
	   << (m_pulseShapeFcal[iMod])[23]
	   << endreq;
  }
  msglog << MSG::INFO
	 << "auto corr FCAL: " 
	 << m_autoCorrFcal[0] << ", "
	 << m_autoCorrFcal[1] << ", "
	 << m_autoCorrFcal[2] << ", "
	 << m_autoCorrFcal[3] << ", "
	 << m_autoCorrFcal[4] << ", "
	 << m_autoCorrFcal[5] << ", "
	 << m_autoCorrFcal[6]
	 << endreq;

  // now the relative gains
  pulsedataname=PathResolver::find_file ("Fcal_ptweights_table7.data", "DATAPATH");
  if (pulsedataname == "") {
    msglog << MSG::ERROR << "Could not locate Fcal_ptweights_table7.data file" <<  endreq;
    return StatusCode::FAILURE;
  }
  pulsedatafile= pulsedataname.c_str() ;
  infile.open(pulsedatafile) ;

  if(!infile) {
    msglog << MSG::ERROR
	   << " cannot open FCAL gains file "
	   << endreq;
    return StatusCode::FAILURE;
  } else {
    msglog << MSG::DEBUG
	   << " FCAL gains file opened "
	   << endreq;
  }

  // first read the gain file for all cells (from John Rutherfoord)
  
  m_cellRelGainFcal.resize(m_fcalHelper->channel_hash_max());
  // file structure:
  const unsigned int colNum=14;
  // number of cells per 'group' (sharing the same gain)
  const unsigned int maxCell = 4;
  std::vector<std::string> TTlabel;
  TTlabel.resize(colNum);
  std::vector<float> gain;
  gain.resize(colNum);
  int iline=0;
  int ngain=0;

  while ( infile >> TTlabel [0] >> gain [0]
	  >> TTlabel [1] >> gain [1]
	  >> TTlabel [2] >> gain [2]
	  >> TTlabel [3] >> gain [3]
	  >> TTlabel [4] >> gain [4]
	  >> TTlabel [5] >> gain [5]
	  >> TTlabel [6] >> gain [6]
	  >> TTlabel [7] >> gain [7]
	  >> TTlabel [8] >> gain [8]
	  >> TTlabel [9] >> gain [9]
	  >> TTlabel [10] >> gain [10]
	  >> TTlabel [11] >> gain [11]
	  >> TTlabel [12] >> gain [12]
	  >> TTlabel [13] >> gain [13]) {

    msglog << MSG::DEBUG
	   << " TTlabel[0], gain[0]= " << TTlabel[0] << ", " << gain[0] 
	   << endreq;
    msglog << MSG::DEBUG
	   << " [...] "
	   << endreq;
    msglog << MSG::DEBUG
	   << " TTlabel[13], gain[13]= " << TTlabel[13] << ", "  << gain[13] 
	   << endreq;

    //int barrel_ec_fcal = 2;
    //int pos_neg = -2; // C side (neg z)
    int detZside = -1;
    if(iline < 32) {detZside = 1;} // A side (pos z)
    //if(iline < 32) {pos_neg = 2;} // A side (pos z)
    for(unsigned int icol=0; icol<colNum; icol++){

      int em_had = 0; // FCAL1
      int layer=0;
      if(icol > 7) {
	em_had = 1;  // FCAL2+FCAL3
	if(icol > 11) {layer=1;}
      }
      int region = 3 ; // FCAL

      std::string TTlab = TTlabel[icol];
      //int Ieta = int(TTlab[0])-48;
      //int Lphi = int(TTlab[1])-64;
      int  eta = int(TTlab[0])-49;
      int  phi = int(TTlab[1])-65;
      int nPhi = 16;
      if(detZside<0) {
	phi = ( phi<nPhi/2 ? nPhi/2 -1 -phi : 3*nPhi/2 -1 -phi );
      }
 
      int group = 1;
      if(TTlab.size() > 3) {group = int(TTlab[3])-48;}

      // added Nov 2009 following introduction of TTCell map "FcalFix" 
      // modified again Jun 2010 following introduction of TTCell map "FcalFix2" (the C side is not symmetric wrt the A side)
      // modified again Jan 2011 in an attempt to correctly load the gains to the correct cells (not all cells were being assigned gains)
      if(em_had) {  // FCAL-had only
	if( (layer==1 && detZside>0) || (layer==0 && detZside<0) ) {
	  if(eta==0||eta==2) {
	    eta+=1;
	    //group+=1; //- OLD CODE (pre Jan2011 change)
		
	  } else {
		if(layer==1) {group+=1;} else { group+=2; }
	  }
	}
	else {
	  if(eta==1||eta==3) {
	    eta-=1;
	    //group+=2; //- OLD CODE (pre Jan2011 change)
	    if(layer==1) {group+=1;} else { group+=2; }
	  }
	}
      }
      //
      // ... create an offline TT+layer identifier from the labels read in.
      //
      Identifier ttId = m_lvl1Helper->layer_id(detZside, em_had, region, eta, phi, layer) ;
      msglog << MSG::DEBUG
	     << "ttId= " << m_lvl1Helper->show_to_string(ttId) 
	     << endreq;
      // 
      //... fill a vector with offline identifier of cells belonging to this tower (with layer info)
      //
      std::vector<Identifier> cellIdVec = m_ttSvc->createCellIDvecLayer(ttId);
      //
      // .... loop on all LAr offline channels belonging to the trigger tower (with layer info) 
      //
      std::vector<unsigned int> hashVec;
      // number of connected cells in the current tower
      unsigned int nCell=0;
      for (unsigned int ichan=0;ichan<cellIdVec.size();ichan++){	
	Identifier cellId=cellIdVec[ichan];
	
	int cellPhi = m_fcalHelper->phi(cellId);
	if(cellPhi >= 0) {  // protection skipping unconnected channels (they have eta=phi=-999) - normally, they should not be in the list
	  nCell++;
	  //
	  // .... convert to FCAL hash
	  //
	  IdentifierHash fcalHash = m_fcalHelper->channel_hash(cellId);
	  //
	  //... save hash indices
	  //
	  hashVec.push_back(fcalHash);
	} // end condition cellPhi>=0
	
      } // end of loop on channels in the tower
      msglog << MSG::DEBUG
	     << "nCell= " << nCell 
	     << endreq;
      
      //
      //... loop on the 4 cells of the current group and put their gain in the gain vector (indexed by hashes)
      //
      for(unsigned int iCell=0; iCell<maxCell; iCell++) {
	unsigned int index0 = (group-1)*maxCell + iCell ;
	if(index0 < nCell) {  // unconnected channels have highest hash ids (because highest eta values)
	  unsigned int index = hashVec[index0] ;
	  m_cellRelGainFcal [index] = gain[icol] ;
	  ngain++;
	  msglog << MSG::DEBUG
		 << " index, gain= " << index << ", " << gain[icol]
		 << endreq;
	}
      }
      
    } // end of loop on columns
    iline++;
  } // end of loop on lines

  
  msglog << MSG::DEBUG
	 << " number of lines found= " 
	 << iline 
	 << endreq;
  infile.close() ;
  msglog << MSG::INFO
	 << " FCAL gains file closed, extracted " << ngain << " gains"
	 << endreq;

  return StatusCode::SUCCESS;

}
//===========================================================================
int LArTTL1Maker::decodeInverse(int region, int eta)
{
//===========================================================================
// Maps [ region , eta ]  to [ Ieta] 
// ==========================================================================
// Problem: this is NOT a bijection, because of the "barrel end" zone. 
//         Convention: only the barrel part of the identifying fields is returned 
//===========================================================================
 
 int Ieta=0;
 if(region == 0){  // Barrel + EC-OW
   if(eta <= 14) {
     Ieta=eta+1;         
   }
   else {
     Ieta=eta-13;
   }
 }
 else if( region == 1 || region == 2) { // EC-IW
   if(region == 1) {   
     Ieta=eta+12;
   }
   else {   
     Ieta=15;
   }
 }
 else if(region == 3) {  // FCAL 
   Ieta=eta+1;
 }
 return Ieta ; 
}


StatusCode LArTTL1Maker::initHitMap()
{

  MsgStream  msglog(messageService(),name());
  int outputLevel = msgSvc()->outputLevel( name() );

  if (m_useMapfromStore) {
      msglog << MSG::INFO << " Use LArHitEMap from detector store. should be filled by LArDigitMaker (digitmaker1)" << endreq;
      return StatusCode::SUCCESS;
  }

  m_hitmap = new LArHitEMap();

//
// ......... make the Sub-detector flag vector
//
  static std::vector<bool> SubDetFlag;
  for (int i=0; i < LArHitEMap::NUMDET ; i++)
  {
   SubDetFlag.push_back(false);
  }

//
// ......... make the LArHit container name list
//
  if ( m_SubDetectors == "LAr_All" )
  {
    m_HitContainer.push_back(m_EmBarrelHitContainerName);
    SubDetFlag[LArHitEMap::EMBARREL_INDEX] = true;
    m_HitContainer.push_back(m_EmEndCapHitContainerName);
    SubDetFlag[LArHitEMap::EMENDCAP_INDEX] = true;
    m_HitContainer.push_back(m_HecHitContainerName);
    SubDetFlag[LArHitEMap::HADENDCAP_INDEX] = true;
    m_HitContainer.push_back(m_ForWardHitContainerName);
    SubDetFlag[LArHitEMap::FORWARD_INDEX] = true;
  }
  else if ( m_SubDetectors == "LAr_Em" )
  {
    m_HitContainer.push_back(m_EmBarrelHitContainerName);
    SubDetFlag[LArHitEMap::EMBARREL_INDEX] = true;
    m_HitContainer.push_back(m_EmEndCapHitContainerName);
    SubDetFlag[LArHitEMap::EMENDCAP_INDEX] = true;
  }
  else if ( m_SubDetectors == "LAr_EmBarrel" )
  {
    m_HitContainer.push_back(m_EmBarrelHitContainerName);
    SubDetFlag[LArHitEMap::EMBARREL_INDEX] = true;
  }
  else if ( m_SubDetectors == "LAr_EmEndCap" )
  {
    m_HitContainer.push_back(m_EmEndCapHitContainerName);
    SubDetFlag[LArHitEMap::EMENDCAP_INDEX] = true;
  }
  else if ( m_SubDetectors == "LAr_HEC" )
  {
    m_HitContainer.push_back(m_HecHitContainerName);
    SubDetFlag[LArHitEMap::HADENDCAP_INDEX] = true;
  }
  else if ( m_SubDetectors == "LAr_Fcal" )
    {
    m_HitContainer.push_back(m_ForWardHitContainerName);
    SubDetFlag[LArHitEMap::FORWARD_INDEX] = true;
    }
  else
  {
    //
    //........ Unknown case
    //
    msglog << MSG::FATAL 
	   << "Invalid SubDetector property : " << m_SubDetectors
	   << "Valid ones are: LAr_All, LAr_Em, LAr_EmBarrel, "
	   << "LAr_EmEndCap, LAr_HEC and LAr_Fcal "
	   << endreq;

    return(StatusCode::FAILURE);
  }

//
//  put all cell energies to 0.
//

  if ( ! m_hitmap->Initialize(SubDetFlag) )
    {
      msglog << MSG::FATAL 
	     << "Making of the cell table failed" 
	     << endreq;
      return StatusCode::FAILURE;
    }
  
  if (outputLevel <= MSG::DEBUG) {
    msglog << MSG::INFO
	   << "Number of created cells in LArHitEMap " 
	   << m_hitmap->GetNbCells() 
	   << endreq;
  }


  return StatusCode::SUCCESS;
}
