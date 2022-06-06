/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#include "LArCalibUtils/LArCaliWaveBuilder.h"

#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"

#include "LArRawEvent/LArAccumulatedCalibDigitContainer.h"
#include "LArRawEvent/LArCalibDigitContainer.h"

#include "LArRawConditions/LArWaveHelper.h"

#include "LArRawEvent/LArFebErrorSummary.h"
#include "LArElecCalib/ILArPedestal.h"

#include <fstream>


using CLHEP::megahertz;
using CLHEP::ns;


LArCaliWaveBuilder::LArCaliWaveBuilder(const std::string& name, ISvcLocator* pSvcLocator) 
 : AthAlgorithm(name, pSvcLocator),
   m_groupingType("ExtendedFeedThrough"), // SubDetector, Single, FeedThrough
   m_onlineID(0),
   m_event_counter(0)
{
 declareProperty("UseAccumulatedDigits",   m_useAccumulatedDigits=true);
 declareProperty("KeyList",                m_keylistproperty);
 declareProperty("KeyOutput",              m_keyoutput="LArCaliWave");
 declareProperty("SubtractPed",            m_pedSub=true);
 declareProperty("NBaseline",              m_baseline=1);
 declareProperty("SamplingPeriod",         m_SamplingPeriod=1./(40.08*megahertz));
 declareProperty("NSteps",                 m_NStep=24); // Number of Delay Steps. FIXME: Try to get this information for somewhere else  
 declareProperty("ADCsaturation",          m_ADCsatur=0) ; 
 declareProperty("GroupingType",           m_groupingType); 
 declareProperty("UseDacAndIsPulsedIndex", m_useDacAndIsPulsedIndex=false);
 declareProperty("CheckEmptyPhases",       m_checkEmptyPhases=false);
 declareProperty("RecAllCells",            m_recAll=false);
 declareProperty("UsePattern",             m_usePatt=-1);
 declareProperty("UseParams",              m_useParams=false); // Read LArCalibParams from DetStore ?
 declareProperty("isSC",                   m_isSC=false);


 m_dt = m_SamplingPeriod/m_NStep;

 m_fatalFebErrorPattern=0xffff;

}

LArCaliWaveBuilder::~LArCaliWaveBuilder() 
{}

StatusCode LArCaliWaveBuilder::initialize()
{ 
  if (!m_keylistproperty.size()) {
     m_keylistproperty.push_back("HIGH");
     m_keylistproperty.push_back("MEDIUM");
     m_keylistproperty.push_back("LOW");
  }
  m_keylist = m_keylistproperty;

  //FIXME probably useless because m_wave isn't written anywhere
  StatusCode sc=m_waves.setGroupingType(m_groupingType,msg());
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Failed to set groupingType for LArCaliWave intermediate object" );
    return sc;
  }

  ATH_MSG_INFO( "Initialize intermediate Wave object" );
  sc=m_waves.initialize(); 
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Failed initialize LArCaliWave intermediate object" );
    return sc;
  }

  m_event_counter=0; 
  m_dt = m_SamplingPeriod/m_NStep;


  if (m_checkEmptyPhases)
    ATH_MSG_INFO( "Empty phases check selected." );  

  //Get pedestal from CondStore
  ATH_CHECK( m_pedKey.initialize(m_pedSub) );
  
  //Get Online helper from DetStore
  if ( m_isSC ) {
    const LArOnline_SuperCellID* ll;
    ATH_CHECK( detStore()->retrieve(ll, "LArOnline_SuperCellID") );
    m_onlineID = (const LArOnlineID_Base*)ll;
    ATH_MSG_DEBUG(" Found the LArOnline_SuperCellID helper. ");
  } else { // m_isSC
    const LArOnlineID* ll;
    ATH_CHECK( detStore()->retrieve(ll, "LArOnlineID") );
    m_onlineID = (const LArOnlineID_Base*)ll;
    ATH_MSG_DEBUG(" Found the LArOnlineID helper. ");
  }

  ATH_CHECK( m_cablingKey.initialize() );
  ATH_CHECK( m_cablingKeySC.initialize(m_isSC) );

  if(m_usePatt >= 0 && !m_useParams) {
     ATH_MSG_ERROR("Inconsistent configuration, for UsePattern > 0 the  UseParams must be true");
     return StatusCode::FAILURE;
  }
  
  ATH_CHECK( m_calibMapKey.initialize(m_useParams) );

  return StatusCode::SUCCESS;
}


StatusCode LArCaliWaveBuilder::execute() 
{
 // using EvtId
 const EventContext& ctx = getContext();
 m_event_counter=ctx.eventID().event_number();

 const LArCalibParams* calibParams = nullptr;
 const LArCalibLineMapping *clcabling=nullptr;
 if(m_useParams) { // we have to check in which event we are, reading only ones corresponding
                      // to our pattern
    ATH_CHECK(detStore()->retrieve(calibParams,"LArCalibParams"));
    unsigned numPatt=calibParams->getNumberPatterns(HWIdentifier(0));

    int counter=m_event_counter;
    if(m_useAccumulatedDigits) counter /= calibParams->NTrigger(HWIdentifier(1007091712));
    if(m_usePatt >= 0 && static_cast<int>(counter % numPatt) != m_usePatt) {
       return StatusCode::SUCCESS;
    }
    ATH_MSG_DEBUG("Good event "<<m_event_counter<<" for pattern " << m_usePatt << " out of " << numPatt << " patterns " << calibParams->NTrigger(HWIdentifier(1007091712)) <<" triggers ");

    const EventContext& ctx = Gaudi::Hive::currentContext();
    SG::ReadCondHandle<LArCalibLineMapping> clHdl{m_calibMapKey, ctx};
    clcabling =*clHdl;
    if(!clcabling) {
       ATH_MSG_WARNING( "Do not have calib line mapping from key " << m_calibMapKey.key() );
       return StatusCode::FAILURE;
    }

 }

 if ( m_event_counter < 1000 || m_event_counter%100==0 ) 
    ATH_MSG_INFO( "Processing event " << m_event_counter );
 
 if (m_keylist.size()==0) {
   ATH_MSG_ERROR( "Key list is empty! No containers to process!" );
   return StatusCode::FAILURE;
 } 


 // execute() method...
 if (m_useAccumulatedDigits)
   return executeWithAccumulatedDigits(calibParams, clcabling);
 else
   return executeWithStandardDigits(calibParams, clcabling);
}

StatusCode LArCaliWaveBuilder::executeWithAccumulatedDigits(const LArCalibParams* calibParams, const LArCalibLineMapping* clcabling)
{
 StatusCode sc;

 const LArAccumulatedCalibDigitContainer* larAccumulatedCalibDigitContainer = nullptr;
 
 std::vector<std::string>::const_iterator key_it=m_keylist.begin();
 std::vector<std::string>::const_iterator key_it_e=m_keylist.end();
 int foundkey = 0;
 for (;key_it!=key_it_e; ++key_it) { //Loop over all containers that are to be processed (e.g. different gains)
 
   sc = evtStore()->retrieve(larAccumulatedCalibDigitContainer,*key_it);
   if (sc.isFailure()) {
     ATH_MSG_WARNING( "Cannot read LArAccumulatedCalibDigitContainer from StoreGate! key=" << *key_it );
     if ( (std::next(key_it) == key_it_e) && foundkey==0 ){
       ATH_MSG_ERROR("None of the provided LArAccumulatedDigitContainer keys could be read");
       return StatusCode::FAILURE;
     }else{
       continue;
     }
   }
   foundkey+=1;
   
   const LArFebErrorSummary* febErrSum=NULL;
   if (evtStore()->contains<LArFebErrorSummary>("LArFebErrorSummary")) {
     sc=evtStore()->retrieve(febErrSum);
     if (sc.isFailure()) {
       ATH_MSG_ERROR( "Failed to retrieve FebErrorSummary object!" );
       return sc;
     }
   }
   else
     if (m_event_counter==0)
       ATH_MSG_WARNING( "No FebErrorSummaryObject found! Feb errors not checked!" );
 
   HWIdentifier  lastFailedFEB(0);
   LArAccumulatedCalibDigitContainer::const_iterator it=larAccumulatedCalibDigitContainer->begin();
   LArAccumulatedCalibDigitContainer::const_iterator it_end=larAccumulatedCalibDigitContainer->end();

   if (it == it_end) {
      ATH_MSG_DEBUG("LArAccumulatedCalibDigitContainer with key=" << *key_it << " is empty ");
      continue; // at this event LArAccumulatedCalibDigitContainer is empty, do not even try to loop on it...
   }
   
   const float delayScale = larAccumulatedCalibDigitContainer->getDelayScale();
   const float deltaDelay = 25*ns/(delayScale*m_NStep);

   for (;it!=it_end; ++it) { // Loop over all cells

     bool ispulsed=false;
     if(m_useParams && calibParams) { // got LArCalibParams from DetStore
        const std::vector<HWIdentifier>& calibLineLeg = clcabling->calibSlotLine((*it)->hardwareID());
        for (const HWIdentifier &calibLineHWID : calibLineLeg) {// loop calib lines
            ispulsed |= calibParams->isPulsed(m_event_counter,calibLineHWID);
        }
     } else {
        ispulsed=(*it)->isPulsed();
     }
     if ( (!m_recAll) && !ispulsed ) {
        ATH_MSG_DEBUG( "Non pulsed cell " << m_onlineID->channel_name((*it)->hardwareID())<<" in evt. "<<m_event_counter ); 
        continue; // Check if cell is pulsed
     }
     HWIdentifier chid=(*it)->hardwareID();
     HWIdentifier febid=m_onlineID->feb_Id(chid);
     if (febErrSum) {
       const uint16_t febErrs=febErrSum->feb_error(febid);
       if (febErrs & m_fatalFebErrorPattern) {
	 if (febid!=lastFailedFEB) {
	   lastFailedFEB=febid;
	   ATH_MSG_ERROR( "Event " << m_event_counter << " Feb " <<  m_onlineID->channel_name(febid) 
	       << " reports error(s):" << febErrSum->error_to_string(febErrs) << ". Data ignored." );
	 }
	 continue;
       }
     }
     CaloGain::CaloGain gain=(*it)->gain();

     if (gain<0 || gain>CaloGain::LARNGAIN) {
       ATH_MSG_ERROR( "Found not-matching gain number ("<< (int)gain <<")" );
       return StatusCode::FAILURE;
     }
     
     // transform sampleSum vector from uint32_t to double
     std::vector<double> samplesum;     
     std::vector < uint64_t >::const_iterator samplesum_it=(*it)->sampleSum().begin();
     std::vector < uint64_t >::const_iterator samplesum_it_e=(*it)->sampleSum().end();
     for (;samplesum_it!=samplesum_it_e; ++samplesum_it) 
       samplesum.push_back((double)(*samplesum_it));     
     
     // transform sample2Sum vector from uint32_t to double
     std::vector<double> sample2sum;     
     std::vector < uint64_t >::const_iterator sample2sum_it=(*it)->sample2Sum().begin();
     std::vector < uint64_t >::const_iterator sample2sum_it_e=(*it)->sample2Sum().end();
     for (;sample2sum_it!=sample2sum_it_e; ++sample2sum_it) 
       sample2sum.push_back((double)(*sample2sum_it));     

     WaveMap& waveMap = m_waves.get(chid,gain);

     //make dacPulsed which has dac and four bits of is pulsed info
     int dacPulsed;
     float delay = (*it)->delay();
     int dac=(*it)->DAC();
     int index;
     int pulsed=0;
     if(m_useParams && calibParams && clcabling) { // get LArCalibParams from DetStore
        const std::vector<HWIdentifier>& calibLineLeg = clcabling->calibSlotLine((*it)->hardwareID());
        if(calibLineLeg.size()==0) {
           ATH_MSG_WARNING("Why do not have calib lines for "<<(*it)->hardwareID()<<" ?");
           continue;
        }      
        dac=calibParams->DAC(m_event_counter,calibLineLeg[0]);
        dacPulsed=dac;
        delay = calibParams->Delay(m_event_counter,calibLineLeg[0]);
        for (unsigned i=0; i<calibLineLeg.size(); ++i) {// loop calib lines
           if(calibParams->isPulsed(m_event_counter,calibLineLeg[i])){
	      ATH_MSG_DEBUG("GR: line pulsed true, line="<<i+1);
	      dacPulsed=(dacPulsed | (0x1 << (15+i+1)));
              pulsed=(pulsed | (0x1 << (15+i+1)));
           }

        }
     } else {
        dacPulsed=(*it)->DAC();
        for(int iLine=1;iLine<5;iLine++){
           if((*it)->isPulsed(iLine)){
	      ATH_MSG_DEBUG("GR: line pulsed true, line="<<iLine);
	      dacPulsed=(dacPulsed | (0x1 << (15+iLine)));
              pulsed=(pulsed | (0x1 << (15+iLine)));
           }
        }
     }

     if(m_useDacAndIsPulsedIndex){//switch used to turn on the option to have indexs that are DAC and isPulsed info
       index = dacPulsed;
     } else {
       index = dac;
     } 

     ATH_MSG_DEBUG( "Pulsed cell " << m_onlineID->channel_name(chid) << " gain: "<<gain ); 
     ATH_MSG_DEBUG( "with " << (*it)->sampleSum().size() << " samples " << index << " DAC " << delay << " delay " << dacPulsed << " dacPulsed " ); 

     WaveMap::iterator itm = waveMap.find(index);
     
     if ( itm == waveMap.end() ) { // A new LArCaliWave is booked
       LArCaliWave wave(samplesum.size()*m_NStep, m_dt, dac, pulsed);
       wave.setFlag( LArWave::meas );
       itm = (waveMap.insert(WaveMap::value_type(index,wave))).first;
       ATH_MSG_DEBUG("index: "<<index<<" new wave inserted");
     }
     (*itm).second.addAccumulatedEvent(int(std::roundf(delay/deltaDelay)), m_NStep, 
	                                samplesum, sample2sum, (*it)->nTriggers());
     
   } //End loop over all cells
 } //End loop over all containers

 return StatusCode::SUCCESS;
}


StatusCode LArCaliWaveBuilder::executeWithStandardDigits(const LArCalibParams* calibParams, const LArCalibLineMapping* clcabling)
{
 StatusCode sc;

 const LArCalibDigitContainer* larCalibDigitContainer = nullptr;
 
 std::vector<std::string>::const_iterator key_it=m_keylist.begin();
 std::vector<std::string>::const_iterator key_it_e=m_keylist.end();

 for (;key_it!=key_it_e; ++key_it) { //Loop over all containers that are to be processed (e.g. different gains)
 
   sc = evtStore()->retrieve(larCalibDigitContainer,*key_it);
   if (sc.isFailure()) {
     ATH_MSG_WARNING( "Cannot read LArCalibDigitContainer from StoreGate! key=" << *key_it );
     continue; // Try next container
   }

   LArCalibDigitContainer::const_iterator it=larCalibDigitContainer->begin();
   LArCalibDigitContainer::const_iterator it_end=larCalibDigitContainer->end();

   if (it == it_end) {
      ATH_MSG_INFO( "LArCalibDigitContainer with key=" << *key_it << " is empty " );
      continue; // at this event LArCalibDigitContainer is empty, do not even try to loop on it...
   }
   
   const float delayScale = larCalibDigitContainer->getDelayScale();
   const float deltaDelay = 25*ns/(delayScale*m_NStep);
   
   for (;it!=it_end; ++it) { // Loop over all cells

     bool pulsed=false;
     int dac = (*it)->DAC();
     float delay= (*it)->delay();
     if(m_useParams && calibParams && clcabling) { // get LArCalibParams from DetStore
        const std::vector<HWIdentifier>& calibLineLeg = clcabling->calibSlotLine((*it)->hardwareID());
        dac=calibParams->DAC(m_event_counter,calibLineLeg[0]); // assuming all calib. boards configured equally
        delay = calibParams->Delay(m_event_counter,calibLineLeg[0]);
        for (const HWIdentifier &calibLineHWID : calibLineLeg) {// loop calib lines
            pulsed |= calibParams->isPulsed(m_event_counter,calibLineHWID);
        }
     } else {
        pulsed=(*it)->isPulsed();
     }
     if ((!m_recAll) &&  !pulsed ) continue ; // Check if cell is pulsed
        
     HWIdentifier chid=(*it)->hardwareID(); 
     CaloGain::CaloGain gain=(*it)->gain();

     if (gain<0 || gain>CaloGain::LARNGAIN) {
       ATH_MSG_ERROR( "Found not-matching gain number ("<< (int)gain <<")" );
       return StatusCode::FAILURE;
     }

     // transform samples vector from uint32_t to double
     std::vector<double> samples;
     for (short sample : (*it)->samples())
       samples.push_back((double)(sample));

     WaveMap& waveMap = m_waves.get(chid,gain);
     WaveMap::iterator itm = waveMap.find(dac);
     
     if ( itm == waveMap.end() ) { // A new LArCaliWave is booked     
       LArCaliWave wave(samples.size()*m_NStep, m_dt, dac,0x1);
       wave.setFlag( LArWave::meas );
       itm = (waveMap.insert(WaveMap::value_type(dac,wave))).first;
     }
     
     (*itm).second.addEvent((int)roundf(delay/deltaDelay), m_NStep, samples);
     
   } //End loop over all cells
 } //End loop over all containers

 return StatusCode::SUCCESS;
}



StatusCode LArCaliWaveBuilder::stop()
{
    // Create wave container using feedthru grouping and initialize
    auto caliWaveContainer = std::make_unique<LArCaliWaveContainer>();
  
    StatusCode sc=caliWaveContainer->setGroupingType(m_groupingType,msg());
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Failed to set groupingType for LArCaliWaveContainer object" );
      return sc;
    }

    ATH_MSG_INFO( "Initialize final Wave object" );
    sc=caliWaveContainer->initialize(); 
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Failed initialize LArCaliWaveContainer object" );
      return sc;
    }
    
    const EventContext& ctx = Gaudi::Hive::currentContext();
    const LArOnOffIdMapping* cabling(0);
    if( m_isSC ){
      SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKeySC, ctx};
      cabling = {*cablingHdl};
      if(!cabling) {
	ATH_MSG_ERROR("Do not have mapping object " << m_cablingKeySC.key());
	return StatusCode::FAILURE;
      }
      
    }else{
      SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey, ctx};
      cabling = {*cablingHdl};
      if(!cabling) {
	ATH_MSG_ERROR("Do not have mapping object " << m_cablingKey.key());
	return StatusCode::FAILURE;
      }
    }
    SG::ReadCondHandle<ILArPedestal> pedHdl{m_pedKey, ctx};
    const ILArPedestal* larPedestal{*pedHdl};
    if(m_pedSub && !larPedestal) {
        ATH_MSG_DEBUG("No pedestal(s) found ");
        ATH_MSG_INFO( "Using fake (baseline) pedestal subtraction..." );  
    }

    LArWaveHelper wHelper;
    int NCaliWave=0;

    // Loop over all gains
    for (unsigned k=0;k<(int)CaloGain::LARNGAIN;k++) {
   
	CaloGain::CaloGain gain=(CaloGain::CaloGain)k;

	/// get iterator for all channels for a gain
	WaveContainer::ConditionsMapIterator cell_it   = m_waves.begin(gain) ; 
	WaveContainer::ConditionsMapIterator cell_it_e = m_waves.end(gain);

 	HWIdentifier lastId;
	
	for (; cell_it!=cell_it_e; ++cell_it) {

   
	    // Get id of this cell - use id from first cell in map
	    //
	    //   Some accumulations may be empty and we must skip this
	    //   cell. WaveContainer has all 128 channels for each FEB
	    //   with at least ONE being read out.
	    //

	    const HWIdentifier hwId = cell_it.channelId();
	    if ((!m_recAll) && (!cabling->isOnlineConnected(hwId))) {
               
                //ATH_MSG_INFO( "Skipping disconnected channel: "<<MSG::hex<<hwId<<MSG::dec );
               continue; //Ignore disconnected channels
            }
	      
	    const WaveMap& waveMap = (*cell_it);
	    if (waveMap.size()==0) {
	      ATH_MSG_INFO( "Empty accumulated wave. Last id: " << MSG::hex 
		//<< lastId << " " << emId->show_to_string(lastId) );
		  << lastId << " this id: "<<hwId<<MSG::dec );
	      continue;
	    }

	    lastId = hwId; // save previous id for debug printout
		
	    // Get the vector of waves for this chid,gain
	    LArCaliWaveContainer::LArCaliWaves& dacWaves = caliWaveContainer->get(hwId, gain);

	    std::map<int, LArCaliWave>::const_iterator dac_it   = cell_it->begin();
	    std::map<int, LArCaliWave>::const_iterator dac_it_e = cell_it->end();
	    
	    HWIdentifier chid = cell_it.channelId();
	    
	    for (; dac_it != dac_it_e; ++dac_it) {
       
		const LArCaliWave& thisWave = dac_it->second ;

		if (m_checkEmptyPhases) {
		  const std::vector<int>& thisTriggers = thisWave.getTriggers();
		  for (unsigned i=0;i<thisTriggers.size();++i) {
		    if (thisTriggers[i]==0) {
		      ATH_MSG_FATAL( "Empty phase found in channel 0x" << MSG::hex << chid << MSG::dec 
				      << "., aborting reconstruction. Sorry." );
		      return StatusCode::FAILURE;
		    }
		  }
		}

		float pedAve = 0.;
       
		if ( m_pedSub ) {
		    if ( larPedestal ) {	      
			float pedestal = larPedestal->pedestal(chid,gain);
			if (pedestal <= (1.0+LArElecCalib::ERRORCODE)) {
			  ATH_MSG_DEBUG("No pedestal(s) found for channel 0x" << MSG::hex << chid << MSG::dec);
			  ATH_MSG_INFO( "Using fake (baseline) pedestal subtraction..." );  
			  pedAve = wHelper.getBaseline(thisWave,m_baseline) ;
			} else {
			    pedAve = pedestal;
			}
		    } else {
		      pedAve = wHelper.getBaseline(thisWave,m_baseline) ;	
		    }
		    ATH_MSG_DEBUG("Pedestal for channel 0x" << MSG::hex << chid << MSG::dec << " is = " << pedAve << " ADC");
		}

		double waveMax = thisWave.getSample( wHelper.getMax(thisWave) ) ;
		if ( (!m_recAll) && m_ADCsatur>0 && waveMax>=m_ADCsatur ) { 
		    ATH_MSG_INFO( "Absolute ADC saturation at DAC = " << thisWave.getDAC() << " ... skip!" ) ;
		    continue ;
		} else {
                    dacWaves.emplace_back(((thisWave)+(-pedAve)).getWave() ,
		                         thisWave.getErrors(),
					 thisWave.getTriggers(),
					 thisWave.getDt(), 
					 (thisWave.getDAC() + (thisWave.getIsPulsedInt()<<24)), 
					 thisWave.getFlag() );
       
		    NCaliWave++;
		}
	    
	    } // end of loop DACs                 

	    // intermediate map cleanup (save memory)
            cell_it->clear();

	} //end loop cells

    } //end loop over m_keyList

    ATH_MSG_INFO( " Summary : Number of cells with a CaliWave  reconstructed : " << NCaliWave  );    
    ATH_MSG_INFO( " Summary : Number of Barrel PS cells side A or C (connected+unconnected):   3904+ 192 =  4096 " );
    ATH_MSG_INFO( " Summary : Number of Barrel    cells side A or C (connected+unconnected):  50944+2304 = 53248 " );
    ATH_MSG_INFO( " Summary : Number of EMEC      cells side A or C (connected+unconnected):  31872+3456 = 35328 " );
    ATH_MSG_INFO( " Summary : Number of HEC       cells side A or C (connected+unconnected):   2816+ 256 =  3072 " );
    ATH_MSG_INFO( " Summary : Number of FCAL      cells side A or C (connected+unconnected):   1762+  30 =  1792 " );
 
    // Record in detector store with key (m_keyoutput)
    ATH_CHECK( detStore()->record(std::move(caliWaveContainer), m_keyoutput) );

    ATH_MSG_INFO( "LArCaliWaveBuilder has finished." );
    return StatusCode::SUCCESS;
}
