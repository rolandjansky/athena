/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArMasterWaveBuilder.h"

#include "GaudiKernel/ToolHandle.h"
#include "LArTools/LArCablingService.h"
#include "LArRawConditions/LArCaliWaveContainer.h"
#include "CaloIdentifier/CaloIdManager.h"
#include "CaloIdentifier/CaloGain.h"

#include <stdio.h>
#include <iostream>
#include <fstream>

typedef LArCaliWaveContainer::ConstConditionsMapIterator   CaliCellIt;
typedef LArCaliWaveContainer::LArCaliWaves::const_iterator CaliWaveIt;

inline std::string ChanDacGain(HWIdentifier ch,int DAC,int gain) {
  char compact[25] ;
  int ich = ch.get_identifier32().get_compact() ;
  sprintf(compact,"[%10d;%5d,%1d]",ich,DAC,gain) ;
  return std::string(compact) ;
}

LArMasterWaveBuilder::LArMasterWaveBuilder (const std::string& name, ISvcLocator* pSvcLocator) : 
  Algorithm(name, pSvcLocator),
  m_waveHelper(0),
  m_groupingType("FeedThrough") // SubDetector, Single, FeedThrough
{
  declareProperty("KeyInput",          m_keyinput  = "LArCaliWave" );
  declareProperty("KeyOutput",         m_keyoutput = "LArMasterWave" );
  declareProperty("Normalize",         m_normalize = false);
  declareProperty("TimeShift",         m_timeShift = false);
  declareProperty("DiffWaveNormPeak",  m_diffWaveNormPeak = false);
  declareProperty("BuildMWwithOneDAC", m_buildMWoneDAC   = false);
  declareProperty("ListAllAnalysedChannels",m_listAllAnalysedChannels = false);  
  declareProperty("LinearityTolerance",m_linearityTolerance=0);
  declareProperty("ADCsaturation",     m_ADCsatur=0) ;
  declareProperty("DACminimum",        m_DACmin=0) ;
  m_dacMinLayer0.resize(0);
  m_dacMinLayer1.resize(0);
  m_dacMinLayer2.resize(0);
  m_dacMinLayer3.resize(0);
  declareProperty("DACMinPS",    m_dacMinLayer0);
  declareProperty("DACMinStrips",m_dacMinLayer1);
  declareProperty("DACMinMiddle",m_dacMinLayer2);
  declareProperty("DACMinBack",  m_dacMinLayer3);
  m_dacMaxLayer0.resize(0);
  m_dacMaxLayer1.resize(0);
  m_dacMaxLayer2.resize(0);
  m_dacMaxLayer3.resize(0);
  declareProperty("DACMaxPS",	 m_dacMaxLayer0);
  declareProperty("DACMaxStrips",m_dacMaxLayer1);
  declareProperty("DACMaxMiddle",m_dacMaxLayer2);
  declareProperty("DACMaxBack",  m_dacMaxLayer3);
  declareProperty("GroupingType",m_groupingType);
}

LArMasterWaveBuilder::~LArMasterWaveBuilder() 
{}

StatusCode LArMasterWaveBuilder::initialize() {
  StatusCode sc ;
  MsgStream log(msgSvc(), name());
  
  // Check DACMin jobOption consistency, in case setup default values
  if ( m_dacMinLayer0.size()<3 ) {
     log << MSG::WARNING << "DACMinPS     jobOption has wrong size. Will use default." << endreq ;
     m_dacMinLayer0.resize(3);
     m_dacMinLayer0[0] = 50; 
     m_dacMinLayer0[1] = 50 ;
     m_dacMinLayer0[2] = 50 ;
  }
  if ( m_dacMinLayer1.size()<3 ) {
     log << MSG::WARNING << "DACMinStrips jobOption has wrong size. Will use default." << endreq ;
     m_dacMinLayer1.resize(3);
     m_dacMinLayer1[0] = 50 ; 
     m_dacMinLayer1[1] = 50 ;
     m_dacMinLayer1[2] = 50 ;
  }
  if ( m_dacMinLayer2.size()<3 ) {
     log << MSG::WARNING << "DACMinMiddle jobOption has wrong size. Will use default." << endreq ;
     m_dacMinLayer2.resize(3);
     m_dacMinLayer2[0] = 50 ; 
     m_dacMinLayer2[1] = 50 ;
     m_dacMinLayer2[2] = 50 ;
  }
  if ( m_dacMinLayer3.size()<3 ) {
     log << MSG::WARNING << "DACMinBack   jobOption has wrong size. Will use default." << endreq ;
     m_dacMinLayer3.resize(3);
     m_dacMinLayer3[0] = 50 ; 
     m_dacMinLayer3[1] = 50 ;
     m_dacMinLayer3[2] = 50 ;
  }

  // Check DACMax jobOption consistency, in case setup default values
  if ( m_dacMaxLayer0.size()<3 ) {
     log << MSG::WARNING << "DACMaxPS     jobOption has wrong size. Will use default." << endreq ;
     m_dacMaxLayer0.resize(3);
     m_dacMaxLayer0[0] = 15000; 
     m_dacMaxLayer0[1] = 50000 ;
     m_dacMaxLayer0[2] = 65000 ;
  }
  if ( m_dacMaxLayer1.size()<3 ) {
     log << MSG::WARNING << "DACMaxStrips jobOption has wrong size. Will use default." << endreq ;
     m_dacMaxLayer1.resize(3);
     m_dacMaxLayer1[0] = 800 ; 
     m_dacMaxLayer1[1] = 8000 ;
     m_dacMaxLayer1[2] = 65000 ;
  }
  if ( m_dacMaxLayer2.size()<3 ) {
     log << MSG::WARNING << "DACMaxMiddle jobOption has wrong size. Will use default." << endreq ;
     m_dacMaxLayer2.resize(3);
     m_dacMaxLayer2[0] = 1000 ; 
     m_dacMaxLayer2[1] = 10000 ;
     m_dacMaxLayer2[2] = 65000 ;
  }
  if ( m_dacMaxLayer3.size()<3 ) {
     log << MSG::WARNING << "DACMaxBack   jobOption has wrong size. Will use default." << endreq ;
     m_dacMaxLayer3.resize(3);
     m_dacMaxLayer3[0] = 800 ; 
     m_dacMaxLayer3[1] = 8000 ;
     m_dacMaxLayer3[2] = 65000 ;
  }

  m_waveHelper = new LArWaveHelper() ;

  return StatusCode::SUCCESS ;
}

StatusCode LArMasterWaveBuilder::stop()
{
  MsgStream log(msgSvc(), name());

  log << MSG::INFO << "LArMasterWaveBuilder in stop..." << endreq ;

  // Get access to the Detector Store
  StoreGateSvc* detStore; 
  StatusCode sc=service("DetectorStore",detStore);
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << "Cannot get DetectorStore!" << endreq;
    return sc;
  }

  const LArOnlineID*   onlineHelper;
  const LArEM_ID*      emId;
  
  // Retrieve LArCablingService
  ToolHandle<LArCablingService> larCablingSvc("LArCablingService");
  sc = larCablingSvc.retrieve();
  if (sc!=StatusCode::SUCCESS) {
    log << MSG::ERROR << " Can't get LArCablingSvc " << endreq;
    return sc;
  }

  sc = detStore->retrieve(onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    log << MSG::ERROR << "Could not get LArOnlineID helper !" << endreq;
    return StatusCode::FAILURE;
  }
  
  const CaloIdManager* caloIdMgr=CaloIdManager::instance();
  emId=caloIdMgr->getEM_ID();


  log << MSG::INFO << "Check on ADC saturation: " ;
  if ( m_ADCsatur > 0 ) {
    log << "absolute maximum less than " << m_ADCsatur << endreq ;
  } else if ( m_ADCsatur < 0 ) {
    log << "maximum relative to sample 0 less than " << -m_ADCsatur << endreq ;
  } else {
    log << "none!" << endreq ;
  }
  log << MSG::INFO << "Check on mimimum DAC value: " ;
  if ( m_DACmin > 0 ) {
    log << m_DACmin << endreq ;
  } else {
    log << "none!" << endreq ;
  }
  if ( m_normalize ) {
    log << MSG::INFO << "Master Waveforms will be normalized" << endreq ;
  } else {
    log << MSG::INFO << "Master Waveforms will NOT be normalized" << endreq ;
  }
  if ( m_timeShift ) {
    log << MSG::INFO << "Master Waveforms will be shifted to the origin" << endreq ;
  } else {
    log << MSG::INFO << "Master Waveforms will NOT be shifted to the origin" << endreq ;
  }
  if ( m_buildMWoneDAC ) {
    log << MSG::INFO << "Build MWs even with only one usable DAC" << endreq ;
  } else {
    log << MSG::INFO << "Don't build MWs if only one usable DAC" << endreq ;
  }
  if ( m_diffWaveNormPeak ) {
    log << MSG::INFO << "For linearity check, normalize to peak " << endreq ;
  } else {
    log << MSG::INFO << "For linearity check, normalize to DAC " << endreq ;
  }
  log << MSG::INFO << "Linearity tolerance = " << m_linearityTolerance << endreq ;


  // Retrieve container
  const LArCaliWaveContainer* caliWaveContainer;
  sc= detStore->retrieve(caliWaveContainer,m_keyinput);
  if (sc.isFailure()) {
      log << MSG::DEBUG << "LArCaliWaveContainer (key = " << m_keyinput << ") not found in StoreGate" 
	  << endreq;
      return StatusCode::FAILURE;
  }
  
  if ( caliWaveContainer == NULL ) {
      log << MSG::DEBUG << "LArCaliWaveContainer (" << m_keyinput << ") is empty" << endreq;
      return StatusCode::FAILURE;
  }
  
  log << MSG::INFO << "Processing LArCaliWaveContainer from StoreGate, key = " << m_keyinput << endreq;

  // create new LArCaliWaveContainer for master waveforms and dac0 waveforms
  LArCaliWaveContainer* mwContainer = new LArCaliWaveContainer();

  sc=mwContainer->setGroupingType(m_groupingType,log);
  if (sc.isFailure()) {
    log << MSG::ERROR << "Failed to set groupingType for LArCaliWaveContainer object" << endreq;
    return sc;
  }
  sc=mwContainer->initialize(); 
  if (sc.isFailure()) {
    log << MSG::ERROR << "Failed initialize LArCaliWaveContainer object" << endreq;
    return sc;
  }

  
  // prepare a map for bad channels/DACs (i.e.not linear, not pulsed...). Access is badDACs[gain][channel][k-DAC]
  std::vector< std::map<HWIdentifier, std::vector<int> > > badDACs ;
  badDACs.resize((int)CaloGain::LARNGAIN) ;

  unsigned nMasterWaves = 0 ;

  for ( unsigned gain_it = CaloGain::LARHIGHGAIN; gain_it!=CaloGain::LARNGAIN; ++gain_it ) { // Loop over Gains

    log << MSG::DEBUG << "Evaluating master wavefroms for gain " << gain_it << endreq ;

    // set gain factor
    unsigned gainFactor = (unsigned)(pow(10,2-gain_it)) ;  // 100 for HG, 10 for MG, 1 for LG

    // set min/max values for DAC for all layers, in this gain
    std::vector<int> minDAC , maxDAC ;
    minDAC.push_back(m_dacMinLayer0[gain_it]) ;
    minDAC.push_back(m_dacMinLayer1[gain_it]) ;
    minDAC.push_back(m_dacMinLayer2[gain_it]) ;
    minDAC.push_back(m_dacMinLayer3[gain_it]) ;
    maxDAC.push_back(m_dacMaxLayer0[gain_it]) ;
    maxDAC.push_back(m_dacMaxLayer1[gain_it]) ;
    maxDAC.push_back(m_dacMaxLayer2[gain_it]) ;
    maxDAC.push_back(m_dacMaxLayer3[gain_it]) ;
    
    log << MSG::INFO << "Minimum DAC allowed for layers 0,1,2,3 and gain " << gain_it << endreq ;
    log << MSG::INFO << "       " << minDAC << endreq ;
    log << MSG::INFO << "Maximum DAC allowed for layers 0,1,2,3 and gain " << gain_it << endreq ;
    log << MSG::INFO << "       " << maxDAC << endreq ;

    int DACMin = 0 ;
    int DACMax = 65000 ;

    CaliCellIt cell_it   = caliWaveContainer->begin(gain_it);
    CaliCellIt cell_it_e = caliWaveContainer->end(gain_it);

    if ( cell_it == cell_it_e ) {
      log << MSG::DEBUG << "LArCaliWaveContainer (key = " << m_keyinput << ") has no wave with gain = " << gain_it << endreq;
      continue;
    } else {
      log << MSG::DEBUG << "Processing LArCaliWaveContainer (key = " << m_keyinput << ") in gain = " << gain_it << endreq;
    }

    for ( ; cell_it!=cell_it_e ; ++cell_it ) { // loop through channels
      
      CaliWaveIt wave_it = cell_it->begin();
      CaliWaveIt wave_it_e = cell_it->end();
      if ( wave_it == wave_it_e ) {
        log << MSG::DEBUG << "Empty channel found..." << endreq ;
        continue; // skip empty channels
      } 
      
      const LArCaliWaveContainer::LArCaliWaves& vCaliWaves = *cell_it;       
      
      HWIdentifier chID = cell_it.channelId() ;
      double dt         = vCaliWaves[0].getDt() ;
      unsigned nDACs    = vCaliWaves.size() ;

      log << MSG::DEBUG << nDACs << " DAC's found for channel 0x" << MSG::hex << chID.get_compact() << MSG::dec 
	                << " in gain " << gain_it << endreq ;
      log << MSG::DEBUG << "DAC list: " ;
      for ( unsigned i=0 ; i<nDACs ; i++ ) 
	  log << vCaliWaves[i].getDAC() << " " ;
      log << endreq ;
      
      long layer  = -1; 
      long region = -1;

      try {
        Identifier id = larCablingSvc->cnvToIdentifier(chID);
        layer  = emId->sampling(id) ;
        region = emId->region(id) ;
      } catch ( LArID_Exception & except ) { 
     	log << MSG::WARNING << "A larCablingSvc exception was caught for channel 0x" 
     	    << MSG::hex << chID.get_compact() << MSG::dec 
     	    << ". Skipping." << endreq;
     	continue ;
      }

      if ( region==0 && layer>=0 && layer<=3 ) {  // so far, only for EMB
	  DACMin = minDAC[layer] ;
	  DACMax = maxDAC[layer] ;
      }

      std::vector<double>   vDACs ;
      std::vector<LArWave*> vWaves ;
      std::vector<bool>     usable ;
      std::vector<int>      thisDAC ;
      std::vector<double>   thisPeak ;
      usable.resize(nDACs) ;
      thisDAC.resize(nDACs) ;
      thisPeak.resize(nDACs) ;
      
      // first loop on the waves of this channel: reject waves out of range and with weak signal
      for ( unsigned i=0 ; i<nDACs ; i++ ) {
	thisDAC[i]  = vCaliWaves[i].getDAC() ;
	thisPeak[i] = vCaliWaves[i].getSample(m_waveHelper->getMax(vCaliWaves[i])) ;
	// check overall DAC threshold
	if ( m_DACmin > 0 && thisDAC[i] < m_DACmin ) {  
	    log << MSG::DEBUG << ChanDacGain(chID,thisDAC[i],gain_it) 
		<< "DAC below threshold... skip!" << endreq ;
	    usable[i] = false ;
	    continue ;
	}
	// check overall ADC saturation
	if ( m_ADCsatur != 0 ) {  
	    if ( m_ADCsatur>0 && thisPeak[i]>m_ADCsatur ) { // suitable if no pedestal subtraction
	      log << MSG::DEBUG << ChanDacGain(chID,thisDAC[i],gain_it) 
		  << " absolute ADC saturation... skip!" << endreq ;
	      usable[i] = false ;
	      continue ;
	    } else if ( m_ADCsatur<0 && (thisPeak[i]-vCaliWaves[i].getSample(0))>(-m_ADCsatur) ) { 
	      // for pedestal subtracted
	      log << MSG::DEBUG << ChanDacGain(chID,thisDAC[i],gain_it) 
		  << " relative ADC saturation... skip!" << endreq ;
	      usable[i] = false ;
	      continue ;
	    }
	}
	// check if clearly no signal
	if ( fabs( thisPeak[i]/thisDAC[i] ) < gainFactor*0.0001 ) {
	    log << MSG::DEBUG << ChanDacGain(chID,thisDAC[i],gain_it) << " low signal... skip!" << endreq ;
	    usable[i] = false ;
	    continue ; 
	}
	usable[i] = true ;
      }

      // seek reference cali wave (for linearity check)
      int refIndex = -1 ;
      double minOverallDiff = 9.e+99 ;
      for ( unsigned i=0 ; i<nDACs ; i++ ) {
	  if ( ! usable[i] )  continue ;
	  if ( thisDAC[i]<DACMin || thisDAC[i]>DACMax ) continue ;
	  double thisOverallDiff = 0 ;
	  for ( unsigned k=0 ; k<nDACs ; k++ ) {
	    if ( ! usable[k] )  continue ;
	    if ( k == i )       continue ;
	    double theDiff = diffWave( vCaliWaves[i] , vCaliWaves[k] , m_diffWaveNormPeak ) ;
	    if ( theDiff>0 )  thisOverallDiff += theDiff ;
	  }
	  if ( thisOverallDiff < minOverallDiff ) {
	    refIndex = i ;
	    minOverallDiff = thisOverallDiff ;
	  }
      }
      int refDAC = thisDAC[refIndex] ;

      // reject strongly non-linear waves (if reference wave was found)
      if ( refIndex >=0 ) {
	  log << MSG::DEBUG << "Check non-linearity against DAC " << refDAC << endreq ;
	  for ( unsigned i=0 ; i<nDACs ; i++ ) {
	    if ( ! usable[i] ) continue ;
	    int tbin ;
	    double relDiff = diffWave(vCaliWaves[i],vCaliWaves[refIndex],tbin,m_diffWaveNormPeak) ;
	    if ( ! m_diffWaveNormPeak )  relDiff *= (refDAC/thisPeak[refIndex]) ;
	    if ( relDiff > m_linearityTolerance ) {
	      log << MSG::DEBUG << ChanDacGain(chID,thisDAC[i],gain_it) 
		  << " non-linearity detected, rel.resid= " << relDiff 
		  << " at t= " << tbin*vCaliWaves[i].getDt() << " tbin= " << tbin
		  << " ... skip!" << endreq ;
	      usable[i] = false ;
	    } else if ( relDiff < 0 ) {
	      log << MSG::DEBUG << ChanDacGain(chID,thisDAC[i],gain_it) 
		  << " could not check linearity... skip!" << endreq ;
	      usable[i] = false ;
	    } else {
	      log << MSG::DEBUG << ChanDacGain(chID,thisDAC[i],gain_it) 
		  << " linear within " << relDiff << endreq ;
	    }
	  } // end for i
      } else {
	  log << MSG::DEBUG << "Could not find reference DAC, just reject all waves not in DAC ["
	      << DACMin << "," << DACMax << "]" << endreq ;
	  for ( unsigned i=0 ; i<nDACs ; i++ ) {
	    if ( thisDAC[i]<DACMin || thisDAC[i]>DACMax )  usable[i] = false ;
	  }
      }

      for ( unsigned i=0 ; i<nDACs ; i++ ) {
	  if ( usable[i] ) {
	    vDACs.push_back( (double)( thisDAC[i] ) ) ;
	    LArWave* wave =  const_cast<LArCaliWave*>(&vCaliWaves[i]);
	    vWaves.push_back(wave);
	  } else {
	      if ( (unsigned int)thisDAC[i] > 1000/gainFactor )   // record bad "non-zero" DACs
	      ( badDACs[(unsigned)gain_it][chID] ).push_back(thisDAC[i]) ;
	  }
      }
      unsigned nGoodDACs = vDACs.size() ;
      log << MSG::DEBUG << nGoodDACs << " DAC's selected for MW evaluation, channel 0x"  
	  << MSG::hex << chID.get_compact() << MSG::dec << endreq ;
      log << MSG::DEBUG << "    : " << vDACs << endreq ;

      std::vector<LArWave> fitWave ;
      if ( nGoodDACs >= 2 ) {
	  fitWave = m_waveHelper->linearMasterWave(vWaves,vDACs) ;
	  if ( fitWave.size()==0 ) {
	    log << MSG::ERROR << "Master waveform linear fit failed! Channel 0x" 
		<< MSG::hex << chID.get_compact() << MSG::dec << ", gain = " << gain_it << endreq ;
	    continue ;  // skip rest and go to next wave
	  }
      } else if ( m_buildMWoneDAC && nGoodDACs==1 && vDACs[0]>=DACMin && vDACs[0]<=DACMax ) {
	  log << MSG::WARNING << "Only one DAC available: " << vDACs[0] 
	      << " just divide caliwave by DAC! Channel 0x" 
	      << MSG::hex << chID.get_compact() << MSG::dec << ", gain = " << gain_it << endreq ;
	  fitWave.resize(2) ;
	  fitWave[1] = (*vWaves[0])*(1./vDACs[0]) ;
	  fitWave[1].setFlag(LArWave::mwf) ;
	  fitWave[0] = LArWave(vWaves[0]->getSize(),vWaves[0]->getDt(),LArWave::dac0) ;
      } else {
	  log << MSG::ERROR << "No DACs available, cannot build MW! Channel 0x" 
	      << MSG::hex << chID.get_compact() << MSG::dec << ", gain = " << gain_it << endreq ;
	  continue ;  // skip rest and go to next wave
      }

      log << MSG::DEBUG << "master waveform evaluation succeeded for channel 0x" 
          << MSG::hex  << chID.get_compact() << MSG::dec << ", gain = " << gain_it << endreq ;
      log << MSG::DEBUG << "   ... with " << nGoodDACs << " DAC values: " << vDACs << endreq ;

      if ( m_timeShift ) {
	  int tStart = m_waveHelper->getStart(fitWave[1]) ;
	  fitWave[1] = m_waveHelper->translate(fitWave[1],-tStart,0) ;
	  fitWave[0] = m_waveHelper->translate(fitWave[0],-tStart,0) ;
      }

      if ( m_normalize ) {
	  double peak = fitWave[1].getSample( m_waveHelper->getMax(fitWave[1]) ) ;
	  if ( peak != 0 ) {
	    fitWave[1] = fitWave[1] * (1/peak) ;
	  } else {
	    log << MSG::WARNING << "Wave peak = 0 , cannot normalize" << endreq ;
	  }
      }

      LArCaliWaveContainer::LArCaliWaves& dacWaves = mwContainer->get(chID, gain_it);
      LArCaliWave masterWave( fitWave[1].getWave(),
                              dt, -1, LArWave::mwf );
      LArCaliWave dac0Wave  ( fitWave[0].getWave(),
                              dt, -2, LArWave::dac0 );
      dacWaves.push_back( masterWave );
      dacWaves.push_back( dac0Wave );
      nMasterWaves ++ ;

      log << MSG::DEBUG << "MW and DAC0 evaluated and written in container, channel 0x" 
                        << MSG::hex << chID.get_compact() << MSG::dec << endreq ;
	
    }  // end loop over channels

    log << MSG::INFO << nMasterWaves << " master wfs generated for gain " << gain_it << endreq ;

  }  // end loop over gains

  // register MW/DAC0 container into detStore
  if ( StatusCode::SUCCESS != detStore->record(mwContainer,m_keyoutput) ) {
      log << MSG::ERROR << "Cannot record master waveform container to StoreGate, key = " << m_keyoutput << endreq ;
      return StatusCode::FAILURE ;
  }
  log << MSG::INFO << "Master waveforms' container recorded into StoreGate, key = " << m_keyoutput << endreq ;

  log << MSG::INFO << "List of DACs rejected" << endreq ; 
  
  for ( unsigned gain_it=0 ; gain_it<CaloGain::LARNGAIN ; gain_it++ ) {  
    
    std::map<HWIdentifier, std::vector<int> > & badDACsGain = badDACs[gain_it] ;
    std::map<HWIdentifier, std::vector<int> >::const_iterator ch_it = badDACsGain.begin() ;
    std::map<HWIdentifier, std::vector<int> >::const_iterator ch_it_e = badDACsGain.end() ;
    
    for ( ; ch_it != ch_it_e ; ch_it++ ) {

      const HWIdentifier chId = ch_it->first ;    
      const std::vector<int> & DACs = ch_it->second ;

      if ( m_listAllAnalysedChannels || DACs.size()>0 ) {
        try {
          Identifier id = larCablingSvc->cnvToIdentifier(chId);
          int region  = emId->region(id);
          int eta     = emId->eta(id); 
          int phi     = emId->phi(id);
          int layer   = emId->sampling(id);
          const std::vector<HWIdentifier>& calibLineV=larCablingSvc->calibSlotLine(chId);
          std::vector<HWIdentifier>::const_iterator calibLineIt=calibLineV.begin();
          int calibLine = 0;
          if ( calibLineV.size()>0 ) 
	      calibLine = onlineHelper->channel(*calibLineIt);
          int channel = onlineHelper->channel(chId) ;
          int slot    = onlineHelper->slot(chId) ;
          int FT      = onlineHelper->feedthrough(chId) ;
          static char gainName[3][3] = { "HG" , "MG" , "LG" } ;
          char formOut[200] ;
          int ich = chId.get_identifier32().get_compact() ;
          sprintf( formOut, "%2s 0x%8x [%1d;%1d;%3d;%2d] [%2d;%3d;%3d;%3d]",
        	 gainName[gain_it], ich, region, layer, eta, phi,
        	 FT, slot, channel, calibLine ) ;
        log << MSG::INFO << "*** " << std::string(formOut) << " : " << DACs << endreq ;
        } 
	catch( LArID_Exception & except ) {
          log << MSG::ERROR << "LArID_Exception: channel 0x" << MSG::hex << chId << MSG::dec << " not registered " << endreq;
        }  

      }
    }
  }
  
  return StatusCode::SUCCESS;
}

double LArMasterWaveBuilder::diffWave(const LArCaliWave& w1, const LArCaliWave& w2,bool normPeak=false) const {
  int tbin=0 ;
  return diffWave(w1,w2,tbin,normPeak) ;
}

double LArMasterWaveBuilder::diffWave(const LArCaliWave& w1, const LArCaliWave& w2,int& tbin,bool normPeak=false) const {
  double norm1, norm2 ;
  if ( normPeak ) {
    norm1 = w1.getSample( m_waveHelper->getMax(w1) ) ;
    norm2 = w2.getSample( m_waveHelper->getMax(w2) ) ;
  } else {
    norm1 = w1.getDAC() ;
    norm2 = w2.getDAC() ;
  }
  if ( norm1 <=0 || norm2 <= 0 )  return -1 ;
  double resid = 0 ;
  for ( unsigned u=0 ; u < w1.getSize() ; u++ ) {
    if ( u >= w2.getSize() ) break ;
    double diff1 = fabs( w1.getSample(u)/norm1 - w2.getSample(u)/norm2 ) ;
    if ( diff1 > resid ) {
      resid = diff1 ;
      tbin = u ;
    }
  }
  return resid ;
}
