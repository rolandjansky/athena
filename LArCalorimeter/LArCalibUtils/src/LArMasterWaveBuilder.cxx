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
  AthAlgorithm(name, pSvcLocator),
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
  // Check DACMin jobOption consistency, in case setup default values
  if ( m_dacMinLayer0.size()<3 ) {
     ATH_MSG_WARNING ( "DACMinPS     jobOption has wrong size. Will use default." );
     m_dacMinLayer0.resize(3);
     m_dacMinLayer0[0] = 50; 
     m_dacMinLayer0[1] = 50 ;
     m_dacMinLayer0[2] = 50 ;
  }
  if ( m_dacMinLayer1.size()<3 ) {
     ATH_MSG_WARNING ( "DACMinStrips jobOption has wrong size. Will use default." );
     m_dacMinLayer1.resize(3);
     m_dacMinLayer1[0] = 50 ; 
     m_dacMinLayer1[1] = 50 ;
     m_dacMinLayer1[2] = 50 ;
  }
  if ( m_dacMinLayer2.size()<3 ) {
     ATH_MSG_WARNING ( "DACMinMiddle jobOption has wrong size. Will use default." );
     m_dacMinLayer2.resize(3);
     m_dacMinLayer2[0] = 50 ; 
     m_dacMinLayer2[1] = 50 ;
     m_dacMinLayer2[2] = 50 ;
  }
  if ( m_dacMinLayer3.size()<3 ) {
     ATH_MSG_WARNING ( "DACMinBack   jobOption has wrong size. Will use default." );
     m_dacMinLayer3.resize(3);
     m_dacMinLayer3[0] = 50 ; 
     m_dacMinLayer3[1] = 50 ;
     m_dacMinLayer3[2] = 50 ;
  }

  // Check DACMax jobOption consistency, in case setup default values
  if ( m_dacMaxLayer0.size()<3 ) {
     ATH_MSG_WARNING ( "DACMaxPS     jobOption has wrong size. Will use default." );
     m_dacMaxLayer0.resize(3);
     m_dacMaxLayer0[0] = 15000; 
     m_dacMaxLayer0[1] = 50000 ;
     m_dacMaxLayer0[2] = 65000 ;
  }
  if ( m_dacMaxLayer1.size()<3 ) {
     ATH_MSG_WARNING ( "DACMaxStrips jobOption has wrong size. Will use default." );
     m_dacMaxLayer1.resize(3);
     m_dacMaxLayer1[0] = 800 ; 
     m_dacMaxLayer1[1] = 8000 ;
     m_dacMaxLayer1[2] = 65000 ;
  }
  if ( m_dacMaxLayer2.size()<3 ) {
     ATH_MSG_WARNING ( "DACMaxMiddle jobOption has wrong size. Will use default." );
     m_dacMaxLayer2.resize(3);
     m_dacMaxLayer2[0] = 1000 ; 
     m_dacMaxLayer2[1] = 10000 ;
     m_dacMaxLayer2[2] = 65000 ;
  }
  if ( m_dacMaxLayer3.size()<3 ) {
     ATH_MSG_WARNING ( "DACMaxBack   jobOption has wrong size. Will use default." );
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
  ATH_MSG_INFO ( "LArMasterWaveBuilder in stop..." );

  const LArOnlineID*   onlineHelper;
  const LArEM_ID*      emId;
  
  // Retrieve LArCablingService
  ToolHandle<LArCablingService> larCablingSvc("LArCablingService");
  ATH_CHECK( larCablingSvc.retrieve() );
  ATH_CHECK(  detStore()->retrieve(onlineHelper, "LArOnlineID") );
  
  const CaloIdManager* caloIdMgr=CaloIdManager::instance();
  emId=caloIdMgr->getEM_ID();

  msg() << MSG::INFO << "Check on ADC saturation: " ;
  if ( m_ADCsatur > 0 ) {
    msg() << "absolute maximum less than " << m_ADCsatur << endreq ;
  } else if ( m_ADCsatur < 0 ) {
    msg() << "maximum relative to sample 0 less than " << -m_ADCsatur << endreq ;
  } else {
    msg() << "none!" << endreq ;
  }
  msg() << MSG::INFO << "Check on mimimum DAC value: " ;
  if ( m_DACmin > 0 ) {
    msg() << m_DACmin << endreq ;
  } else {
    msg() << "none!" << endreq ;
  }
  if ( m_normalize ) {
    ATH_MSG_INFO ( "Master Waveforms will be normalized" );
  } else {
    ATH_MSG_INFO ( "Master Waveforms will NOT be normalized" );
  }
  if ( m_timeShift ) {
    ATH_MSG_INFO ( "Master Waveforms will be shifted to the origin" );
  } else {
    ATH_MSG_INFO ( "Master Waveforms will NOT be shifted to the origin" );
  }
  if ( m_buildMWoneDAC ) {
    ATH_MSG_INFO ( "Build MWs even with only one usable DAC" );
  } else {
    ATH_MSG_INFO ( "Don't build MWs if only one usable DAC" );
  }
  if ( m_diffWaveNormPeak ) {
    ATH_MSG_INFO ( "For linearity check, normalize to peak " );
  } else {
    ATH_MSG_INFO ( "For linearity check, normalize to DAC " );
  }
  ATH_MSG_INFO ( "Linearity tolerance = " << m_linearityTolerance );

  // Retrieve container
  const LArCaliWaveContainer* caliWaveContainer = nullptr;
  ATH_CHECK( detStore()->retrieve(caliWaveContainer,m_keyinput) );
  
  if ( caliWaveContainer == NULL ) {
    ATH_MSG_DEBUG ( "LArCaliWaveContainer (" << m_keyinput << ") is empty" );
    return StatusCode::FAILURE;
  }
  
  ATH_MSG_INFO ( "Processing LArCaliWaveContainer from StoreGate, key = " << m_keyinput );

  // create new LArCaliWaveContainer for master waveforms and dac0 waveforms
  LArCaliWaveContainer* mwContainer = new LArCaliWaveContainer();

  ATH_CHECK( mwContainer->setGroupingType(m_groupingType,msg()) );
  ATH_CHECK( mwContainer->initialize() );
  
  // prepare a map for bad channels/DACs (i.e.not linear, not pulsed...). Access is badDACs[gain][channel][k-DAC]
  std::vector< std::map<HWIdentifier, std::vector<int> > > badDACs ;
  badDACs.resize((int)CaloGain::LARNGAIN) ;

  unsigned nMasterWaves = 0 ;

  for ( unsigned gain_it = CaloGain::LARHIGHGAIN; gain_it!=CaloGain::LARNGAIN; ++gain_it ) { // Loop over Gains

    ATH_MSG_DEBUG ( "Evaluating master wavefroms for gain " << gain_it );

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
    
    ATH_MSG_INFO ( "Minimum DAC allowed for layers 0,1,2,3 and gain " << gain_it );
    ATH_MSG_INFO ( "       " << minDAC );
    ATH_MSG_INFO ( "Maximum DAC allowed for layers 0,1,2,3 and gain " << gain_it );
    ATH_MSG_INFO ( "       " << maxDAC );

    int DACMin = 0 ;
    int DACMax = 65000 ;

    CaliCellIt cell_it   = caliWaveContainer->begin(gain_it);
    CaliCellIt cell_it_e = caliWaveContainer->end(gain_it);

    if ( cell_it == cell_it_e ) {
      ATH_MSG_DEBUG ( "LArCaliWaveContainer (key = " << m_keyinput << ") has no wave with gain = " << gain_it );
      continue;
    } else {
      ATH_MSG_DEBUG ( "Processing LArCaliWaveContainer (key = " << m_keyinput << ") in gain = " << gain_it );
    }

    for ( ; cell_it!=cell_it_e ; ++cell_it ) { // loop through channels
      
      CaliWaveIt wave_it = cell_it->begin();
      CaliWaveIt wave_it_e = cell_it->end();
      if ( wave_it == wave_it_e ) {
        ATH_MSG_DEBUG ( "Empty channel found..." );
        continue; // skip empty channels
      } 
      
      const LArCaliWaveContainer::LArCaliWaves& vCaliWaves = *cell_it;       
      
      HWIdentifier chID = cell_it.channelId() ;
      double dt         = vCaliWaves[0].getDt() ;
      unsigned nDACs    = vCaliWaves.size() ;

      ATH_MSG_DEBUG ( nDACs << " DAC's found for channel 0x" << MSG::hex << chID.get_compact() << MSG::dec 
                      << " in gain " << gain_it );
      msg() << MSG::DEBUG << "DAC list: " ;
      for ( unsigned i=0 ; i<nDACs ; i++ ) 
        msg() << vCaliWaves[i].getDAC() << " " ;
      msg() << endreq ;
      
      long layer  = -1; 
      long region = -1;

      try {
        Identifier id = larCablingSvc->cnvToIdentifier(chID);
        layer  = emId->sampling(id) ;
        region = emId->region(id) ;
      } catch ( LArID_Exception & except ) { 
     	ATH_MSG_WARNING ( "A larCablingSvc exception was caught for channel 0x" 
                          << MSG::hex << chID.get_compact() << MSG::dec 
                          << ". Skipping." );
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
            ATH_MSG_DEBUG ( ChanDacGain(chID,thisDAC[i],gain_it) 
                            << "DAC below threshold... skip!" );
	    usable[i] = false ;
	    continue ;
	}
	// check overall ADC saturation
	if ( m_ADCsatur != 0 ) {  
	    if ( m_ADCsatur>0 && thisPeak[i]>m_ADCsatur ) { // suitable if no pedestal subtraction
	      ATH_MSG_DEBUG ( ChanDacGain(chID,thisDAC[i],gain_it) 
                              << " absolute ADC saturation... skip!" );
	      usable[i] = false ;
	      continue ;
	    } else if ( m_ADCsatur<0 && (thisPeak[i]-vCaliWaves[i].getSample(0))>(-m_ADCsatur) ) { 
	      // for pedestal subtracted
	      ATH_MSG_DEBUG ( ChanDacGain(chID,thisDAC[i],gain_it) 
                              << " relative ADC saturation... skip!" );
	      usable[i] = false ;
	      continue ;
	    }
	}
	// check if clearly no signal
	if ( fabs( thisPeak[i]/thisDAC[i] ) < gainFactor*0.0001 ) {
          ATH_MSG_DEBUG ( ChanDacGain(chID,thisDAC[i],gain_it) << " low signal... skip!" );
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
      // reject strongly non-linear waves (if reference wave was found)
      if ( refIndex >=0 ) {
          int refDAC = thisDAC[refIndex] ;
          ATH_MSG_DEBUG ( "Check non-linearity against DAC " << refDAC );
	  for ( unsigned i=0 ; i<nDACs ; i++ ) {
	    if ( ! usable[i] ) continue ;
	    int tbin ;
	    double relDiff = diffWave(vCaliWaves[i],vCaliWaves[refIndex],tbin,m_diffWaveNormPeak) ;
	    if ( ! m_diffWaveNormPeak )  relDiff *= (refDAC/thisPeak[refIndex]) ;
	    if ( relDiff > m_linearityTolerance ) {
	      ATH_MSG_DEBUG ( ChanDacGain(chID,thisDAC[i],gain_it) 
                              << " non-linearity detected, rel.resid= " << relDiff 
                              << " at t= " << tbin*vCaliWaves[i].getDt() << " tbin= " << tbin
                              << " ... skip!" );
	      usable[i] = false ;
	    } else if ( relDiff < 0 ) {
	      ATH_MSG_DEBUG ( ChanDacGain(chID,thisDAC[i],gain_it) 
                              << " could not check linearity... skip!" );
	      usable[i] = false ;
	    } else {
	      ATH_MSG_DEBUG ( ChanDacGain(chID,thisDAC[i],gain_it) 
                              << " linear within " << relDiff );
	    }
	  } // end for i
      } else {
        ATH_MSG_DEBUG ( "Could not find reference DAC, just reject all waves not in DAC ["
                        << DACMin << "," << DACMax << "]" );
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
      ATH_MSG_DEBUG ( nGoodDACs << " DAC's selected for MW evaluation, channel 0x"  
                      << MSG::hex << chID.get_compact() << MSG::dec );
      ATH_MSG_DEBUG ( "    : " << vDACs );

      std::vector<LArWave> fitWave ;
      if ( nGoodDACs >= 2 ) {
	  fitWave = m_waveHelper->linearMasterWave(vWaves,vDACs) ;
	  if ( fitWave.size()==0 ) {
	    ATH_MSG_ERROR ( "Master waveform linear fit failed! Channel 0x" 
                            << MSG::hex << chID.get_compact() << MSG::dec << ", gain = " << gain_it );
	    continue ;  // skip rest and go to next wave
	  }
      } else if ( m_buildMWoneDAC && nGoodDACs==1 && vDACs[0]>=DACMin && vDACs[0]<=DACMax ) {
        ATH_MSG_WARNING ( "Only one DAC available: " << vDACs[0] 
                          << " just divide caliwave by DAC! Channel 0x" 
                          << MSG::hex << chID.get_compact() << MSG::dec << ", gain = " << gain_it );
	  fitWave.resize(2) ;
	  fitWave[1] = (*vWaves[0])*(1./vDACs[0]) ;
	  fitWave[1].setFlag(LArWave::mwf) ;
	  fitWave[0] = LArWave(vWaves[0]->getSize(),vWaves[0]->getDt(),LArWave::dac0) ;
      } else {
        ATH_MSG_ERROR ( "No DACs available, cannot build MW! Channel 0x" 
                        << MSG::hex << chID.get_compact() << MSG::dec << ", gain = " << gain_it );
        continue ;  // skip rest and go to next wave
      }

      ATH_MSG_DEBUG ( "master waveform evaluation succeeded for channel 0x" 
                      << MSG::hex  << chID.get_compact() << MSG::dec << ", gain = " << gain_it );
      ATH_MSG_DEBUG ( "   ... with " << nGoodDACs << " DAC values: " << vDACs );

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
	    ATH_MSG_WARNING ( "Wave peak = 0 , cannot normalize" );
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

      ATH_MSG_DEBUG ( "MW and DAC0 evaluated and written in container, channel 0x" 
                      << MSG::hex << chID.get_compact() << MSG::dec );
	
    }  // end loop over channels

    ATH_MSG_INFO ( nMasterWaves << " master wfs generated for gain " << gain_it );

  }  // end loop over gains

  // register MW/DAC0 container into detStore
  ATH_CHECK( detStore()->record(mwContainer,m_keyoutput) );
  ATH_MSG_INFO ( "Master waveforms' container recorded into StoreGate, key = " << m_keyoutput );

  ATH_MSG_INFO ( "List of DACs rejected" );
  
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
          ATH_MSG_INFO ( "*** " << std::string(formOut) << " : " << DACs );
        } 
	catch( LArID_Exception & except ) {
          ATH_MSG_ERROR ( "LArID_Exception: channel 0x" << MSG::hex << chId << MSG::dec << " not registered " );
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
