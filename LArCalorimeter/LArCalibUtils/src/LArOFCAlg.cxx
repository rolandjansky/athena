//Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// LArOFC: Algorithm to calculate optimal filtering constants.

#include "LArCalibUtils/LArOFCAlg.h"

#include "LArRawConditions/LArWaveHelper.h"
#include "LArRawConditions/LArOFCComplete.h"
#include "LArRawConditions/LArShapeComplete.h"
#include "LArRawConditions/LArOFCComplete.h"
#include "LArRawConditions/LArOFCBinComplete.h"
#include "LArRawConditions/LArPhysCaliTdiffComplete.h"
#include "LArCOOLConditions/LArDSPConfig.h"
#include "CoralBase/Blob.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"

#include "LArIdentifier/LArOnlineID.h"
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include <cassert>

#include "tbb/parallel_for.h"

//#define LAROFCALG_DEBUGOUTPUT


LArOFCAlg::LArOFCAlg(const std::string& name, ISvcLocator* pSvcLocator) 
	: AthAlgorithm(name, pSvcLocator),
          m_onlineID(nullptr),
          m_calo_dd_man(nullptr),
	  m_larPhysWaveBin(nullptr),
	  m_groupingType("SubDetector"), // SubDetector, Single, FeedThrough
          m_DSPConfig(nullptr)
{

  declareProperty("Nsample",m_nSamples = 5);
  declareProperty("Nphase", m_nPhases  = 50);
  declareProperty("Dphase", m_dPhases  = 1);
  declareProperty("Ndelay", m_nDelays  = 24);
  
  declareProperty("KeyList",           m_keylist);

  declareProperty("ReadCaliWave",      m_readCaliWave=true); // false == PhysWave
  declareProperty("FillShape",         m_fillShape=false); 
  declareProperty("KeyOFC",            m_ofcKey="LArOFC"); 
  declareProperty("KeyOFCV2",          m_ofcKeyV2="LArOFCV2"); 
  declareProperty("KeyShape",          m_shapeKey="LArShape"); 

  declareProperty("Normalize",         m_normalize=false);
  declareProperty("TimeShift",         m_timeShift=false);
  declareProperty("TimeShiftByIndex",  m_timeShiftByIndex=-1);
  
  declareProperty("Verify",            m_verify=true);
  declareProperty("ErrAmplitude",      m_errAmpl=0.01);
  declareProperty("ErrTime",           m_errTime=0.01);
  
  declareProperty("DumpOFCfile",       m_dumpOFCfile=std::string(""));  

  declareProperty("GroupingType",      m_groupingType);
  declareProperty("StoreMaxPhase",     m_storeMaxPhase=false);
  declareProperty("LArOFCBinKey",      m_ofcBinKey="LArOFCPhase");

  declareProperty("LArPhysWaveBinKey", m_larPhysWaveBinKey="");

  declareProperty("AddTimeOffset",     m_addOffset=0.);

  declareProperty("ComputeOFCV2",      m_computeV2=false);

  declareProperty("UseDelta",          m_useDelta=0); // 0= not use Delta, 1=only EMECIW/HEC/FCAL, 2=all , 3 = only EMECIW/HEC/FCAL1+high eta FCAL2-3
  declareProperty("UseDeltaV2",        m_useDeltaV2=0); // 0= not use Delta, 1=only EMECIW/HEC/FCAL, 2=all , 3 = only EMECIW/HEC/FCAL1+high eta FCAL2-3

  declareProperty("DecoderTool",       m_AutoCorrDecoder); 
  declareProperty("DecoderToolV2",       m_AutoCorrDecoderV2); 

  declareProperty("RunThreaded",       m_runThreaded=false);

  declareProperty("ReadDSPConfig",     m_readDSPConfig=false);
  declareProperty("DSPConfigFolder",   m_DSPConfigFolder="/LAR/Configuration/DSPConfiguration");

  declareProperty("ForceShift",        m_forceShift=false);

  m_nPoints = m_nDelays * ( m_nSamples-1 ) + m_nPhases * m_dPhases ;
}



StatusCode LArOFCAlg::initialize(){

  m_nPoints = m_nDelays * ( m_nSamples-1 ) + m_nPhases * m_dPhases ;

  if ( m_nSamples>32 ) {
    ATH_MSG_ERROR( "You are not allowed to compute OFC for Nsamples = " << m_nSamples ) ;
    return  StatusCode::FAILURE;
  } 

  StatusCode sc = m_AutoCorrDecoder.retrieve();
  if (sc.isFailure()) {
        ATH_MSG_FATAL( "Could not retrieve AutoCorrDecoder " << m_AutoCorrDecoder );
        return StatusCode::FAILURE;
  } else {
     ATH_MSG_INFO( "Retrieved Decoder Tool: "<< m_AutoCorrDecoder );
  }


  if (m_computeV2) {
    sc = m_AutoCorrDecoderV2.retrieve();
    if (sc.isFailure()) {
      ATH_MSG_FATAL( "Could not retrieve AutoCorrDecoderV2 " << m_AutoCorrDecoderV2 );
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_INFO( "Retrieved Decoder Tool: "<< m_AutoCorrDecoderV2 );
    }
  }


  sc = detStore()->retrieve(m_onlineID); 
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "failed to retrieve LArOnlineID " );
    return sc;
  }

  ATH_CHECK(m_cablingKey.initialize());

  ATH_MSG_INFO( "Number of wave points needed : " << m_nPoints  ) ;
  if (m_computeV2) {
    ATH_MSG_INFO( "Will compute two flavors of OFCs" );
    ATH_MSG_INFO( "Version 1: useDelta= " << m_useDelta <<", output key: " <<  m_ofcKey << " AutoCorrDecoder: " << m_AutoCorrDecoder.name() );
    ATH_MSG_INFO( "Version 2: useDelta= " << m_useDeltaV2 <<", output key: " <<  m_ofcKeyV2 << " AutoCorrDecoder: " << m_AutoCorrDecoderV2.name() );
  }
  return StatusCode::SUCCESS;
}


StatusCode LArOFCAlg::stop() 
{

  ATH_MSG_DEBUG( "In LArOFCAlg finalize()");
  
  ATH_MSG_INFO( "Number of samples            : " << m_nSamples ) ;
  ATH_MSG_INFO( "Number of delays acquired    : " << m_nDelays  ) ;
  ATH_MSG_INFO( "Number of phases in OFC      : " << m_nPhases  ) ;
  ATH_MSG_INFO( "Spacing between two phases   : " << m_dPhases  ) ;

  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey};
  const LArOnOffIdMapping* cabling{*cablingHdl};
  if(!cabling) {
     ATH_MSG_ERROR("Do not have mapping object " << m_cablingKey.key());
     return StatusCode::FAILURE;
  }

  StatusCode sc;
  if (m_useDelta == 3 || m_useDeltaV2==3){
    sc = detStore()->retrieve(m_calo_dd_man); 
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "failed to CaloDetDescrManager " );
      return sc;
    }
  }

  if ( m_timeShift ) {
    if( m_timeShiftByIndex == -1 ) {
      ATH_MSG_INFO( "    Will use helper class for start time." );
    } else {
      ATH_MSG_INFO( "    Manually shifting pulses by time index " << m_timeShiftByIndex );
    }
  }
  

  if (m_larPhysWaveBinKey.size()) {
    sc=detStore()->retrieve(m_larPhysWaveBin,m_larPhysWaveBinKey);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Failed to retrieve LArOFCBinComplete object with key " << m_larPhysWaveBinKey );
      return sc;
    }
  }

  if (m_readCaliWave) {
    sc=this->initCaliWaveContainer();
  }
  else {
    sc=this->initPhysWaveContainer(cabling);
  }

  if (m_readDSPConfig) {
     const AthenaAttributeList* attrList=0;
     sc=detStore()->retrieve(attrList, m_DSPConfigFolder);
     if (sc.isFailure()) {
         ATH_MSG_ERROR( "Failed to retrieve AthenaAttributeList with key " << m_DSPConfigFolder );
         return sc;
     }

     const coral::Blob& blob = (attrList->coralList())["febdata"].data<coral::Blob>();
     if (blob.size()<3) {
        ATH_MSG_INFO( "Found empty blob, nothing to do");
     } else {
        m_DSPConfig = new LArDSPConfig(attrList);
     }
  }

  if (sc.isFailure()) return sc;

  if (m_allChannelData.size()==0) {
    ATH_MSG_ERROR( "No input waves found" );
    return StatusCode::FAILURE;
  }

  ////////////
  if (m_runThreaded) {
    //There are sone external tools, etc. that potentially cached stuff.
    //We need to call them at least once to make sure all caches are filled before we go multi-threaded
    perChannelData_t& chanData=m_allChannelData[0];

    //ToolHandle<ILArAutoCorrDecoderTool> m_AutoCorrDecoder;
    m_AutoCorrDecoder->AutoCorr(chanData.chid,(CaloGain::CaloGain)chanData.gain,m_nSamples);
    if (m_computeV2)
      m_AutoCorrDecoderV2->AutoCorr(chanData.chid,(CaloGain::CaloGain)chanData.gain,m_nSamples);
      
    Identifier id=cabling->cnvToIdentifier(chanData.chid);
    if (m_useDelta==3 || m_useDeltaV2) {
      //const CaloDetDescrManager* m_calo_dd_man;
      m_calo_dd_man->get_element(id);
    }

    //const LArOnlineID*       m_onlineID; 
    m_onlineID->isFCALchannel(chanData.chid);

    ///const LArOFCBinComplete* m_larPhysWaveBin;
    m_larPhysWaveBin->bin(chanData.chid,(CaloGain::CaloGain)chanData.gain);

    //Instanciated the functor and start parallel_for
    Looper looper(&m_allChannelData,cabling, this);
    tbb::blocked_range<size_t> range(0, m_allChannelData.size());
    ATH_MSG_INFO( "Starting parallel execution" );
    tbb::parallel_for(tbb::blocked_range<size_t>(0, m_allChannelData.size()),looper);

    ATH_MSG_INFO( "Done with parallel execution" );

  }
  else {
    ATH_MSG_INFO( "Single threaded execution" );
    for (perChannelData_t& chanData : m_allChannelData) {
      this->process(chanData,cabling);
    }
  }

  /////////////

  // OFC persistent object
  LArOFCComplete *larOFCComplete=new LArOFCComplete();
  sc = larOFCComplete->setGroupingType(m_groupingType,msg());
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Failed to set groupingType for LArOFCComplete object" );
    return sc;
  }
  sc=larOFCComplete->initialize(); 
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Failed initialize LArOFCComplete object" );
    return sc;
  }


  LArOFCComplete *larOFCCompleteV2=new LArOFCComplete();
  sc = larOFCComplete->setGroupingType(m_groupingType,msg());
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Failed to set groupingType for LArOFCComplete object" );
    return sc;
  }
  sc=larOFCCompleteV2->initialize(); 
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Failed initialize LArOFCComplete object" );
    return sc;
  }


  LArOFCBinComplete *larOFCBinComplete=NULL;
  if (m_storeMaxPhase) {
    larOFCBinComplete=new LArOFCBinComplete();
    sc=larOFCBinComplete->setGroupingType(m_groupingType,msg());
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Failed to set groupingType for LArOFCBinComplete object" );
      return sc;
    }
    sc  = larOFCBinComplete->initialize(); 
    if ( sc.isFailure() ) {
      ATH_MSG_ERROR( "Could not initialize LArOFCComplete data object - exit!" ) ;
      return sc ;
    }
  }

  // LArShape persistent object
  LArShapeComplete *larShapeComplete=NULL;
  if (m_fillShape) {
    larShapeComplete = new LArShapeComplete();
    sc=larShapeComplete->setGroupingType(m_groupingType,msg());
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Failed to set groupingType for LArShapeComplete object" );
      return sc;
    }
    sc=larShapeComplete->initialize(); 
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Failed initialize LArShapeComplete object" );
      return sc;
    }
  }


  //Counters (for information only):
  unsigned nChannels=0;
  unsigned nFailed=0;

  for (const perChannelData_t& chanData : m_allChannelData) {
    ++nChannels;
    if (chanData.faultyOFC) ++nFailed;
    // register channel to LArOFCComplete
    const HWIdentifier ch_id=chanData.chid;
    const int gain=chanData.gain;
    ATH_MSG_DEBUG( "add to LArOFCComplete, channel " << m_onlineID->channel_name(ch_id) << ", gain=" << (int)gain);
    const std::vector<std::vector<float> > & allPhaseOFCa=chanData.ofc_a;
    const std::vector<std::vector<float> > & allPhaseOFCb=chanData.ofc_b;
    const std::vector<std::vector<float> > & allPhaseShape=chanData.shape;
    const std::vector<std::vector<float> > & allPhaseShapeDer=chanData.shapeDer;
    larOFCComplete->set(ch_id,gain,allPhaseOFCa,allPhaseOFCb,chanData.tstart,chanData.timeBinWidthOFC);
    if (larOFCBinComplete) larOFCBinComplete->set(ch_id,gain,chanData.phasewMaxAt3);      
    if ( m_fillShape ) larShapeComplete->set(ch_id,gain,allPhaseShape,allPhaseShapeDer,chanData.tstart,chanData.timeBinWidthOFC);      
    
    
    if (m_computeV2) {
      const std::vector<std::vector<float> > & allPhaseOFCV2a=chanData.ofcV2_a;
      const std::vector<std::vector<float> > & allPhaseOFCV2b=chanData.ofcV2_b;
      larOFCCompleteV2->set(ch_id,gain,allPhaseOFCV2a,allPhaseOFCV2b,chanData.tstart,chanData.timeBinWidthOFC);
    }
    
  } // end loop over m_allChannelData

  ATH_MSG_INFO( " Summary : Computed OFCs for " << nChannels << " channels * gains" );
  if (nFailed) 
    ATH_MSG_ERROR( "Number of channels * gains with failed OFC verification: " <<  nFailed );

  // record and symlink LArOFCComplete object
  sc = detStore()->record(larOFCComplete,m_ofcKey);
  if (sc.isFailure()) {
      ATH_MSG_ERROR( "Could not record LArOFCComplete to DetStore with key " << m_ofcKey );
      return StatusCode::FAILURE;
  }
  ATH_MSG_INFO( "LArOFCComplete object recorded with key " << m_ofcKey ) ;

  sc = detStore()->symLink(larOFCComplete,(ILArOFC*)larOFCComplete);
  if (sc.isFailure()) {
      ATH_MSG_ERROR( "Could not symlink ILArOFC with LArOFCComplete." );
      return StatusCode::FAILURE;
  } 
  ATH_MSG_INFO( "Symlink with ILArOFC done" ) ;
  

  // record and symlink second version of LArOFCComplete object
  if (m_computeV2) {
    sc = detStore()->record(larOFCCompleteV2,m_ofcKeyV2);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Could not record LArOFCComplete to DetStore with key " << m_ofcKeyV2 );
      return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "LArOFCComplete object recorded with key " << m_ofcKeyV2 ) ;

    sc = detStore()->symLink(larOFCCompleteV2,(ILArOFC*)larOFCCompleteV2);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Could not symlink ILArOFC with LArOFCComplete." );
      return StatusCode::FAILURE;
    } 
    ATH_MSG_INFO( "Symlink with ILArOFC done" ) ;
  }

  if ( m_dumpOFCfile.size()) {
    ATH_MSG_INFO( "Dumping OFCs to file " << m_dumpOFCfile ) ;
    larOFCComplete->dumpOFC(m_dumpOFCfile) ;
  }

  if (larOFCBinComplete) {
    sc = detStore()->record(larOFCBinComplete,m_ofcBinKey);
    if (sc.isFailure()) {
       ATH_MSG_ERROR( "Could not record LArOFCBinCompete object" );
       return StatusCode::FAILURE;
    }
  }
  
  // record and symlink LArShapeComplete object
  if ( m_fillShape ) {
    ATH_MSG_DEBUG( "Trying to record LArShapeComplete object to detector store, key = " << m_shapeKey);
    sc = detStore()->record(larShapeComplete,m_shapeKey);
    if (sc.isFailure()) {
       ATH_MSG_ERROR( "Could not record LArShapeComplete to DetStore with key " << m_shapeKey );
       return StatusCode::FAILURE;
    }
    ATH_MSG_INFO( "LArShapeComplete object recorded to DetStore successfully with key " << m_shapeKey ) ;
    ATH_MSG_DEBUG( "Trying to symlink ILArShape with LArShapeComplete");
    ILArShape* larShape = 0;
    sc = detStore()->symLink(larShapeComplete,larShape);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Could not symlink ILArShape with LArShapeComplete." );
      return StatusCode::FAILURE;
    } 
    ATH_MSG_INFO( "ILArShape symlink with LArShapeComplete successfully" ) ;
  }  


  return StatusCode::SUCCESS;
}


void LArOFCAlg::process(perChannelData_t& chanData, const LArOnOffIdMapping* cabling) const {

  LArWaveHelper larWaveHelper;
  const LArWaveCumul* nextWave=chanData.inputWave;
  if (!nextWave) {
    ATH_MSG_ERROR( "input wave is 0" );
    return;
  }

  if ( nextWave->getFlag() == LArWave::dac0 ) return ; // skip dac0 waves

  const HWIdentifier ch_id=chanData.chid;
  const unsigned gain=chanData.gain;
  ATH_MSG_DEBUG( "Computing OFC for channel " << m_onlineID->channel_name(ch_id) << " in gain = " << gain);
      
      
  // check constistency of settings
  if ( m_nPoints > nextWave->getSize() ) {
    ATH_MSG_ERROR( "Channel " << m_onlineID->channel_name(ch_id) <<": Wave size (" << nextWave->getSize() 
		    << ") is too small to fit your OFC request (" << m_nPoints << " points)" ) ;
    chanData.shortWave=true;
    return;
  }

  // the current waveform
  LArWave aWave = *nextWave; // Actually *copying* the Wave to get rid of the const: need to manipulate Wave to normalize...

  if (m_larPhysWaveBin) {
    const int bin = m_larPhysWaveBin->bin(ch_id,gain);
    if (bin>-998) { //>ERRORCODE
      ATH_MSG_VERBOSE("Channel " << m_onlineID->channel_name(ch_id) << ": shift by index " << bin);
      aWave=larWaveHelper.translate(aWave,-bin,0);
    }
    else
      ATH_MSG_VERBOSE("Channel 0x" << MSG::hex << ch_id.get_identifier32().get_compact() << MSG::dec << ": No valid index for shifting");
  }//end if larPhysWaveBin


  // normalize input wave, if requested      
  if (m_normalize) {
    const double peak = aWave.getSample( larWaveHelper.getMax(aWave) );
    if ( peak == 0 ) {
      ATH_MSG_ERROR( "Wave maximum is zero, skipping channel " << m_onlineID->channel_name(ch_id) ) ;
      return;
    }

    ATH_MSG_VERBOSE("Channel 0x" << m_onlineID->channel_name(ch_id) << " has amplitude = " << peak << ": normalizing...");
    aWave = aWave * (1./peak);
  } 

  ATH_MSG_VERBOSE("Channel " << m_onlineID->channel_name(ch_id) <<  " has now amplitude = " <<  aWave.getSample( larWaveHelper.getMax(aWave)));
  // compute tstart to shift input wave, if requested
  if ( m_timeShift ) {
    if( m_timeShiftByIndex == -1 ) {
      chanData.tstart = larWaveHelper.getStart(aWave) ;
    } else {
      chanData.tstart = m_timeShiftByIndex;
    }
  }
      
  ATH_MSG_DEBUG("Channel" << m_onlineID->channel_name(ch_id) << ", Tstart = " << chanData.tstart);

  //Calculate derivative for this wave
  LArWave aDerivedWave = larWaveHelper.derive_smooth(aWave);

  chanData.timeBinWidthOFC = m_dPhases*aWave.getDt();

  float maxSampleValAt3=-1;
      
  //prepare output vectors
  chanData.ofc_a.resize(m_nPhases);
  chanData.ofc_b.resize(m_nPhases);  
  chanData.shape.resize(m_nPhases);     
  chanData.shapeDer.resize(m_nPhases);  

  if (m_computeV2) {
     chanData.ofcV2_a.resize(m_nPhases);
     chanData.ofcV2_b.resize(m_nPhases);  
  }


  const Eigen::MatrixXd acInverse=m_AutoCorrDecoder->AutoCorr(chanData.chid,(CaloGain::CaloGain)chanData.gain,m_nSamples).inverse();
  Eigen::MatrixXd acInverseV2;
  if (m_computeV2)
    acInverseV2=m_AutoCorrDecoderV2->AutoCorr(chanData.chid,(CaloGain::CaloGain)chanData.gain,m_nSamples).inverse();

  unsigned tShift=0;
  if(m_readDSPConfig && m_DSPConfig){
     if(m_DSPConfig->peakSample(m_onlineID->feb_Id(ch_id)) < 2 ) { // 2 is canonical value for peak
        tShift = 2 - m_DSPConfig->peakSample(m_onlineID->feb_Id(ch_id));
     }
  }
  if(m_forceShift) tShift=1;

  ATH_MSG_DEBUG("Channel " << m_onlineID->channel_name(ch_id) << "shift: " << tShift);

  for (unsigned iPhase=0;iPhase<m_nPhases;iPhase++) { //Loop over all phases
      	
    ATH_MSG_VERBOSE ("Channel " << m_onlineID->channel_name(ch_id) 
		     << ", Gain = " << gain << ", Phase = " << iPhase << ":");


    //Reference to the samples and deriviative to be filled
    std::vector<float>& theSamples=chanData.shape[iPhase];
    std::vector<float>& theSamplesDer=chanData.shapeDer[iPhase];

    //Extract the points where we compute the OFCs from the wave and the derived wave
    //and fill the samples an derivative vector
    theSamples.reserve(m_nSamples);
    theSamplesDer.reserve(m_nSamples);
    for (unsigned iSample=0;iSample<m_nSamples;++iSample){ //Loop over all samples
      const unsigned tbin = chanData.tstart + iPhase*m_dPhases + (iSample+tShift)*m_nDelays ;	  
      theSamples.push_back( aWave.getSample(tbin) );
      theSamplesDer.push_back( aDerivedWave.getSample(tbin) );
    } //End loop over samples


    if (m_storeMaxPhase && m_nSamples>2 && theSamples[2]>maxSampleValAt3) {
      maxSampleValAt3=theSamples[2];
      chanData.phasewMaxAt3=iPhase;
    }

    bool thisChanUseDelta=useDelta(ch_id,m_useDelta,cabling);
    bool thisChanUseDeltaV2=m_computeV2 && useDelta(ch_id,m_useDeltaV2,cabling);
    Eigen::VectorXd delta;

    if (thisChanUseDelta || thisChanUseDeltaV2) { // will need delta for at least one of the two versions
      std::vector<float> theSamples32;
      theSamples32.reserve(32);
      for (unsigned iSample=0;iSample<32 ;++iSample){ //Loop over all samples
	const unsigned tbin = chanData.tstart + iPhase*m_dPhases + (iSample+tShift)*m_nDelays ;	  
	if (tbin>=aWave.getSize()) continue;
	theSamples32.push_back( aWave.getSample(tbin) );
      } //End loop over samples
      delta=getDelta(theSamples32,ch_id,m_nSamples);
    }

    //OFC V1 computiation (i.e. not pileup-optimized)
    //Reference to the OFCa and OFCb to be filled
    std::vector<float>& vOFC_a= chanData.ofc_a[iPhase];
    std::vector<float>& vOFC_b= chanData.ofc_b[iPhase];

    if (thisChanUseDelta) {      
      optFiltDelta(theSamples,theSamplesDer,acInverse,delta,vOFC_a,vOFC_b);  
    } 
    else { //don't use Delta
      optFilt(theSamples,theSamplesDer,acInverse,vOFC_a,vOFC_b);
    }
    
    // verify OFC consistency
    if (m_verify) {
      chanData.faultyOFC |= verify(chanData.chid,vOFC_a,vOFC_b,theSamples,"OFC",iPhase);
    } 
	

    if (m_computeV2) {
      //OFC V2 computiation (i.e. pileup-optimized)
      //Reference to the OFCa and OFCb to be filled
      std::vector<float>& vOFCV2_a= chanData.ofcV2_a[iPhase];
      std::vector<float>& vOFCV2_b= chanData.ofcV2_b[iPhase];
      
      if (thisChanUseDeltaV2) {
	optFiltDelta(theSamples,theSamplesDer,acInverseV2,delta,vOFCV2_a,vOFCV2_b);
      } 
      else { //don't use Delta
	optFilt(theSamples,theSamplesDer,acInverseV2,vOFCV2_a,vOFCV2_b);
      }

      // verify OFC consistency
      if (m_verify) {
	chanData.faultyOFC |= verify(chanData.chid,vOFCV2_a,vOFCV2_b,theSamples,"OFCV2",iPhase);
      } 
    }//end if computeV2
  } //End loop over all phases

  // in case we're dealing with a LArPhysWave, add any possible previous time shift
  if (!m_readCaliWave) {
    const LArPhysWave* pwave=dynamic_cast<const LArPhysWave*>(nextWave);
    if (pwave)
      chanData.tstart += pwave->getTimeOffset()+m_addOffset;
  }
  return;
}




StatusCode LArOFCAlg::initPhysWaveContainer(const LArOnOffIdMapping* cabling) {

  typedef LArPhysWaveContainer::ConstConditionsMapIterator WAVEIT;

  for (unsigned k=0 ; k<m_keylist.size() ; k++ ) { // Loop over all containers that are to be processed (e.g. different gains)
    ATH_MSG_INFO( "Processing WaveContainer from StoreGate! key = " << m_keylist[k] );

    const LArPhysWaveContainer* waveCnt;
    StatusCode sc=detStore()->retrieve(waveCnt,m_keylist[k]);
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Failed to retrieve a LArPhysWaveContainer with key " << m_keylist[k] );
      return sc;
    }
      
    m_allChannelData.reserve(m_allChannelData.size()+128*waveCnt->size());// Size doesn't give the expected response on a CondtitionsContainer 

    for (unsigned gain = CaloGain::LARHIGHGAIN ; gain < CaloGain::LARNGAIN ; gain++ ) { // loop on possible gains
      WAVEIT it=waveCnt->begin(gain);
      WAVEIT it_e=waveCnt->end(gain);
      for (;it!=it_e;++it) {
	const HWIdentifier chid=it.channelId();
	if (cabling->isOnlineConnected (chid)){
	  const LArWaveCumul* wave= &(*it); //down-cast
	  if (!wave->isEmpty()) {
	    m_allChannelData.emplace_back(wave, chid,gain);
	  }
	}
	//std::cout << "Got wave size=" << wave->getSize() <<", id=" << it.channelId().get_identifier32().get_compact() << ", gain=" << gain << std::endl;
      } //end loop over channels
    }//end loop over gains
  }//end loop over SG keys
  return StatusCode::SUCCESS;
}   


StatusCode LArOFCAlg::initCaliWaveContainer() {

  typedef LArCaliWaveContainer::ConstConditionsMapIterator WAVEIT;
  for (unsigned k=0 ; k<m_keylist.size() ; k++ ) { // Loop over all containers that are to be processed (e.g. different gains)
    ATH_MSG_INFO( "Processing WaveContainer from StoreGate! key = " << m_keylist[k] );

    //Input cali-wave might come from the same job. In this case we see a non-const container in SG probably w/o corrections applied.
    //Try non-const retrieve:
    const LArCaliWaveContainer* waveCnt = nullptr;
    LArCaliWaveContainer* waveCnt_nc=detStore()->tryRetrieve<LArCaliWaveContainer>(m_keylist[k]);
    if (waveCnt_nc) {
      waveCnt=waveCnt_nc; //Retain const pointer
      if (!waveCnt_nc->correctionsApplied()) {
	ATH_MSG_INFO( "LArCaliWaveContainer: Corrections not yet applied, applying them now..." );
	if (waveCnt_nc->applyCorrections().isFailure()) {
	  ATH_MSG_ERROR( "Failed to apply corrections to LArCaliWaveContainer!" );
	  return StatusCode::FAILURE;
	}
      }
    }
    else {
      waveCnt=detStore()->tryConstRetrieve<LArCaliWaveContainer>(m_keylist[k]);
      if (!waveCnt) {
	ATH_MSG_ERROR( "Failed to retrieve a LArCaliWaveContainer with key " << m_keylist[k] );
	return  StatusCode::FAILURE;
      }
    }   
   

    m_allChannelData.reserve(m_allChannelData.size()+128*waveCnt->size());// Size doesn't give the expected response on a CondtitionsContainer 

    for (unsigned gain = CaloGain::LARHIGHGAIN ; gain < CaloGain::LARNGAIN ; gain++ ) { // loop on possible gains
      WAVEIT it=waveCnt->begin(gain);
      WAVEIT it_e=waveCnt->end(gain);
      for (;it!=it_e;++it) {
	const LArCaliWaveVec& wVec=*it;
	for (const auto& cw : wVec) {
	  const LArWaveCumul* wave= &(cw); //down-cast
	  if (!wave->isEmpty()) {
	    m_allChannelData.emplace_back(wave,it.channelId(),gain);
	  }
	}
      } //end loop over channels
    }//end loop over gains
  }//end loop over SG keys
  return StatusCode::SUCCESS;
}


void  LArOFCAlg::optFilt(const std::vector<float> &gWave, const std::vector<float>  &gDerivWave, const Eigen::MatrixXd& acInverse, //input variables
			 std::vector<float>& vecOFCa, std::vector<float>& vecOFCb) const { // Output variables;
  assert(gWave.size()==gDerivWave.size());
  //assert autoCorr size ....
  const int optNpt = gWave.size();
  
  Eigen::VectorXd gResp(optNpt), gDerivResp(optNpt);
  for (int i=0;i<optNpt;i++) {
    gResp[i] = gWave[i];
    gDerivResp[i] = gDerivWave[i];
  }

  Eigen::Matrix2d isol;
  isol << 
    (gResp.transpose()*acInverse*gResp)[0],
    (gResp.transpose()*acInverse*gDerivResp)[0],
    (gDerivResp.transpose()*acInverse*gResp)[0],
    (gDerivResp.transpose()*acInverse*gDerivResp)[0];

  Eigen::Vector2d Amp; 
  Eigen::Vector2d Atau;
  Eigen::Vector2d Ktemp;
  Eigen::Matrix2d isolInv = isol.inverse();

  //Output vector (eigen version)
  //Eigen::VectorXd(m_optNpt) a;
  //Eigen::VectorXd(m_optNpt) b;

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
  Eigen::VectorXd OFCa = Amp[0]*acInverse*gResp + Amp[1]*acInverse*gDerivResp;
  Eigen::VectorXd OFCb = Atau[0]*acInverse*gResp + Atau[1]*acInverse*gDerivResp;
  
  //Convert back to std::vector
  vecOFCa.resize(optNpt);
  vecOFCb.resize(optNpt);
  for (int i=0;i<optNpt;i++) {
    vecOFCa[i]=OFCa[i];
    vecOFCb[i]=OFCb[i];
  }
  return;
}






void  LArOFCAlg::optFiltDelta(const std::vector<float> &gWave, const std::vector<float>  &gDerivWave, 
			      const Eigen::MatrixXd& acInverse, const Eigen::VectorXd& delta, 
			      std::vector<float>& vecOFCa, std::vector<float>& vecOFCb) const{ 

  

  assert(gWave.size()==gDerivWave.size());
  //assert autoCorr size ....
  const int optNpt = gWave.size();
  
  Eigen::VectorXd gResp(optNpt), gDerivResp(optNpt);
  for (int i=0;i<optNpt;i++) {
    gResp[i] = gWave[i];
    gDerivResp[i] = gDerivWave[i];
  }

  // try 3X3 matrix with offsets

  
  /**
  HepVector delta(5) ; 
  
  delta[0]=3.79198742146;
  delta[1]=-4.84165741965;
  delta[2]=3.20189843985;
  delta[3]=-5.90850592618;
  delta[4]=1.8260451328;
  */
  Eigen::Matrix3d isol;
  isol << 
    (gResp.transpose()*acInverse*gResp)[0],
    (gResp.transpose()*acInverse*gDerivResp),
    (gResp.transpose()*acInverse*delta)[0],
    
    (gDerivResp.transpose()*acInverse*gResp)[0],
    (gDerivResp.transpose()*acInverse*gDerivResp)[0],
    (gDerivResp.transpose()*acInverse*delta)[0],
    
    (delta.transpose()*acInverse*gResp)[0],
    (delta.transpose()*acInverse*gDerivResp)[0],
    (delta.transpose()*acInverse*delta)[0];
  

  Eigen::Vector3d Amp; 
  Eigen::Vector3d Atau;
  Eigen::Vector3d Ktemp;
  Eigen::Matrix3d isolInv = isol.inverse();

  //  we solve for the lagrange multiplers

  Ktemp[0] = 1.;
  Ktemp[1] = 0.;
  Ktemp[2] = 0.;

  Amp = isolInv*Ktemp;
    
  Ktemp[0] = 0.; 
  Ktemp[1] = -1.;
  Atau = isolInv*Ktemp;

  // we express the a and b vectors in terms of the lagrange multipliers
  Eigen::VectorXd OFCa = Amp[0]*acInverse*gResp + Amp[1]*acInverse*gDerivResp + Amp[2]*acInverse * delta;
  Eigen::VectorXd OFCb = Atau[0]*acInverse*gResp + Atau[1]*acInverse*gDerivResp + Atau[2]*acInverse * delta  ;


  //Convert back to std::vector
  vecOFCa.resize(optNpt);
  vecOFCb.resize(optNpt);
  for (int i=0;i<optNpt;i++) {
    vecOFCa[i]=OFCa[i];
    vecOFCb[i]=OFCb[i];
  }
  return;
}



const float LArOFCAlg::m_fcal3Delta[5]  ={0.0790199937765,  0.0952000226825,  0.0790199937765,  0.0952000226825,  0.0790199937765};
const float LArOFCAlg::m_fcal2Delta[5]={-0.01589001104,  -0.0740399733186, -0.01589001104,   -0.0740399733186, -0.01589001104};
const float LArOFCAlg::m_fcal1Delta[5] ={0.0679600232979, -0.139479996869,   0.0679600232979, -0.139479996869,   0.0679600232979};

Eigen::VectorXd LArOFCAlg::getDelta(std::vector<float>& samples, const HWIdentifier chid, unsigned nSamples) const{

  if (nSamples>5) nSamples=5;

  Eigen::VectorXd delta(nSamples); //return value

  if (m_onlineID->isFCALchannel(chid) ){
    // FCAL use fixed delta from data. 
    const int slot = m_onlineID->slot(chid) ; 
    if ( slot <=9){ // FCAL 1
      for (unsigned i=0;i<nSamples;++i) {
	delta[i]=m_fcal1Delta[i];
      }
    }else
      if(slot <=13){ // FCAL 2
	for (unsigned i=0;i<nSamples;++i) {
	  delta[i]=m_fcal2Delta[i];
	}
      }else {  //FCAL 3 
	for (unsigned i=0;i<nSamples;++i) {
	  delta[i]=m_fcal3Delta[i];
	}
      }

  }else
    { // from Shape 
      float odd = 0.;
      float even = 0.;
      for (unsigned int i = 0;i<samples.size();++i) {
	  if (i%2==0){
	    even  += samples[i];
	  }
	  else {
	    odd += samples[i];
	  }
	}

      for (unsigned i=0;i<nSamples;++i) {
	if (i%2==0)
	  delta[i]=even;
	else
	  delta[i]=odd;
      }
    }

  return delta;

}

 bool LArOFCAlg::useDelta(const HWIdentifier chid, const int jobOFlag, const LArOnOffIdMapping* cabling) const {

  if (jobOFlag==2){
      return true;
    }

  if(jobOFlag==1) { // only HEC/EMECIW/FCAL
    if (m_onlineID->isEMECIW(chid) || m_onlineID->isFCALchannel(chid) || m_onlineID->isHECchannel(chid)) {
      return true;
    }
  }

  if(jobOFlag==3) { // only HEC/EMECIW/FCAL1 and high eta FCAL2-3 
    if (m_onlineID->isEMECIW(chid) ||  m_onlineID->isHECchannel(chid)) {
      return true;
    }
    else if (m_onlineID->isFCALchannel(chid) ){

      if (cabling->isOnlineConnected (chid)){
	Identifier ofl_id = cabling->cnvToIdentifier(chid);
	const CaloDetDescrElement* dde = m_calo_dd_man->get_element(ofl_id);
	if (! dde) {
	  ATH_MSG_ERROR( " dde = 0 , onl_id, ofl_id= "<< chid<<" "<<ofl_id );
	  return false; // Exception better? 
	}
	CaloCell_ID::CaloSample sampling = dde->getSampling();
	if (sampling==CaloCell_ID::FCAL0){
	  return true;
	}
	else {
	  if (fabs(dde->eta())>4.0){
	   return true;
	  }
	}
	
      }//end if connected
    }//end if isFCALchannel

  }//else if jobOFlag=3

  return false;
}



bool LArOFCAlg::verify(const HWIdentifier chid, const std::vector<float>& OFCa, const std::vector<float>& OFCb, 
		       const std::vector<float>& Shape, const char* ofcversion, const unsigned phase) const {

  bool result=false;
  float recAmpl=0, recTime=0;
  for (unsigned iSample=0;iSample<m_nSamples;++iSample){

#ifdef LAROFCALG_DEBUGOUTPUT
    ATH_MSG_VERBOSE("a["<<iSample<<"]="<<vOFC_a[iSample] << " b["<<iSample<<"]="<<vOFC_b[iSample] 
		    << " Sample=" << aWave.getSample(tbin));
#endif
    recAmpl += OFCa[iSample] * Shape[iSample];
    recTime += OFCb[iSample] * Shape[iSample];
  } //End loop over samples

  recTime /= recAmpl ;

  // At this point the reconstructed amplitude must be = 1 by definition, whatever the initial normalisation!
  ATH_MSG_VERBOSE("recAmp=" << recAmpl << " ; recTime=" << recTime);
  if ( fabs(1.-recAmpl) > m_errAmpl ) {
    ATH_MSG_WARNING( "Applying phase " << phase << " of " << ofcversion << " to original wave yields an Amplitude of "<< recAmpl 
		      << " instead of 1. -> Wrong OFCs? channel " << m_onlineID->channel_name(chid) );
    this->printOFCVec(OFCa,msg());
    result=true;
  }
  if ( fabs(recTime) > m_errTime ) {
    ATH_MSG_WARNING( "Applying  phase " << phase << " of " << ofcversion << " to original wave yields a time offset of " << recTime 
		       << " -> Wrong OFCs? channel " << m_onlineID->channel_name(chid) );
    this->printOFCVec(OFCb,msg());
    result=true; 
  }
  return result;
}

void LArOFCAlg::printOFCVec(const std::vector<float>& vec, MsgStream& mLog) const {
  mLog << MSG::WARNING << "OFCs";
    for(float v : vec) 
      mLog << " " << v;
  mLog << endmsg;
  return;
}


#ifdef LAROFCALG_DEBUGOUTPUT
#undef LAROFCALG_DEBUGOUTPUT
#endif
