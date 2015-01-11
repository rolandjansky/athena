/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArCaliWaveMerger.h"
#include "CaloIdentifier/CaloGain.h"

typedef LArCaliWaveContainer::ConstConditionsMapIterator   CaliCellIt;
typedef LArCaliWaveContainer::LArCaliWaves::const_iterator CaliWaveIt;

LArCaliWaveMerger::LArCaliWaveMerger (const std::string& name, ISvcLocator* pSvcLocator) : 
  AthAlgorithm(name, pSvcLocator),
  m_groupingType("ExtendedFeedThrough") // SubDetector, Single, FeedThrough, ExtendedFeedThrough
{
  declareProperty("KeyList",      m_keylist);
  declareProperty("KeyOutput",    m_keyout);
  declareProperty("GroupingType", m_groupingType); 
}

LArCaliWaveMerger::~LArCaliWaveMerger() 
{}

StatusCode LArCaliWaveMerger::stop() 
{
  ATH_MSG_INFO ( "LArCaliWaveMerger in stop()" );
  
  // create empty LArCaliWaveContainer to store all LArCaliWave
  LArCaliWaveContainer* mergedCaliWaveContainer = new LArCaliWaveContainer();
  mergedCaliWaveContainer->setGroupingType(m_groupingType,msg());
  mergedCaliWaveContainer->initialize();  

  std::vector<std::string>::const_iterator key_it   = m_keylist.begin();
  std::vector<std::string>::const_iterator key_it_e = m_keylist.end();

  for (;key_it!=key_it_e; ++key_it) { // Containers
  
    const LArCaliWaveContainer* larCaliWaveContainer = nullptr;
    ATH_CHECK( detStore()->retrieve(larCaliWaveContainer,*key_it) );
    ATH_MSG_INFO ( "Loaded LArCaliWaveContainer with key = " << *key_it );

    for ( unsigned gain_it = CaloGain::LARHIGHGAIN ; gain_it < CaloGain::LARNGAIN ; ++gain_it ) { // Gains
      
      CaliCellIt cell_it = larCaliWaveContainer->begin(gain_it) ;
      CaliCellIt cell_it_e = larCaliWaveContainer->end(gain_it) ;
      
      if ( cell_it == cell_it_e ) {
        ATH_MSG_DEBUG ( "LArCaliWaveContainer (key = " << *key_it << ") has no wave with gain = " << gain_it );
        continue;
      } else {
        ATH_MSG_DEBUG ( "Processing LArCaliWaveContainer (key = " << *key_it << ") in gain = " << gain_it );
      }
      
      unsigned nchannels = 0;
      unsigned nwaves    = 0;

      for ( ; cell_it != cell_it_e; ++cell_it) { // Channels
       
        CaliWaveIt wave_it = cell_it->begin();
	CaliWaveIt wave_it_e = cell_it->end();
        if ( wave_it == wave_it_e ) {
	  ATH_MSG_DEBUG ( "Empty channel found..." );
	  continue; // skip empty channels
        } 
	
	HWIdentifier chid = cell_it.channelId();

	ATH_MSG_DEBUG ( "Adding " << cell_it->size() << " waves for channel 0x" << MSG::hex << chid << MSG::dec );

	nchannels++;
	//nwaves += cell_it->size();

	LArCaliWaveContainer::LArCaliWaves& dacWaves = mergedCaliWaveContainer->get(chid,gain_it);
		
	for ( ; wave_it != wave_it_e; ++wave_it) { // DACs
	
	  dacWaves.push_back(*wave_it);
	  nwaves++;
	
	} // end of loop over DACs
	
	// intermediate map cleanup (save memory)
	const LArCaliWaveVec* cvec = &(*cell_it);
        LArCaliWaveVec* vec = const_cast<LArCaliWaveVec*>(cvec);
        vec->clear();
		
      } // end of loop over Channels
      
      ATH_MSG_INFO ( "Added " 
                     << nwaves    << " waves for " 
                     << nchannels << " channels from LArCaliWaveContainer (key = " 
                     << *key_it << ") in gain = " << gain_it );
      
    } // end of loop over Gains

  } // end of loop over Containers
    
  // Record in detector store with key (m_keyoutput)
  ATH_CHECK( detStore()->record(mergedCaliWaveContainer,m_keyout) );
  return StatusCode::SUCCESS ;
}

