/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// +======================================================================+
// +                                                                      +
// + Author ........: Denis O. Damazio                                    +
// + Institut ......: BNL                                                 +
// + Creation date .: 18/11/2013                                          +
// +                                                                      +
// +======================================================================+
// 
// ........ includes
//
#include "LArL1Sim/LArSCL1Maker.h"
// .......... utilities
//
#include <math.h>
#include <fstream>
#include "AtlasCLHEP_RandomGenerators/RandGaussZiggurat.h"
#include "CLHEP/Random/RandomEngine.h"
#include "AthenaKernel/IAtRndmGenSvc.h"
#include "GaudiKernel/ServiceHandle.h"

#include "LArRawEvent/LArDigit.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArSimEvent/LArHitContainer.h"
#include "LArDigitization/LArHitList.h"

#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/LArID.h"
#include "CaloIdentifier/CaloID_Exception.h"
#include "CaloIdentifier/CaloLVL1_ID.h"
#include "LArIdentifier/LArIdManager.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"
#include "LArTools/LArCablingService.h"
#include "LArTools/LArSuperCellCablingTool.h"
#include "CaloIdentifier/CaloCell_SuperCell_ID.h"
#include "CaloTriggerTool/ICaloSuperCellIDTool.h"
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

#include "LArElecCalib/ILArfSampl.h"
#include "LArElecCalib/ILArShape.h"
#include "LArElecCalib/ILArPedestal.h"
#include "LArElecCalib/ILArNoise.h"
#include "LArElecCalib/ILArAutoCorrNoiseTool.h"
#include "LArElecCalib/ILArADC2MeVTool.h"

// Pile up
#include "PileUpTools/PileUpMergeSvc.h"
// trigger time
#include "AthenaKernel/ITriggerTime.h"

//


using CLHEP::RandGaussZiggurat;


LArSCL1Maker::LArSCL1Maker(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator)
  , m_storeGateSvc("StoreGateSvc",name)
  , m_atRndmGenSvc("AtRndmGenSvc",name)
  , m_rndmEngineName("LArSCL1Maker")
  , m_rndmEngine(0)
  , m_hitmap(0)
  , m_autoCorrNoiseTool("LArAutoCorrNoiseTool")
  , m_adc2mevTool("LArADC2MeVTool")
  , m_fSamplKey("LARfSamplSC")
  , m_shapesKey("LArShapeSC")
  , m_noiseKey("LArNoiseSC")
  , m_pedestalKey("LArPedestalSC")
// + -------------------------------------------------------------------- +
// + Author ........: Denis O. Damazio                                    +
// + Creation date .: 18/11/2013                                          +
// + Subject: SCL1 Maker constructor                                     +
// + -------------------------------------------------------------------- +
{
  m_first=true;
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

  m_BeginRunPriority      = 100;

  m_cablingSCSvc          = 0;
  //m_ttSvc                 = 0;
  m_scHelper            = 0;

  m_SubDetectors             = "LAr_All";
  m_SCL1ContainerName        = "LArDigitSCL1";
  

  m_NoiseOnOff               = true;
  m_PileUp                   = false;
  m_noEmCalibMode            = false;
  m_noHadCalibMode           = false;
  m_chronoTest               = false;
  m_debugThresh              = 5000.;

  //
  // ........ declare the private data as properties
  //

  declareProperty("EventStore",m_storeGateSvc,"StoreGate Service");
  declareProperty("SubDetectors",m_SubDetectors);
  declareProperty("RndmSvc", m_atRndmGenSvc);
  declareProperty("AutoCorrNoiseTool",m_autoCorrNoiseTool,"Tool handle for electronic noise covariance");
  declareProperty("ADC2MeVTool",m_adc2mevTool,"Tool handle for ADC2MeV Conversion");


  declareProperty("SCL1ContainerName",m_SCL1ContainerName);

  declareProperty("NoiseOnOff",m_NoiseOnOff);

  declareProperty("PileUp",m_PileUp);
  declareProperty("UseTriggerTime",m_useTriggerTime);
  declareProperty("TriggerTimeToolName",m_triggerTimeToolName);

  declareProperty("FirstSample",m_firstSample=-1);
  declareProperty("NSamples",m_nSamples=7);

  declareProperty("ChronoTest",m_chronoTest);
  declareProperty("DebugThreshold",m_debugThresh);

//
return;
}


LArSCL1Maker::~LArSCL1Maker()
{  
  /**  SCL1 Maker destructor                                      */

return;
}


StatusCode LArSCL1Maker::initialize()
{
// +======================================================================+
// +                                                                      +
// + Author ........: Denis O. Damazio                                    +
// + Creation date .: 18/11/2013                                          +
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
	 << "* Steering options for LArSCL1Maker algorithm *" 
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
	   << "Electronic noise will be added in each SC for selected sub-detectors." 
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
        << "  => LArSCL1Maker will not apply Trigger Time " << endreq;
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

    sc = m_detectorStore->retrieve(m_OnlSCHelper);
    if (sc.isFailure()) {
      msglog << MSG::ERROR << "Unable to retrieve LArOnline_SuperCellID from DetectorStore" << endreq;
      return StatusCode::FAILURE;
    } else {
      if (outputLevel <= MSG::DEBUG) {
        msglog << MSG::DEBUG << "Successfully retrieved LArOnline_SuperCellID from DetectorStore" << endreq;
      }
    }

  }   else {
    msglog << MSG::ERROR << "Could not locate DetectorStore" << endreq;
    return StatusCode::FAILURE;
  }

  //
  //..... need of course the LVL1 helper
  //
  m_scHelper = caloMgr->getCaloCell_SuperCell_ID();
  if (!m_scHelper) {
    msglog << MSG::ERROR << "Could not access CaloCell_SuperCell_ID helper" << endreq;
    return StatusCode::FAILURE;
  } else {
    if (outputLevel <= MSG::DEBUG) {
      msglog << MSG::DEBUG << "Successfully accessed CaloCell_SuperCell_ID helper" << endreq;
    }
  }
  
  // ..... need cabling services, to get channels associated to each SC
  //
  IToolSvc* toolSvc;
  StatusCode status   = service( "ToolSvc",toolSvc  );
  if(status.isSuccess()) {
    sc = toolSvc->retrieveTool("LArSuperCellCablingTool",m_cablingSCSvc);
    if(sc.isFailure()) {
      msglog << MSG::ERROR << "Could not retrieve LArSuperCellCablingTool"<< endreq;
      return(StatusCode::FAILURE);
    }
    sc = toolSvc->retrieveTool("CaloSuperCellIDTool",m_scidtool);
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

  if ( m_autoCorrNoiseTool.retrieve().isFailure() ) {
   msglog << MSG::ERROR << "Unable to find LArAutoCorrNoiseTool" << endreq;
   return StatusCode::FAILURE;
  }

  if ( m_adc2mevTool.retrieve().isFailure() ) {
   msglog << MSG::ERROR << "Unable to find LArADC2MeVTool" << endreq;
   return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

}



void LArSCL1Maker::handle(const Incident& /* inc*/ )
{
  MsgStream msglog( msgSvc(), name() );
  msglog << MSG::DEBUG << "LArSCL1Maker handle()" << endreq;

  StatusCode sc = this->updateConditions();
  if (sc.isFailure()) {
    msglog << MSG::ERROR << " Error from updateConditions " << endreq;
  }


  return;
}

StatusCode LArSCL1Maker::execute()
{
// +======================================================================+
// +                                                                      +
// + Author: Denis O. Damazio                                             +
// + Creation date: 2003/01/13                                            +
// + Subject: Make the SCL1s and put them into the container             +
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

  if ( totHit==0) {
    msglog << MSG::WARNING << " No LAr hit in the event "  << endreq;
    msglog << MSG::WARNING << "cannot process this event" << endreq;
    return StatusCode::SUCCESS;
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


  LArDigitContainer *scContainer = new LArDigitContainer();
  if ( scContainer == 0 ) {
    msglog << MSG::ERROR 
	   << "Could not allocate a new LArSCDigitContainer" 
	   << endreq;
    return StatusCode::FAILURE;	  
  }


  // ...... register the TTL1 containers into the TES 
  //
  StatusCode sc = m_storeGateSvc->record( scContainer ,  m_SCL1ContainerName) ;
  if( sc.isFailure() )
  {
    msglog << MSG::ERROR 
	   << "Could not record new LArDigitContainer in TES : " 
	   << m_SCL1ContainerName 
	   << endreq;
    return StatusCode::FAILURE;	  
  }

  //
  // ... initialise vectors for sums of energy in each TT
  //
  unsigned int nbSC = (unsigned int)m_scHelper->calo_cell_hash_max() ;
  if (outputLevel <= MSG::DEBUG) {
    msglog << MSG::DEBUG
  	   << "Max number of LAr Super-Cell= " << nbSC 
	   << endreq;
  }
  std::vector<std::vector<float> > sumEnergy ;   // inner index = time slot (from 0 to visEvecSize-1)
  std::vector<std::vector<float> > sumTime ;   // inner index = time slot (from 0 to visEvecSize-1)
  sumEnergy.resize(nbSC);
  sumTime.resize(nbSC);
  std::vector<float> scSumE;
  int scSumEvecSize=5;

  scSumE.reserve(scSumEvecSize);
  std::vector< std::vector < float> > scFloatContainerTmp;

  int it = 0;
  int it_end = m_hitmap->GetNbCells();
  scContainer->reserve( nbSC ); //container ordered by hash
  scFloatContainerTmp.assign( nbSC, std::vector<float>(0) ); //container ordered by hash
  std::vector<bool> alreadyThere;
  alreadyThere.resize( nbSC );
  alreadyThere.assign( nbSC, false );

  std::vector<HWIdentifier> hwid;
  hwid.resize( nbSC );
  hwid.assign( nbSC, HWIdentifier(0) );

  //m_nSamples=5;
  std::vector<float> samples;
  samples.resize( m_nSamples );
  std::vector<short> samplesInt;
  samplesInt.resize( m_nSamples );
  CaloGain::CaloGain scGain = CaloGain::LARHIGHGAIN;

  for( ; it!=it_end;++it) {
    LArHitList * hitlist = m_hitmap->GetCell(it);
    if ( hitlist != (LArHitList*) NULL ) {

      const std::vector<std::pair<float,float> >* timeE = hitlist->getData();
      if (timeE->size() > 0 ) {
        Identifier  cellId = hitlist->getIdentifier();


          Identifier scId = m_scidtool->offlineToSuperCellID(cellId);
          IdentifierHash scHash = m_scHelper->calo_cell_hash(scId) ;
	  if ( scHash.value() == 999999 ) continue;
	  HWIdentifier hwSC = m_cablingSCSvc->createSignalChannelID(scId);
	  IdentifierHash scHWHash = m_OnlSCHelper->channel_Hash(hwSC);
          hwid[scHWHash] = hwSC;

	  float factor = 1.0;
          if ( m_first ) {
		printConditions(hwSC);
	  }
	  factor = (m_adc2mevTool->ADC2MEV(hwSC,(CaloGain::CaloGain)1))[1] ;
	  factor = 1.0/m_fracS->FSAMPL(hwSC)/factor;

          ConvertHits2Samples(hwSC, scGain, timeE, samples);
	  std::vector< float >& vec = scFloatContainerTmp.at(scHWHash);
	  if ( !alreadyThere[scHWHash] ){
		alreadyThere[scHWHash]=true;
	  	for(unsigned int i=0; i< samples.size(); i++)
		   vec.push_back( factor * samples[i] );
	  } else {
	  	for(unsigned int i=0; i< samples.size(); i++)
		   vec[i]+= ( factor * samples[i] );
	  }
	}


    } // if hit list is not empty
  } // it end

  it=0;
  it_end=nbSC;
  int cc=0;
  short MAXADC=4096; // 12 bits ADC?
  std::vector<float> noise(m_nSamples);
  double Rndm[32];
  for( ; it != it_end; ++it){
	
	if ( alreadyThere[it] ){
		const std::vector< float >&  vec= scFloatContainerTmp.at(it);
		HWIdentifier id = hwid[it];
		// reset noise
		noise.assign(m_nSamples,0);

		// noise definition
		if ( m_NoiseOnOff ) {
		float SigmaNoise = (m_NoiseSC->noise(id,0));
		int index;
		const std::vector<float>* CorrGen = &(m_autoCorrNoiseTool->autoCorrSqrt(id,0,m_nSamples));

		RandGaussZiggurat::shootArray(m_rndmEngine,m_nSamples,Rndm,0.,1.);

		for(int i=0;i<(int)m_nSamples;i++){
		  noise[i]=0.;
		  for(int j=0;j<=i;j++){
			index = i* m_nSamples + j;
			noise[i] += Rndm[j] * (*CorrGen)[index];
		  }
		  noise[i]=noise[i]*SigmaNoise;
		}
		}

		int ped = m_PedestalSC->pedestal(id,0);
	  	samplesInt.assign( m_nSamples, 0 );
		for(unsigned int i=0; i< vec.size(); i++) {
			samplesInt[i]=rint(vec[i]+ped+noise[i]);
			if ( samplesInt[i] >= MAXADC ) samplesInt[i]=MAXADC-1;
			if ( samplesInt[i] < 0 ) samplesInt[i]=0;
		}
		LArDigit* dig = new LArDigit(id, scGain, samplesInt );
		scContainer->push_back(dig);
	} else {
		cc++;
	}
  }

  if(m_chronoTest) {
    m_chronSvc->chronoStop( "LArSCL1Mk hit loop " );
    m_chronSvc->chronoPrint( "LArSCL1Mk hit loop " );
    m_chronSvc->chronoStart( "LArSCL1Mk SC loop " );
  }

  m_first = false;
  return StatusCode::SUCCESS;
}


StatusCode LArSCL1Maker::finalize()
{
// +======================================================================+
// +                                                                      +
// + Author: Denis O. Damazio                                             +
// + Creation date: 18/11/2013                                            +
// +                                                                      +
// +======================================================================+
//
// ......... declaration
//
  MsgStream  msglog(messageService(),name());

  msglog << MSG::INFO << " LArSCL1Maker finalize completed successfully" 
	 << endreq;

  m_chronSvc->chronoPrint( "LArSCL1Mk hit loop " );
  m_chronSvc->chronoPrint( "LArSCL1Mk TT loop " );
  
  return StatusCode::SUCCESS;

}


StatusCode LArSCL1Maker::fillEMap(int& totHit) 

{
// +======================================================================+
// +                                                                      +
// + Author: Denis O. Damazio                                             +
// + Creation date: 18/11/2013                                            +
// +                                                                      +
// +======================================================================+


  MsgStream  msglog(messageService(),name());
//
// ........ reset the map Energies
//
  StatusCode sc = m_detectorStore->retrieve(m_hitmap,"LArHitEMap");
  if (sc.isFailure()) {
        msglog << " cannot retrieve LArHitEMap from detector Store " << endreq;
        return sc;
  }
  totHit = m_hitmap->GetNbCells();

  return StatusCode::SUCCESS;
}


//===========================================================================
int LArSCL1Maker::decodeInverse(int region, int eta)
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


StatusCode LArSCL1Maker::updateConditions(){

  MsgStream  msglog(messageService(),name());
  msglog << MSG::DEBUG << "Updating conditions" << endreq;

  if (m_detectorStore == 0 ) {
	msglog << "No valid DetStore, forget this" << endreq;
	return StatusCode::FAILURE;
  }

  if ( (m_detectorStore->retrieve(m_fracS,m_fSamplKey)).isFailure() && !m_fracS ) {
        msglog << "could not get fSampl conditions with key " << m_fSamplKey << endreq;
	return StatusCode::FAILURE;
  }

  if ( (m_detectorStore->retrieve(m_shapes,m_shapesKey)).isFailure() && !m_shapes ) {
        msglog << "could not get shape conditions with key " << m_shapesKey << endreq;
	return StatusCode::FAILURE;
  }

  if ( (m_detectorStore->retrieve(m_NoiseSC,m_noiseKey)).isFailure() && !m_NoiseSC ) {
        msglog << "could not get noise conditions with key " << m_noiseKey << endreq;
	return StatusCode::FAILURE;
  }

  if ( (m_detectorStore->retrieve(m_PedestalSC,m_pedestalKey)).isFailure() && !m_PedestalSC ) {
        msglog << "could not get pedestal conditions with key " << m_pedestalKey << endreq;
	return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;

}

void LArSCL1Maker::printConditions(const HWIdentifier& hwSC){

  	  MsgStream  msglog(messageService(),name());
	  msglog << MSG::INFO << "HW Identifier : " << hwSC.get_identifier32().get_compact() << endreq;
          if ( m_shapes ) {
                ILArShape::ShapeRef_t shape = m_shapes->Shape(hwSC,0);
                ILArShape::ShapeRef_t shapeder = m_shapes->ShapeDer(hwSC,0);
                msglog << MSG::INFO  << "shape0.size() : " << shape.size() << endreq;
                for(unsigned int i=0;i<shape.size();i++)
                   msglog << "shape[" << i << "]=" << shape[i] << " - " << shapeder[i] << "; ";
                msglog << endreq;
          }
          if ( m_fracS  ) {
                msglog << MSG::INFO << "fSample : " << m_fracS->FSAMPL(hwSC) << endreq;
          }
          if ( m_adc2mevTool ) {
                msglog << MSG::INFO << "Ramp (gain0) : " << (m_adc2mevTool->ADC2MEV(hwSC,(CaloGain::CaloGain)0))[1] << endreq;
                msglog << MSG::INFO << "Ramp (gain1) : " << (m_adc2mevTool->ADC2MEV(hwSC,(CaloGain::CaloGain)1))[1] << endreq;
                msglog << MSG::INFO << "Ramp (gain2) : " << (m_adc2mevTool->ADC2MEV(hwSC,(CaloGain::CaloGain)2))[1] << endreq;
          }
          if ( m_PedestalSC ) {
                msglog << MSG::INFO << "Pedestal : " << m_PedestalSC->pedestal(hwSC,0) << endreq;
          }
          if ( m_NoiseSC ) {
                msglog << "Noise : " << m_NoiseSC->noise(hwSC,0) << endreq;
          }
          if ( m_autoCorrNoiseTool ) {
		const std::vector<float>* CorrGen = &(m_autoCorrNoiseTool->autoCorrSqrt(hwSC,0,m_nSamples));
		msglog << MSG::INFO << "Auto : " ;
		for(size_t ii=0;ii<m_nSamples;++ii){
		msglog << CorrGen->at(ii) << " ";
		}
		msglog << endreq;
	  }
}

void LArSCL1Maker::ConvertHits2Samples(const HWIdentifier & hwSC, CaloGain::CaloGain igain,
                   const std::vector<std::pair<float,float> >  *TimeE, std::vector<float>& samples)
{
// Converts  hits of a particular LAr cell into energy samples
// declarations

   int nsamples ;
   int nsamples_der ;
   int i ;
   int j ;
   float energy ;
   float time ;

// ........ retrieve data (1/2) ................................
//
   ILArShape::ShapeRef_t Shape = m_shapes->Shape(hwSC,igain);
   ILArShape::ShapeRef_t ShapeDer = m_shapes->ShapeDer(hwSC,igain);

  nsamples = Shape.size();
  nsamples_der = ShapeDer.size();

  if (nsamples==0) {
    std::cout << " No samples for cell = " << hwSC << std::endl;
    return;
  }

  std::vector<std::pair<float,float> >::const_iterator first = TimeE->begin();
  std::vector<std::pair<float,float> >::const_iterator last  = TimeE->end();
  samples.clear();
  samples.assign(m_nSamples,0);
  //m_firstSample=0;

  while (first != last)
  {
   energy = (*first).first;
   time   = (*first).second;

 // Atlas like mode where we use 25ns binned pulse shape and derivative to deal with time offsets
// shift between reference shape and this time
      int ishift=(int)(rint(time/25.));
      double dtime=time-25.*((double)(ishift));

      for (i=0;i<(int)m_nSamples;i++)
      {
       j = i - ishift + m_firstSample;
       if (j >=0 && j < nsamples ) {
         if (j<nsamples_der && std::fabs(ShapeDer[j])<10. )
              samples[i] += (Shape[j]- ShapeDer[j]*dtime)*energy ;
         else samples[i] += Shape[j]*energy ;
       }
      }

   ++first;
  }         // loop over hits

   return;

}
