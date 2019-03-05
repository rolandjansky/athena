/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
  AthAlgorithm(name, pSvcLocator),
  m_onlineHelper(0),
  m_emId(0),
  m_hecId(0),
  m_fcalId(0),
  m_condSvc("CondSvc",name)
{  
  m_chids.clear();
  declareProperty("ChannelIDs",     m_chids);
  declareProperty("GroupingType",   m_groupingType  = "ExtendedFeedThrough"); // SubDetector, Single, FeedThrough, ExtendedFeedThrough
}

LArCaliWaveAverage::~LArCaliWaveAverage() {}

StatusCode LArCaliWaveAverage::initialize() {
  const CaloCell_ID* idHelper = nullptr;
  ATH_CHECK( detStore()->retrieve (idHelper, "CaloCell_ID") );
  m_emId = idHelper->em_idHelper();
  if (!m_emId) {
    ATH_MSG_ERROR ( "Could not get lar EM ID helper!" );
    return StatusCode::FAILURE;
  }
  m_fcalId=idHelper->fcal_idHelper();
  if (!m_fcalId) {
    ATH_MSG_ERROR ( "Could not get lar FCAL ID helper" );
    return StatusCode::FAILURE;
  }
  m_hecId=idHelper->hec_idHelper();
  if (!m_hecId) {
    ATH_MSG_ERROR ( "Could not get lar HEC ID helper" );
    return StatusCode::FAILURE;
  }

  ATH_CHECK( detStore()->retrieve(m_onlineHelper, "LArOnlineID") );
  
  ATH_CHECK(m_cablingKey.initialize());
  ATH_CHECK(m_keyInput.initialize());
  ATH_CHECK(m_keyOutputCorr.initialize());
  ATH_CHECK(m_keyOutputSymm.initialize());

  if ( m_chids.size() > 0 ) {
    ATH_MSG_INFO ( m_chids.size() << " channels selected for averaging." );
  } else {
    ATH_MSG_ERROR ( "No channels selected for averaging!" );
    return StatusCode::FAILURE;
  }

  // Register write handle
  ATH_CHECK( m_condSvc.retrieve() );
  if (m_condSvc->regHandle(this, m_keyOutputCorr).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_keyOutputCorr.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }
  if (m_condSvc->regHandle(this, m_keyOutputSymm).isFailure()) {
    ATH_MSG_ERROR("unable to register WriteCondHandle " << m_keyOutputSymm.fullKey() << " with CondSvc");
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}

StatusCode LArCaliWaveAverage::execute() {
  ATH_MSG_INFO ( "execute()" );
  return StatusCode::SUCCESS;
}

StatusCode LArCaliWaveAverage::stop() {
  ATH_MSG_INFO ( "stop()" );
  
  // Get input LArCaliWaveContainer
  SG::ReadCondHandle<LArCaliWaveContainer> inHdl(m_keyInput);
  const LArCaliWaveContainer* theLArCaliWaveContainer = *inHdl;
  if (! theLArCaliWaveContainer ) {
    ATH_MSG_ERROR ( "LArCaliWaveContainer (key = " << m_keyInput << ") is empty" );
    return StatusCode::FAILURE;
  }
  
  // create correction LArCaliWaveContainer
  auto larCaliWaveContainerCorr = std::make_unique<LArCaliWaveContainer>();
  ATH_CHECK( larCaliWaveContainerCorr->setGroupingType(m_groupingType,msg()) );
  ATH_CHECK( larCaliWaveContainerCorr->initialize() );
  
  // create symmetric waves LArCaliWaveContainer
  auto larCaliWaveContainerSymm = std::make_unique<LArCaliWaveContainer>();
  ATH_CHECK( larCaliWaveContainerSymm->setGroupingType(m_groupingType,msg()) );

  if (larCaliWaveContainerSymm->initialize()) {
    ATH_MSG_ERROR ( "Failed to initialize LArCaliWaveContainer object" );
    return StatusCode::FAILURE;
  }

  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  if(!cabling) {
     ATH_MSG_ERROR("Do not have mapping object " << m_cablingKey.key() );
     return StatusCode::FAILURE;
  }
  
  for ( unsigned i=0; i<m_chids.size();++i) { // loop over selected channels
    
    const HWIdentifier chid(m_chids[i]);

    std::vector<HWIdentifier> theSymmetricChannels = SymmetricChannels(chid,m_chids,cabling);
    
    if ( theSymmetricChannels.size() == 0) {
      ATH_MSG_WARNING ( "No symmetric channels found for channel 0x" << MSG::hex << chid << MSG::dec << ". Cannot average." );
      continue;
    } 
    
    ATH_MSG_INFO ( theSymmetricChannels.size() << " symmetric channels indentified for channel 0x" << MSG::hex << chid << MSG::dec );
    
    for ( unsigned gain = CaloGain::LARHIGHGAIN ; gain < CaloGain::LARNGAIN ; ++ gain ) { // loop over gains
      
      ATH_MSG_VERBOSE ( "Now processing gain = " << gain );
    
      // get Wave vector for current ChID/Gain, assuming it exists
      const LArCaliWaveContainer::LArCaliWaves& theCaliWaves = theLArCaliWaveContainer->get(chid,gain);
      if ( theCaliWaves.size()==0 ){
        ATH_MSG_WARNING ( "No pulses found for channel 0x" << MSG::hex << chid << MSG::dec 
                          << " in gain " << gain 
                          << ". Are you sure that readout is working? For the time being I'm skipping..." );
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
	  
	  ATH_MSG_INFO ( theSymmetricWavesThisDAC.size() 
                         << " symmetric LArCaliWaves found for channel 0x" << MSG::hex << chid << MSG::dec 
                         << " for DAC " << theDAC << " in gain " << gain 
                         << ". Now averaging ... "  );
	  
	  // Average symmetric waves corresponding to this DAC/gain
          LArCaliWave theAverageWave = WaveAverage(theSymmetricWavesThisDAC);

	  ATH_MSG_VERBOSE ( "... Done. Saving average to LArCaliWave vector." );
	  
	  theAverageWaves.push_back(theAverageWave);

          ATH_MSG_VERBOSE ( "Storing symmetric waves for the current DAC..." );

          for (unsigned kSym=0;kSym<theSymmetricWavesThisDAC.size();++kSym) {
	    theSymmetricWavesAll.push_back(theSymmetricWavesThisDAC[kSym]);
	  }


        } else {
	  ATH_MSG_WARNING ( "No symmetrich waves found for channel 0x" << MSG::hex << chid << MSG::dec 
                            << " for DAC " << theDAC << ": no average will be computed." );
	}
		
      } // end of loop over DAC values  
    
      // fill new LArCaliWaveContainer with average wave
      ATH_MSG_VERBOSE ( "Saving LArCaliWave vector to correction container." );
      larCaliWaveContainerCorr->setPdata(chid,theAverageWaves,gain);

      // store symmetric waves in other LArCaliWaveContainer (this is working only for a sinle DAC value!)
      ATH_MSG_VERBOSE ( "Saving LArCaliWave vector to symmetric waves' container (flag contains FT info)" );
      larCaliWaveContainerSymm->setPdata(chid,theSymmetricWavesAll,gain);

    } // end of loop over gains
    
  } // end of loop over selected channels
  
  // Record average LArCaliWaveContainer to DetectorStore
  // Define validity of the output cond object 
  const EventIDBase start{EventIDBase::UNDEFNUM, EventIDBase::UNDEFEVT, 0, 0, EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};
  const EventIDBase stop{EventIDBase::UNDEFNUM, EventIDBase::UNDEFEVT, EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM-1, EventIDBase::UNDEFNUM, EventIDBase::UNDEFNUM};
  EventIDRange rangeW{start, stop};
  SG::WriteCondHandle<LArCaliWaveContainer> corrHdl(m_keyOutputCorr);
  const EventIDRange crangeW(rangeW);
  if(corrHdl.record(crangeW,larCaliWaveContainerCorr.release()).isFailure()) {
    ATH_MSG_ERROR("Could not record LArCaliWaveContainer  object with " << m_keyOutputCorr.key()
                  << " with EventRange " << crangeW << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new " << m_keyOutputCorr.key() << " with range " << crangeW << " into Conditions Store");
  SG::WriteCondHandle<LArCaliWaveContainer> symHdl(m_keyOutputSymm);
  if(symHdl.record(crangeW,larCaliWaveContainerSymm.release()).isFailure()) {
    ATH_MSG_ERROR("Could not record LArCaliWaveContainer  object with " << m_keyOutputSymm.key()
                  << " with EventRange " << crangeW << " into Conditions Store");
    return StatusCode::FAILURE;
  }
  ATH_MSG_INFO("recorded new " << m_keyOutputCorr.key() << " with range " << crangeW << " into Conditions Store");
  return StatusCode::SUCCESS;
}

/* 
  SymmetricChannels() returns a vector of HWIdentifier corresponding 
  to the list of channels in the same et/phi position in all other FTs
*/
std::vector<HWIdentifier> LArCaliWaveAverage::SymmetricChannels(HWIdentifier ChID, std::vector<unsigned> ChannelsNotToUse, const LArOnOffIdMapping* cabling) 
{
  ATH_MSG_VERBOSE ( "Seeking symmetric cells for channel 0x" << MSG::hex << ChID << MSG::dec );
  
  std::vector<HWIdentifier> theSymmetricChannels;
  theSymmetricChannels.resize(0);
    
  Identifier id;
  try {
    id = cabling->cnvToIdentifier(ChID);
  } catch (LArID_Exception & execpt) {
    ATH_MSG_ERROR ( "LArCabling exception caught for channel 0x" << MSG::hex << ChID << MSG::dec );
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

  ATH_MSG_VERBOSE ( "  Detector  = "  << detector  );
  ATH_MSG_VERBOSE ( "  PosNeg    = "  << pos_neg   );
  ATH_MSG_VERBOSE ( "  Barrel/EC = "  << barrel_ec );

  ATH_MSG_VERBOSE ( "  FT        = "  << FT      );
  ATH_MSG_VERBOSE ( "  Slot      = "  << slot    );
  ATH_MSG_VERBOSE ( "  Channel   = "  << channel );
  
  /*
    ATH_MSG_VERBOSE ( "  Region   = "   << region );
    ATH_MSG_VERBOSE ( "  Layer    = "   << layer  );
    ATH_MSG_VERBOSE ( "  Eta      = "   << eta    );
    ATH_MSG_VERBOSE ( "  Phi      = "   << phi    );
  */
  
  unsigned nFT = 0;
  
  if ( detector != 0 ) {
    ATH_MSG_ERROR ( "Sorry, still implemented for EM only :-(" );
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
  if ( ToBeAveraged.size()>0 ) { 
    
    ATH_MSG_VERBOSE ( "... Averaging wave number 1" );
    LArWave theWaveAverage = (LArWave)ToBeAveraged[0];
    
    for (unsigned i=1;i<ToBeAveraged.size();++i) {
      ATH_MSG_VERBOSE ( "... Averaging wave number " << i+1 );
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
