/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
// NAME:     LArFEBMon.cxx
//
// AUTHORS:   R. Kehoe   10/15/2005
//            Andy Haas  12/25/2005
//            R. Kehoe   06/10/2006
//            P. Krieger 06/10/2006
// 2007- : Algorithm fully rewritten and optimized by B.Trocme
// ********************************************************************

//#include "GaudiKernel/AlgFactory.h"
//#include "GaudiKernel/SmartDataPtr.h"
//#include "GaudiKernel/IToolSvc.h"

//#include "StoreGate/DataHandle.h"

#include "LArRecEvent/LArEventBitInfo.h"
#include "LArRawEvent/LArFebHeaderContainer.h"
#include "LArRawEvent/LArFebErrorSummary.h"

#include "LArRawConditions/LArDSPThresholdsComplete.h"
#include "AthenaPoolUtilities/AthenaAttributeList.h"
#include "LArCOOLConditions/LArDSPThresholdsFlat.h"

#include "TTree.h"
#include "LWHists/TH1I_LW.h"
#include "LWHists/TH2I_LW.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TProfile2D_LW.h"
#include "LWHists/TProfile_LW.h"

#include "LArMonTools/LArFEBMon.h"

#include <stdint.h>
#include <algorithm>
#include <math.h>
//#include <functional>
//#include <set>

#include <sys/types.h>

#include "xAODEventInfo/EventInfo.h"
#include "LArTrigStreamMatching.h"

const unsigned nFEBnominal=1524;

// ********************************************************************
LArFEBMon::LArFEBMon(const std::string& type, 
			       const std::string& name,
			       const IInterface* parent) 
  : ManagedMonitorToolBase(type, name, parent), 
    m_eventRejected(false),
    m_currentFebStatus(false),
    m_eventTime(0),
    m_eventTime_ns(0),
    m_l1Trig(0),
    m_strHelper(0),
    m_trigDec("Trig::TrigDecisionTool/TrigDecisionTool"),
    m_trigok(false),
    m_barrelCSummary(),
    m_barrelASummary(),
    m_emecCSummary(),
    m_emecASummary(),
    m_hecCSummary(),
    m_hecASummary(),
    m_fcalCSummary(),
    m_fcalASummary(),
    m_febInError(),
    m_anyfebIE(false) 
{
  declareProperty("IgnoreMissingHeaderEMB",m_ignoreMissingHeaderEMB = false);
  declareProperty("IgnoreMissingHeaderPS",m_ignoreMissingHeaderPS = false);
  declareProperty("keyDSPThresholds",m_keyDSPThresholds="LArDSPThresholds");
  declareProperty("LArBadChannelTool",m_badChannelTool);
  /** Give the name of the streams you want to monitor, if empty, only simple profile per partition (offline case):*/
  declareProperty("Streams",m_streams);
  declareProperty("ExcludeCosmicCalo",m_excoscalo);
 
  declareProperty("IsOnline",m_isOnline=true);

  declareProperty("m_lumi_blocks", m_lumi_blocks = 3000 );
  
  m_onlineHelper	= NULL;
  m_strHelper		= NULL;

  m_eventsCounter = 0;
  
  for (unsigned i = 0;i < nFEBnominal; i++) {
    m_febInError[i] = NULL;
    m_bfebIE[i]     = false;
  }
  
  m_FEBmin	= -20.5;
  m_FEBmax	= 1599.5;
  m_FEBnbins	= 1620;
  
  m_LArAllErrors_dE	= NULL;
  m_rejectedYield	= NULL;
  m_rejectedHisto	= NULL;
  m_rejectedLBProfile	= NULL;
  m_rejBitsHisto	= NULL;
  
  m_eventsLB		= NULL;
  m_rejectedYieldLB	= NULL;
  m_rejectedYieldLBout	= NULL;
  m_eventSizeLB		= NULL;
  m_nbOfEvts2D		= NULL;
  m_eventType		= NULL;
  m_nbOfSamples		= NULL;
  m_totNbSw2		= NULL;
  m_dspThresholds_ADC	= NULL;
  m_dspThresholds_qtime = NULL;
  
  m_hTriggerType	= NULL;
  m_hTriggerTypeAllDSP	= NULL;
  m_hTimeDistrib	= NULL;
  m_nbOfFebBlocksTotal	= NULL;
  
  m_CorruptTree		= NULL;
  m_stream_eventSizeLB  = NULL;

  //m_useLumi = false; // Turn off lumi-feature of base-class
}

// ********************************************************************
LArFEBMon::~LArFEBMon() {
  if( m_strHelper != NULL ) delete m_strHelper;
}

// ********************************************************************
StatusCode LArFEBMon::initialize() {
  
  ATH_MSG_INFO( "Initializing LArFEBMon " );
  
  StatusCode sc = detStore()->retrieve(m_onlineHelper, "LArOnlineID");
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Could not get LArOnlineID helper !" );
    return StatusCode::FAILURE;
  }
  
  m_strHelper = new  LArOnlineIDStrHelper(m_onlineHelper);
  m_strHelper->setDefaultNameType(LArOnlineIDStrHelper::LARONLINEID);
  
  // Get BadChannelTool
  sc=m_badChannelTool.retrieve();
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Could not retrieve LArBadChannelTool " << m_badChannelTool );
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG( "LArBadChannelTool" << m_badChannelTool << " retrieved" );
  }

  ManagedMonitorToolBase::initialize().ignore(); 
  
  return sc;
}

// ********************************************************************
StatusCode LArFEBMon::bookHistograms() {
   
  // General summary histos
  
  StatusCode sc = StatusCode::SUCCESS;
  
  //  if(isNewRun){
    MonGroup summaryGroupW( this, "/LAr/FEBMon/Summary", run, ATTRIB_MANAGED, "", "weightedEff" );
    MonGroup summaryGroup( this, "/LAr/FEBMon/Summary", run, ATTRIB_MANAGED );
    MonGroup perPartitionDataGroup( this, "/LAr/FEBMon/perPartitionData", run, ATTRIB_MANAGED );
    MonGroup perPartitionDataGroupLowerLB( this, "/LAr/FEBMon/perPartitionData", run, ATTRIB_MANAGED, "", "lowerLB" );
    // General summary histos
    m_rejectedHisto = TH1F_LW::create("EventsRejected","Nb of events rejected (at least one error)",3,0.5,3.5);
    (m_rejectedHisto->GetXaxis())->SetBinLabel(1,"Whole event corrupted");
    (m_rejectedHisto->GetXaxis())->SetBinLabel(2,"Single FEB corrupted");
    (m_rejectedHisto->GetXaxis())->SetBinLabel(3,"Accepted");
    //sc = sc && summaryGroup.regHist(m_rejectedHisto);
    
    sc = regHist(m_rejectedHisto,  "/LAr/FEBMon/Summary", run);
    
    m_rejectedYield = TH1F_LW::create("EventsRejectedYield","Data corruption yield",3,0.5,3.5);
    (m_rejectedYield->GetXaxis())->SetBinLabel(1,"Whole event corrupted");
    (m_rejectedYield->GetXaxis())->SetBinLabel(2,"Single FEB corrupted");
    (m_rejectedYield->GetXaxis())->SetBinLabel(3,"Accepted");
    (m_rejectedYield->GetYaxis())->SetTitle("Yield(%)");
    sc = sc && summaryGroupW.regHist(m_rejectedYield);
    
    if (m_isOnline){ // Histo useless offline -> switch off in order to avoid merging warning
      m_rejectedLBProfile = TProfile_LW::create("EventsRejectedLB","% of events rejected in current LB (online only)",1,0,1);
      (m_rejectedLBProfile->GetXaxis())->SetBinLabel(1,"% of events");
      m_rejectedLBProfile->SetMinimum(0);
      m_rejectedLBProfile->SetMaximum(100);
      sc = sc && summaryGroup.regHist(m_rejectedLBProfile);
    }
    
    m_rejBitsHisto = TH1I_LW::create("rejectionBits","Errors at the origin of event rejection",8192,0.5,8191.5);
    (m_rejBitsHisto->GetYaxis())->SetTitle("Number of (rejected) events");
    sc = sc && summaryGroup.regHist(m_rejBitsHisto);
    
    m_LArAllErrors_dE = TH2I_LW::create("NbOfLArFEBMonErrors_dE","# of data corruption errors",13,0.5,13.5,8,-0.5,7.5);
    (m_LArAllErrors_dE->GetXaxis())->SetBinLabel(1,"Parity");
    (m_LArAllErrors_dE->GetXaxis())->SetBinLabel(2,"BCID");
    (m_LArAllErrors_dE->GetXaxis())->SetBinLabel(3,"Sample Header");
    (m_LArAllErrors_dE->GetXaxis())->SetBinLabel(4,"EVTID");
    (m_LArAllErrors_dE->GetXaxis())->SetBinLabel(5,"SCAC status");
    (m_LArAllErrors_dE->GetXaxis())->SetBinLabel(6,"Sca out of range");
    (m_LArAllErrors_dE->GetXaxis())->SetBinLabel(7,"Gain mismatch");
    (m_LArAllErrors_dE->GetXaxis())->SetBinLabel(8,"Type mismatch");
    (m_LArAllErrors_dE->GetXaxis())->SetBinLabel(9,"# of samples");
    (m_LArAllErrors_dE->GetXaxis())->SetBinLabel(10,"Empty data block");
    (m_LArAllErrors_dE->GetXaxis())->SetBinLabel(11,"Checksum / block size");
    (m_LArAllErrors_dE->GetXaxis())->SetBinLabel(12,"Missing header");
    (m_LArAllErrors_dE->GetXaxis())->SetBinLabel(13,"Bad gain");
    (m_LArAllErrors_dE->GetYaxis())->SetBinLabel(2,"EMBA");
    (m_LArAllErrors_dE->GetYaxis())->SetBinLabel(1,"EMBC");
    (m_LArAllErrors_dE->GetYaxis())->SetBinLabel(4,"EMECA");
    (m_LArAllErrors_dE->GetYaxis())->SetBinLabel(3,"EMECC");
    (m_LArAllErrors_dE->GetYaxis())->SetBinLabel(6,"HECA");
    (m_LArAllErrors_dE->GetYaxis())->SetBinLabel(5,"HECC");
    (m_LArAllErrors_dE->GetYaxis())->SetBinLabel(8,"FCALA");
    (m_LArAllErrors_dE->GetYaxis())->SetBinLabel(7,"FCALC");
    sc = sc && summaryGroup.regHist(m_LArAllErrors_dE);    
        
    // Number of events per minute vs LB
    m_eventsLB = TH1I_LW::create("NbOfEventsVsLB","Nb of events per LB",m_lumi_blocks+1,-0.5,(float)m_lumi_blocks+0.5);
    (m_eventsLB->GetXaxis())->SetTitle("Luminosity Block");
    sc = sc && summaryGroup.regHist(m_eventsLB);
    
    // Number of events rejected per LB
    m_rejectedYieldLB = TProfile_LW::create("YieldOfRejectedEventsVsLB","Yield of corrupted events",m_lumi_blocks+1,-0.5,(float)m_lumi_blocks+0.5);
    (m_rejectedYieldLB->GetXaxis())->SetTitle("Luminosity Block");
    (m_rejectedYieldLB->GetYaxis())->SetTitle("Yield(%)");
    m_rejectedYieldLB->SetMinimum(-5.);
    sc = sc && summaryGroup.regHist(m_rejectedYieldLB);
    
    // Number of events rejected per LB outside time veto window
    m_rejectedYieldLBout = TProfile_LW::create("YieldOfRejectedEventsVsLBout","Yield of corrupted events not vetoed by time window",m_lumi_blocks+1,-0.5,(float)m_lumi_blocks+0.5);
    (m_rejectedYieldLBout->GetXaxis())->SetTitle("Luminosity Block");
    m_rejectedYieldLBout->SetMinimum(-5.);
    sc = sc && summaryGroup.regHist(m_rejectedYieldLBout);
    
    // Mean event size per LB
    m_eventSizeLB = TProfile_LW::create("eventSizeVsLB","LAr event size (w/o ROS headers)",m_lumi_blocks+1,-0.5,(float)m_lumi_blocks+0.5);
    (m_eventSizeLB->GetXaxis())->SetTitle("Luminosity Block");
    (m_eventSizeLB->GetYaxis())->SetTitle("Megabytes");
    sc = sc && summaryGroup.regHist(m_eventSizeLB);

    if (m_isOnline){ // Histo useless offline -> switch off in order to avoid merging warning
       // Mean event size per stream per LB
       int nStreams = m_streams.size();
       if(nStreams > 0) {
          m_stream_eventSizeLB = TProfile2D_LW::create("eventSizeStreamVsLB","LAr event size per stream per LB (w/o ROS headers)",m_lumi_blocks+1,-0.5,(float)m_lumi_blocks+0.5,nStreams+1,-0.5,nStreams+0.5);
          (m_stream_eventSizeLB->GetXaxis())->SetTitle("Luminosity Block");
          for (int str = 0; str < nStreams; str++) {
             (m_stream_eventSizeLB->GetYaxis())->SetBinLabel(str+1,m_streams[str].c_str());
          }
          (m_stream_eventSizeLB->GetYaxis())->SetBinLabel(nStreams+1,"others");
          sc = sc && summaryGroup.regHist(m_stream_eventSizeLB);
       }
    }
   
    
    // All nb of readout FEB per partition in one TH2
    m_nbOfEvts2D = TH2I_LW::create("NbOfEvts2d","# of readout FEB/DSP header",500,-20.5,479.5,4,-0.5,3.5);    
    (m_nbOfEvts2D->GetYaxis())->SetBinLabel(2,"Barrel A");
    (m_nbOfEvts2D->GetYaxis())->SetBinLabel(1,"Barrel C");
    (m_nbOfEvts2D->GetYaxis())->SetBinLabel(4,"Endcap A");
    (m_nbOfEvts2D->GetYaxis())->SetBinLabel(3,"Endcap C");
    sc = sc && perPartitionDataGroup.regHist(m_nbOfEvts2D);
    
    // Global nb of readout FEB
    m_nbOfFebBlocksTotal = TH1I_LW::create("NbOfReadoutFEBGlobal","# of readout FEB/DSP header",m_FEBnbins,m_FEBmin,m_FEBmax);
    sc = sc && perPartitionDataGroup.regHist(m_nbOfFebBlocksTotal);
    
    // DSP threshold
    m_dspThresholds_ADC = TH1I_LW::create("dspThresholdsADC","DSP thresholds to readout samples",2000,-100.5,1899.5);
    m_dspThresholds_ADC->GetYaxis()->SetTitle("Number of cells");
    m_dspThresholds_ADC->GetXaxis()->SetTitle("Cell threshold in ADC counts");
    sc = sc && perPartitionDataGroupLowerLB.regHist(m_dspThresholds_ADC);
    m_dspThresholds_qtime = TH1I_LW::create("dspThresholds_qfactortime","DSP thresholds to readout (qfactor+time)",2000,-100.5,1899.5);
    m_dspThresholds_qtime->GetYaxis()->SetTitle("Number of cells");
    m_dspThresholds_qtime->GetXaxis()->SetTitle("Cell threshold in ADC counts");
    sc = sc && perPartitionDataGroupLowerLB.regHist(m_dspThresholds_qtime);
    
    // Event type
    m_eventType = TH1I_LW::create("Eventtype","Event type (1st readout FEB)",5,0.,15.);
    (m_eventType->GetXaxis())->SetBinLabel(1,"Raw data - Transparent");
    (m_eventType->GetXaxis())->SetBinLabel(2,"Result - Physic");
    (m_eventType->GetXaxis())->SetBinLabel(3,"Calibration");
    (m_eventType->GetXaxis())->SetBinLabel(4,"Pedestals");
    (m_eventType->GetXaxis())->SetBinLabel(5,"Raw data+Result");
    sc = sc && perPartitionDataGroup.regHist(m_eventType);
    
    // Trigger types
    m_hTriggerType = TH1I_LW::create("TriggerWord","Number of Events per L1 trigger word (8 bits)", 256, -0.5, 255.5);
    m_hTriggerType->GetXaxis()->SetTitle("L1 trigger word");
    sc = sc && perPartitionDataGroup.regHist(m_hTriggerType);
    m_hTriggerTypeAllDSP = TH1I_LW::create("TriggerWordAllDSP","Number L1 trigger word per DSP (8 bits)", 256, -0.5, 255.5);
    m_hTriggerTypeAllDSP->GetXaxis()->SetTitle("L1 trigger word");
    (m_hTriggerTypeAllDSP->GetYaxis())->SetTitle("Number of events");
    sc = sc && perPartitionDataGroup.regHist(m_hTriggerTypeAllDSP);
    
    // Nb of samples
    m_nbOfSamples = TH1I_LW::create("NbOfSamples","# of samples (1st readout FEB)",35,-1.5,33.5);
    (m_nbOfSamples->GetYaxis())->SetTitle("Number of events");
    sc = sc && perPartitionDataGroup.regHist(m_nbOfSamples);
    
    // Nb of cells above DSP samples threshold
    m_totNbSw2 = TH1I_LW::create("NbOfSw2","# of cells with samples readout",2000,-1000,199000);
    (m_totNbSw2->GetXaxis())->SetTitle("Number of cells");
    (m_totNbSw2->GetYaxis())->SetTitle("Number of events");
    sc = sc && perPartitionDataGroup.regHist(m_totNbSw2);
    
    // Book per partition set of histograms
    sc = sc && bookNewPartitionSumm(m_barrelCSummary,"EMBC");
    sc = sc && bookNewPartitionSumm(m_barrelASummary,"EMBA");
    sc = sc && bookNewPartitionSumm(m_emecCSummary,"EMECC");
    sc = sc && bookNewPartitionSumm(m_emecASummary,"EMECA");
    sc = sc && bookNewPartitionSumm(m_hecCSummary,"HECC");
    sc = sc && bookNewPartitionSumm(m_hecASummary,"HECA");
    sc = sc && bookNewPartitionSumm(m_fcalCSummary,"FcalC");
    sc = sc && bookNewPartitionSumm(m_fcalASummary,"FcalA");
    
    plotMaskedFEB();
    
    if(m_isOnline) {
       // setup trigger decision tool
       m_trigDec.retrieve().ignore();
       const Trig::ChainGroup* allL1 = m_trigDec->getChainGroup("L1_.*");
       std::vector<std::string> l1triggers = allL1->getListOfTriggers();
        
       ATH_MSG_INFO( "lvl1 item names: [" );
       for (unsigned int i=0;i< l1triggers.size();i++) {
            ATH_MSG_INFO( i << " " << l1triggers.at(i) << " , " );
       }
       ATH_MSG_INFO( "] " );

    if (l1triggers.size()>0) {m_trigok=true;} else {m_trigok=false;}
    }

    //
    m_CorruptTree = new TTree("LArCorrupted","Timestamps of corrupted LAr events");
    m_CorruptTree->Branch("time",&m_eventTime,"time/i");
    m_CorruptTree->Branch("time_ns",&m_eventTime_ns,"time_ns/i");
    m_CorruptTree->Branch("febHwId",&m_febInErrorTree);
    m_CorruptTree->Branch("febErrorType",&m_febErrorTypeTree);
    summaryGroup.regTree(m_CorruptTree).ignore();
    //  }
  
  ATH_MSG_DEBUG( "done with bookHists()" );
  
  if (sc.isFailure()) {
    ATH_MSG_ERROR( "Bookhists failed" );
  }
  
  return sc;
}


// ********************************************************************
//called on each event
StatusCode LArFEBMon::fillHistograms() {
  
  m_eventRejected = false;
  m_rejectionBits.reset();
  m_febInErrorTree.clear();
  m_febErrorTypeTree.clear();
  
  m_eventsCounter++;

  // Retrieve event info to get event time,trigger type...
  // Retrieved at beg of method now to get the LVL1 type
  // to check consistency with DSP trigger type
  const xAOD::EventInfo* thisEvent=0;

  if(evtStore()->retrieve(thisEvent).isFailure()) {
    ATH_MSG_WARNING( "Failed to retrieve EventInfo object" );
    return StatusCode::FAILURE;
  }

  m_l1Trig = thisEvent->level1TriggerType();
  m_hTriggerType->Fill(m_l1Trig);

  m_eventTime=thisEvent->timeStamp();
  m_eventTime_ns=thisEvent->timeStampNSOffset();
    
  unsigned lumi_block = thisEvent->lumiBlock();
  bool lar_inerror = (thisEvent->errorState(xAOD::EventInfo::LAr)==xAOD::EventInfo::Error) ? true : false;
  
  //ATH_MSG_INFO( "LArFEBMon Lumi block: "<<lumi_block);

  const LArFebHeaderContainer* hdrCont;
  const LArFebErrorSummary* lArFebErrorSummary;
  StatusCode sc = evtStore()->retrieve(hdrCont);
  if (sc.isFailure() || !hdrCont) {
    ATH_MSG_WARNING( "No LArFebHeaderContainer found in TDS" ); 
    return sc;
  }
  
  if (hdrCont->size()==0) {
    ATH_MSG_WARNING( "Got empty LArFebHeaderContainer. Do nothing" );
    return StatusCode::FAILURE;
  }

  sc=evtStore()->retrieve( lArFebErrorSummary, "LArFebErrorSummary");
  if (sc.isFailure()) {
    ATH_MSG_WARNING( "No LArFebErrorSummary found in TDS" );
    return StatusCode::FAILURE;
  }
  
  
  uint32_t firstEventType = (*hdrCont->begin())->DetEventType(); 

  // At 1st event, retrieve DSP thresholds and fill histogram with values for all channels
  if (m_eventsCounter == 1 && firstEventType == 4) {
    m_dspThresholds_ADC->SetTitle(Form("DSP thresholds for ADC-sample transmission (only in physics) - LB %4d",lumi_block));
    m_dspThresholds_qtime->SetTitle(Form("DSP thresholds for qfactor+time calculation (only in physics) - LB %4d",lumi_block));
    if (m_keyDSPThresholds.size()>0) { 
      if (detStore()->contains<LArDSPThresholdsComplete>(m_keyDSPThresholds)) {
	ATH_MSG_DEBUG("Loading run1 version of LAr DSP Thresholds");
	const LArDSPThresholdsComplete* dspThresh=0;
	sc=detStore()->retrieve(dspThresh,m_keyDSPThresholds);
	if(!sc.isSuccess()) {
	  ATH_MSG_WARNING(" Failed to retrieve LArDSPThresholds with key " << m_keyDSPThresholds 
			  << ". Will not fill histograms" );
	}
	else {
	  auto chIt=m_onlineHelper->channel_begin();
	  auto chIt_e=m_onlineHelper->channel_end();
	  for (;chIt!=chIt_e;++chIt) {
	    m_dspThresholds_ADC->Fill(dspThresh->samplesThr(*chIt));
	    m_dspThresholds_qtime->Fill(dspThresh->tQThr(*chIt));
	  }//end loop over channels
	}//end got DSP Thresholds
      } //end if contains LArDSPThresholdsComplete

      else {
	ATH_MSG_DEBUG("Loading run2 version of LAr DSP Thresholds");
	const AthenaAttributeList* dspThrshAttr=0; 
	sc=detStore()->retrieve(dspThrshAttr,m_keyDSPThresholds);
	if (sc.isFailure()) {
	  ATH_MSG_WARNING( "Failed to retrieve AttributeList with key (folder) " << m_keyDSPThresholds 
			  << ", containing DSP Thresholds. Will not fill histograms." );
	}
	else {
	  const LArDSPThresholdsFlat* dspThreshFlat=new LArDSPThresholdsFlat(dspThrshAttr);
	  if (!dspThreshFlat->good()) {
	    ATH_MSG_WARNING( "Failed to initialize LArDSPThresholdFlat from attribute list loaded from " << m_keyDSPThresholds
			    << ". Will not fill histograms." ); 
	  }//end if not good
	  const IdentifierHash chanMax=m_onlineHelper->channelHashMax();
	  for (unsigned iChan=0;iChan<chanMax;++iChan) {
	    m_dspThresholds_ADC->Fill(dspThreshFlat->samplesThrByHash(iChan));
	    m_dspThresholds_qtime->Fill(dspThreshFlat->tQThrByHash(iChan));
	  }

	  delete dspThreshFlat;
	}//end else got DSP Thresholds
      }// else run 2
    }// end load DSP thresholds from DB
    else 
      ATH_MSG_WARNING( "No LArDSPThresholds key specificed. Will not fill these histograms" );
  }//end if eventsCounter==1


  //Adjust event type if we have raw data in the ROD block
  if (firstEventType == 4 && (*hdrCont->begin())->RodRawDataSize() != 0) firstEventType = 14;

  unsigned int nfeb[8] = {0,0,0,0,0,0,0,0};
  unsigned int firstEventNbSp=0;
  unsigned int totNbOfSweet2 = 0;
  float larEventSize = 0;
  float larEventSize_part[8] = {0.,0.,0.,0.,0.,0.,0.,0.};

  LArFebHeaderContainer::const_iterator it = hdrCont->begin(); 
  LArFebHeaderContainer::const_iterator it_e = hdrCont->end(); 
  
  // Loop on all available FEBHeader to fill basic parameters histogram : event type, event size, nb of sweet cells...
  for ( ; it!=it_e;++it) {
    HWIdentifier febid=(*it)->FEBId();
    
    if (febid.get_identifier32().get_compact() >= 0x38000000 && febid.get_identifier32().get_compact() <= 0x3bc60000 && !(febid.get_identifier32().get_compact() & 0xFFF)) {
      int barrel_ec = m_onlineHelper->barrel_ec(febid);
      int pos_neg   = m_onlineHelper->pos_neg(febid);
      int ft        = m_onlineHelper->feedthrough(febid);
      int slot      = m_onlineHelper->slot(febid);      
      int partitionNb_dE = returnPartition(barrel_ec,pos_neg,ft,slot);

      if (partitionNb_dE >= 0)
        nfeb[partitionNb_dE]++ ;
      larEventSize += (float) ((*it)->RodRawDataSize() + (*it)->RodResults1Size() + (*it)->RodResults2Size()); // This quantity is now in megabytes
      if(partitionNb_dE>=0 && partitionNb_dE<8) larEventSize_part[partitionNb_dE] += (float) ((*it)->RodRawDataSize() + (*it)->RodResults1Size() + (*it)->RodResults2Size());
      
      // Eventype = 2 : transparent/raw data - 4 : Physic - 7 : calibration - 10 : pedestal - 14 : raw data + results
      uint32_t eventType = (*it)->DetEventType();      
      // If physic mode && raw data != 0, we are in rawdata+results
      if (eventType == 4 && (*it)->RodRawDataSize() != 0) eventType = 14;
      //if (firstEventType == 999) firstEventType = eventType;
      if (firstEventNbSp == 0)
      	if ((*it)->NbSamples() != 0) 
	  firstEventNbSp=(*it)->NbSamples();

      
      m_hTriggerTypeAllDSP->Fill((*it)->LVL1TigType());
      if (firstEventType == 4) totNbOfSweet2 = totNbOfSweet2+(*it)->NbSweetCells2();
      // Fill (nb of sweet cells per FEB) histos
      switch (partitionNb_dE){
	case 0:
	  (m_barrelCSummary.nbOfEvts)->Fill(slot,ft);
	  if (firstEventType == 4) (m_barrelCSummary.nbOfSweet1)->Fill(slot,ft,(*it)->NbSweetCells1());
	  if (firstEventType == 4) (m_barrelCSummary.nbOfSweet2)->Fill(slot,ft,(*it)->NbSweetCells2());
	  if ((*it)->LVL1TigType() == 0 || (*it)->LVL1TigType() == 170 || (*it)->LVL1TigType() != m_l1Trig) (m_barrelCSummary.missingTriggerType)->Fill(slot,ft);
	  ATH_MSG_DEBUG(" EMBC : " <<  m_onlineHelper->show_to_string(febid) << " LVL1 type" << (*it)->LVL1TigType());
	  break;
	case 1:
	  (m_barrelASummary.nbOfEvts)->Fill(slot,ft);
   	  if (firstEventType == 4) (m_barrelASummary.nbOfSweet1)->Fill(slot,ft,(*it)->NbSweetCells1());
	  if (firstEventType == 4) (m_barrelASummary.nbOfSweet2)->Fill(slot,ft,(*it)->NbSweetCells2());
	  if ((*it)->LVL1TigType() == 0 || (*it)->LVL1TigType() == 170 || (*it)->LVL1TigType() != m_l1Trig) (m_barrelASummary.missingTriggerType)->Fill(slot,ft);
	  ATH_MSG_DEBUG(" EMBA : " <<  m_onlineHelper->show_to_string(febid) << " LVL1 type" << (*it)->LVL1TigType());
	  break;
	case 2:
	  (m_emecCSummary.nbOfEvts)->Fill(slot,ft);
	  if (firstEventType == 4) (m_emecCSummary.nbOfSweet1)->Fill(slot,ft,(*it)->NbSweetCells1());
	  if (firstEventType == 4) (m_emecCSummary.nbOfSweet2)->Fill(slot,ft,(*it)->NbSweetCells2());
	  if ((*it)->LVL1TigType() == 0 || (*it)->LVL1TigType() == 170 || (*it)->LVL1TigType() != m_l1Trig) (m_emecCSummary.missingTriggerType)->Fill(slot,ft);
	  ATH_MSG_DEBUG(" EMECC : " <<  m_onlineHelper->show_to_string(febid) << " LVL1 type" << (*it)->LVL1TigType());
	  break;
	case 3:
	  (m_emecASummary.nbOfEvts)->Fill(slot,ft);
	  if (firstEventType == 4) (m_emecASummary.nbOfSweet1)->Fill(slot,ft,(*it)->NbSweetCells1());
	  if (firstEventType == 4) (m_emecASummary.nbOfSweet2)->Fill(slot,ft,(*it)->NbSweetCells2());
	  if ((*it)->LVL1TigType() == 0 || (*it)->LVL1TigType() == 170 || (*it)->LVL1TigType() != m_l1Trig) (m_emecASummary.missingTriggerType)->Fill(slot,ft);
	  ATH_MSG_DEBUG(" EMECA : " <<  m_onlineHelper->show_to_string(febid) << " LVL1 type" << (*it)->LVL1TigType());
	  break;	
	case 4:
	  (m_hecCSummary.nbOfEvts)->Fill(slot,ft);
	  if (firstEventType == 4) (m_hecCSummary.nbOfSweet1)->Fill(slot,ft,(*it)->NbSweetCells1());
	  if (firstEventType == 4) (m_hecCSummary.nbOfSweet2)->Fill(slot,ft,(*it)->NbSweetCells2());
	  if ((*it)->LVL1TigType() == 0 || (*it)->LVL1TigType() == 170 || (*it)->LVL1TigType() != m_l1Trig) (m_hecCSummary.missingTriggerType)->Fill(slot,ft);
	  ATH_MSG_DEBUG(" HECC : " <<  m_onlineHelper->show_to_string(febid) << " LVL1 type" << (*it)->LVL1TigType());
	  break;
	case 5:
	  (m_hecASummary.nbOfEvts)->Fill(slot,ft);
	  if (firstEventType == 4) (m_hecASummary.nbOfSweet1)->Fill(slot,ft,(*it)->NbSweetCells1());
	  if (firstEventType == 4) (m_hecASummary.nbOfSweet2)->Fill(slot,ft,(*it)->NbSweetCells2());
	  if ((*it)->LVL1TigType() == 0 || (*it)->LVL1TigType() == 170 || (*it)->LVL1TigType() != m_l1Trig) (m_hecASummary.missingTriggerType)->Fill(slot,ft);
	  ATH_MSG_DEBUG(" HECA : " <<  m_onlineHelper->show_to_string(febid) << " LVL1 type" << (*it)->LVL1TigType());
	  break;
	case 6:
	  (m_fcalCSummary.nbOfEvts)->Fill(slot,ft);
	  if (firstEventType == 4) (m_fcalCSummary.nbOfSweet1)->Fill(slot,ft,(*it)->NbSweetCells1());
	  if (firstEventType == 4) (m_fcalCSummary.nbOfSweet2)->Fill(slot,ft,(*it)->NbSweetCells2());
	  if ((*it)->LVL1TigType() == 0 || (*it)->LVL1TigType() == 170 || (*it)->LVL1TigType() != m_l1Trig) (m_fcalCSummary.missingTriggerType)->Fill(slot,ft);
	  ATH_MSG_DEBUG(" FCALC : " <<  m_onlineHelper->show_to_string(febid) << " LVL1 type" << (*it)->LVL1TigType());
	  break;
	case 7:
	  (m_fcalASummary.nbOfEvts)->Fill(slot,ft);
	  if (firstEventType == 4) (m_fcalASummary.nbOfSweet1)->Fill(slot,ft,(*it)->NbSweetCells1());
	  if (firstEventType == 4) (m_fcalASummary.nbOfSweet2)->Fill(slot,ft,(*it)->NbSweetCells2());
	  if ((*it)->LVL1TigType() == 0 || (*it)->LVL1TigType() == 170 || (*it)->LVL1TigType() != m_l1Trig) (m_fcalASummary.missingTriggerType)->Fill(slot,ft);
	  ATH_MSG_DEBUG(" FCALA : " <<  m_onlineHelper->show_to_string(febid) << " LVL1 type" << (*it)->LVL1TigType());
	  break;
      }      
    }//Test on FEBid
  }//end of loop over FEB headers
  
  // Loop over all febs to plot the error from statusword
  // This is mandatory to also monitor the FEBs with missing headers
  m_anyfebIE = false;
  for (std::vector<HWIdentifier>::const_iterator allFeb = m_onlineHelper->feb_begin(); 
       allFeb != m_onlineHelper->feb_end(); ++allFeb) {
    HWIdentifier febid = HWIdentifier(*allFeb);
    m_currentFebStatus = false;
    uint16_t feberrorSummary = lArFebErrorSummary->feb_error(febid);
    
    if ( feberrorSummary != 0 ){
      int barrel_ec = m_onlineHelper->barrel_ec(febid);
      int pos_neg   = m_onlineHelper->pos_neg(febid);
      int ft        = m_onlineHelper->feedthrough(febid);
      int slot      = m_onlineHelper->slot(febid);      
      int partitionNb_dE = returnPartition(barrel_ec,pos_neg,ft,slot);
      
      // Fill the errors in partition histograms
      switch (partitionNb_dE){
	case 0:
	  fillErrorsSummary(m_barrelCSummary,partitionNb_dE,ft,slot,feberrorSummary,lumi_block,lar_inerror);
	  break;
	case 1:
	  fillErrorsSummary(m_barrelASummary,partitionNb_dE,ft,slot,feberrorSummary,lumi_block,lar_inerror);
	  break;
	case 2:
	  fillErrorsSummary(m_emecCSummary,partitionNb_dE,ft,slot,feberrorSummary,lumi_block,lar_inerror);
	  break;
	case 3:
	  fillErrorsSummary(m_emecASummary,partitionNb_dE,ft,slot,feberrorSummary,lumi_block,lar_inerror);
	  break;	
	case 4:
	  fillErrorsSummary(m_hecCSummary,partitionNb_dE,ft,slot,feberrorSummary,lumi_block,lar_inerror);
	  break;
	case 5:
	  fillErrorsSummary(m_hecASummary,partitionNb_dE,ft,slot,feberrorSummary,lumi_block,lar_inerror);
	  break;
	case 6:
	  fillErrorsSummary(m_fcalCSummary,partitionNb_dE,ft,slot,feberrorSummary,lumi_block,lar_inerror);
	  break;
	case 7:	    
	  fillErrorsSummary(m_fcalASummary,partitionNb_dE,ft,slot,feberrorSummary,lumi_block,lar_inerror);
	  break;
      }
      if (m_currentFebStatus && m_febInErrorTree.size()<33){
	m_febInErrorTree.push_back(febid.get_identifier32().get_compact());
	m_febErrorTypeTree.push_back(m_rejectionBits.to_ulong());
      }
    }  
    if(m_currentFebStatus) m_anyfebIE = m_currentFebStatus;
  }
  // Check and reset the rejected histo per partition
  // Use the ENTRIES variable to store the LB and reset if needed!
  if (m_isOnline){
     if ((m_barrelCSummary.m_rejectedLBProfilePart)->GetEntries() != lumi_block) {
                      (m_barrelCSummary.m_rejectedLBProfilePart)->Reset();
                      (m_barrelCSummary.m_rejectedLBProfilePart)->SetEntries(lumi_block);
     }
     if ((m_barrelASummary.m_rejectedLBProfilePart)->GetEntries() != lumi_block) {
                      (m_barrelASummary.m_rejectedLBProfilePart)->Reset();
                      (m_barrelASummary.m_rejectedLBProfilePart)->SetEntries(lumi_block);
     }
     if ((m_emecASummary.m_rejectedLBProfilePart)->GetEntries() != lumi_block) {
                      (m_emecASummary.m_rejectedLBProfilePart)->Reset();
                      (m_emecASummary.m_rejectedLBProfilePart)->SetEntries(lumi_block);
     }
     if ((m_emecCSummary.m_rejectedLBProfilePart)->GetEntries() != lumi_block) {
                      (m_emecCSummary.m_rejectedLBProfilePart)->Reset();
                      (m_emecCSummary.m_rejectedLBProfilePart)->SetEntries(lumi_block);
     }
     if ((m_hecCSummary.m_rejectedLBProfilePart)->GetEntries() != lumi_block) {
                      (m_hecCSummary.m_rejectedLBProfilePart)->Reset();
                      (m_hecCSummary.m_rejectedLBProfilePart)->SetEntries(lumi_block);
     }
     if ((m_hecASummary.m_rejectedLBProfilePart)->GetEntries() != lumi_block) {
                      (m_hecASummary.m_rejectedLBProfilePart)->Reset();
                      (m_hecASummary.m_rejectedLBProfilePart)->SetEntries(lumi_block);
     }
     if ((m_fcalASummary.m_rejectedLBProfilePart)->GetEntries() != lumi_block) {
                      (m_fcalASummary.m_rejectedLBProfilePart)->Reset();
                      (m_fcalASummary.m_rejectedLBProfilePart)->SetEntries(lumi_block);
     }
     if ((m_fcalCSummary.m_rejectedLBProfilePart)->GetEntries() != lumi_block) {
                      (m_fcalCSummary.m_rejectedLBProfilePart)->Reset();
                      (m_fcalCSummary.m_rejectedLBProfilePart)->SetEntries(lumi_block);
     }
  }
  
  //Fill general data histos
  m_eventType->Fill(firstEventType);
  float nbOfFeb = (float)  (nfeb[0]+nfeb[1]+nfeb[2]+nfeb[3]+nfeb[4]+nfeb[5]+nfeb[6]+nfeb[7]);
  if (firstEventType == 2 || firstEventType == 4 || firstEventType == 14) m_nbOfSamples->Fill(firstEventNbSp);
  
  // Fill the total number of FEB (test on DSP headers) and for each partition (Dirac plots)
  m_nbOfFebBlocksTotal->Fill(nbOfFeb);
  
  m_barrelASummary.nbOfFebBlocks->Fill(nfeb[1]);	
  m_nbOfEvts2D->Fill(nfeb[1],1);
  
  m_barrelCSummary.nbOfFebBlocks->Fill(nfeb[0]);	
  m_nbOfEvts2D->Fill(nfeb[0],0);
  
  m_nbOfEvts2D->Fill(nfeb[3]+nfeb[5]+nfeb[7],3);
  m_emecASummary.nbOfFebBlocks->Fill(nfeb[3]);
  m_hecASummary.nbOfFebBlocks->Fill(nfeb[5]);
  m_fcalASummary.nbOfFebBlocks->Fill(nfeb[7]);
  
  m_nbOfEvts2D->Fill(nfeb[2]+nfeb[4]+nfeb[6],2);
  m_emecCSummary.nbOfFebBlocks->Fill(nfeb[2]);
  m_hecCSummary.nbOfFebBlocks->Fill(nfeb[4]);
  m_fcalCSummary.nbOfFebBlocks->Fill(nfeb[6]);
  
  // If the nb of DSP headers is lower than the maximum, this means that there are some missing FEBs, probably
  // due to missing ROS fragment
  // This assumes that the maximum of DSP headers found is the expected one
  //  ATH_MSG_ERROR( "ICI" << nbOfFeb << " " << m_nbOfFebBlocksTotal->GetBinLowEdge(m_nbOfFebBlocksTotal->GetMaximumBin()) );
  
  if (nbOfFebOK(nbOfFeb,m_nbOfFebBlocksTotal)){
    // The nb of readout FEB is lower than the maximum number of FEBs observed in this run
    m_rejectedHisto->Fill(1);
    //    m_rejectedLBProfile->Fill(0.5,100);
  }
  else{
    // There was at least one FEB in error
    if (m_eventRejected) {
      m_rejectedHisto->Fill(2);
      m_rejBitsHisto->Fill(m_rejectionBits.to_ulong());
      //   m_rejectedLBProfile->Fill(0.5,100);
    }
    // The event is accepted
    else {
      m_rejectedHisto->Fill(3);
      //m_rejectedLBProfile->Fill(0.5,0);
    }
  }
  
    
  m_eventsLB->Fill(lumi_block);    
  m_totNbSw2->Fill(totNbOfSweet2);
  larEventSize = larEventSize/262144;
  m_eventSizeLB->Fill(lumi_block,larEventSize);
    
  // Use the ENTRIES variable to store the LB and reset if needed!
  if (m_isOnline){
    if (m_rejectedLBProfile->GetEntries() != lumi_block)
      m_rejectedLBProfile->Reset();
  }

  if(m_anyfebIE) { m_CorruptTree->Fill(); }

  if ((m_eventRejected) || nbOfFebOK(nbOfFeb,m_nbOfFebBlocksTotal) || nbOfFeb < nFEBnominal){
    if ((m_eventRejected) || nbOfFebOK(nbOfFeb,m_nbOfFebBlocksTotal)) m_rejectedYieldLB->Fill(lumi_block,100); else m_rejectedYieldLB->Fill(lumi_block,50);

    if (!(thisEvent->isEventFlagBitSet(xAOD::EventInfo::LAr,LArEventBitInfo::DATACORRUPTEDVETO)))
      m_rejectedYieldLBout->Fill(lumi_block,100);
    else
      m_rejectedYieldLBout->Fill(lumi_block,0);

    if (m_isOnline) {
       if (lar_inerror) m_rejectedLBProfile->Fill(0.5,100); else m_rejectedLBProfile->Fill(0.5,50);
    }
  } else{
    m_rejectedYieldLB->Fill(lumi_block,0);
    m_rejectedYieldLBout->Fill(lumi_block,0);
    if (m_isOnline) m_rejectedLBProfile->Fill(0.5,0);
  }
  if (m_isOnline) m_rejectedLBProfile->SetEntries(lumi_block);
  

  //Trigger part: Already done at the beginning of this function
  //uint m_l1Trig = (uint) (trig->level1TriggerType());
  //m_hTriggerType->Fill(m_l1Trig);

  if(m_isOnline && m_streams.size() > 0) {
    m_streamsThisEvent=trigStreamMatching(m_streams,thisEvent->streamTags());
 
    // Now we could fill the event size
      
    unsigned ie;
    for(unsigned int str=0; str<m_streamsThisEvent.size(); ++str) {
      if(m_streamsThisEvent[str] > m_streams.size()) break;
      if(m_trigok && m_streamsThisEvent[str] < m_streams.size() && (m_streams[m_streamsThisEvent[str]].find("CosmicCalo") != std::string::npos)) { // test excluding events
	for(ie=0; ie<m_excoscalo.size(); ++ie) {
	  if(m_trigDec->isPassed(m_excoscalo[ie])) break;
	}
	if(ie<m_excoscalo.size()) {
	  std::cout<<"Skipping "<<m_excoscalo[ie]<<" for CosmicCalo "<<std::endl;
	  continue; // we should skip this trigger
	}
      }
              
      m_stream_eventSizeLB->Fill(lumi_block, m_streamsThisEvent[str], larEventSize) ;
      
      m_barrelASummary.stream_eventSizeLB->Fill(lumi_block, m_streamsThisEvent[str], larEventSize_part[1]/262144);	
      m_barrelCSummary.stream_eventSizeLB->Fill(lumi_block, m_streamsThisEvent[str], larEventSize_part[0]/262144);
      
      m_emecASummary.stream_eventSizeLB->Fill(lumi_block, m_streamsThisEvent[str], larEventSize_part[3]/262144);
      m_hecASummary.stream_eventSizeLB->Fill(lumi_block, m_streamsThisEvent[str], larEventSize_part[5]/262144);
      m_fcalASummary.stream_eventSizeLB->Fill(lumi_block, m_streamsThisEvent[str], larEventSize_part[7]/262144);
      
      m_emecCSummary.stream_eventSizeLB->Fill(lumi_block, m_streamsThisEvent[str], larEventSize_part[2]/262144);
      m_hecCSummary.stream_eventSizeLB->Fill(lumi_block, m_streamsThisEvent[str], larEventSize_part[4]/262144);
      m_fcalCSummary.stream_eventSizeLB->Fill(lumi_block, m_streamsThisEvent[str], larEventSize_part[6]/262144);
    }
  } else { // we are filling only simple profiles
    m_barrelASummary.eventSizeLB->Fill(lumi_block,  larEventSize_part[1]/262144);	
    m_barrelCSummary.eventSizeLB->Fill(lumi_block,  larEventSize_part[0]/262144);
      
    m_emecASummary.eventSizeLB->Fill(lumi_block,  larEventSize_part[3]/262144);
    m_hecASummary.eventSizeLB->Fill(lumi_block,  larEventSize_part[5]/262144);
    m_fcalASummary.eventSizeLB->Fill(lumi_block,  larEventSize_part[7]/262144);
   
    m_emecCSummary.eventSizeLB->Fill(lumi_block,  larEventSize_part[2]/262144);
    m_hecCSummary.eventSizeLB->Fill(lumi_block,  larEventSize_part[4]/262144);
    m_fcalCSummary.eventSizeLB->Fill(lumi_block,  larEventSize_part[6]/262144);
  }

   
  return sc;
}


// ********************************************************************
void LArFEBMon::fillErrorsSummary(summaryPartition& summ,int partitNb_2,int ft,int slot,uint16_t error, unsigned lumi_block, bool lar_inerror)
{  
  if (summ.maskedFEB->GetBinContent(slot,ft+1) != 0) return;
  
  if ( error & (1<<0) ){
    (summ.parity)->Fill(slot,ft);
    m_currentFebStatus = true;
    m_rejectionBits.set(0);
    m_LArAllErrors_dE->Fill(1,partitNb_2);
  }
  
  if ( error & (1<<1) ){
    (summ.BCID_2halves)->Fill(slot,ft);
    m_currentFebStatus = true;
    m_rejectionBits.set(1);
    m_LArAllErrors_dE->Fill(2,partitNb_2);
  }
  
  if ( error & (1<<2) ){
    (summ.RADD_2halves)->Fill(slot,ft);
    m_currentFebStatus = true;
    m_rejectionBits.set(2);
    m_LArAllErrors_dE->Fill(3,partitNb_2);
  }
  
  if ( error & (1<<3) ){
    (summ.EVTID_2halves)->Fill(slot,ft);
    m_currentFebStatus = true;
    m_rejectionBits.set(3);
    m_LArAllErrors_dE->Fill(4,partitNb_2);
  }
  
  if ( error & (1<<4) ){
    (summ.SCACStatus)->Fill(slot,ft);
    m_currentFebStatus = true;
    m_rejectionBits.set(4);
    m_LArAllErrors_dE->Fill(5,partitNb_2);
  }
  
  if ( error & (1<<5) ){
    (summ.scaOutOfRange)->Fill(slot,ft);
    m_currentFebStatus = true;
    m_rejectionBits.set(5);
    m_LArAllErrors_dE->Fill(6,partitNb_2);
  }
  
  if ( error & (1<<6) ){
    (summ.gainMismatch)->Fill(slot,ft);
    m_currentFebStatus = true;
    m_rejectionBits.set(6);
    m_LArAllErrors_dE->Fill(7,partitNb_2);
  }
  
  if ( error & (1<<7) ){
    (summ.typeMismatch)->Fill(slot,ft);
    m_currentFebStatus = true;
    m_rejectionBits.set(7);
    m_LArAllErrors_dE->Fill(8,partitNb_2);
  }
  
  if ( error & (1<<8) ){
    (summ.badNbOfSamp)->Fill(slot,ft);
    m_currentFebStatus = true;
    m_rejectionBits.set(8);
    m_LArAllErrors_dE->Fill(9,partitNb_2);
  }
  
  if ( error & (1<<9) ){
    (summ.zeroSamp)->Fill(slot,ft);
    m_currentFebStatus = true;
    m_rejectionBits.set(9);
    m_LArAllErrors_dE->Fill(10,partitNb_2);
  }
  
  
  if ( error & (1<<11) ){
    (summ.checkSum)->Fill(slot,ft);
    m_currentFebStatus = true;
    m_rejectionBits.set(10);
    m_LArAllErrors_dE->Fill(11,partitNb_2);
  }
  
  if ( error & (1<<12) ){
    // Check whether this error can be ignored. Useful for calibration run of PS or EMB
    if (!((m_ignoreMissingHeaderEMB && partitNb_2<2 && slot>=2) || (m_ignoreMissingHeaderPS && partitNb_2<2 && slot==1))){
      (summ.missingHeader)->Fill(slot,ft);
      m_currentFebStatus = true;
      m_rejectionBits.set(11);
      m_LArAllErrors_dE->Fill(12,partitNb_2);
    }
  }
  
  if ( error & (1<<13) ){
    (summ.badGain)->Fill(slot,ft);
    m_currentFebStatus = true;
    m_rejectionBits.set(12);
    m_LArAllErrors_dE->Fill(13,partitNb_2);
  }
  

  if (m_currentFebStatus){
    (summ.LArAllErrors)->Fill(slot,ft);
    if (lar_inerror) {// LArinError
       m_eventRejected = true;
       if (m_isOnline) (summ.m_rejectedLBProfilePart)->Fill(0.5,100);
    } else {
       if (m_isOnline)  (summ.m_rejectedLBProfilePart)->Fill(0.5,50);
    }
  } else {
    if (m_isOnline)  (summ.m_rejectedLBProfilePart)->Fill(0.5,0);
  }
  
  if (m_isOnline) {
     (summ.m_rejectedLBProfilePart)->SetEntries(lumi_block);
      //std::cout<<" Setting m_rejectedLBProfilePart Entries: "<<lumi_block<<std::endl;   
  }

  return;
}

//********************************************************************
StatusCode LArFEBMon::bookNewPartitionSumm(summaryPartition& summ,std::string summName)
{
  ATH_MSG_DEBUG( "In bookNewPartitionSumm ->" << summName );
  
  MonGroup perPartitionGroup( this, "/LAr/FEBMon/perPartition", run, ATTRIB_MANAGED );
  MonGroup perPartitionYieldGroup( this, "/LAr/FEBMon/perPartition", run, ATTRIB_MANAGED, "" , "weightedEff" );
  MonGroup perPartitionDataGroup( this, "/LAr/FEBMon/perPartitionData", run, ATTRIB_MANAGED );
  MonGroup perPartitionMiscGroup( this, "/LAr/FEBMon/perPartitionMisc", run, ATTRIB_MANAGED );
  
  int nbOfFT = 25;
  int nbOfSlot = 15;
  if (summName.find("B",0) != std::string::npos){
    nbOfFT = 32;
    nbOfSlot = 14;
  }
  double ftMax = (double) nbOfFT - 0.5;
  double slotMax = (double) nbOfSlot + 0.5;
  
  StatusCode sc = StatusCode::SUCCESS;
  
  // These are error histograms-This should be empty
  // They are stored in /perPartition directory
  std::string hName = "Parity"+summName;
  std::string hTitle = "Parity error - " + summName;  
  summ.parity = TH2I_LW::create(hName.c_str(),hTitle.c_str(),nbOfSlot,0.5,slotMax,nbOfFT,-0.5,ftMax);
  sc = sc && m_strHelper->definePartitionSummProp(summ.parity);
  sc = sc && perPartitionGroup.regHist(summ.parity);
  
  hName = "BCID"+summName;
  hTitle = "BCID mismatch betw. 2 halves of FEB - " + summName;
  summ.BCID_2halves = TH2I_LW::create(hName.c_str(),hTitle.c_str(),nbOfSlot,0.5,slotMax,nbOfFT,-0.5,ftMax);
  sc = sc && m_strHelper->definePartitionSummProp(summ.BCID_2halves);
  sc = sc && perPartitionGroup.regHist(summ.BCID_2halves);
  
  hName = "RADD"+summName;
  hTitle = "Sample header mismatch betw. 2 halves of FEB - " + summName;
  summ.RADD_2halves = TH2I_LW::create(hName.c_str(),hTitle.c_str(),nbOfSlot,0.5,slotMax,nbOfFT,-0.5,ftMax);
  sc = sc && m_strHelper->definePartitionSummProp(summ.RADD_2halves);
  sc = sc && perPartitionGroup.regHist(summ.RADD_2halves);
  
  hName = "EVTID"+summName;
  hTitle = "EVTID mismatch betw. 2 halves of FEB - " + summName;
  summ.EVTID_2halves = TH2I_LW::create(hName.c_str(),hTitle.c_str(),nbOfSlot,0.5,slotMax,nbOfFT,-0.5,ftMax);
  sc = sc && m_strHelper->definePartitionSummProp(summ.EVTID_2halves);
  sc = sc && perPartitionGroup.regHist(summ.EVTID_2halves);
  
  hName = "SCACStatus"+summName;
  hTitle = "Wrong SCAC status in one half of a FEB - " + summName;
  summ.SCACStatus = TH2I_LW::create(hName.c_str(),hTitle.c_str(),nbOfSlot,0.5,slotMax,nbOfFT,-0.5,ftMax);
  sc = sc && m_strHelper->definePartitionSummProp(summ.SCACStatus);
  sc = sc && perPartitionGroup.regHist(summ.SCACStatus);
  
  hName = "scaOutOfRange"+summName;
  hTitle = "Sca out of range - " + summName;
  summ.scaOutOfRange = TH2I_LW::create(hName.c_str(),hTitle.c_str(),nbOfSlot,0.5,slotMax,nbOfFT,-0.5,ftMax);
  sc = sc && m_strHelper->definePartitionSummProp(summ.scaOutOfRange);
  sc = sc && perPartitionGroup.regHist(summ.scaOutOfRange);
  
  hName = "gainMismatch"+summName;
  hTitle = "Gain mismatch within time samples - " + summName;
  summ.gainMismatch = TH2I_LW::create(hName.c_str(),hTitle.c_str(),nbOfSlot,0.5,slotMax,nbOfFT,-0.5,ftMax);
  sc = sc && m_strHelper->definePartitionSummProp(summ.gainMismatch);
  sc = sc && perPartitionGroup.regHist(summ.gainMismatch);
  
  hName = "typeMismatch"+summName;
  hTitle = "Event type mismatch - " + summName;
  summ.typeMismatch = TH2I_LW::create(hName.c_str(),hTitle.c_str(),nbOfSlot,0.5,slotMax,nbOfFT,-0.5,ftMax);
  sc = sc && m_strHelper->definePartitionSummProp(summ.typeMismatch);
  sc = sc && perPartitionGroup.regHist(summ.typeMismatch);
  
  hName = "badNbOfSamp"+summName;
  hTitle = "Non uniform number of samples - " + summName;
  summ.badNbOfSamp = TH2I_LW::create(hName.c_str(),hTitle.c_str(),nbOfSlot,0.5,slotMax,nbOfFT,-0.5,ftMax);
  sc = sc && m_strHelper->definePartitionSummProp(summ.badNbOfSamp);
  sc = sc && perPartitionGroup.regHist(summ.badNbOfSamp);
  
  hName = "zeroSamp"+summName;
  hTitle = "Empty FEB data blocks - " + summName;
  summ.zeroSamp = TH2I_LW::create(hName.c_str(),hTitle.c_str(),nbOfSlot,0.5,slotMax,nbOfFT,-0.5,ftMax);
  sc = sc && m_strHelper->definePartitionSummProp(summ.zeroSamp);
  sc = sc && perPartitionGroup.regHist(summ.zeroSamp);
  
  hName = "checkSum"+summName;
  hTitle = "Checksum / DSP block size - " + summName;
  summ.checkSum = TH2I_LW::create(hName.c_str(),hTitle.c_str(),nbOfSlot,0.5,slotMax,nbOfFT,-0.5,ftMax);
  sc = sc && m_strHelper->definePartitionSummProp(summ.checkSum);
  sc = sc && perPartitionGroup.regHist(summ.checkSum);
  
  hName = "missingHeader"+summName;
  hTitle = "Missing header " + summName;
  summ.missingHeader = TH2I_LW::create(hName.c_str(),hTitle.c_str(),nbOfSlot,0.5,slotMax,nbOfFT,-0.5,ftMax);
  sc = sc && m_strHelper->definePartitionSummProp(summ.missingHeader);
  sc = sc && perPartitionGroup.regHist(summ.missingHeader);
  
  hName = "badGain"+summName;
  hTitle = "Bad gain " + summName;
  summ.badGain = TH2I_LW::create(hName.c_str(),hTitle.c_str(),nbOfSlot,0.5,slotMax,nbOfFT,-0.5,ftMax);
  sc = sc && m_strHelper->definePartitionSummProp(summ.badGain);
  sc = sc && perPartitionGroup.regHist(summ.badGain);
  
  hName = "LArFEBMonErrorsAbsolute"+summName;
  hTitle = "Nb of events with at least one error - " + summName;
  summ.LArAllErrors = TH2I_LW::create(hName.c_str(),hTitle.c_str(),nbOfSlot,0.5,slotMax,nbOfFT,-0.5,ftMax);
  sc = sc && m_strHelper->definePartitionSummProp(summ.LArAllErrors);
  sc = sc && perPartitionGroup.regHist(summ.LArAllErrors);
  
  hName = "LArFEBMonErrors"+summName;
  hTitle = "% of events with at least one error - " + summName;
  summ.LArAllErrorsYield = TH2F_LW::create(hName.c_str(),hTitle.c_str(),nbOfSlot,0.5,slotMax,nbOfFT,-0.5,ftMax);
  sc = sc && m_strHelper->definePartitionSummProp(summ.LArAllErrorsYield);
  sc = sc && perPartitionYieldGroup.regHist(summ.LArAllErrorsYield);
  
  hName = "knownFaultyFEB"+summName;
  hTitle = "FEB with known errors (1:err. ignored 2:FEB masked) - " + summName;  
  summ.maskedFEB = TH2I_LW::create(hName.c_str(),hTitle.c_str(),nbOfSlot,0.5,slotMax,nbOfFT,-0.5,ftMax);
  sc = sc && m_strHelper->definePartitionSummProp(summ.maskedFEB);
  sc = sc && perPartitionGroup.regHist(summ.maskedFEB);
  
  // These are misc histograms-This may be changed in error histograms if needed
  // They are stored in /perPartitionMisc directory
  hName = "missingTriggerType"+summName;
  hTitle = "LVL1 trigger type missing or different from event type - " + summName;  
  summ.missingTriggerType = TH2I_LW::create(hName.c_str(),hTitle.c_str(),nbOfSlot,0.5,slotMax,nbOfFT,-0.5,ftMax);
  sc = sc && m_strHelper->definePartitionSummProp(summ.missingTriggerType);
  sc = sc && perPartitionMiscGroup.regHist(summ.missingTriggerType);
  
  
  // These are general data histograms
  // They are stored in /perPartitionData directory
  hName = "nbOfEvts"+summName;
  hTitle = "Nb of events (DSP header check only) - " + summName;  
  summ.nbOfEvts = TH2I_LW::create(hName.c_str(),hTitle.c_str(),nbOfSlot,0.5,slotMax,nbOfFT,-0.5,ftMax);
  sc = sc && m_strHelper->definePartitionSummProp(summ.nbOfEvts);
  sc = sc && perPartitionDataGroup.regHist(summ.nbOfEvts);
  
  hName = "NbOfSweet1PerFEB"+summName;
  hTitle = "Average # of cells with (qfactor+time) readout - " + summName;    
  summ.nbOfSweet1 = TProfile2D_LW::create(hName.c_str(),hTitle.c_str(),nbOfSlot,0.5,slotMax,nbOfFT,-0.5,ftMax);
  sc = sc && m_strHelper->definePartitionSummProp(summ.nbOfSweet1);
  sc = sc && perPartitionDataGroup.regHist(summ.nbOfSweet1);
  
  hName = "NbOfSweet2PerFEB"+summName;
  hTitle = "Average # of cells with samples readout - " + summName;    
  summ.nbOfSweet2 = TProfile2D_LW::create(hName.c_str(),hTitle.c_str(),nbOfSlot,0.5,slotMax,nbOfFT,-0.5,ftMax);
  sc = sc && m_strHelper->definePartitionSummProp(summ.nbOfSweet2);
  sc = sc && perPartitionDataGroup.regHist(summ.nbOfSweet2);
  
  hName = "nbOfFebBlocks"+summName;
  hTitle = "# of readout FEBs (DSP header check only) - " + summName;  
  summ.nbOfFebBlocks = TH1I_LW::create(hName.c_str(),hTitle.c_str(),600,-99.5,500.5);
  (summ.nbOfFebBlocks)->GetXaxis()->SetTitle("# of readout FEBs");
  sc = sc && perPartitionDataGroup.regHist(summ.nbOfFebBlocks);
  
  int nStreams=m_streams.size();
  if(m_isOnline && nStreams > 0) { // book 2d histo with asked streams 
    hName = "eventSizeStreamVsLB"+summName;
    hTitle = "LAr event size per stream per LB (w/o ROS headers)" + summName;  
    summ.stream_eventSizeLB = TProfile2D_LW::create(hName.c_str(),hTitle.c_str(),m_lumi_blocks,-0.5,(float)m_lumi_blocks+0.5,nStreams+1,-0.5,nStreams+0.5);
    (summ.stream_eventSizeLB)->GetXaxis()->SetTitle("Luminosity Block");
    for (int str = 0; str < nStreams; str++) {
         (summ.stream_eventSizeLB->GetYaxis())->SetBinLabel(str+1,m_streams[str].c_str());
    }
    (summ.stream_eventSizeLB->GetYaxis())->SetBinLabel(nStreams+1,"others");
    sc = sc && perPartitionDataGroup.regHist(summ.stream_eventSizeLB);
  } else { // book simple profile
    hName = "eventSizeVsLB"+summName;
    hTitle = "LAr event size per LB (w/o ROS headers)" + summName;  
    summ.eventSizeLB = TProfile_LW::create(hName.c_str(),hTitle.c_str(),m_lumi_blocks,-0.5,(float)m_lumi_blocks+0.5);
    (summ.eventSizeLB)->GetXaxis()->SetTitle("Luminosity Block");
    sc = sc && perPartitionDataGroup.regHist(summ.eventSizeLB);
  }

  if(m_isOnline) { 
    hName = "EventsRejectedLB"+summName;
    hTitle = " % of events rejected in current LB (online only) - " + summName;  
    summ.m_rejectedLBProfilePart = TProfile_LW::create(hName.c_str(),hTitle.c_str(),1,0,1);
    (summ.m_rejectedLBProfilePart)->GetXaxis()->SetBinLabel(1,"% of events");
    (summ.m_rejectedLBProfilePart)->SetMinimum(0);
    (summ.m_rejectedLBProfilePart)->SetMaximum(100);
    sc = sc && perPartitionDataGroup.regHist(summ.m_rejectedLBProfilePart);
    
    if (sc.isFailure()) {
      ATH_MSG_FATAL( "Unable to book partitions histograms" );
    } 
  } 
  return sc;
}

/*---------------------------------------------------------*/
StatusCode 
LArFEBMon::procHistograms()
{
  
  if (m_eventsCounter != 0 && endOfEventsBlockFlag()){ 
    float scaleFactor = 100./((float) m_eventsCounter); 
    AddHistos(m_rejectedYield,m_rejectedHisto,m_rejectedHisto,scaleFactor,0.);
    m_rejectedYield->SetEntries(m_eventsCounter);
  }
  
  // Reset now done manually to be sure it works online!
  //  if (isEndOfLumiBlockFlag(){
  //    m_rejectedLBProfile->Reset();
  //  }
  
  if (endOfRunFlag() || endOfEventsBlockFlag()){
    // Book dynamically all FEBs in error
    for (int iError = 1;iError<14;iError++){
      fillFebInError(m_barrelCSummary,iError,0,0,"EMBC");
      fillFebInError(m_barrelASummary,iError,0,1,"EMBA");
      fillFebInError(m_emecCSummary,iError,1,0,"EMECC");
      fillFebInError(m_emecASummary,iError,1,1,"EMECA");
      fillFebInError(m_hecCSummary,iError,1,0,"HECC");
      fillFebInError(m_hecASummary,iError,1,1,"HECA");
      fillFebInError(m_fcalCSummary,iError,1,0,"FCALC");
      fillFebInError(m_fcalASummary,iError,1,1,"FCALA");
    }
  }
  
  //  if (isEndOfLumiBlockFlag() && m_iovStart!=0){
  //    m_iovStop=m_iovStop+1; // Add 1second to cope with time needed to resync the QPLL (S.Simion June LAr week)
  //  }
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
void 
LArFEBMon::plotMaskedFEB(){
  
  // Loop over all FEBs
  for (std::vector<HWIdentifier>::const_iterator allFeb = m_onlineHelper->feb_begin(); 
       allFeb != m_onlineHelper->feb_end(); ++allFeb) {
    HWIdentifier febid = HWIdentifier(*allFeb);
    const LArBadFeb febStatus = m_badChannelTool->febStatus(febid);
    int binContent = 0;
    if (febStatus.inError() || febStatus.deadReadout() || febStatus.deadAll() || febStatus.deactivatedInOKS()) binContent = 2;
    if (febStatus.ignoreErrors() > 0)  binContent = 1;
    if (binContent != 0){
      int barrel_ec = m_onlineHelper->barrel_ec(febid);
      int pos_neg   = m_onlineHelper->pos_neg(febid);
      int ft        = m_onlineHelper->feedthrough(febid);
      int slot      = m_onlineHelper->slot(febid);  
      int partitionNb_dE = returnPartition(barrel_ec,pos_neg,ft,slot);
      
      switch (partitionNb_dE){
	case 0:
	  m_barrelCSummary.maskedFEB->SetBinContent(slot,ft+1,binContent);
	  break;
	case 1:
	  m_barrelASummary.maskedFEB->SetBinContent(slot,ft+1,binContent);
	  break;
	case 2:
	  m_emecCSummary.maskedFEB->SetBinContent(slot,ft+1,binContent);
	  break;
	case 3:
	  m_emecASummary.maskedFEB->SetBinContent(slot,ft+1,binContent);
	  break;
	case 4:
	  m_hecCSummary.maskedFEB->SetBinContent(slot,ft+1,binContent);
	  break;
	case 5:
	  m_hecASummary.maskedFEB->SetBinContent(slot,ft+1,binContent);
	  break;
	case 6:
	  m_fcalCSummary.maskedFEB->SetBinContent(slot,ft+1,binContent);
	  break;
	case 7:
	  m_fcalASummary.maskedFEB->SetBinContent(slot,ft+1,binContent);
	  break;
      }
    }
  }
}


/*---------------------------------------------------------*/
void 
LArFEBMon::fillFebInError(const summaryPartition& summ,int errorType,int barrel_ec,int pos_neg,std::string summName)
{
  ATH_MSG_DEBUG( "In fillFebInError" );
  
  //  TH2I* tempHisto = TH2I_LW::create(*summ.parity);
  
  std::string hName = "/LAr/FEBMon/perPartition/FebInErrors/" + summName;
  
  MonGroup generalGroup( this, hName.c_str(), run, ATTRIB_MANAGED);
  
  // Loop on TH2D to find which FEB is in error
  // parity is used to extract binning range. Ok as this is uniform along different errors
  int nbOfFEBErrors = 0;
  
  for (unsigned int ix=1; ix <= (summ.parity)->GetNbinsX();ix++){
    for (unsigned int iy=1; iy <= (summ.parity)->GetNbinsY();iy++){
      // Found a faulty FEB
      // If more than 15 FEBs in error in a partition, ignore other FEBs (mandatory to avoid 
      // creation of 1500  histos when a run is bad!).
      double binContent =0;
      switch (errorType){
	case 1:
	  binContent = (summ.parity)->GetBinContent(ix,iy);
	  break;
	case 2:
	  binContent = (summ.BCID_2halves)->GetBinContent(ix,iy);
	  break;
	case 3:
	  binContent = (summ.RADD_2halves)->GetBinContent(ix,iy);
	  break;
	case 4:
	  binContent = (summ.EVTID_2halves)->GetBinContent(ix,iy);
	  break;
	case 5:
	  binContent = (summ.SCACStatus)->GetBinContent(ix,iy);
	  break;
	case 6:
	  binContent = (summ.scaOutOfRange)->GetBinContent(ix,iy);
	  break;
	case 7:
	  binContent = (summ.gainMismatch)->GetBinContent(ix,iy);
	  break;
	case 8:
	  binContent = (summ.typeMismatch)->GetBinContent(ix,iy);
	  break;
	case 9:
	  binContent = (summ.badNbOfSamp)->GetBinContent(ix,iy);
	  break;
	case 10:
	  binContent = (summ.zeroSamp)->GetBinContent(ix,iy);
	  break;
	case 11:
	  binContent = (summ.checkSum)->GetBinContent(ix,iy);
	  break;
	case 12:
	  binContent = (summ.missingHeader)->GetBinContent(ix,iy);
	  break;
	case 13:
	  binContent = (summ.badGain)->GetBinContent(ix,iy);
	  break;
      }
      
      if (binContent>0 && nbOfFEBErrors < 15){
	HWIdentifier errorFebId = m_onlineHelper->feb_Id(barrel_ec,pos_neg,iy-1,ix);
	IdentifierHash hashId = m_onlineHelper->feb_Hash(errorFebId);
	if (!m_bfebIE[hashId]) {
	  m_febInError[hashId] = TH1F_LW::create((m_strHelper->feb_str(errorFebId)).c_str(),(m_strHelper->feb_str(errorFebId)+"(UNRELIABLE if # of faulty FEBs>15)").c_str(),13,0.5,13.5);
	  //	  tempHisto->Print();	
	  (m_febInError[hashId]->GetXaxis())->SetBinLabel(1,"Parity");
	  (m_febInError[hashId]->GetXaxis())->SetBinLabel(2,"BCID");
	  (m_febInError[hashId]->GetXaxis())->SetBinLabel(3,"Sample Header");
	  (m_febInError[hashId]->GetXaxis())->SetBinLabel(4,"EVTID");
	  (m_febInError[hashId]->GetXaxis())->SetBinLabel(5,"SCAC status");
	  (m_febInError[hashId]->GetXaxis())->SetBinLabel(6,"Sca out of range");
	  (m_febInError[hashId]->GetXaxis())->SetBinLabel(7,"Gain mismatch");
	  (m_febInError[hashId]->GetXaxis())->SetBinLabel(8,"Type mismatch");
	  (m_febInError[hashId]->GetXaxis())->SetBinLabel(9,"# of samples");
	  (m_febInError[hashId]->GetXaxis())->SetBinLabel(10,"Empty data block");
	  (m_febInError[hashId]->GetXaxis())->SetBinLabel(11,"Checksum / block size");
	  (m_febInError[hashId]->GetXaxis())->SetBinLabel(12,"Missing header");
	  (m_febInError[hashId]->GetXaxis())->SetBinLabel(13,"Bad gain");
	  (m_febInError[hashId]->GetYaxis())->SetTitle("Number of errors");
	  
	  StatusCode sc = generalGroup.regHist(m_febInError[hashId]);
	  if (sc.isFailure()) ATH_MSG_ERROR( "Failed to register Feb histogram!" );
	  else m_bfebIE[hashId] = true;
	}
	m_febInError[hashId]->SetBinContent(errorType,binContent);
	m_febInError[hashId]->SetEntries(m_eventsCounter);
        nbOfFEBErrors++;
      }
    }
  }
  
  fillYieldHistos(summ.LArAllErrors,summ.LArAllErrorsYield);
  
  return;
}

/*---------------------------------------------------------*/
void LArFEBMon::fillYieldHistos(TH2I_LW* summaryHisto,TH2F_LW* statusHisto)
// Compute the yield of odd events in error per FEB
// The number of events in error is stored in summaryHisto and the yield is stored in statusHisto
{
  ATH_MSG_DEBUG( "in fillYieldHistos() - " << m_eventsCounter );
  
  if (m_eventsCounter != 0){
    for (unsigned int ix=1;ix<=summaryHisto->GetNbinsX();ix++){
      for (unsigned int iy=1;iy<=summaryHisto->GetNbinsY();iy++){
	// Initialize normalization factor
	float normFactor = (float)m_eventsCounter;
	
	float numer = (float) (summaryHisto->GetBinContent(ix,iy));
	float propOfErrors = numer/normFactor*100;
	statusHisto->SetBinContent(ix,iy,propOfErrors);
	//statusHisto->SetBinError(ix,iy,0);// BT:Winter 12 histo cleaning
      }// End of loop on y axis
    }//End of loop on x axis
  }//End of test on nb of events
  statusHisto->SetEntries(m_eventsCounter);
  
  return;
}

/*---------------------------------------------------------*/
int LArFEBMon::returnPartition(int be,int pn,int ft,int sl){
  // partitionNb_dE = 0 : EMBC / 1 : EMBA / 2 : EMECC / 3 : EMECA / 4 : HECC / 5 : HECA / 6 : FCALC / 7 : FCALA
  int part = be*2+pn;
  if (be == 1){
    // This is a HEC FEB - Dirty method as IsHECOnlineFEBId is buggy!
    if ((ft == 3 || ft == 10 || ft == 16 || ft == 22) && (sl > 2)) part = be*2+pn + 2;
    if (ft == 6) part = be*2 + pn + 4; // This is FCAL FEB
  }        
  return part;
}


/*---------------------------------------------------------*/
void LArFEBMon::AddHistos(TH2F_LW* h0,TH2F_LW* h1,TH2F_LW* h2){
  for (unsigned int ix=1;ix<=h1->GetNbinsX();ix++){
    for (unsigned int iy=1;iy<=h2->GetNbinsY();iy++){
      h0->SetBinContent(ix,iy,h1->GetBinContent(ix,iy)+h2->GetBinContent(ix,iy));
      h0->SetBinError(ix,iy,sqrt(pow(h1->GetBinError(ix,iy),2.0)+pow(h2->GetBinError(ix,iy),2.0)));
    }
  }
}

/*---------------------------------------------------------*/
void LArFEBMon::AddHistos(TH1F_LW* h0,TH1F_LW* h1,TH1F_LW* h2,float s1,float s2){
  for (unsigned int ix=1;ix<=h1->GetNbinsX();ix++){
    h0->SetBinContent(ix,s1*h1->GetBinContent(ix)+s2*h2->GetBinContent(ix));
    // h0->SetBinError(ix,sqrt(pow(s1*s1*h1->GetBinError(ix),2.0)+pow(s2*s2*h2->GetBinError(ix),2.0)));
  }
}


/*---------------------------------------------------------*/
bool LArFEBMon::nbOfFebOK(float nfeb,TH1I_LW* h){
  int ixmax(0);
  float imax(0);
  for (unsigned int ix=1;ix<=h->GetNbinsX();ix++){
    float i=h->GetBinContent(ix);
    if(i>imax){
      imax=i;
      ixmax=ix;
    }
  }
  return (nfeb<((m_FEBmax-m_FEBmin)/m_FEBnbins)*((float)(ixmax-1))+m_FEBmin);
}

