/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArCaliWaveAverage.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ToolHandle.h"

#include "LArRawConditions/LArCaliWaveContainer.h"
#include "CaloIdentifier/CaloGain.h"

#include <vector>
#include <map>

typedef std::map<int, LArCaliWave> WaveMap;
  
LArCaliWaveAverage::LArCaliWaveAverage(const std::string& name, ISvcLocator* pSvcLocator) :
  Algorithm(name, pSvcLocator),
  m_detStore(0),
  m_larCablingSvc(0),
  m_onlineHelper(0),
  m_emId(0),
  m_hecId(0),
  m_fcalId(0)
{  
  declareProperty("KeyInput",       m_keyInput      = "LArCaliWave");
  declareProperty("KeyOutputCorr",  m_keyOutputCorr = "LArCaliWaveCorr");  
  declareProperty("KeyOutputSymm",  m_keyOutputSymm = "LArCaliWaveSymm");  
  m_chids.clear();
  declareProperty("ChannelIDs",     m_chids);
  declareProperty("GroupingType",   m_groupingType  = "ExtendedFeedThrough"); // SubDetector, Single, FeedThrough, ExtendedFeedThrough
}

LArCaliWaveAverage::~LArCaliWaveAverage() {}

StatusCode LArCaliWaveAverage::initialize() {
  MsgStream log(msgSvc(), name());
  
  StatusCode sc = service("DetectorStore",m_detStore);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "Cannot get DetectorStore!" << endreq;
    return sc;
  }
  
  const CaloIdManager *caloIdMgr = CaloIdManager::instance();
  m_emId = caloIdMgr->getEM_ID();
  if (!m_emId) {
    log << MSG::ERROR << "Could not get lar EM ID helper!" << endreq;
    return StatusCode::FAILURE;
  }
  m_fcalId=caloIdMgr->getFCAL_ID();
  if (!m_fcalId) {
    log << MSG::ERROR << "Could not get lar FCAL ID helper" << endreq;
    return StatusCode::FAILURE;
  }
  m_hecId=caloIdMgr->getHEC_ID();
  if (!m_hecId) {
    log << MSG::ERROR << "Could not get lar HEC ID helper" << endreq;
    return StatusCode::FAILURE;
  }

  sc = m_detStore->retrieve(m_onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    log << MSG::ERROR << "Could not get LArOnlineID!" << endreq;
    return sc;
  }
  
  ToolHandle<LArCablingService> larCablingSvc("LArCablingService");
  sc = larCablingSvc.retrieve();
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << " Can't get LArCablingSvc " << endreq;
    return sc;
  }

  if ( m_chids.size() > 0 ) {
     log << MSG::INFO << m_chids.size() << " channels selected for averaging." << endreq;
  } else {
    log << MSG::ERROR << "No channels selected for averaging!" << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

StatusCode LArCaliWaveAverage::execute() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "execute()" << endreq;
  return StatusCode::SUCCESS;
}

StatusCode LArCaliWaveAverage::stop() {
  MsgStream log(msgSvc(), name());
  log << MSG::INFO << "stop()" << endreq;
  
  // Get input LArCaliWaveContainer
  const LArCaliWaveContainer* theLArCaliWaveContainer;
  StatusCode sc = m_detStore->retrieve(theLArCaliWaveContainer,m_keyInput);
  if (sc.isFailure()) {
    log << MSG::ERROR << "LArCaliWaveContainer (key = " << m_keyInput << ") not found in StoreGate" << endreq;
    return sc;
  }
  if ( theLArCaliWaveContainer == NULL ) {
    log << MSG::ERROR << "LArCaliWaveContainer (key = " << m_keyInput << ") is empty" << endreq;
    return StatusCode::FAILURE;
  }
  
  // create correction LArCaliWaveContainer
  LArCaliWaveContainer* larCaliWaveContainerCorr = new LArCaliWaveContainer();
  if (larCaliWaveContainerCorr->setGroupingType(m_groupingType,log).isFailure()) {
    log << MSG::ERROR << "Failed to set groupingType for LArCaliWaveContainer object" << endreq;
    return StatusCode::FAILURE;
  }

  if (larCaliWaveContainerCorr->initialize().isFailure()) {
    log << MSG::ERROR << "Failed to initialize LArCaliWaveContainer object" << endreq;
    return StatusCode::FAILURE;
  }
  
  // create symmetric waves LArCaliWaveContainer
  LArCaliWaveContainer* larCaliWaveContainerSymm = new LArCaliWaveContainer();
  if (larCaliWaveContainerSymm->setGroupingType(m_groupingType,log).isFailure()) {
    log << MSG::ERROR << "Failed to set groupingType for LArCaliWaveContainer object" << endreq;
    return StatusCode::FAILURE;
  }

  if (larCaliWaveContainerSymm->initialize()) {
    log << MSG::ERROR << "Failed to initialize LArCaliWaveContainer object" << endreq;
    return StatusCode::FAILURE;
  }
  
  for ( unsigned i=0; i<m_chids.size();++i) { // loop over selected channels
    
    const HWIdentifier chid(m_chids[i]);

    std::vector<HWIdentifier> theSymmetricChannels = SymmetricChannels(chid,m_chids);
    
    if ( theSymmetricChannels.size() == 0) {
      log << MSG::WARNING << "No symmetric channels found for channel 0x" << MSG::hex << chid << MSG::dec << ". Cannot average." << endreq;    
      continue;
    } 
    
    log << MSG::INFO << theSymmetricChannels.size() << " symmetric channels indentified for channel 0x" << MSG::hex << chid << MSG::dec << endreq;    
    
    for ( unsigned gain = CaloGain::LARHIGHGAIN ; gain < CaloGain::LARNGAIN ; ++ gain ) { // loop over gains
      
      log << MSG::VERBOSE << "Now processing gain = " << gain << endreq;
    
      // get Wave vector for current ChID/Gain, assuming it exists
      const LArCaliWaveContainer::LArCaliWaves& theCaliWaves = theLArCaliWaveContainer->get(chid,gain);
      if ( theCaliWaves.size()==0 ){
        log << MSG::WARNING << "No pulses found for channel 0x" << MSG::hex << chid << MSG::dec 
	                    << " in gain " << gain 
			    << ". Are you sure that readout is working? For the time being I'm skipping..."
			    << endreq;
        continue;	
      }

      LArCaliWaveVec theAverageWaves;

      LArCaliWaveVec theSymmetricWavesAll; // vector of symmetric waves for the all DAC       
      
      for (unsigned iDAC=0;iDAC<theCaliWaves.size();++iDAC) { // loop over DAC values
        
        int theDAC = theCaliWaves[iDAC].getDAC();    
	
	LArCaliWaveVec theSymmetricWavesThisDAC; // vector of symmetric waves for the current DAC 
	
        for (unsigned iSym=0;iSym<theSymmetricChannels.size();++iSym) { // loop over SymmetricChannels and fill LArCaliWave vector corresponding to current DAC
	
	   const LArCaliWaveContainer::LArCaliWaves& theseCaliWaves = theLArCaliWaveContainer->get(HWIdentifier(theSymmetricChannels[iSym]),gain);
	   
	   for (unsigned kDAC=0;kDAC<theseCaliWaves.size();++kDAC) {
	      if ( theseCaliWaves[kDAC].getDAC() == theDAC ) {
	         LArCaliWave thisWave = theseCaliWaves[kDAC];
	         // store additional information in LArWave flag, e.g. FT
                 unsigned theFlag = m_onlineHelper->feedthrough(HWIdentifier(theSymmetricChannels[iSym]));
		 thisWave.setFlag(theFlag);
		 // save symmetric wave in vetor to be averaged
                 theSymmetricWavesThisDAC.push_back(thisWave);
	      } 
	   }
	   	   
	}
	
	if ( theSymmetricWavesThisDAC.size()>0 ) {
	  
	  log << MSG::INFO << theSymmetricWavesThisDAC.size() 
	                   << " symmetric LArCaliWaves found for channel 0x" << MSG::hex << chid << MSG::dec 
	                   << " for DAC " << theDAC << " in gain " << gain 
                           << ". Now averaging ... " 
			   << endreq;  
	  
	  // Average symmetric waves corresponding to this DAC/gain
          LArCaliWave theAverageWave = WaveAverage(theSymmetricWavesThisDAC);

	  log << MSG::VERBOSE << "... Done. Saving average to LArCaliWave vector." << endreq;
	  
	  theAverageWaves.push_back(theAverageWave);

          log << MSG::VERBOSE << "Storing symmetric waves for the current DAC..." << endreq;

          for (unsigned kSym=0;kSym<theSymmetricWavesThisDAC.size();++kSym) {
	    theSymmetricWavesAll.push_back(theSymmetricWavesThisDAC[kSym]);
	  }


        } else {
	  log << MSG::WARNING << "No symmetrich waves found for channel 0x" << MSG::hex << chid << MSG::dec 
	                      << " for DAC " << theDAC << ": no average will be computed." << endreq;  
	}
		
      } // end of loop over DAC values  
    
      // fill new LArCaliWaveContainer with average wave
      log << MSG::VERBOSE << "Saving LArCaliWave vector to correction container." << endreq;
      larCaliWaveContainerCorr->setPdata(chid,theAverageWaves,gain);

      // store symmetric waves in other LArCaliWaveContainer (this is working only for a sinle DAC value!)
      log << MSG::VERBOSE << "Saving LArCaliWave vector to symmetric waves' container (flag contains FT info)" << endreq;
      larCaliWaveContainerSymm->setPdata(chid,theSymmetricWavesAll,gain);

    } // end of loop over gains
    
  } // end of loop over selected channels
  
  // Record average LArCaliWaveContainer to DetectorStore
  sc=m_detStore->record(larCaliWaveContainerCorr,m_keyOutputCorr);
  if (sc.isFailure()) {
    log << MSG::FATAL << "Cannot record LArCaliWaveContainer to StoreGate with key = " << m_keyOutputCorr << endreq;
    return sc;
  }

  sc=m_detStore->record(larCaliWaveContainerSymm,m_keyOutputSymm);
  if (sc.isFailure()) {
    log << MSG::FATAL << "Cannot record LArCaliWaveContainer to StoreGate with key = " << m_keyOutputSymm << endreq;
    return sc;
  }
  
  return StatusCode::SUCCESS;
}

/* 
  SymmetricChannels() returns a vector of HWIdentifier corresponding 
  to the list of channels in the same et/phi position in all other FTs
*/
std::vector<HWIdentifier> LArCaliWaveAverage::SymmetricChannels(HWIdentifier ChID, std::vector<unsigned> ChannelsNotToUse) 
{
  MsgStream log(msgSvc(), name());

  log << MSG::VERBOSE << "Seeking symmetric cells for channel 0x" << MSG::hex << ChID << MSG::dec << endreq;
  
  std::vector<HWIdentifier> theSymmetricChannels;
  theSymmetricChannels.resize(0);
    
  Identifier id;
  try {
    id = m_larCablingSvc->cnvToIdentifier(ChID);
  } catch (LArID_Exception & execpt) {
    log << MSG::ERROR << "LArCabling exception caught for channel 0x" << MSG::hex << ChID << MSG::dec << endreq ;
    return theSymmetricChannels;
  }
  
  int detector  = -999;
  int pos_neg   = m_onlineHelper->pos_neg(ChID);
  int barrel_ec = m_onlineHelper->barrel_ec(ChID);
  
  int FT	= m_onlineHelper->feedthrough(ChID);
  int slot      = m_onlineHelper->slot(ChID);
  int channel   = m_onlineHelper->channel(ChID);
  
  /*
  int region    = -999;
  int layer     = -999;
  int eta       = -999;
  int phi       = -999;
  */
  
  if ( m_emId->is_em_barrel(id) || m_emId->is_em_endcap(id) ) {
     detector = 0;
     /*
     eta      = m_emId->eta(id);
     phi      = m_emId->phi(id);
     layer    = m_emId->sampling(id);
     region   = m_emId->region(id);
     */
  }
  if ( m_hecId->is_lar_hec(id) ) {
     detector = 1;
     /*
     eta      = m_hecId->eta(id);
     phi      = m_hecId->phi(id);
     layer    = m_hecId->sampling(id);
     region   = m_hecId->region(id);
     */
  }
  if ( m_fcalId->is_lar_fcal(id) ) {
     detector = 2;
     /*
     eta      = m_fcalId->eta(id);
     phi      = m_fcalId->phi(id);
     layer    = m_fcalId->module(id);
     region   = 0;
     */
  }

  log << MSG::VERBOSE << "  Detector  = "  << detector  << endreq;
  log << MSG::VERBOSE << "  PosNeg    = "  << pos_neg   << endreq;
  log << MSG::VERBOSE << "  Barrel/EC = "  << barrel_ec << endreq;

  log << MSG::VERBOSE << "  FT        = "  << FT      << endreq;
  log << MSG::VERBOSE << "  Slot      = "  << slot    << endreq;
  log << MSG::VERBOSE << "  Channel   = "  << channel << endreq;
  
  /*
  log << MSG::VERBOSE << "  Region   = "   << region << endreq;
  log << MSG::VERBOSE << "  Layer    = "   << layer  << endreq;  
  log << MSG::VERBOSE << "  Eta      = "   << eta    << endreq;
  log << MSG::VERBOSE << "  Phi      = "   << phi    << endreq;
  */
  
  unsigned nFT = 0;
  
  if ( detector != 0 ) {
    log << MSG::ERROR << "Sorry, still implemented for EM only :-(" << endreq ;
    return theSymmetricChannels;   
  } else {
    nFT = 32;
  }
  
  for ( unsigned iFT = 0;iFT<nFT;++iFT) { // loop over all FTs
     
     // compute ChID fpr channel in iFT in same position
     bool useThisChannel = true;
     HWIdentifier thisChID = m_onlineHelper->channel_Id(barrel_ec,pos_neg,iFT,slot,channel);
     
     // check if channel is allowed...
     for ( unsigned i=0;i<ChannelsNotToUse.size();++i) {
       if ( thisChID == HWIdentifier(ChannelsNotToUse[i]) ) {
          useThisChannel = false;
	  break;
       } 
     }
     
     if ( useThisChannel ) 
       theSymmetricChannels.push_back(thisChID);   
  }
  
  return theSymmetricChannels;
} 


LArCaliWave LArCaliWaveAverage::WaveAverage(std::vector<LArCaliWave> ToBeAveraged) 
{   
  MsgStream log(msgSvc(), name()); 
  
  if ( ToBeAveraged.size()>0 ) { 
    
    log << MSG::VERBOSE << "... Averaging wave number 1" << endreq;
    LArWave theWaveAverage = (LArWave)ToBeAveraged[0];
    
    for (unsigned i=1;i<ToBeAveraged.size();++i) {
      log << MSG::VERBOSE << "... Averaging wave number " << i+1 << endreq;
      theWaveAverage = theWaveAverage + (LArWave)ToBeAveraged[i];
    }
    
    theWaveAverage = theWaveAverage * (1./float(ToBeAveraged.size()));
    
    LArCaliWave theCaliWaveAverage( theWaveAverage.getWave() ,
				    ToBeAveraged[0].getErrors(),
				    ToBeAveraged[0].getTriggers(),
				    ToBeAveraged[0].getDt(), 
				   (ToBeAveraged[0].getDAC() + (ToBeAveraged[0].getIsPulsedInt()<<16)), 
				    ToBeAveraged[0].getFlag() );
    
    return theCaliWaveAverage;
        
  } else {
    LArCaliWave emptyWave = LArCaliWave();
    return emptyWave;
  }
  
}
