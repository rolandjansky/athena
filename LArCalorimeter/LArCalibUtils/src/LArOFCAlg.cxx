//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// LArOFC: Algorithm to calculate optimal filtering constants.

#include "LArCalibUtils/LArOFCAlg.h"

//#include "GaudiKernel/MsgStream.h"
//#include "GaudiKernel/ISvcLocator.h" 
//#include "StoreGate/StoreGateSvc.h"

#include "LArRawConditions/LArWaveHelper.h"
#include "LArRawConditions/LArOFCComplete.h"
#include "LArRawConditions/LArShapeComplete.h"
#include "LArRawConditions/LArOFCComplete.h"
#include "LArRawConditions/LArOFCBinComplete.h"
#include "LArRawConditions/LArPhysCaliTdiffComplete.h"

#include "LArIdentifier/LArOnlineID.h"
#include "LArTools/LArCablingService.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include <cassert>


//#define LAROFCALG_DEBUGOUTPUT

LArOFCAlg::LArOFCAlg(const std::string& name, ISvcLocator* pSvcLocator) 
	: AthAlgorithm(name, pSvcLocator),
          m_optNpt(0),
	  m_AutoCorrDecoder(0),
	  m_larCaliWaveContainer(0),
	  m_larPhysWaveContainer(0),
	  m_groupingType("SubDetector"), // SubDetector, Single, FeedThrough
          m_onlineID(0),
          m_calo_dd_man(0)
{

  declareProperty("Nsample",m_nSamples = 5);
  declareProperty("Nphase", m_nPhases  = 50);
  declareProperty("Dphase", m_dPhases  = 1);
  declareProperty("Ndelay", m_nDelays  = 24);
  
  declareProperty("KeyList",           m_keylist);

  declareProperty("ReadCaliWave",      m_readCaliWave=true); // false == PhysWave
  declareProperty("FillShape",         m_fillShape=false); 
  declareProperty("KeyOFC",            m_ofcKey="LArOFC"); 
  declareProperty("KeyShape",          m_shapeKey="LArShape"); 

  declareProperty("Normalize",         m_normalize=false);
  declareProperty("TimeShift",         m_timeShift=false);
  declareProperty("TimeShiftByIndex",  m_timeShiftByIndex=-1);
  
  declareProperty("Verify",            m_verify=true);
  declareProperty("ErrAmplitude",      m_errAmpl=0.001);
  declareProperty("ErrTime",           m_errTime=0.001);
  
  declareProperty("DumpOFCfile",       m_dumpOFCfile=std::string(""));  

  declareProperty("GroupingType",      m_groupingType);
  declareProperty("StoreMaxPhase",     m_storeMaxPhase=false);
  declareProperty("LArOFCBinKey",      m_ofcBinKey="LArOFCPhase");

  declareProperty("LArPhysWaveBinKey", m_larPhysWaveBinKey="");

  declareProperty("AddTimeOffset"      ,m_addOffset=0.);

  declareProperty("UseDelta",          m_useDelta=0); // 0= not use Delta, 1=only EMECIW/HEC/FCAL, 2=all , 3 = only EMECIW/HEC/FCAL1+high eta FCAL2-3

  declareProperty("DecoderTool",       m_AutoCorrDecoder); 

}



StatusCode LArOFCAlg::initialize(){

  if ( m_nSamples>32 ) {
    msg(MSG::ERROR) << "You are not allowed to compute OFC for Nsamples = " << m_nSamples << endreq ;
    return  StatusCode::FAILURE;
  } 

  StatusCode sc = m_AutoCorrDecoder.retrieve();
  if (sc.isFailure()) {
        msg(MSG::FATAL) << "Could not retrieve AutoCorrDecoder " << m_AutoCorrDecoder << endreq;
        return StatusCode::FAILURE;
  } else {
     msg(MSG::INFO) << "Retrieved Decoder Tool: "<< m_AutoCorrDecoder << endreq;
  }


  if (m_useDelta > 0 ){

    StatusCode sc = detStore()->retrieve(m_onlineID); 
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "failed to retrieve LArOnlineID " << endreq;
      return sc;
    }

    if (m_useDelta == 3 ){
      /**
      sc = detStore()->retrieve(m_calo_dd_man); 
      if (sc.isFailure()) {
	msg(MSG::ERROR) << "failed to CaloDetDescrManager " << endreq;
	return sc;
      }
      */

      sc = m_cablingService.retrieve() ;
      if (sc.isFailure()) {
	msg(MSG::ERROR) << "failed to retrieve LArCablingService " << endreq;
	return sc;
      }
      msg(MSG::INFO) << " retrieved CaloDetDescrManager and LArCablingService " << endreq;
    }
  }

  return StatusCode::SUCCESS;
}


StatusCode LArOFCAlg::stop() 
{

  ATH_MSG_DEBUG( "In LArOFCAlg finalize()");
  
  msg(MSG::INFO) << "Number of samples            : " << m_nSamples << endreq ;
  msg(MSG::INFO) << "Number of delays acquired    : " << m_nDelays  << endreq ;
  msg(MSG::INFO) << "Number of phases in OFC      : " << m_nPhases  << endreq ;
  msg(MSG::INFO) << "Spacing between two phases   : " << m_dPhases  << endreq ;


  if (m_useDelta == 3 ){
    StatusCode sc = detStore()->retrieve(m_calo_dd_man); 
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "failed to CaloDetDescrManager " << endreq;
      return sc;
    }
  }


  unsigned nPoints = m_nDelays * ( m_nSamples-1 ) + m_nPhases * m_dPhases ;

  msg(MSG::INFO) << "Number of wave points needed : " << nPoints  << endreq ;

  if ( m_timeShift ) {
    if( m_timeShiftByIndex == -1 ) {
      msg(MSG::INFO) << "    Will use helper class for start time." << endreq;
    } else {
      msg(MSG::INFO) << "    Manually shifting pulses by time index " << m_timeShiftByIndex << endreq;
    }
  }
  
  // OFC persistent object
  LArOFCComplete *larOFCComplete=new LArOFCComplete();
  StatusCode sc = larOFCComplete->setGroupingType(m_groupingType,msg());
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed to set groupingType for LArOFCComplete object" << endreq;
    return sc;
  }
  sc=larOFCComplete->initialize(); 
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Failed initialize LArOFCComplete object" << endreq;
    return sc;
  }

  LArOFCBinComplete *larOFCBinComplete=NULL;
  if (m_storeMaxPhase) {
    larOFCBinComplete=new LArOFCBinComplete();
    sc=larOFCBinComplete->setGroupingType(m_groupingType,msg());
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Failed to set groupingType for LArOFCBinComplete object" << endreq;
      return sc;
    }
    sc  = larOFCBinComplete->initialize(); 
    if ( sc.isFailure() ) {
      msg(MSG::ERROR) << "Could not initialize LArOFCComplete data object - exit!" << endreq ;
      return sc ;
    }
  }

  // LArShape persistent object
  LArShapeComplete *larShapeComplete=NULL;
  if (m_fillShape) {
    larShapeComplete = new LArShapeComplete();
    sc=larShapeComplete->setGroupingType(m_groupingType,msg());
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Failed to set groupingType for LArShapeComplete object" << endreq;
      return sc;
    }
    sc=larShapeComplete->initialize(); 
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Failed initialize LArShapeComplete object" << endreq;
      return sc;
    }
  }


  const LArOFCBinComplete* larPhysWaveBin=NULL;
  if (m_larPhysWaveBinKey.size()) {
    sc=detStore()->retrieve(larPhysWaveBin,m_larPhysWaveBinKey);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Failed to retrieve LArOFCBinComplete object with key " << m_larPhysWaveBinKey << endreq;
      return sc;
    }
  }

  LArWaveHelper larWaveHelper;
  int NOFC=0;
  
  for ( unsigned k=0 ; k<m_keylist.size() ; k++ ) { // Loop over all containers that are to be processed (e.g. different gains)
        
    msg(MSG::INFO) << "Processing WaveContainer from StoreGate! key = " << m_keylist[k] << endreq;

    // Iterators for LArCaliWaveContainer
    unsigned   caliGain_it;
    CaliCellIt caliCell_it;
    CaliWaveIt caliWave_it;
    // Iterator for LArPhysWaveContainer
    unsigned   physGain_it;
    PhysWaveIt physWave_it;
     
    const LArWaveCumul* nextWave = 0;
    HWIdentifier ch_id;
    CaloGain::CaloGain gain;
    if (m_readCaliWave) {
 	// Init for LArCaliWaveContainer
 	sc = initCaliWaveContainer(m_keylist[k], caliGain_it, caliCell_it, caliWave_it); 
 	if (sc.isFailure()) {
	  ATH_MSG_DEBUG( "Cannot read CaliWaveContainer from StoreGate!");
	  continue ;
         }
	 nextWave = &(*caliWave_it);
         ch_id    = caliCell_it.channelId();
         gain     = (CaloGain::CaloGain)caliGain_it;
    }
    else {
	// Init for LArPhysWaveContainer
	sc = initPhysWaveContainer(m_keylist[k], physGain_it, physWave_it) ; 
	if (sc.isFailure()) {
	  ATH_MSG_DEBUG( "Cannot read PhysWaveContainer from StoreGate!");
	  continue ;
	}
	nextWave = &(*physWave_it);
        ch_id    = physWave_it.channelId();
        gain     = (CaloGain::CaloGain)physGain_it;
    }
    
    unsigned nItem      = 0;
    unsigned nChanTot   = 0;
    
    unsigned nWaveSmall = 0;
    unsigned nACmissing = 0;
    unsigned nFaultyOFC = 0;

    while (nextWave) { // loop through channels
    
      if ( nextWave->getFlag() == LArWave::dac0 ) continue ; // skip dac0 waves
      ++nChanTot;

      ATH_MSG_DEBUG( "Computing OFC for channel 0x" 
		     << std::hex << ch_id.get_compact() << std::dec 
		     << " in gain = " << gain);
          
      // the current waveform
      //const LArWave& aWave = *nextWave;
      // Actually *copying* the Wave to get rid of the const: need to manipulate Wave to normalize...
      LArWave aWave = *nextWave;
    
      // check constistency of settings
      if ( nPoints > aWave.getSize() ) {
        msg(MSG::ERROR) << "Wave size (" << aWave.getSize() 
			<< ") is too small to fit your OFC request (" << nPoints << " points)" << endreq ;
	nWaveSmall++ ;
	// move to next wave
	if (m_readCaliWave) {
	  nextWave = getWave(caliGain_it, caliCell_it, caliWave_it);
          ch_id    = caliCell_it.channelId();
          gain     = (CaloGain::CaloGain)caliGain_it;
        } else {
	  nextWave = getWave(physGain_it, physWave_it);
          ch_id    = physWave_it.channelId();
          gain     = (CaloGain::CaloGain)physGain_it;
        }
	continue ; // skip channel if wave is too small
      }


      if (larPhysWaveBin) {
	const int bin = larPhysWaveBin->bin(ch_id,gain);
	if (bin>-998) { //>ERRORCODE
	  ATH_MSG_VERBOSE("Channel 0x" << MSG::hex << ch_id.get_compact() << MSG::dec << ": shift by index " << bin);
	  aWave=larWaveHelper.translate(aWave,-bin,0);
	}
	else
	  ATH_MSG_VERBOSE("Channel 0x" << MSG::hex << ch_id.get_compact() << MSG::dec << ": No valid index for shifting");
      }//end if larPhysWaveBin
      // normalize input wave, if requested      
      if (m_normalize) {
	double peak = aWave.getSample( larWaveHelper.getMax(aWave) );
        ATH_MSG_VERBOSE("Channel 0x" << std::hex << ch_id.get_compact() << std::dec 
			<< " has amplitude = " << peak << ": normalizing...");

	if ( peak == 0 ) {
	  msg(MSG::ERROR) << "Wave maximum is zero, skipping channel 0x" << std::hex << ch_id.get_compact() << std::dec << endreq ;
	  // Move to next wave
          if (m_readCaliWave) {
	    nextWave = getWave(caliGain_it, caliCell_it, caliWave_it);
            ch_id    = caliCell_it.channelId();
            gain     = (CaloGain::CaloGain)caliGain_it;
          } else {
	    nextWave = getWave(physGain_it, physWave_it);
            ch_id    = physWave_it.channelId();
            gain     = (CaloGain::CaloGain)physGain_it;
          }
	  continue ;
	}
        aWave = aWave * (1./peak);
      } 

      double Amplitude = aWave.getSample( larWaveHelper.getMax(aWave) );
      ATH_MSG_VERBOSE("Channel 0x" << std::hex << ch_id.get_compact() << std::dec << " has now amplitude = " << Amplitude);
      // compute tstart to shift input wave, if requested
      int tstart = 0 ;
      if ( m_timeShift ) {
        if( m_timeShiftByIndex == -1 ) {
          tstart = larWaveHelper.getStart(aWave) ;
        } else {
          tstart = m_timeShiftByIndex;
        }
      }
      
      ATH_MSG_DEBUG("Channel 0x" << std::hex << ch_id.get_compact() << std::dec << ", Tstart = " << tstart);

      //Calculate derivative for this wave
      LArWave aDerivedWave = larWaveHelper.derive_smooth(aWave);

      // Get the AutoCorrelatio  matrix
      m_AutoCorrMatrix = m_AutoCorrDecoder->AutoCorr(ch_id,gain,m_nSamples);

      //Define vectors for OFC coeffs.(Outer vector: tbin, inner one: coeffs)
      std::vector<std::vector<float> > vOFC_a; 
      std::vector<std::vector<float> > vOFC_b;
      
      //Define vectors for Shape and ShapeDer (Outer vector: tbin, inner one: Shapes)
      std::vector<std::vector<float> > vShape; 
      std::vector<std::vector<float> > vShapeDer;

      bool faultyOFC = false ;

      float timeBinWidthOFC = m_dPhases*aWave.getDt();

      unsigned phasewMaxAt3=999;
      float maxSampleValAt3=-1;

      m_id = ch_id; 

      for (unsigned iPhase=0;iPhase<m_nPhases;iPhase++) { //Loop over all phases
      	
	ATH_MSG_VERBOSE ("Channel 0x" << std::hex << ch_id.get_compact() << std::dec 
			 << ", Gain = " << (int)(gain) << ", Phase = " << iPhase << ":");

	std::vector<float> theSamples;
	std::vector<float> theSamplesDer;

	for (unsigned iSample=0;iSample<m_nSamples;++iSample){ //Loop over all samples
	
	  unsigned tbin = tstart + iPhase*m_dPhases + iSample*m_nDelays ;
	  
	  theSamples.push_back( aWave.getSample(tbin) );
	  theSamplesDer.push_back( aDerivedWave.getSample(tbin) );
#ifdef LAROFCALG_DEBUGOUTPUT
          ATH_MSG_VERBOSE("tbin: " << tbin);
	  ATH_MSG_VERBOSE("g[" << iSample << "][" << iPhase << "] = " << aWave.getSample(tbin)
			  << " g'[" << iSample << "][" << iPhase << "]=" << aDerivedWave.getSample(tbin));
#endif
	} //End loop over samples

	std::vector<float> theSamples32;
	std::vector<float> theSamplesDer32;
	unsigned nSample_shape=32;
	
	for (unsigned iSample=0;iSample<nSample_shape ;++iSample){ //Loop over all samples
	
	    unsigned tbin = tstart + iPhase*m_dPhases + iSample*m_nDelays ;	  
	    if ( tbin>=aWave.getSize() )
	      {
		continue;
	      }
	    theSamples32.push_back( aWave.getSample(tbin) );
	    theSamplesDer32.push_back( aDerivedWave.getSample(tbin) );
	} //End loop over samples


	m_thisChan_useDelta = false;
	if (m_useDelta==2)
	  {
	    m_thisChan_useDelta = true;
	  }
	else if(m_useDelta==1)
	  { // only HEC/EMECIW/FCAL
	    if (m_onlineID->isEMECIW(m_id) || m_onlineID->isFCALchannel(m_id) || m_onlineID->isHECchannel(m_id)) 
	      {
		m_thisChan_useDelta = true;
	      }
	  }
	else if(m_useDelta==3)
	  { // only HEC/EMECIW/FCAL1 and high eta FCAL2-3 
	    if (m_onlineID->isEMECIW(m_id) ||  m_onlineID->isHECchannel(m_id)) 
	      {
		m_thisChan_useDelta = true;
	      }
	    else if (m_onlineID->isFCALchannel(m_id) ){

	      if (m_cablingService->isOnlineConnected (m_id)){
		Identifier ofl_id = m_cablingService->cnvToIdentifier(m_id);
		const CaloDetDescrElement* dde = m_calo_dd_man->get_element(ofl_id);
		if (! dde) {
		  ATH_MSG_ERROR( " dde = 0 , onl_id, ofl_id= "<< m_id<<" "<<ofl_id );
		  return StatusCode::FAILURE; 
		}
		CaloCell_ID::CaloSample sampling = dde->getSampling();
		float eta = dde->eta();
		if (sampling==CaloCell_ID::FCAL0){
		  m_thisChan_useDelta = true;
		}
		else
		  {
		    if (fabs(eta)>4.0){
		      m_thisChan_useDelta = true;
		    }
		  }
		
	      }
	    }

	  }
	
	if ( m_thisChan_useDelta ){
	  getDelta(theSamples32 );
	  if (m_delta.size()==0){
	    m_thisChan_useDelta=false;
	  }
	}


	// Fill Shape and ShapeDer vectors
	if (m_fillShape) {
	  vShape.push_back(theSamples);
	  vShapeDer.push_back(theSamplesDer);
	}

	if (m_storeMaxPhase && m_nSamples>2 && theSamples[2]>maxSampleValAt3) {
	  maxSampleValAt3=theSamples[2];
	  phasewMaxAt3=iPhase;
	}

	// OFC calculation for the current phase
	Optfilt(theSamples,theSamplesDer);
	
	// Fill OFC data vectors
	std::vector<float> vOFC_as,vOFC_bs;
	for (unsigned iSample=0;iSample<m_nSamples;++iSample){
	  vOFC_as.push_back(m_a[iSample]);
	  vOFC_bs.push_back(m_b[iSample]);
	}
	vOFC_a.push_back(vOFC_as);
	vOFC_b.push_back(vOFC_bs);

        // verify OFC consistency
	if (m_verify) {
	  double recAmpl=0, recTime=0;
	  for (unsigned iSample=0;iSample<m_nSamples;++iSample){
	    unsigned tbin = tstart + iPhase*m_dPhases + iSample*m_nDelays ;
            #ifdef LAROFCALG_DEBUGOUTPUT
	    ATH_MSG_VERBOSE("a["<<iSample<<"]="<<vOFC_as[iSample] << " b["<<iSample<<"]="<<vOFC_bs[iSample] 
			    << " Sample=" << aWave.getSample(tbin));
            #endif
	    recAmpl += vOFC_as[iSample] * ( aWave.getSample(tbin) ) ;
	    recTime += vOFC_bs[iSample] * ( aWave.getSample(tbin) ) ;
	  }
	  recTime /= recAmpl ;
	  // At this point the reconstructed amplitude must be = 1 by definition, whatever the initial normalisation!
	  ATH_MSG_VERBOSE("recAmp=" << recAmpl << " ; recTime=" << recTime);
	  if ( fabs(1.-recAmpl) > m_errAmpl ) {
	    msg(MSG::WARNING) << "Applying the OFC to original wave yields an Amplitude of "<< recAmpl 
			      << " instead of 1. -> Wrong OFCs? channel 0x" << std::hex << ch_id.get_compact() << std::dec << endreq;
	    faultyOFC = true ;
	    continue ;
	  }
	  if ( fabs(recTime) > m_errTime ) {
	    msg( MSG::WARNING) << "Applying the OFC to original wave yields a time offset of " << recTime 
			       << " -> Wrong OFCs? channel 0x" << std::hex << ch_id.get_compact() << std::dec << endreq;
	    faultyOFC = true ;
	    continue ;
	  }
	} // end if (m_verify)
	
      } //End loop over all phases

      //std::cout << "WL: Channel 0x" << std::hex << ch_id.get_compact() << std::dec << " ; gain=" << (int)gain 
      //	<< " maxPhase= " << phasewMaxAt3 << " Sample[2]= " << maxSampleValAt3 << std::endl;
      if ( faultyOFC ) {
        nFaultyOFC++ ;
        // Move to next wave
        if (m_readCaliWave) {
	  nextWave = getWave(caliGain_it, caliCell_it, caliWave_it);
          ch_id    = caliCell_it.channelId();
          gain     = (CaloGain::CaloGain)caliGain_it;
        } else {
	  nextWave = getWave(physGain_it, physWave_it);
          ch_id    = physWave_it.channelId();
          gain     = (CaloGain::CaloGain)physGain_it;
        }
        continue; // skip registration of previous channel (that failed OFC verification!)
      }

      NOFC++;      

      // in case we're dealing with a LArPhysWave, add any possible previous time shift
      if (!m_readCaliWave) tstart += (*physWave_it).getTimeOffset()+m_addOffset;

      // register channel to LArOFCComplete
      ATH_MSG_DEBUG( "add to LArOFCComplete, channel 0x" 
		     << std::hex << ch_id.get_compact() << std::dec << " ; gain=" << (int)gain);
      larOFCComplete->set(ch_id,(int)gain,vOFC_a,vOFC_b,tstart,timeBinWidthOFC);
      if (larOFCBinComplete) larOFCBinComplete->set(ch_id,(int)gain,phasewMaxAt3);      
      if ( m_fillShape ) larShapeComplete->set(ch_id,(int)gain,vShape,vShapeDer,tstart,timeBinWidthOFC);      
      ATH_MSG_DEBUG( "... added!  ( item = " << nItem++ << " )");

      // Move to next wave
      if (m_readCaliWave) {
	nextWave = getWave(caliGain_it, caliCell_it, caliWave_it);
        ch_id    = caliCell_it.channelId();
        gain     = (CaloGain::CaloGain)caliGain_it;
      } else {
	nextWave = getWave(physGain_it, physWave_it);
        ch_id    = physWave_it.channelId();
        gain     = (CaloGain::CaloGain)physGain_it;
      }

    } // end loop over waves

    msg(MSG::INFO) << nWaveSmall << " channel(s) out of " << nChanTot
 	<< " skipped because wave was too small." << endreq;
  
    msg(MSG::INFO) << nACmissing << " channel(s) out of " << nChanTot
 	<< " skipped due to missing AC matrix." << endreq;
    
    msg(MSG::INFO) << nFaultyOFC << " channel(s) out of " << nChanTot
        << " skipped due to faulty OFC." << endreq;
    

  } // end loop over containers

  //msg(MSG::INFO) << " Summary : Number of cells with a OFC value computed : " << larOFCComplete->totalNumberOfConditions()  << endreq;
  msg(MSG::INFO) << " Summary : Number of cells with a OFC value computed : " << NOFC  << endreq;
  msg(MSG::INFO) << " Summary : Number of Barrel PS cells side A or C (connected+unconnected):   3904+ 192 =  4096 " << endreq;
  msg(MSG::INFO) << " Summary : Number of Barrel    cells side A or C (connected+unconnected):  50944+2304 = 53248 " << endreq;
  msg(MSG::INFO) << " Summary : Number of EMEC      cells side A or C (connected+unconnected):  31872+3456 = 35328 " << endreq;
  msg(MSG::INFO) << " Summary : Number of HEC       cells side A or C (connected+unconnected):   2816+ 256 =  3072 " << endreq;
  msg(MSG::INFO) << " Summary : Number of FCAL      cells side A or C (connected+unconnected):   1762+  30 =  1792 " << endreq;
  
  // record and symlynk LArOFCComplete object
  ATH_MSG_DEBUG( "Trying to record LArOFCComplete object to detector store, key = " << m_ofcKey);
  sc = detStore()->record(larOFCComplete,m_ofcKey);
  if (sc.isFailure()) {
      msg(MSG::ERROR) << "Could not record LArOFCComplete to DetStore with key " << m_ofcKey << endreq;
      return StatusCode::FAILURE;
  }
  msg(MSG::INFO) << "LArOFCComplete object recorded to DetStore successfully with key " << m_ofcKey << endreq ;

  ATH_MSG_DEBUG( "Trying to symlink ILArOFC with LArOFCComplete");
  ILArOFC* larOFC = 0;
  sc = detStore()->symLink(larOFCComplete, larOFC);
  if (sc.isFailure()) {
      msg(MSG::ERROR) << "Could not symlink ILArOFC with LArOFCComplete." << endreq;
      return StatusCode::FAILURE;
  } 
  msg(MSG::INFO) << "ILArOFC symlink with LArOFCComplete successfully" << endreq ;
  
  if ( m_dumpOFCfile != std::string("") ) {
    msg(MSG::INFO) << "Dumping OFCs to file " << m_dumpOFCfile << endreq ;
    larOFCComplete->dumpOFC(m_dumpOFCfile) ;
  }

  if (larOFCBinComplete) {
    sc = detStore()->record(larOFCBinComplete,m_ofcBinKey);
    if (sc.isFailure()) {
       msg(MSG::ERROR) << "Could not record LArOFCBinCompete object" << endreq;
       return StatusCode::FAILURE;
    }
  }
  
  // record and symlynk LArShapeComplete object
  if ( m_fillShape ) {
    ATH_MSG_DEBUG( "Trying to record LArShapeComplete object to detector store, key = " << m_shapeKey);
    sc = detStore()->record(larShapeComplete,m_shapeKey);
    if (sc.isFailure()) {
       msg(MSG::ERROR) << "Could not record LArShapeComplete to DetStore with key " << m_shapeKey << endreq;
       return StatusCode::FAILURE;
    }
    msg(MSG::INFO) << "LArShapeComplete object recorded to DetStore successfully with key " << m_shapeKey << endreq ;
    ATH_MSG_DEBUG( "Trying to symlink ILArShape with LArShapeComplete");
    ILArShape* larShape = 0;
    sc = detStore()->symLink(larShapeComplete,larShape);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Could not symlink ILArShape with LArShapeComplete." << endreq;
      return StatusCode::FAILURE;
    } 
    msg(MSG::INFO) << "ILArShape symlink with LArShapeComplete successfully" << endreq ;
  }  
  // undo corrections, if applied
  if(m_readCaliWave) {
     if(m_larCaliWaveContainer->correctionsApplied()) {
        LArCaliWaveContainer* cnt=const_cast<LArCaliWaveContainer*>(m_larCaliWaveContainer);
        if(cnt->correctionsApplied()) {
          msg(MSG::INFO) << "Undoing the correction in CaliWaveContainer." << endreq;
          sc = cnt->undoCorrections();
          if (sc.isFailure()) {
           msg(MSG::ERROR) << "Could not undo the correction in CaliWaveContainer." << endreq;
          }
        }
     }
  } else {
     LArPhysWaveContainer* cnt=const_cast<LArPhysWaveContainer*>(m_larPhysWaveContainer);
     if(cnt->correctionsApplied()) {
        sc = cnt->undoCorrections();
          if (sc.isFailure()) {
           msg(MSG::ERROR) << "Could not undo the correction in PhysWaveContainer." << endreq;
          }
     }
  }
  
  return StatusCode::SUCCESS;
}


StatusCode
LArOFCAlg::initPhysWaveContainer(const std::string& keyPhys,
                                       unsigned&   gain_it,	
				       PhysWaveIt& wave_it)
{
     ATH_MSG_DEBUG( "initPhysWaveContainer ");
    // Retrieve the container with the given key	
    std::string key = keyPhys;
    StatusCode sc = detStore()->retrieve(m_larPhysWaveContainer,key);	    
    if (sc.isFailure()) {
    	msg(MSG::ERROR) << "Cannot read LArPhysWaveContainer from StoreGate! key=" << key << endreq;
    	return StatusCode::FAILURE;
    } else {
    	msg(MSG::INFO) << "Read LArPhysWaveContainer from StoreGate! key= "  << key << endreq;
    }	 

    if (!m_larPhysWaveContainer->correctionsApplied()) {
      msg(MSG::INFO) << "LArPhysWaveContainer: Corrections not yet applied, applying them now..." << endreq;
      LArPhysWaveContainer* cnt=const_cast<LArPhysWaveContainer*>(m_larPhysWaveContainer);
      sc=cnt->applyCorrections();
      if (sc.isFailure()) {
	msg(MSG::ERROR) << "Failed to apply corrections to LArPhysWaveContainer!" << endreq;
	return sc;
      }
    }
    else
      msg(MSG::INFO) << "LArPhysWaveContainer: Corrections already applied" << endreq;

    // set iterators for first wave
    for ( gain_it = CaloGain::LARHIGHGAIN ; gain_it < CaloGain::LARNGAIN ; gain_it++ ) { // loop on possible gains
       int ncell = 0;
       for ( wave_it = m_larPhysWaveContainer->begin(gain_it); wave_it != m_larPhysWaveContainer->end(gain_it); ++wave_it) {
            if ( wave_it == m_larPhysWaveContainer->begin(gain_it) )
	      ATH_MSG_DEBUG( "LArPhysWaveContainer (key = " << key << ") has at least one wave with gain = " << gain_it);
	    ATH_MSG_DEBUG( "Seeking for first non-empty channel at position " << ++ncell);
	    if ( !wave_it->isEmpty() ) {
	      ATH_MSG_DEBUG( "First non-empty channel found at position " << ncell);
	      return StatusCode::SUCCESS;
	    }   
       }  
       ATH_MSG_DEBUG( "LArPhysWaveContainer (key = " << key << ") has no wave with gain = " << gain_it);
    }
    ATH_MSG_DEBUG( "LArPhysWaveContainer (key = " << key << ") is empty!");
    return StatusCode::FAILURE;
}


const LArWaveCumul*
LArOFCAlg::getWave(unsigned& gain_it, PhysWaveIt& wave_it) 
{
    const LArWaveCumul* result = 0;
    while ( gain_it < CaloGain::LARNGAIN ) {
	while ( wave_it != m_larPhysWaveContainer->end(gain_it) ) {
	  ATH_MSG_DEBUG( "Trying next cell...");
	    ++wave_it;
	    if ( wave_it != m_larPhysWaveContainer->end(gain_it) 
		 && wave_it->getSize() != 0 ) {
		result = &(*wave_it);
		return result ;
	    }
	}
	// try next gain
	ATH_MSG_DEBUG( "Trying next gain...");
	++gain_it;
	if ( gain_it < CaloGain::LARNGAIN ) {
	    wave_it = m_larPhysWaveContainer->begin(gain_it);
	    if ( wave_it != m_larPhysWaveContainer->end(gain_it) 
		 && wave_it->getSize() != 0 ) {
		result = &(*wave_it);
		return (result);  	
	}
      }
    }
    
    ATH_MSG_DEBUG("LArPhysWaveContainer exausted.");
    return (result);
}



StatusCode
LArOFCAlg::initCaliWaveContainer(const std::string& keyCali,
                                       unsigned&   gain_it,
				       CaliCellIt& cell_it, 	
				       CaliWaveIt& wave_it)
{
 
    ATH_MSG_DEBUG("initCaliWaveContainer");
    // Retrieve the container with the given key	
    std::string key = keyCali;
    StatusCode sc = detStore()->retrieve(m_larCaliWaveContainer,key);	    
    if (sc.isFailure()) {
    	msg(MSG::ERROR) << "Cannot read LArCaliWaveContainer from StoreGate! key=" << key << endreq;
    	return StatusCode::FAILURE;
    } else {
    	msg(MSG::INFO) << "Read LArCaliWaveContainer from StoreGate! key= "  << key << endreq;
    }
    if (!m_larCaliWaveContainer->correctionsApplied()) {
      msg(MSG::INFO) << "LArCaliWaveContainer: Corrections not yet applied, applying them now..." << endreq;
      LArCaliWaveContainer* cnt=const_cast<LArCaliWaveContainer*>(m_larCaliWaveContainer);
      sc=cnt->applyCorrections();
      if (sc.isFailure()) {
	msg(MSG::ERROR) << "Failed to apply corrections to LArCaliWaveContainer!" << endreq;
	return sc;
      }	 
    }
    else
      msg(MSG::INFO) << "LArCaliWaveContainer: Corrections already applied" << endreq;

    // set iterators for first wave
    for ( gain_it = CaloGain::LARHIGHGAIN ; gain_it < CaloGain::LARNGAIN ; gain_it++ ) { // loop on possible gains
       int ncell = 0;
       for ( cell_it = m_larCaliWaveContainer->begin(gain_it); cell_it != m_larCaliWaveContainer->end(gain_it); ++ cell_it) {         
	    
	    if (cell_it == m_larCaliWaveContainer->begin(gain_it) )
	      ATH_MSG_DEBUG( "LArCaliWaveContainer (key = " << key << ") has at least one wave with gain = " << gain_it);
	    
	    ATH_MSG_DEBUG( "Seeking for first non-empty channel at position " << ++ncell);
	    
	    if ( cell_it->begin() != cell_it->end() ) {
	      ATH_MSG_DEBUG( "First non-empty channel found at position " << ncell);
		wave_it = cell_it->begin();
	        return StatusCode::SUCCESS;
	    }
	    
       }  
       ATH_MSG_DEBUG( "LArCaliWaveContainer (key = " << key << ") has no wave with gain = " << gain_it);
    }
    ATH_MSG_DEBUG( "LArCaliWaveContainer (key = " << key << ") is empty!");
    return StatusCode::FAILURE;
}



const LArWaveCumul*
LArOFCAlg::getWave(unsigned& gain_it, CaliCellIt& cell_it, CaliWaveIt& wave_it) 
{
    const LArWaveCumul* result = 0;
    
    while ( gain_it < CaloGain::LARNGAIN ) {
	while ( cell_it != m_larCaliWaveContainer->end(gain_it) ) {
	    while ( wave_it != cell_it->end() ) {
	      ATH_MSG_DEBUG( "Trying next wave for current cell...");
	       ++wave_it; // check if there are other waves for the current cell
	       if ( wave_it != cell_it->end() &&
	            wave_it->getSize() != 0 ) {
		  result = &(*wave_it);
		  return result ;
	       }
	    }
	    // try next cell
	    ATH_MSG_DEBUG( "Trying next cell...");
	    ++cell_it;
	    if ( cell_it != m_larCaliWaveContainer->end(gain_it) ) {
	       wave_it = cell_it->begin();
	       if ( wave_it != cell_it->end() &&
	            wave_it->getSize() != 0 ) {
	         result = &(*wave_it);
		 return (result); 
	       }
	    }
	}
	// try next gain
	ATH_MSG_DEBUG( "Trying next gain...");
	++gain_it;
	if ( gain_it < CaloGain::LARNGAIN ) {
	    cell_it = m_larCaliWaveContainer->begin(gain_it);
	    if ( cell_it != m_larCaliWaveContainer->end(gain_it) ) {
	       wave_it = cell_it->begin();
	       if ( wave_it != cell_it->end() &&
	            wave_it->getSize() != 0 ) {
	         result = &(*wave_it);
		 return (result); 
	       }
	    }
        }
    }
    
    ATH_MSG_DEBUG( "LArCaliWaveContainer exausted.");
    return (result);
}


void LArOFCAlg::Optfilt(std::vector<float> &gWave_in, std::vector<float>  &gDerivWave_in) 
{

  assert(gWave_in.size()==gDerivWave_in.size());
  m_optNpt = gWave_in.size();

  Eigen::VectorXd gResp_in(m_optNpt), gDerivResp_in(m_optNpt);
  
  for (int i=0;i<m_optNpt;i++) {
    gResp_in[i] = gWave_in[i];
    gDerivResp_in[i] = gDerivWave_in[i];
  }

  m_gResp= gResp_in;
  m_gDerivResp = gDerivResp_in;

  perCalc();

}


void LArOFCAlg::perCalc() {
  

#ifdef LAROFCALG_DEBUGOUTPUT
  static bool firstTime=true ;
  if ( firstTime ) {
    std::cout << m_AutoCorrMatrix  << std::endl ;
    firstTime=false ;
  }
#endif

  //HepMatrix acInverse = m_AutoCorrMatrix.inverse(ifail);
  //assert (ifail == 0 );
 
  // //const unsigned acSize
//   Eigen::MatrixXd ac(m_AutoCorrMatrix.num_row(),m_AutoCorrMatrix.num_col());
//   for (int r=0;r<m_AutoCorrMatrix.num_row();++r) {
//     for (int c=0;c<m_AutoCorrMatrix.num_col();++c) {
//       ac(r,c)=m_AutoCorrMatrix[r][c];
//     }
//   }
  const Eigen::MatrixXd acInverse=m_AutoCorrMatrix.inverse();
  

  if (! m_thisChan_useDelta){

    Eigen::Matrix2d isol;
    isol << 
      (m_gResp.transpose()*acInverse*m_gResp)[0],
      (m_gResp.transpose()*acInverse*m_gDerivResp)[0],
      (m_gDerivResp.transpose()*acInverse*m_gResp)[0],
      (m_gDerivResp.transpose()*acInverse*m_gDerivResp)[0];

    Eigen::Vector2d Amp; 
    Eigen::Vector2d Atau;
    Eigen::Vector2d Ktemp;
    Eigen::MatrixXd isolInv = isol.inverse();

    m_a = Eigen::VectorXd(m_optNpt);
    m_b = Eigen::VectorXd(m_optNpt);

  // assert(ifail == 0);
  //  we solve for the lagrange multiplers

    Ktemp[0] = 1.;
    Ktemp[1] = 0.;
    Amp = isolInv*Ktemp;
  
    Ktemp[0] = 0.; 
    Ktemp[1] = -1.;
    Atau = isolInv*Ktemp;

    //m_a = HepVector(m_optNpt);
    //m_b = HepVector(m_optNpt);
    // we express the a and b vectors in terms of the lagrange multipliers
    m_a = Amp[0]*acInverse*m_gResp + Amp[1]*acInverse*m_gDerivResp;
    m_b = Atau[0]*acInverse*m_gResp + Atau[1]*acInverse*m_gDerivResp;


  }
  else
  {

  // try 3X3 matrix with offsets

    Eigen::Matrix3d isol;
  /**
  HepVector delta(5) ; 
  
  delta[0]=3.79198742146;
  delta[1]=-4.84165741965;
  delta[2]=3.20189843985;
  delta[3]=-5.90850592618;
  delta[4]=1.8260451328;
  */
    isol << 
      (m_gResp.transpose()*acInverse*m_gResp)[0],
      (m_gResp.transpose()*acInverse*m_gDerivResp),
      (m_gResp.transpose()*acInverse*m_delta)[0],

      (m_gDerivResp.transpose()*acInverse*m_gResp)[0],
      (m_gDerivResp.transpose()*acInverse*m_gDerivResp)[0],
      (m_gDerivResp.transpose()*acInverse*m_delta)[0],

      (m_delta.transpose()*acInverse*m_gResp)[0],
      (m_delta.transpose()*acInverse*m_gDerivResp)[0],
      (m_delta.transpose()*acInverse*m_delta)[0];


    Eigen::Vector3d Amp; 
    Eigen::Vector3d Atau;
    Eigen::Vector3d Ktemp;
    Eigen::MatrixXd isolInv = isol.inverse();

    m_a = Eigen::VectorXd(m_optNpt);
    m_b = Eigen::VectorXd(m_optNpt);
 

    // assert(ifail == 0);
    //  we solve for the lagrange multiplers

    Ktemp[0] = 1.;
    Ktemp[1] = 0.;
    Ktemp[2] = 0.;

    Amp = isolInv*Ktemp;
    
    Ktemp[0] = 0.; 
    Ktemp[1] = -1.;
    Atau = isolInv*Ktemp;

    // we express the a and b vectors in terms of the lagrange multipliers
    m_a = Amp[0]*acInverse*m_gResp + Amp[1]*acInverse*m_gDerivResp + Amp[2]*acInverse * m_delta;
    m_b = Atau[0]*acInverse*m_gResp + Atau[1]*acInverse*m_gDerivResp + Atau[2]*acInverse * m_delta  ;
  }
}


void LArOFCAlg::getDelta( std::vector<float>& samples ){

  m_delta= Eigen::VectorXd(5);
  if (m_onlineID->isFCALchannel(m_id) ){
    // FCAL use fixed delta from data. 

    int slot = m_onlineID->slot(m_id) ; 
    if ( slot <=9){
      // FCAL 1
      m_delta[0]= 0.0679600232979;
      m_delta[1]= -0.139479996869;
      m_delta[2]=0.0679600232979;
      m_delta[3]= -0.139479996869;
      m_delta[4]=0.0679600232979;
    }else
      if(slot <=13){
	// FCAL 2
	m_delta[0]=-0.01589001104;
	m_delta[1]=-0.0740399733186;
	m_delta[2]=-0.01589001104;
	m_delta[3]=-0.0740399733186;
	m_delta[4]=-0.01589001104;
    }else
	{ //FCAL 3 
	m_delta[0]=0.0790199937765;
	m_delta[1]=0.0952000226825;
	m_delta[2]=0.0790199937765;
	m_delta[3]=0.0952000226825;
	m_delta[4]=0.0790199937765;
      }

  }else
    { // from Shape 

      float odd = 0.;
      float even = 0.;
      for (unsigned int i = 0;i<samples.size();++i)
	{
	  if (i%2==0){
	    even  += samples[i];
	  }
	  else
	  {
	    odd += samples[i];
	  }
	}

      m_delta[0]=even;
      m_delta[1]=odd;
      m_delta[2]=even;
      m_delta[3]=odd;
      m_delta[4]=even;
  }

  return ;

}

Eigen::VectorXd LArOFCAlg::getAmpCoef(){
  return(m_a);
}

Eigen::VectorXd LArOFCAlg::getATauCoef(){
  return(m_b);
}

#ifdef LAROFCALG_DEBUGOUTPUT
#undef LAROFCALG_DEBUGOUTPUT
#endif
