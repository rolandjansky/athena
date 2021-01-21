/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     LArDigitMonALg.cxx
// PACKAGE:  LArMonitoring
//
// AUTHOR:   Pavol Strizenec (pavol@mail.cern.ch)
//           Based on LAtDigitMon tool by L. Hellary and LArOddCellsMonTool.cxx  by Benjamin Trocme
// 
// Monitor a few things in the LArDigit...
//
//	1) Check that the highest value of the LArDigit is contained in an interval. 
//         If it is not the case increment 3 histograms for each subdetector:
//	      a) Out of range histograms
//	      b) The average histograms: give the average value of the highest digit sample 
//	      c) Channel VS FEB histograms: gives wich slot on wich FEB has his highest digit sample ou of the range
//	2) Check if a digits samples are in saturation. If it's the case increment the saturation histograms. 
//
// Available cuts in the jo file:
//
//   a) SampleRangeLow-SampleRangeUp: range to check the digit sample.
//   b) ADCcut : To select Digits Samples with signal.
//   c) ADCsature: lowest value to check if a Digit sample is in saturation.
// ********************************************************************

#include "LArDigitMonAlg.h"

//Histograms
//LAr infos:
#include "Identifier/HWIdentifier.h"
#include "LArIdentifier/LArOnlineID.h"
#include "LArRawEvent/LArDigit.h"

#include "LArTrigStreamMatching.h"

//STL:
#include <sstream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <algorithm>

/*---------------------------------------------------------*/
LArDigitMonAlg::LArDigitMonAlg(const std::string& name, ISvcLocator* pSvcLocator )
  : AthMonitorAlgorithm(name, pSvcLocator),
    m_badChannelMask("BadLArRawChannelMask",this),
    m_LArOnlineIDHelper(0),
    m_LArEM_IDHelper(0),
    m_Samplenbr(-1)
{	
  declareProperty("LArBadChannelMask",m_badChannelMask);
  
}

/*---------------------------------------------------------*/
LArDigitMonAlg::~LArDigitMonAlg()
{
}

/*---------------------------------------------------------*/
StatusCode 
LArDigitMonAlg::initialize()
{
  
  ATH_MSG_INFO( "Initialize LArDigitMonAlg" );
  
  
  /** Get LAr Online Id Helper*/
  if ( detStore()->retrieve( m_LArOnlineIDHelper, "LArOnlineID" ).isSuccess() ) {
    
    ATH_MSG_DEBUG("connected non-tool: LArOnlineID" );
    
  } else {
    
    ATH_MSG_FATAL( "unable to connect non-tool: LArOnlineID" );
    return StatusCode::FAILURE;
    
  }
  
  
  // Get LArEM Id Helper, not used now...
  if ( detStore()->retrieve( m_LArEM_IDHelper, "LArEM_ID" ).isSuccess() ) {
    
    ATH_MSG_DEBUG("connected non-tool: LArEM_ID" );
    
  } else {
    
    ATH_MSG_FATAL( "unable to connect non-tool: LArEM_ID" );
    return StatusCode::FAILURE;
    
  }

  ATH_CHECK(m_digitContainerKey.initialize());  
  ATH_CHECK(m_eventInfoKey.initialize());
  ATH_CHECK(m_keyPedestal.initialize());
  ATH_CHECK(m_cablingKey.initialize());
  ATH_CHECK(m_noisyROSummaryKey.initialize());

  

  /** Get bad-channel mask (only if jO IgnoreBadChannels is true)*/
  if (m_ignoreKnownBadChannels) { 
    StatusCode sc=m_badChannelMask.retrieve();
    if (sc.isFailure()) {
      ATH_MSG_ERROR( "Could not retrieve BadChannelMask" << m_badChannelMask);
      return StatusCode::FAILURE;
    }
  } else {
    m_badChannelMask.disable();
  }

  m_histoGroups.reserve(m_SubDetNames.size());
  for (unsigned i=0; i<m_SubDetNames.size(); ++i) {
    std::vector<std::string> part;
    part.push_back(m_partitions[2*i]);
    part.push_back(m_partitions[2*i+1]);
    m_histoGroups.push_back(Monitored::buildToolMap<int>(m_tools,m_SubDetNames[i],part));
  }
  
  return AthMonitorAlgorithm::initialize();
}

StatusCode LArDigitMonAlg::fillHistograms(const EventContext& ctx) const
{
  
  ATH_MSG_DEBUG("in fillHists()" );
  
  
  
  // retrieve LArNoisyROSummary and skip the event if number of FEB is greater than the one declare in JO.
  SG::ReadHandle<LArNoisyROSummary> noisyRO{m_noisyROSummaryKey, ctx};

  if(noisyRO.isValid()) {
     const std::vector<HWIdentifier>& noisyFEB = noisyRO->get_noisy_febs();
     if(int(noisyFEB.size())>m_NumberBadFebs) {
       ATH_MSG_DEBUG("Skip this Noisy event ");
       return StatusCode::SUCCESS;
     }
  }
  
  //monitored variables
  auto weight = Monitored::Scalar<float>("weight",1.);
  auto partition = Monitored::Scalar<int>("partition",-1);
  auto gain = Monitored::Scalar<int>("gain",-1);
  auto sumbin = Monitored::Scalar<int>("sumbin",-1);
  auto slot_o = Monitored::Scalar<int>("Outslot",-1);
  auto ft_o = Monitored::Scalar<int>("OutFT",-1);
  auto weight_o = Monitored::Scalar<float>("Outweight",1.);
  auto slot_n = Monitored::Scalar<int>("Nullslot",-1);
  auto ft_n = Monitored::Scalar<int>("NullFT",-1);
  auto weight_n = Monitored::Scalar<float>("Nullweight",1.);
  auto slot_s = Monitored::Scalar<int>("Saturslot",-1);
  auto ft_s = Monitored::Scalar<int>("SaturFT",-1);
  auto weight_s = Monitored::Scalar<float>("Saturweight",1.);
  auto slot_sl = Monitored::Scalar<int>("SaturLowslot",-1);
  auto ft_sl = Monitored::Scalar<int>("SaturLowFT",-1);
  auto weight_sl = Monitored::Scalar<float>("SaturLowweight",1.);
  auto crate_o = Monitored::Scalar<int>("Outcrate",-1);
  auto chan_o = Monitored::Scalar<int>("Outchan",-1);
  auto crate_n = Monitored::Scalar<int>("Nullcrate",-1);
  auto chan_n = Monitored::Scalar<int>("Nullchan",-1);
  auto crate_s = Monitored::Scalar<int>("Saturcrate",-1);
  auto chan_s = Monitored::Scalar<int>("Saturchan",-1);
  auto crate_sl = Monitored::Scalar<int>("SaturLowcrate",-1);
  auto chan_sl = Monitored::Scalar<int>("SaturLowchan",-1);


  auto LBN = Monitored::Scalar<int>("LBN",-1);
  auto slot = Monitored::Scalar<int>("slot",-1);
  auto ft = Monitored::Scalar<int>("FT",-1);
  auto maxpos = Monitored::Scalar<int>("MaxPos",-1);
  auto l1Trig = Monitored::Scalar<int>("l1trig",-1);
  auto energy = Monitored::Scalar<int>("Energy",-1);
  auto sample = Monitored::Scalar<int>("Sample",-1);
  auto signal = Monitored::Scalar<float>("SignalNorm",-1);

  /**EventID is a part of EventInfo, search event informations:*/
  SG::ReadHandle<xAOD::EventInfo> thisEvent{m_eventInfoKey,ctx};

  unsigned thisl1Trig = thisEvent->level1TriggerType();
  unsigned thisLBN = thisEvent->lumiBlock();
    
  LBN=thisLBN;
  fill(m_summaryMonGroupName,LBN);

  const std::vector<unsigned> streamsThisEvent=LArMon::trigStreamMatching(m_streams,thisEvent->streamTags());
  
  SG::ReadCondHandle<ILArPedestal> pedestalHdl{m_keyPedestal,ctx};
  const ILArPedestal* pedestals=*pedestalHdl;

  SG::ReadCondHandle<LArOnOffIdMapping> cablingHdl{m_cablingKey,ctx};
  const LArOnOffIdMapping* cabling=*cablingHdl;


  SG::ReadHandle<LArDigitContainer> pLArDigitContainer{m_digitContainerKey,ctx};
  
  /** Define iterators to loop over Digits containers*/
  LArDigitContainer::const_iterator itDig = pLArDigitContainer->begin(); 
  LArDigitContainer::const_iterator itDig_e= pLArDigitContainer->end(); 
  const LArDigit* pLArDigit;
  
  
  
  /** First event : define range to look for max sample. 
      If not set by user, depends on number of samples*/
  
  
  if(m_Samplenbr < 0) {	// get it from somewhere
    std::lock_guard<std::mutex> lock(m_lock);

    if(m_SampleNumberFromDB!=0) { // from jO
      m_Samplenbr=m_SampleNumberFromDB;
      ATH_MSG_DEBUG("Retrieve nSamples from jO" );
    } else { // from data
      if (itDig!=itDig_e) {
	pLArDigit = *itDig;
	m_Samplenbr=pLArDigit->nsamples();      
	ATH_MSG_DEBUG("Retrieve nSamples from first event" );
      }
    }
  }
    
  if((m_AskedSampleRangeLow < 0) && (m_AskedSampleRangeUp < 0)) { // detect automatically
      if(m_ExpectedSampleMax!=0) { // set from expected max sample
	m_SampleRangeLow=m_ExpectedSampleMax-1;
	m_SampleRangeUp=m_ExpectedSampleMax+1;
	ATH_MSG_DEBUG("Set Range number from expected Max sample" );
      } else {
	ATH_MSG_DEBUG("Set range by hand " );
	/** Set range to look for the maximum*/
	if(m_Samplenbr == 5) {
	  m_SampleRangeLow=2;
	  m_SampleRangeUp=4;	  
	}
	
	if(m_Samplenbr == 7) {
	  m_SampleRangeLow=3;//nota has to be checked in real run, none are available now...
	  m_SampleRangeUp=5;
	}
	
	if(m_Samplenbr == 10) {
	  m_SampleRangeLow=4;
	  m_SampleRangeUp=6;
	}
	
	if(m_Samplenbr == 32) {
	  m_SampleRangeLow=5;
	  m_SampleRangeUp=9;
	}
	
	/** Now if we have an exotic configuration...*/
	if(m_Samplenbr < 10&&m_Samplenbr!=5&&m_Samplenbr!=7) {
	  m_SampleRangeLow=2;//default is 4,7
	  m_SampleRangeUp=4;
	}
	
	if(m_Samplenbr <= 20&&m_Samplenbr>10) {
	  m_SampleRangeLow=4;//default is 4,7
	  m_SampleRangeUp=7;
	}
	
	if(m_Samplenbr > 20&&m_Samplenbr!=32) {
	  m_SampleRangeLow=5;//default is 4,7
	  m_SampleRangeUp=9;
	}
      } 
  } else {
     ATH_MSG_DEBUG("Range has been set by user" );
     m_SampleRangeLow=m_AskedSampleRangeLow;
     m_SampleRangeUp=m_AskedSampleRangeUp;
  }//Range and sample max are known now....
  
  
  ATH_MSG_DEBUG("Range set to "<< m_SampleRangeLow << " - " << m_SampleRangeUp );
  
  /** Loop over digits*/
  for ( ; itDig!=itDig_e;++itDig) {
    pLArDigit = *itDig;
    
    
    /** Remove problematic channels*/
    if (m_ignoreKnownBadChannels ) {
      HWIdentifier id = pLArDigit->hardwareID();
      //CaloGain::CaloGain gain = pLArDigit->gain();
      if ( m_badChannelMask->cellShouldBeMasked(id)) {
	continue;
      }
    }

    /** Fill the gain*/
    int cgain = pLArDigit->gain();
    gain=cgain;
    fill(m_summaryMonGroupName, gain, partition);
    
    /** Retrieve pedestals */
    HWIdentifier id = pLArDigit->hardwareID();
    float pedestal = pedestals->pedestal(id,cgain);
    float pedestalRMS=pedestals->pedestalRMS(id,cgain);
    
    /**skip cells with no pedestals reference in db.*/
    if(pedestal <= (1.0+LArElecCalib::ERRORCODE)) continue;
    
    /**skip disconnected channels:*/
    if(!cabling->isOnlineConnected(id)) continue;
    
    /** Determine to which partition this channel belongs to*/
    int side = m_LArOnlineIDHelper->pos_neg(id);
    const int ThisPartition=WhatPartition(id,side);
    std::string spart = m_partitions[ThisPartition];
    partition = ThisPartition;
    int subdet = ThisPartition / 2;

    /** Retrieve once, all the cell info:*/
    int channel = m_LArOnlineIDHelper->channel(id);
    int thisSlot = m_LArOnlineIDHelper->slot(m_LArOnlineIDHelper->feb_Id(id));
    int feedthrough = m_LArOnlineIDHelper->feedthrough(m_LArOnlineIDHelper->feedthrough_Id(id));
    int crate=thisSlot + m_Nslots[ThisPartition]*feedthrough;

    /** Retrieve samples*/
    const std::vector<short>* digito = &pLArDigit->samples();
    
    /**retrieve the max sample digit ie digitot.back().*/
    std::vector<short>::const_iterator maxSam = std::max_element(digito->begin(), digito->end());
    std::vector<short>::const_iterator minSam = std::min_element(digito->begin(), digito->end());
    int thismaxPos=(maxSam-digito->begin());
    std::vector<short>::const_iterator iterSam= digito->begin();
    
    /** if event pass the sigma cut:*/
    if((*maxSam-pedestal)>pedestalRMS*m_SigmaCut)
    {
      /** Loop over the samples and get signal shape:*/
      for(int i=0;i<pLArDigit->nsamples();i++,iterSam++){
         sample = i;
         signal= (*iterSam-pedestal)/(*maxSam-pedestal);
         weight = (*maxSam-pedestal);
         fill(m_tools[m_histoGroups.at(subdet).at(spart)],sample,signal,weight);
      }
      
      /** Then fill histo about max sample:*/
      slot = thisSlot;
      ft = feedthrough;
      maxpos=thismaxPos;
      energy=(*maxSam-pedestal);
      l1Trig=thisl1Trig; 
      fill(m_tools[m_histoGroups.at(subdet).at(spart)],slot,ft,LBN,maxpos,energy,l1Trig);
      
      /** fill histo out of range:*/
      if(!(thismaxPos>=m_SampleRangeLow&&thismaxPos<=m_SampleRangeUp)){
         sumbin=0;
         slot_o = thisSlot;
         ft_o = feedthrough;
         weight_o=100./128;
         crate_o=crate;
         chan_o=channel;
         fill(m_tools[m_histoGroups.at(subdet).at(spart)],slot_o,ft_o,weight_o,crate_o,chan_o);
      }
    }
    
    /**Second monitoring of the saturating cells.*/
    if ((*maxSam)>=m_ADCsatureCut) {
       if(gain==0 || gain==1){
          slot_s = thisSlot;
          ft_s = feedthrough;
          weight_s=100./128;
          crate_s=crate;
          chan_s=channel;
          fill(m_tools[m_histoGroups.at(subdet).at(spart)],slot_s,ft_s,weight_s,crate_s,chan_s);
       } else {
          slot_sl = thisSlot;
          ft_sl = feedthrough;
          weight_sl=100./128;
          crate_sl=crate;
          chan_sl=channel;
          fill(m_tools[m_histoGroups.at(subdet).at(spart)],slot_sl,ft_sl,weight_sl,crate_sl,chan_sl);
       }
    } 
    /** monitoring cells containing one sample==0.*/
    if((*minSam)==0){
       sumbin=2;
       fill(m_summaryMonGroupName, sumbin, partition); 
       slot_n = thisSlot;
       ft_n = feedthrough;
       weight_n=100./128;
       crate_n=crate;
       chan_n=channel;
       fill(m_tools[m_histoGroups.at(subdet).at(spart)],slot_n,ft_n,weight_n,crate_n,chan_n);
    }
    
  }/** End of loop on LArDigit*/
  
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
/** Say which partition is a channel*/

int LArDigitMonAlg::WhatPartition(HWIdentifier id, int side) const
{
  
  if (m_LArOnlineIDHelper->isEmBarrelOnline(id)) {
    if(side==0) return 0;
    else return 1;
  } else if (m_LArOnlineIDHelper-> isEMECchannel(id)) {
    if(side==0) return 2;
    else return 3;
  } else if (m_LArOnlineIDHelper->isHECchannel(id)) {
    if(side==0) return 4;
    else return 5;
  } else {
    if(side==0) return 6;
    else return 7;
  }
}
