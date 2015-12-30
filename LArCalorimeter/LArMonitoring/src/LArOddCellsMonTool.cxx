/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     LArOddCellsMonTool.cxx
// PACKAGE:  LArMonitoring
//
// AUTHOR:   Benjamin Trocme
//
// Class for monitoring the odd (hot or dead) cells noise
//
// ********************************************************************

#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/IToolSvc.h"

#include "Identifier/IdentifierHash.h"

#include "LArIdentifier/LArOnlineID.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include "LWHists/TProfile2D_LW.h"
#include "TTree.h"

#include <sstream>
#include <iomanip>
#include <cmath>

/*
#include "EventInfo/EventInfo.h"
#include "EventInfo/TriggerInfo.h"
#include "EventInfo/EventID.h"
*/
#include "xAODEventInfo/EventInfo.h"

#include "LArRawEvent/LArDigit.h"
#include "LArRawEvent/LArDigitContainer.h"
#include "LArRawEvent/LArFebHeaderContainer.h"
#include "LArRawConditions/LArDSPThresholdsComplete.h"

#include "LArMonitoring/LArOddCellsMonTool.h"

#include "LArRecEvent/LArNoisyROSummary.h"

#include "LWHists/TH1I_LW.h"
#include "LWHists/TH1D_LW.h"
#include "LWHists/TH2I_LW.h"
#include "LWHists/TH2F_LW.h"

#include "CxxUtils/BasicTypes.h"

//A simply specialised histogram for counting floats indexed by
//integers and with no under/overflow tracking:
class LArOddCellsMonTool::Simple2DSumHist {
public:
  Simple2DSumHist(unsigned nx,unsigned ny) :m_nx(nx),m_ny(ny),m_data(new float[nx*ny]) { assert(nx&&ny); Reset(); }
  void Reset() { std::memset(m_data,0,sizeof(m_data[0])*m_nx*m_ny); }
  void Fill(unsigned ix,unsigned iy,float weight) { if(ix>=m_nx||iy>=m_ny) return;assert(ix*m_ny+iy<m_nx*m_ny);m_data[ix*m_ny+iy]+=weight;}
  double GetBinContent(unsigned ix,unsigned iy) const {if(ix>=m_nx||iy>=m_ny) return 0.0;assert(ix*m_ny+iy<m_nx*m_ny);return m_data[ix*m_ny+iy]; }
  unsigned GetNbinsX() const { return m_nx; }
  unsigned GetNbinsY() const { return m_ny; }
  ~Simple2DSumHist() {delete[] m_data;}
private:
  Simple2DSumHist (const Simple2DSumHist&);
  Simple2DSumHist& operator= (const Simple2DSumHist&);

  unsigned m_nx;
  unsigned m_ny;
  float *m_data;
};

/*---------------------------------------------------------*/
LArOddCellsMonTool::LArOddCellsMonTool(const std::string& type, 
                                         const std::string& name,
                                         const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent),
    m_LArOnlineIDHelper(0),
    m_LArEM_IDHelper(0),
    m_strHelper(0),
    m_LArCablingService("LArCablingService"),
    m_badChannelMask("BadLArDigitMask"),
    m_evtTime(0),
    m_tOdd(0),
    m_treeNbOfOdd(0),
    m_treeBarrelEc(),
    m_treePosNeg(),
    m_treeFT(),
    m_treeSlot(),
    m_treeChannel(),
    m_treeSample(),
    m_treeSampling(),
    m_treeRegion(),
    m_treeEta(),
    m_treePhi(),
    m_treeSignif(),
    m_treeBurstyPartition(),
    m_evtId(0),
    m_eventToBeStored(false),
    m_skipEvent(false),
    m_hMonitoredChan(0),
    m_hTimeDistrib(0),
    m_hOddEventsYield(0)
{
  m_feedthroughNames.clear();
  declareProperty("feedthroughNames", m_feedthroughNames);
  declareProperty("LArPedestalKey",m_larPedestalKey="Pedestal",
		  "SG key of the LArPedestal");
  declareProperty("LArDigitContainerKey", m_LArDigitContainerKey = "HIGH");
  declareProperty("gainMonitored", m_gainMonitored);
  declareProperty("sampleNumber", m_sampleNumber = 0);
  declareProperty("oddnessThreshold", m_oddnessThreshold = 5);
  declareProperty("posNeg", m_posNeg = 0);
  declareProperty("minimumRefNoise", m_minimumRefNoise);
  declareProperty("maximumRefNoise", m_maximumRefNoise);
  declareProperty("burstyEventsThreshold", m_burstyThreshold = 1);
  declareProperty("burstyNoisyROFEB", m_burstyThresholdQ = 2);
  declareProperty("maskBadChannels", m_maskBadChannels = false);
  declareProperty("retrieveFromCool", m_coolRetrieval = true);
  declareProperty("nbEvtsComputePeds", m_computePeds = 1000);
  declareProperty("useEvtCounter", m_useEvtCounter = true);
  declareProperty("maxOfTimingHisto",m_maxTimingHisto = 4000);
  declareProperty("createTree", m_createTree = false);
  declareProperty("monitorInPhysic", m_monPhys = false);
  declareProperty("storeRefs", m_storeRefs = false);
  declareProperty("storePerFT", m_storePerFT = false);
  declareProperty("DetailEndCap",m_detailEndCap = true);
  declareProperty("LArBadChannelMask",m_badChannelMask);
  declareProperty("keyDSPThresholds",m_keyDSPThresholds="LArDSPThresholds");
  declareProperty( "monitoredStreams",m_noise_streams);
  
  m_febPedRetrieved.clear();
  m_eventsCounter = 0;
  for (int i = 0; i<10; i++) m_nbMonitoredChan[i] = 0;
  m_nbMonitoredTotal = 0;
  m_nbOfSamples = 0;
  m_skipAlgo = false;
}

/*---------------------------------------------------------*/
LArOddCellsMonTool::~LArOddCellsMonTool()
{
  delete m_strHelper;
}

/*---------------------------------------------------------*/
StatusCode 
LArOddCellsMonTool::initialize()
{
  //if (msgLvl(MSG::DEBUG)) 
  msg(MSG::WARNING) << "Initialize LArOddCellsMonTool" << endreq;
  
  StatusCode sc;

  sc = detStore()->retrieve(m_LArOnlineIDHelper, "LArOnlineID");
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not get LArOnlineIDHelper" << endreq;
    return sc;
  }

  sc = detStore()->retrieve(m_LArEM_IDHelper, "LArEM_ID");
  if (sc.isFailure()) {
    msg(MSG::FATAL) << "Could not get LArEM_IDHelper" << endreq;
    return sc;
  }
  
  if (m_maskBadChannels){
    sc=m_badChannelMask.retrieve();
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "Could not retrieve BadChannelMask" << m_badChannelMask<< endreq;
      return StatusCode::FAILURE;
    }
  }
  
  // Get LAr Cabling Service
  sc=m_LArCablingService.retrieve();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not retrieve LArCablingService" << endreq;
    return StatusCode::FAILURE;
  }

  // LArOnlineIDStrHelper
  m_strHelper = new LArOnlineIDStrHelper(m_LArOnlineIDHelper);
  m_strHelper->setDefaultNameType(LArOnlineIDStrHelper::LARONLINEID);
  if (msgLvl(MSG::DEBUG)) m_strHelper->test();

  // initialize monitoring bookkeeping info
  sc = this->initMonInfo();
  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Could not initialize monitoring bookkeeping info" << endreq;
  }

  if (m_coolRetrieval) {
    sc =  detStore()->regHandle(m_larPedestal,m_larPedestalKey);
    if (sc.isFailure()) {
      msg(MSG::ERROR) << "could not register handle for pedestal " << endreq;
    }
  }

  ManagedMonitorToolBase::initialize().ignore();

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode 
LArOddCellsMonTool::bookHistograms()
{

  StatusCode sc = StatusCode::SUCCESS;
  
  //if(isNewRun){
    MonGroup generalGroup( this, "LAr/OddCells/General", run, ATTRIB_MANAGED );
    MonGroup generalGroup_wA( this, "LAr/OddCells/General", run, ATTRIB_MANAGED, "", "weightedAverage" );

    // Book general histos
    std::string hName = "OddEventsTime";
    std::string hTitle = "Odd events temporal distribution";
    m_hTimeDistrib = TH1I_LW::create(hName.c_str(), hTitle.c_str(),m_maxTimingHisto,0,m_maxTimingHisto);
    if (m_useEvtCounter) m_hTimeDistrib->GetXaxis()->SetTitle("Event Counter");
    else m_hTimeDistrib->GetXaxis()->SetTitle("Event Id");
    sc = sc && generalGroup.regHist(m_hTimeDistrib);  

    hName = "OddEventsYield";
    hTitle = "Odd channels yield per event - " + format("%.0f",m_oddnessThreshold) + " sigma - " + m_gainMonitored[0] + " / " + m_gainMonitored[1];
//    if (m_oddnessThreshold == 3 && m_posNeg == 0) hTitle = hTitle + " (Gauss : 0.27% exp.)";
//    if (m_oddnessThreshold == 4 && m_posNeg == 0) hTitle = hTitle + " (Gauss : 0.063% exp.)";
//    if (m_oddnessThreshold == 3 && m_posNeg != 0) hTitle = hTitle + " (Gauss : 0.14% exp.)";
//    if (m_oddnessThreshold == 4 && m_posNeg != 0) hTitle = hTitle + " (Gauss : 0.032% exp.)";
    for (uint i=0;i<m_noise_streams.size();i++) hTitle = hTitle + " - " + m_noise_streams[i];
    m_hOddEventsYield = TH1D_LW::create(hName.c_str(), hTitle.c_str(),500,-0.2,2.3);
    m_hOddEventsYield->GetXaxis()->SetTitle("Yield(%)");
    sc = sc && generalGroup.regHist(m_hOddEventsYield);  
    
    hName = "NbOfMonitoredChan";
    hTitle = "Nb of monitored channels";
    if (m_coolRetrieval) hTitle = hTitle + " - Pedestals from COOL ";
    else hTitle = hTitle + " - Pedestals from " + format("%d",m_computePeds) + " first events";
    if (m_sampleNumber<0) hTitle = hTitle +" - All samples";
    else hTitle = hTitle +" - Sample " + format("%1d only",m_sampleNumber);

    if (m_detailEndCap){
      m_hMonitoredChan = TH1I_LW::create(hName.c_str(), hTitle.c_str(),8,0.5,8.5);
      (m_hMonitoredChan->GetXaxis())->SetBinLabel(4,"Emec A");
      (m_hMonitoredChan->GetXaxis())->SetBinLabel(3,"Emec C");
      (m_hMonitoredChan->GetXaxis())->SetBinLabel(6,"Hec A");
      (m_hMonitoredChan->GetXaxis())->SetBinLabel(5,"Hec C");
      (m_hMonitoredChan->GetXaxis())->SetBinLabel(8,"Fcal A");
      (m_hMonitoredChan->GetXaxis())->SetBinLabel(7,"Fcal C");
    }
    else{
      m_hMonitoredChan = TH1I_LW::create(hName.c_str(), hTitle.c_str(),4,0.5,4.5);
      (m_hMonitoredChan->GetXaxis())->SetBinLabel(4,"Endcap A");
      (m_hMonitoredChan->GetXaxis())->SetBinLabel(3,"Endcap C");
    }
    (m_hMonitoredChan->GetXaxis())->SetBinLabel(2,"Barrel A");
    (m_hMonitoredChan->GetXaxis())->SetBinLabel(1,"Barrel C");

    sc = sc && generalGroup_wA.regHist(m_hMonitoredChan);
        
    if (m_createTree){
      // Book tree
      m_tOdd = new TTree("OddEventsTree","OddEventsTree");
      sc = sc && generalGroup.regTree(m_tOdd);
      m_tOdd->Branch("EvtCnt",&m_eventsCounter,"m_eventsCounter/I");
      m_tOdd->Branch("EvtId",&m_evtId,"m_evtId/I");
      m_tOdd->Branch("EvtTime",&m_evtTime,"m_evtTime/F");
      m_tOdd->Branch("NbOfOdd",&m_treeNbOfOdd,"m_treeNbOfOdd/I");
      m_tOdd->Branch("NbOfMonitoredChTotal",&m_nbMonitoredTotal,"m_nbMonitoredTotal/I");
      m_tOdd->Branch("NbOfMonitoredChPerPart",m_nbMonitoredChan,"m_nbMonitoredChan[10]/I");
      m_tOdd->Branch("BarrelEc",m_treeBarrelEc,"m_treeBarrelEc[m_treeNbOfOdd]/I");
      m_tOdd->Branch("PosNeg",m_treePosNeg,"m_treePosNeg[m_treeNbOfOdd]/I");
      m_tOdd->Branch("FT",m_treeFT,"m_treeFT[m_treeNbOfOdd]/I");
      m_tOdd->Branch("Slot",m_treeSlot,"m_treeSlot[m_treeNbOfOdd]/I");
      m_tOdd->Branch("Channel",m_treeChannel,"m_treeChannel[m_treeNbOfOdd]/I");
      m_tOdd->Branch("Sample",m_treeSample,"m_treeSample[m_treeNbOfOdd]/I");
      m_tOdd->Branch("Sampling",m_treeSampling,"m_treeSampling[m_treeNbOfOdd]/I");
      m_tOdd->Branch("Region",m_treeRegion,"m_treeRegion[m_treeNbOfOdd]/I");
      m_tOdd->Branch("Eta",m_treeEta,"m_treeEta[m_treeNbOfOdd]/I");
      m_tOdd->Branch("Phi",m_treePhi,"m_treePhi[m_treeNbOfOdd]/I");
      m_tOdd->Branch("Signif",m_treeSignif,"m_treeSignif[m_treeNbOfOdd]/F");
      m_tOdd->Branch("BurstyPartition",m_treeBurstyPartition,"m_treeBurstyPartition[10]/I");
    }
    
    bookHistsPartition(m_barrelCSummary,"BarrelC");
    bookHistsPartition(m_barrelASummary,"BarrelA");
    bookHistsPartition(m_endcapCSummary,"EndcapC");
    bookHistsPartition(m_endcapASummary,"EndcapA");
    if (m_detailEndCap){
      bookHistsPartition(m_emecCSummary,"EmecC");
      bookHistsPartition(m_emecASummary,"EmecA");
      bookHistsPartition(m_hecCSummary,"HecC");
      bookHistsPartition(m_hecASummary,"HecA");
      bookHistsPartition(m_fcalCSummary,"FcalC");
      bookHistsPartition(m_fcalASummary,"FcalA");      
    }
    //  }

  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Bookhistograms failed" << endreq;
  }

  return sc;
}

/*---------------------------------------------------------*/
StatusCode 
LArOddCellsMonTool::fillHistograms()
{
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in fillHists()" << endreq;

  StatusCode sc(StatusCode::SUCCESS); 

  // At first event, if required (m_monPhys), check whether the data are in physic mode
  // with non zero DSP threshold. If so, request to skip the algo.
  if (!m_monPhys && m_eventsCounter == 0){
    const LArFebHeaderContainer* hdrCont;
    sc = evtStore()->retrieve(hdrCont);
    if (sc.isFailure() || !hdrCont) {
      msg(MSG::WARNING) << "No LArFEB container found in TDS" << endreq; 
      return sc;
    }
    LArFebHeaderContainer::const_iterator it = hdrCont->begin(); 
    LArFebHeaderContainer::const_iterator it_e = hdrCont->end(); 
    for ( ; it!=it_e;++it) {
      uint32_t eventType = (*it)->DetEventType();
      // Check if we are in physic mode and not in rawdata+results
      if (eventType == 4 && (*it)->RodRawDataSize() == 0){
	const LArDSPThresholdsComplete* dspThresh=NULL;
	if (m_keyDSPThresholds.size()>0) {
	  sc=detStore()->retrieve(dspThresh,m_keyDSPThresholds);
	  if(!sc.isSuccess()){
	    msg(MSG::WARNING) <<" Failed to retrieve LArDSPThresholds with key " << m_keyDSPThresholds << endreq;
	    return sc; 
	  }
	  HWIdentifier febid=(*it)->FEBId();
	  int barrel_ec = m_LArOnlineIDHelper->barrel_ec(febid);
	  int pos_neg   = m_LArOnlineIDHelper->pos_neg(febid);
	  int ft        = m_LArOnlineIDHelper->feedthrough(febid);
	  int slot      = m_LArOnlineIDHelper->slot(febid);
	  for (int ich=0;ich<128;ich++){
	    HWIdentifier chId = m_LArOnlineIDHelper->channel_Id(barrel_ec,pos_neg,ft,slot,ich);
	    // If any threshold (samples) is different from zero, skip algorithm
	    if (dspThresh->samplesThr(chId)) m_skipAlgo = true;
	  }
	}
      }
    }
    if (m_skipAlgo){
      m_hMonitoredChan->SetTitle("Not available : physic mode with non zero DSP threshold");
      m_hOddEventsYield->SetTitle("Not available : physic mode with non zero DSP threshold");
      m_hTimeDistrib->SetTitle("Not available : physic mode with non zero DSP threshold");
    }
  }

  if (m_skipAlgo){
    return sc;
  } 

  // Increment event counter and initialize the global number of odd samples for the tree
  m_eventsCounter++;
  // If a yield in any partition is greater m_burstyThreshold, store the event in ntuple (if activated in jO)
  m_eventToBeStored = false;

  const xAOD::EventInfo* thisEventInfo;
  sc=evtStore()->retrieve(thisEventInfo);
  if (sc!=StatusCode::SUCCESS)
    msg(MSG::WARNING) << "No EventInfo object found! Can't read run number!" << endreq;
  else{
    m_evtId = thisEventInfo->eventNumber();
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Event nb " << thisEventInfo->eventNumber() << endreq;
    int Ofst = thisEventInfo->timeStamp()/(24*3600);
    m_evtTime = float(thisEventInfo->timeStamp() - (Ofst*24*3600))*(1./60);

    // If stream awareness is enabled, retrieve stream info    
    if (m_noise_streams.size() != 0){
      m_skipEvent = true;
      const std::vector<xAOD::EventInfo::StreamTag>& event_stream_tags = thisEventInfo->streamTags();
      for (uint i=0;i<event_stream_tags.size();i++){
	for (uint j=0;j<m_noise_streams.size();j++){
	  msg(MSG::DEBUG) << "This is Stream Tag: " << event_stream_tags[i].type() << "_" << event_stream_tags[i].name()  << endreq;
	  if( m_noise_streams[j] == event_stream_tags[i].name()) {
	    m_skipEvent = false;
	    break;
	  }
	}
      }// End of double loop on streams
      if (m_skipEvent){
	return sc;
      }  
    }// End of (if stream awareness)
  }
  
  m_treeNbOfOdd = 0; // Used only in the case of tree filling
  // The index corresponds to the partitionNb; the 2 last one are related to 
  // the whole endcap and are filled at the end of the event
  double oddEventYield[10] = {0,0,0,0,0,0,0,0,0,0};
  double nbOfBurstyFEB[10] = {0,0,0,0,0,0,0,0,0,0};
  for(int i=0;i<10;i++) m_treeBurstyPartition[i] = 0;
    
  // retrieve LArDigitContainer
  const LArDigitContainer* pLArDigitContainer;
  sc = evtStore()->retrieve(pLArDigitContainer, m_LArDigitContainerKey);
  if (sc.isFailure()) {
    msg(MSG::WARNING) << "Can\'t retrieve LArDigitContainer with key " 
        << m_LArDigitContainerKey << endreq;
    return StatusCode::SUCCESS;
  }


  // loop over LArDigit to retrieve data
  LArDigitContainer::const_iterator itDig = pLArDigitContainer->begin(); 
  LArDigitContainer::const_iterator itDig_e= pLArDigitContainer->end(); 
  const LArDigit* pLArDigit;
  for ( ; itDig!=itDig_e;++itDig) {
    pLArDigit = *itDig;

    HWIdentifier id = pLArDigit->hardwareID();
      
    HWIdentifier febID = m_LArOnlineIDHelper->feb_Id(id);
    HWIdentifier feedthroughID = m_LArOnlineIDHelper->feedthrough_Id(id);
    IdentifierHash feedthroughHash = m_LArOnlineIDHelper->feedthrough_Hash(feedthroughID);
    int channel = m_LArOnlineIDHelper->channel(id);
    int slot = m_LArOnlineIDHelper->slot(febID);
    int feedthrough = m_LArOnlineIDHelper->feedthrough(feedthroughID);
    int barrEc = m_LArOnlineIDHelper->barrel_ec(feedthroughID);
    int posNeg = m_LArOnlineIDHelper->pos_neg(feedthroughID);
    int partitionNb = barrEc*2 + posNeg;

    int refNoiseIndex = 0;

    if (barrEc == 1 && (feedthrough == 3 || feedthrough == 10 || feedthrough == 16 || feedthrough == 22) && (slot > 2)){
      partitionNb = barrEc*2 + posNeg + 2;// This is HEC FEB
      refNoiseIndex = 1;
    }

    if (barrEc == 1 && feedthrough == 6) partitionNb = barrEc*2 + posNeg + 4;// This is FCAL FEB

    CaloGain::CaloGain gain = pLArDigit->gain();
    feedthroughInfo& feedthroughinfo = m_feedthroughInfo[feedthroughHash][gain];
    
    // check if corresponding feedthrough is to be monitored
    if (feedthroughinfo.toBeMonitored) {
      // check if corresponding feedthrough histos are booked
      if (!feedthroughinfo.bookedHists) {
	// if not, book on demand
	sc = this->bookPerFeedthrough(feedthroughID, gain);
	if (sc.isFailure()) {
	  msg(MSG::FATAL) << "Problem booking histos for feedthroughID " << std::hex << feedthroughID << std::dec << endreq;
	  return StatusCode::FAILURE;
	}
      }// End of booking on demand

      // check if corresponding feb pedestals have been retrieved - If not do it
      if (m_coolRetrieval){
	HWIdentifier febID = m_LArOnlineIDHelper->feb_Id(id);
	IdentifierHash febHash = m_LArOnlineIDHelper->feb_Hash(febID);	
	if (!m_febPedRetrieved[febHash][gain]){
	  sc = this->retrievePedPerFeb(febID, feedthroughID, gain);	
	  if (sc.isFailure()) msg(MSG::FATAL) << "Problem when retrieving the pedestals" << endreq;
	}// End of pedestal retrieving
      }

      // If the reference pedestal/noise are not retrieved from the cool
      // they are computed from the first m_computePeds events
      if (!(m_coolRetrieval) && m_eventsCounter<=m_computePeds){
	const std::vector < short > samples = pLArDigit->samples();
	if (samples.size() > 0) feedthroughinfo.hReference->Fill(slot,channel,samples[0]);
	if (m_eventsCounter == m_computePeds){
	  double computedNoise = feedthroughinfo.hReference->GetBinError(slot,channel+1);
	  // Bad channel masking added on 23/4/08
	  bool bool_badChannel = true;
	  if (m_maskBadChannels){
	    if (m_badChannelMask->cellShouldBeMasked(id)) bool_badChannel = false;
	  }
	  if (computedNoise>m_minimumRefNoise[refNoiseIndex] && computedNoise<m_maximumRefNoise[refNoiseIndex] && bool_badChannel){
	    // Add 1 to the number of cells monitored per FEB
	    fillFeedthroughHistos(feedthroughID,slot,1,true);
	  }
	  else{
	    // The channels is not monitored (noise outside bounds or bad channel masked)
	    // The error is arbitrary set to 0 to ignore odd events here
	    feedthroughinfo.hReference->SetBinError(slot,channel+1,0);
	  }
	}
      }
      else {
	// Pedestal have been retrieved from COOL or computed from first events - Therefore now look for odd cells
	// First extract ped/noise from reference histos
	double refPed;
	double refNoise;
	if (m_coolRetrieval){
	  feedthroughinfo.hReferenceCool->GetBinContentAndError(slot,channel+1,refPed,refNoise);
	}
	else{
	  refPed = feedthroughinfo.hReference->GetBinContent(slot,channel+1);
	  refNoise = feedthroughinfo.hReference->GetBinError(slot,channel+1);
	}
	// The check below is mandatory to treat FEB with less than 128 channels
	// In this case, the reference pedestal was set to 0
	if (refPed != 0 && refNoise != 0){
	  const std::vector < short > samples = pLArDigit->samples();
	  int nbOfOddSamples = 0;
	  m_nbOfSamples = (int) samples.size();
	  // Count the nb of odd samples - Consider only one sample
	  if (m_sampleNumber >= 0 && m_sampleNumber<m_nbOfSamples) {
	    double signif = 0;
	    double signedSignif = (double(samples[m_sampleNumber]) - refPed) / refNoise;
	    if (m_posNeg == 0 ) signif = fabs(signedSignif);
	    if (std::abs(m_posNeg) == 1 ) signif = m_posNeg * signedSignif;
	    // Store temporarily ADC-Ped for coherent noise analysis (have to sum whole FEB before looking for odd sums)
	    feedthroughinfo.hTempSumComput->Fill(slot-1,m_sampleNumber,double(samples[m_sampleNumber]) - refPed);
	    
	    if (signif>m_oddnessThreshold) {
	      nbOfOddSamples++;

	      if (m_createTree && m_treeNbOfOdd<60000){
		//Retrieve offline ID
		Identifier offlineId;      
		try {
		  offlineId = m_LArCablingService->cnvToIdentifier(id); 
		}
		catch(LArID_Exception& except) {
		  msg(MSG::ERROR) << "LArID_Exception " << (std::string) except << endreq ;
		}

		m_treeBarrelEc[m_treeNbOfOdd] = barrEc;
		m_treePosNeg[m_treeNbOfOdd] = posNeg;
		m_treeFT[m_treeNbOfOdd] = feedthrough;
		m_treeSlot[m_treeNbOfOdd] = slot;
		m_treeChannel[m_treeNbOfOdd] = channel;
		m_treeSample[m_treeNbOfOdd] = m_sampleNumber;
		m_treeSignif[m_treeNbOfOdd] = signedSignif;
		m_treeSampling[m_treeNbOfOdd] = m_LArEM_IDHelper->sampling(offlineId);
		m_treeRegion[m_treeNbOfOdd] = m_LArEM_IDHelper->region(offlineId);
		m_treeEta[m_treeNbOfOdd] = m_LArEM_IDHelper->eta(offlineId);
		m_treePhi[m_treeNbOfOdd] = m_LArEM_IDHelper->phi(offlineId);
		
		m_treeNbOfOdd++;
	      }// End of tree treatment
	    }// End of (if oddevents exist)
	  } else {// Consider all samples (see joboption) 
	    for (int j = 0; j <(m_nbOfSamples); ++j) {
	      double signif = 0;
	      double signedSignif = (double) ((samples[j] - refPed)/refNoise);
	      if (m_posNeg == 0) signif = fabs(signedSignif);
	      if (std::abs(m_posNeg) == 1) signif = m_posNeg * (signedSignif);
	      // Store temporarily ADC-Ped for coherent noise analysis (have to sum whole FEB before looking for odd sums)
	      feedthroughinfo.hTempSumComput->Fill(slot-1,j,double(samples[j]) - refPed);

	      if (signif>m_oddnessThreshold){
		nbOfOddSamples++;	    

		if (m_createTree && m_treeNbOfOdd<60000){
		  //Retrieve offline ID
		  Identifier offlineId;      
		  try {
		    offlineId = m_LArCablingService->cnvToIdentifier(id); 
		  }
		  catch(LArID_Exception& except) {
		    msg(MSG::ERROR) << "LArID_Exception " << (std::string) except << endreq ;
		  }      

		  m_treeBarrelEc[m_treeNbOfOdd] = barrEc;
		  m_treePosNeg[m_treeNbOfOdd] = posNeg;
		  m_treeFT[m_treeNbOfOdd] = feedthrough;
		  m_treeSlot[m_treeNbOfOdd] = slot;
		  m_treeChannel[m_treeNbOfOdd] = channel;
		  m_treeSample[m_treeNbOfOdd] = j;
		  m_treeSignif[m_treeNbOfOdd] = signedSignif;
		  m_treeSampling[m_treeNbOfOdd] = m_LArEM_IDHelper->sampling(offlineId);
		  m_treeRegion[m_treeNbOfOdd] = m_LArEM_IDHelper->region(offlineId);
		  m_treeEta[m_treeNbOfOdd] = m_LArEM_IDHelper->eta(offlineId);
		  m_treePhi[m_treeNbOfOdd] = m_LArEM_IDHelper->phi(offlineId);
		  
		  m_treeNbOfOdd++;
		}// End of tree treatment
	      }// End of (if oddevents exist)
	    }// End of loop on all samples
	  }// End of (all samples case)

	  // Some odd samples have been found
	  if (nbOfOddSamples != 0) {
	    // Fill base histograms
	    oddEventYield[partitionNb] = oddEventYield[partitionNb] + ((double) nbOfOddSamples);
	    // If m_useEvtCounter is true, fill event counter instead for event id
	    // Useful for cosmics runs where evt id might be huge!
	    if (m_useEvtCounter) m_hTimeDistrib->Fill(m_eventsCounter,nbOfOddSamples);
	    else m_hTimeDistrib->Fill(m_evtId,nbOfOddSamples);
	    feedthroughinfo.NbOddPerChan->Fill(slot,channel,nbOfOddSamples);	  
	    // Fill summary histogram
	    fillFeedthroughHistos(feedthroughID,slot,nbOfOddSamples);
	  }// End of histogram filling
	}// End of (if refPed != 0)
      }// End of search of oddcells (cf treatment of m_coolRetrieval)
    }// End of feedthrough treatment (if is to be monitored)
  }// End of loop on LArDigit


  // If a search for odd cells have been performed, fill the yield, and search for odd sums
  // and fill the tree, if requested
  if (!(!(m_coolRetrieval) && m_eventsCounter<=m_computePeds)){

    // Retrieve bursty event based on q factor
    LArNoisyROSummary* noisyRO;
    sc = evtStore()->retrieve(noisyRO,"LArNoisyROSummary");
    if (sc.isFailure())
      {
	msg(MSG::WARNING) << "Can't retrieve LArNoisyROSummary " <<endreq;
	return StatusCode::SUCCESS;
      }    
    const std::vector<HWIdentifier>& noisyFEB = noisyRO->get_noisy_febs();    

    for (size_t i = 0; i<noisyFEB.size(); i++){
	const HWIdentifier& id = noisyFEB[i];
	int barrEc = m_LArOnlineIDHelper->barrel_ec(id);
	int posNeg = m_LArOnlineIDHelper->pos_neg(id);
	int slot = m_LArOnlineIDHelper->slot(id);
	int feedthrough = m_LArOnlineIDHelper->feedthrough(id);
	int partitionNb = barrEc*2 + posNeg;
	if (barrEc == 1 && (feedthrough == 3 || feedthrough == 10 || feedthrough == 16 || feedthrough == 22) && (slot > 2))
	  partitionNb = barrEc*2 + posNeg + 2;// This is HEC FEB
	if (barrEc == 1 && feedthrough == 6) partitionNb = barrEc*2 + posNeg + 4;// This is FCAL FEB
	nbOfBurstyFEB[partitionNb]++;
    }

    // Yield computation
    // First count nb of odd events in the endcap by summing different sub partition
    oddEventYield[8] = oddEventYield[2] + oddEventYield[4] + oddEventYield[6];
    oddEventYield[9] = oddEventYield[3] + oddEventYield[5] + oddEventYield[7];
    // Total nb of events
    double overallYield = oddEventYield[0] + oddEventYield[1] + oddEventYield[8] + oddEventYield[9];
    // Compute for each partition the nb of odd cells
    for (int iPart=0; iPart<10; iPart++){
      // Check that the nb of monitored channels and samples are different from 0
      if (m_nbMonitoredChan[iPart] != 0 && m_nbOfSamples != 0){
	// Then renormalize the yield
	if (m_sampleNumber < 0 || m_sampleNumber >= m_nbOfSamples) oddEventYield[iPart] = 100*oddEventYield[iPart]/((double) (m_nbMonitoredChan[iPart]*m_nbOfSamples));  
	else oddEventYield[iPart] = 100*oddEventYield[iPart]/((double) m_nbMonitoredChan[iPart]);
      }
      if (oddEventYield[iPart] > m_burstyThreshold){
	m_treeBurstyPartition[iPart] = m_treeBurstyPartition[iPart] + 1;
      }
      if (nbOfBurstyFEB[iPart] >= m_burstyThresholdQ){
	m_treeBurstyPartition[iPart] = m_treeBurstyPartition[iPart] + 2;
      }
    }

    // Fill the yield of odd events per partition for all partition
    fillYieldPerEvent(m_barrelCSummary,oddEventYield[0],m_treeBurstyPartition[0]);
    fillYieldPerEvent(m_barrelASummary,oddEventYield[1],m_treeBurstyPartition[1]);
    fillYieldPerEvent(m_endcapCSummary,oddEventYield[8],m_treeBurstyPartition[8]);
    fillYieldPerEvent(m_endcapASummary,oddEventYield[9],m_treeBurstyPartition[9]);
    if (m_detailEndCap){
      fillYieldPerEvent(m_emecCSummary,oddEventYield[2],m_treeBurstyPartition[2]);
      fillYieldPerEvent(m_emecASummary,oddEventYield[3],m_treeBurstyPartition[3]);
      fillYieldPerEvent(m_hecCSummary,oddEventYield[4],m_treeBurstyPartition[4]);
      fillYieldPerEvent(m_hecASummary,oddEventYield[5],m_treeBurstyPartition[5]);
      fillYieldPerEvent(m_fcalCSummary,oddEventYield[6],m_treeBurstyPartition[6]);
      fillYieldPerEvent(m_fcalASummary,oddEventYield[7],m_treeBurstyPartition[7]);
    }
    // And the total yield
    if (m_nbMonitoredTotal != 0 && m_nbOfSamples != 0){
      // Then renormalize the yield
	if (m_sampleNumber < 0 || m_sampleNumber >= m_nbOfSamples) overallYield = 100*overallYield/((double) (m_nbMonitoredTotal*m_nbOfSamples));  
	else overallYield = 100*overallYield/((double) m_nbMonitoredTotal);  
    }    
    m_hOddEventsYield->Fill(overallYield);

    msg(MSG::DEBUG) << "End of event - Fill coherent noise histos" << endreq;

    std::vector<CaloGain::CaloGain> gains;
    gains.push_back(CaloGain::LARHIGHGAIN);
    gains.push_back(CaloGain::LARMEDIUMGAIN);
    //    gains.push_back(CaloGain::LARLOWGAIN); 
    
    for (unsigned int ftHash = 0; ftHash < m_LArOnlineIDHelper->feedthroughHashMax(); ++ftHash) {
      std::vector<CaloGain::CaloGain>::iterator pGainEnd(gains.end());
      for (std::vector<CaloGain::CaloGain>::iterator pGain = gains.begin();pGain != pGainEnd; ++pGain) {
	CaloGain::CaloGain gain = *pGain;
	feedthroughInfo& ftinfo = m_feedthroughInfo[ftHash][gain];
	if (ftinfo.bookedHists) {
	  fillSumPerFEBHistos(ftinfo);
	  ftinfo.hTempSumComput->Reset();		  
	}
      }
    }
    
    // Tree filling
    if (m_createTree && m_eventToBeStored){
      m_tOdd->Fill();
    }
  }

  return sc;
}
 
/*---------------------------------------------------------*/
StatusCode 
LArOddCellsMonTool::checkHists(bool fromFinalize)
{
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in checkHists() - " << m_eventsCounter << " events - Finalize ?" << fromFinalize << endreq;

  if (m_eventsCounter == 0) return StatusCode::SUCCESS;

  std::vector<CaloGain::CaloGain> gains;
  gains.push_back(CaloGain::LARHIGHGAIN);
  gains.push_back(CaloGain::LARMEDIUMGAIN);
  //  gains.push_back(CaloGain::LARLOWGAIN); 

  // Loop over all FT to compute yield of odd events per channel and extract nb of odd channels
  const unsigned hashmax(m_LArOnlineIDHelper->feedthroughHashMax());
  for (unsigned int ftHash = 0; ftHash < hashmax; ++ftHash) {
    std::vector<CaloGain::CaloGain>::iterator pGainEnd(gains.end());
    for (std::vector<CaloGain::CaloGain>::iterator pGain = gains.begin();pGain != pGainEnd; ++pGain) {
      CaloGain::CaloGain gain = *pGain;
      feedthroughInfo& ftinfo = m_feedthroughInfo[ftHash][gain];

      if (ftinfo.bookedHists){ 
	// Compute the yield of odd events
	fillYieldHistos(ftinfo.NbOddPerChan,ftinfo.YieldPerChan);
	copyChanYield(ftinfo);
      }
    }    
  }// End of loop on all all FT

  // Fill yield of odd events per FEB
  fillYieldHistos(m_barrelCSummary.NbOddPerFEB,m_barrelCSummary.YieldPerFEB,m_barrelCSummary.CellNbPerSlot);
  fillYieldHistos(m_barrelASummary.NbOddPerFEB,m_barrelASummary.YieldPerFEB,m_barrelASummary.CellNbPerSlot);
  fillYieldHistos(m_endcapCSummary.NbOddPerFEB,m_endcapCSummary.YieldPerFEB,m_endcapCSummary.CellNbPerSlot);
  fillYieldHistos(m_endcapASummary.NbOddPerFEB,m_endcapASummary.YieldPerFEB,m_endcapASummary.CellNbPerSlot);

  if (m_detailEndCap){
    fillYieldHistos(m_emecCSummary.NbOddPerFEB,m_emecCSummary.YieldPerFEB,m_emecCSummary.CellNbPerSlot);
    fillYieldHistos(m_emecASummary.NbOddPerFEB,m_emecASummary.YieldPerFEB,m_emecASummary.CellNbPerSlot);    
    fillYieldHistos(m_hecCSummary.NbOddPerFEB,m_hecCSummary.YieldPerFEB,m_hecCSummary.CellNbPerSlot);
    fillYieldHistos(m_hecASummary.NbOddPerFEB,m_hecASummary.YieldPerFEB,m_hecASummary.CellNbPerSlot);    
    fillYieldHistos(m_fcalCSummary.NbOddPerFEB,m_fcalCSummary.YieldPerFEB,m_fcalCSummary.CellNbPerSlot);
    fillYieldHistos(m_fcalASummary.NbOddPerFEB,m_fcalASummary.YieldPerFEB,m_fcalASummary.CellNbPerSlot);    
  }

  // Fill yield of odd sums per FEB
  fillYieldHistos(m_barrelCSummary.NbOddSumPerFEB,m_barrelCSummary.YieldSumPerFEB);
  fillYieldHistos(m_barrelASummary.NbOddSumPerFEB,m_barrelASummary.YieldSumPerFEB);
  fillYieldHistos(m_endcapCSummary.NbOddSumPerFEB,m_endcapCSummary.YieldSumPerFEB);
  fillYieldHistos(m_endcapASummary.NbOddSumPerFEB,m_endcapASummary.YieldSumPerFEB);

  if (m_detailEndCap){
    fillYieldHistos(m_emecCSummary.NbOddSumPerFEB,m_emecCSummary.YieldSumPerFEB);
    fillYieldHistos(m_emecASummary.NbOddSumPerFEB,m_emecASummary.YieldSumPerFEB);    
    fillYieldHistos(m_hecCSummary.NbOddSumPerFEB,m_hecCSummary.YieldSumPerFEB);
    fillYieldHistos(m_hecASummary.NbOddSumPerFEB,m_hecASummary.YieldSumPerFEB);    
    fillYieldHistos(m_fcalCSummary.NbOddSumPerFEB,m_fcalCSummary.YieldSumPerFEB);
    fillYieldHistos(m_fcalASummary.NbOddSumPerFEB,m_fcalASummary.YieldSumPerFEB);    
  }

  // At the end of the job,deregister temporary sum computation histos and 
  // reference histos (according to the job option value)
  if (fromFinalize){
    std::vector<CaloGain::CaloGain> gains;
    gains.push_back(CaloGain::LARHIGHGAIN);
    gains.push_back(CaloGain::LARMEDIUMGAIN);
    //    gains.push_back(CaloGain::LARLOWGAIN); 
    for (unsigned int feedthroughHash = 0; feedthroughHash < m_LArOnlineIDHelper->feedthroughHashMax(); ++feedthroughHash) {
      std::vector<CaloGain::CaloGain>::iterator pGainEnd(gains.end());
      for (std::vector<CaloGain::CaloGain>::iterator pGain = gains.begin();pGain != pGainEnd; ++pGain) {      
	CaloGain::CaloGain gain = *pGain;
	feedthroughInfo& ftinfo = m_feedthroughInfo[feedthroughHash][gain];
	delete ftinfo.hTempSumComput;
	if (!m_storeRefs) {
	  if ((ftinfo).bookedHists){ 
	    deregisterHisto(ftinfo.hReference);
	    deregisterHisto(ftinfo.hReferenceCool);
	    deregisterHisto(ftinfo.hReferenceCool_sum2);
	    //deregisterHisto(ftinfo.hTempSumComput);
	  }
	}
	if (!m_storePerFT) {
	  if ((ftinfo).bookedHists){ 
	    deregisterHisto(ftinfo.NbOddPerChan);
	    deregisterHisto(ftinfo.YieldPerChan);
	  }
	}
      }
    }
    deregisterHisto(m_barrelCSummary.CellNbPerSlot);
    deregisterHisto(m_barrelASummary.CellNbPerSlot);
    deregisterHisto(m_endcapCSummary.CellNbPerSlot);
    deregisterHisto(m_endcapASummary.CellNbPerSlot);
    if (m_detailEndCap){
      deregisterHisto(m_emecCSummary.CellNbPerSlot);
      deregisterHisto(m_emecASummary.CellNbPerSlot);
      deregisterHisto(m_hecCSummary.CellNbPerSlot);
      deregisterHisto(m_hecASummary.CellNbPerSlot);
      deregisterHisto(m_fcalCSummary.CellNbPerSlot);
      deregisterHisto(m_fcalASummary.CellNbPerSlot);      
    }
  }
  
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "CheckHists() Over " << endreq;
  
  return StatusCode::SUCCESS;
}
/*---------------------------------------------------------*/
StatusCode 
LArOddCellsMonTool::procHistograms()
{
  if (msgLvl(MSG::DEBUG)) {
    if (m_sampleNumber <0 || m_sampleNumber>=m_nbOfSamples) msg(MSG::DEBUG) << "I have monitored " << m_nbOfSamples << " samples of " << m_nbMonitoredTotal << " channels" ;
    else msg(MSG::DEBUG) << "I have monitored sample " << m_sampleNumber << " of " << m_nbMonitoredTotal << " channels";

    if (m_coolRetrieval) msg(MSG::DEBUG) << " with pedestal/noise extracted from COOL" << endreq;
    else msg(MSG::DEBUG) << " with pedestal/noise computed from " << m_computePeds << " first events from this run" << endreq;
  }

  
  StatusCode sc = StatusCode::SUCCESS;
  if (!m_skipAlgo){
    sc = this->checkHists(false);
    if(sc.isFailure()) {
      msg(MSG::ERROR) << "checkHists in finalHists returned failure!" << endreq;
    }
  }
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "End of procHistograms " << endreq;
  
  return sc;
}

/*---------------------------------------------------------*/
StatusCode
LArOddCellsMonTool::initMonInfo()  
{
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in initMonInfo()" << endreq;
  
  std::vector<CaloGain::CaloGain> gains;
  gains.push_back(CaloGain::LARHIGHGAIN);
  gains.push_back(CaloGain::LARMEDIUMGAIN);
  //  gains.push_back(CaloGain::LARLOWGAIN); 

  // initialize all m_feedthroughInfo objects
  m_feedthroughInfo.resize(m_LArOnlineIDHelper->feedthroughHashMax());
  const unsigned hashmax=m_LArOnlineIDHelper->feedthroughHashMax();
  for (unsigned int ftHash = 0; ftHash < hashmax; ++ftHash) {
    std::vector<CaloGain::CaloGain>::iterator pGainEnd(gains.end());
    for (std::vector<CaloGain::CaloGain>::iterator pGain = gains.begin();pGain != pGainEnd; ++pGain) {
      CaloGain::CaloGain gain = *pGain;
      feedthroughInfo& ftinfo = m_feedthroughInfo[ftHash][gain];
      ftinfo.bookedHists = false;
      ftinfo.toBeMonitored = false;
      ftinfo.hReference = 0;
      ftinfo.hReferenceCool = 0;
      ftinfo.hReferenceCool_sum2 = 0;
      ftinfo.hTempSumComput = 0;
      ftinfo.NbOddPerChan = 0;
      ftinfo.YieldPerChan = 0;
      ftinfo.hIndivTimeDistrib = 0;
      HWIdentifier feedthroughID = m_LArOnlineIDHelper->feedthrough_Id(ftHash);
      ftinfo.barrel_ec= m_LArOnlineIDHelper->barrel_ec(feedthroughID);
      ftinfo.pos_neg  = m_LArOnlineIDHelper->pos_neg(feedthroughID);
      ftinfo.feedthrough = m_LArOnlineIDHelper->feedthrough(feedthroughID);
    }
  }

  // enable appropriate feedthrough for monitoring
  // enable all gains for now.  Booking is on demand.
  if ((int)m_feedthroughNames.size() == 0) {
    // take all feedthroughs
    const unsigned hashmax=m_LArOnlineIDHelper->feedthroughHashMax();
    for (unsigned int ftHash = 0; ftHash < hashmax; ++ftHash) {
      // Monitor only the gain defined in jO
      if (m_gainMonitored[0].find("H",0) != std::string::npos) m_feedthroughInfo[ftHash][CaloGain::LARHIGHGAIN  ].toBeMonitored = true;
      if (m_gainMonitored[0].find("M",0) != std::string::npos) m_feedthroughInfo[ftHash][CaloGain::LARMEDIUMGAIN ].toBeMonitored = true;
      if (m_gainMonitored[0].find("L",0) != std::string::npos) m_feedthroughInfo[ftHash][CaloGain::LARLOWGAIN ].toBeMonitored = true;
      // This feedthrough also contains HEC FEB - Get the adequate gain to monitor
      if (m_feedthroughInfo[ftHash][CaloGain::LARHIGHGAIN].barrel_ec == 1 && (m_feedthroughInfo[ftHash][CaloGain::LARHIGHGAIN].feedthrough == 3 || m_feedthroughInfo[ftHash][CaloGain::LARHIGHGAIN].feedthrough == 10 || m_feedthroughInfo[ftHash][CaloGain::LARHIGHGAIN].feedthrough == 16 || m_feedthroughInfo[ftHash][CaloGain::LARHIGHGAIN].feedthrough == 22)){
	if (m_gainMonitored[1].find("H",0) != std::string::npos) m_feedthroughInfo[ftHash][CaloGain::LARHIGHGAIN  ].toBeMonitored = true;
	if (m_gainMonitored[1].find("M",0) != std::string::npos) m_feedthroughInfo[ftHash][CaloGain::LARMEDIUMGAIN ].toBeMonitored = true;
	if (m_gainMonitored[1].find("L",0) != std::string::npos) m_feedthroughInfo[ftHash][CaloGain::LARLOWGAIN ].toBeMonitored = true;
      }
    }
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Monitoring enabled for all " 
        << m_LArOnlineIDHelper->feedthroughHashMax() << " feedthroughs" << endreq;
  } else {
    // only take selected feedthroughs
    std::vector<std::string>::iterator pEnd(m_feedthroughNames.end());
    for (std::vector<std::string>::iterator p = m_feedthroughNames.begin(); p != pEnd; ++p) {
      //      HWIdentifier feedthroughID = HWIdentifier(*p);
      HWIdentifier feedthroughID = m_strHelper->feedthrough_id(*p);
      IdentifierHash ftHash = m_LArOnlineIDHelper->feedthrough_Hash(feedthroughID);
      // Monitor only the gain defined in jO
      if (m_gainMonitored[0].find("H",0) != std::string::npos) m_feedthroughInfo[ftHash][CaloGain::LARHIGHGAIN  ].toBeMonitored = true;
      if (m_gainMonitored[0].find("M",0) != std::string::npos) m_feedthroughInfo[ftHash][CaloGain::LARMEDIUMGAIN ].toBeMonitored = true;
      if (m_gainMonitored[0].find("L",0) != std::string::npos) m_feedthroughInfo[ftHash][CaloGain::LARLOWGAIN ].toBeMonitored = true;
      // This feedthrough also contains HEC FEB - Get the adequate gain to monitor
      if (m_feedthroughInfo[ftHash][CaloGain::LARHIGHGAIN].barrel_ec == 1 && (m_feedthroughInfo[ftHash][CaloGain::LARHIGHGAIN].feedthrough == 3 || m_feedthroughInfo[ftHash][CaloGain::LARHIGHGAIN].feedthrough == 10 || m_feedthroughInfo[ftHash][CaloGain::LARHIGHGAIN].feedthrough == 16 || m_feedthroughInfo[ftHash][CaloGain::LARHIGHGAIN].feedthrough == 22)){
	if (m_gainMonitored[1].find("H",0) != std::string::npos) m_feedthroughInfo[ftHash][CaloGain::LARHIGHGAIN  ].toBeMonitored = true;
	if (m_gainMonitored[1].find("M",0) != std::string::npos) m_feedthroughInfo[ftHash][CaloGain::LARMEDIUMGAIN ].toBeMonitored = true;
	if (m_gainMonitored[1].find("L",0) != std::string::npos) m_feedthroughInfo[ftHash][CaloGain::LARLOWGAIN ].toBeMonitored = true;
      }
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Monitoring enabled for feedthrough " <<  m_LArOnlineIDHelper->show_to_string(feedthroughID) << endreq;
    }
  }
  
// initialize m_febPedRetrieved object
  m_febPedRetrieved.resize(m_LArOnlineIDHelper->febHashMax());
  //  const unsigned hashmax = m_LArOnlineIDHelper->febHashMax();
  for (unsigned int febHash = 0; febHash < hashmax; ++febHash) {
    std::vector<CaloGain::CaloGain>::iterator pGainEnd(gains.end());
    for (std::vector<CaloGain::CaloGain>::iterator pGain = gains.begin(); pGain != pGainEnd; ++pGain) {
      CaloGain::CaloGain gain = *pGain;
      m_febPedRetrieved[febHash][gain] = false;
    }
  }
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode
LArOddCellsMonTool::bookPerFeedthrough(HWIdentifier feedthroughID, CaloGain::CaloGain gain)
{
  StatusCode sc;

  // get feedthrough Hash 
  IdentifierHash feedthroughHash = m_LArOnlineIDHelper->feedthrough_Hash(feedthroughID);

  feedthroughInfo& feedthroughinfo = m_feedthroughInfo[feedthroughHash][gain];

  // get gain and feb strings
  std::string gainName = this->gain_str(gain);
  std::string feedthroughName = m_strHelper->feedthrough_str(feedthroughID,LArOnlineIDStrHelper::EXPERTS);
  std::string feedthroughTitle = m_strHelper->feedthrough_str(feedthroughID,LArOnlineIDStrHelper::EXPERTS,false);

  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in bookPerFeedthrough(" << feedthroughName << " - "  << gainName << endreq;

  // histo common quantities
  std::string hName;
  std::string hTitle;
  int nbin = this->feedthroughSlotMax(feedthroughID);
  float xMin = 0.5;
  float xMax = nbin + xMin;
  int nbin2 = 128; // Maximum nb of channels per feb
  float xMin2 = -0.5;
  float xMax2 = nbin2 + xMin2;
  std::string fullPath;

  MonGroup OddChannelsDebug( this, "LAr/OddCells/OddChannels", run, ATTRIB_MANAGED );
  MonGroup OddChannelsDebugEffic( this, "LAr/OddCells/OddChannels", run, ATTRIB_MANAGED ,"","effAsPerCent");

  hName = "OddChannels" + feedthroughName + gainName;
  hTitle = "Nb of odd events " + feedthroughTitle + " " + gainName + " - " + format("%.0f",m_oddnessThreshold) + " sigma";
  feedthroughinfo.NbOddPerChan = TH2I_LW::create(hName.c_str(), hTitle.c_str(), nbin, xMin, xMax, nbin2, xMin2, xMax2);
  feedthroughinfo.NbOddPerChan->GetXaxis()->SetTitle("Slot number");
  feedthroughinfo.NbOddPerChan->GetYaxis()->SetTitle("Channel number");
  sc = sc && OddChannelsDebug.regHist(feedthroughinfo.NbOddPerChan);

  hName = "OddChannelsStatus" + feedthroughName + gainName;
  hTitle = "Prop. of odd events (" + format("%.0f",m_oddnessThreshold) + " sigma ) - " + feedthroughTitle + " " + gainName;
  feedthroughinfo.YieldPerChan = TH2F_LW::create(hName.c_str(), hTitle.c_str(), nbin, xMin, xMax, nbin2, xMin2, xMax2);
  feedthroughinfo.YieldPerChan->GetXaxis()->SetTitle("Slot number");
  feedthroughinfo.YieldPerChan->GetYaxis()->SetTitle("Channel number");
  sc = sc && OddChannelsDebugEffic.regHist(feedthroughinfo.YieldPerChan);

  hName = "PedReference" + feedthroughName + gainName;
  hTitle = "Pedestals (from " + format("%d",m_computePeds) + " first events) - " + feedthroughName + " " + gainName;
  feedthroughinfo.hReference = TProfile2D_LW::create(hName.c_str(), hTitle.c_str(), nbin, xMin, xMax, nbin2, xMin2, xMax2,"s");
  sc = sc && OddChannelsDebug.regHist(feedthroughinfo.hReference);

  hName = "PedReferenceCool" + feedthroughName + gainName;
  hTitle = "Pedestals (from COOL) - " + feedthroughName + " " + gainName;
  feedthroughinfo.hReferenceCool = TH2F_LW::create(hName.c_str(), hTitle.c_str(), nbin, xMin, xMax, nbin2, xMin2, xMax2);
  sc = sc && OddChannelsDebug.regHist(feedthroughinfo.hReferenceCool);

  hName = "NoiseReferenceCool_sum2" + feedthroughName + gainName;
  hTitle = "Quadratic noise per FEB (from COOL) - " + feedthroughName + " " + gainName;
  feedthroughinfo.hReferenceCool_sum2 = TH1D_LW::create(hName.c_str(), hTitle.c_str(), nbin, xMin, xMax);
  sc = sc && OddChannelsDebug.regHist(feedthroughinfo.hReferenceCool_sum2);

//   hName = "hTempSumComput" + feedthroughName + gainName;
//   feedthroughinfo.hTempSumComput = TH2F_LW::create(hName.c_str(), hName.c_str(), nbin, xMin, xMax, 32, -0.5, 31.5);
//   sc = sc && OddChannelsDebug.regHist(feedthroughinfo.hTempSumComput);
  feedthroughinfo.hTempSumComput=new Simple2DSumHist(nbin,32);

  // flag booking
  feedthroughinfo.bookedHists = true;

  if (sc.isFailure()) {
    msg(MSG::ERROR) << "Registration per feedthrough failed" << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode
LArOddCellsMonTool::retrievePedPerFeb(HWIdentifier febID, HWIdentifier feedthroughID, CaloGain::CaloGain gain)
{
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in retrievePedPerFeb(" << std::hex << febID << std::dec << ", " << gain << ")" << endreq;


  IdentifierHash febHash = m_LArOnlineIDHelper->feb_Hash(febID);
  int barrelEc = m_LArOnlineIDHelper->barrel_ec(febID);
  int feedthrough = m_LArOnlineIDHelper->feedthrough(febID);
  int slot = m_LArOnlineIDHelper->slot(febID);
  IdentifierHash feedthroughHash = m_LArOnlineIDHelper->feedthrough_Hash(feedthroughID);  
  int refNoiseIndex = 0;

  feedthroughInfo& feedthroughinfo = m_feedthroughInfo[feedthroughHash][gain];

  double sum2 = 0;
  // This is a HEC FEB
  // Dirty method as IsHECOnlineFEBId is buggy!
  if ((barrelEc == 1) && (feedthrough == 3 || feedthrough == 10 || feedthrough == 16 || feedthrough == 22) && (slot > 2)) refNoiseIndex = 1;
  
  for (int iChannel= 0 ; iChannel<128 ;iChannel++){
    if (m_larPedestal){
      HWIdentifier chid = m_LArOnlineIDHelper->channel_Id(febID,iChannel);
      float pedestal = m_larPedestal->pedestal(chid,gain);	  
      float pedestalRMS = m_larPedestal->pedestalRMS(chid,gain);

      if (pedestal>= (1.0+LArElecCalib::ERRORCODE) && pedestalRMS>=(1.0+LArElecCalib::ERRORCODE)){
	feedthroughinfo.hReferenceCool->SetBinContent(slot,iChannel+1,pedestal);
	// The condition below allows to exclude channels that are not connected 
	// Some of them have in fact a non zero noise (around 1-1.2) due to the
	// chips connections in the FEB
	// It also allows to exclude very noisy channels that may bias measurement
	// NB : a refnoise equal to 0 leads to a signif equal to 0 (see initialization)
	// and therefore no odd channels
	// Bad channel masking added on 23/4/08
	bool bool_badChannel = true;
	if (m_maskBadChannels){
	  if (m_badChannelMask->cellShouldBeMasked(chid)) bool_badChannel = false;
	}
	if (pedestalRMS>m_minimumRefNoise[refNoiseIndex] && pedestalRMS<m_maximumRefNoise[refNoiseIndex] && bool_badChannel) {
	  feedthroughinfo.hReferenceCool->SetBinError(slot,iChannel+1,pedestalRMS);
	  sum2 = sum2 + pedestalRMS * pedestalRMS;
	  // Add 1 to the number of cells considered per FEB
	  fillFeedthroughHistos(feedthroughID,slot,1,true);
	}
	else
	  // Here the noise is outside the allowed bound. It is arbitrary set to 0 to ignore odd events here
	  feedthroughinfo.hReferenceCool->SetBinError(slot,iChannel+1,0);            
      }
      else{
 	feedthroughinfo.hReferenceCool->SetBinContentAndError(slot,iChannel+1,0,0);
      }
    }      
  } // End of loop on 128 channels

  if (sum2>=0) feedthroughinfo.hReferenceCool_sum2->SetBinContent(slot,sqrt(sum2));

  m_febPedRetrieved[febHash][gain] = true;
  
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Leaving retrievePedPerFeb..." << endreq;

  return StatusCode::SUCCESS;
}


/*---------------------------------------------------------*/
int 
LArOddCellsMonTool::feedthroughSlotMax(HWIdentifier id) 
{
  // get the max slot number in the corresponding feedthrough
  // Barrel:14  EMECStandard:13  EMECSpecial:15  HEC:10  FCAL:15
  int slotMax = 0;
  if (m_LArOnlineIDHelper->isEmBarrelOnline(id)) {
    slotMax = 14;
  } else if (m_LArOnlineIDHelper->isEmEndcapStandardOnline(id)) {
    slotMax = 13;
  } else if (m_LArOnlineIDHelper->isEmEndcapSpecialOnline(id)) {
    slotMax = 15;
  } else if (m_LArOnlineIDHelper->isHecOnlineFebId(id)) {
    slotMax = 10;
  } else if (m_LArOnlineIDHelper->isFcalOnline(id)) {
    slotMax = 15;
  }
  
  return slotMax;
}

/*---------------------------------------------------------*/
void LArOddCellsMonTool::fillFeedthroughHistos(HWIdentifier FTId,int slotNumber,int weight,bool cellCnt)
{
  int feedthrough = m_LArOnlineIDHelper->feedthrough(FTId);

  // no detailedEndcap : partitionNb = 0 : EMBC / 1 : EMBA / 2 : EndcapC / 3 : EndcapA 
  // detailedEndcap :    partitionNb = 0 : EMBC / 1 : EMBA / 2 : EMECC / 3 : EMECA / 4 : HECC / 5 : HECA / 6 : FCALC / 7 : FCALA
  int partitionNb = m_LArOnlineIDHelper->barrel_ec(FTId)*2+m_LArOnlineIDHelper->pos_neg(FTId);
  if (m_detailEndCap && m_LArOnlineIDHelper->barrel_ec(FTId)  == 1){
    if ((feedthrough == 3 || feedthrough == 10 || feedthrough == 16 || feedthrough == 22) && (slotNumber > 2)) partitionNb = m_LArOnlineIDHelper->barrel_ec(FTId)*2+m_LArOnlineIDHelper->pos_neg(FTId) + 2; // This is HEC FEB
    if ( feedthrough == 6) partitionNb = m_LArOnlineIDHelper->barrel_ec(FTId)*2+m_LArOnlineIDHelper->pos_neg(FTId) + 4; // This is FCAL FEB
  }

  // If cellCnt = true, just count the number of cells to be monitored in the 
  // considered feb
  if (cellCnt){
    m_hMonitoredChan->Fill(partitionNb+1);
    m_nbMonitoredChan[partitionNb]++;
//    HWIdentifier febID = m_LArOnlineIDHelper->feb_Id(m_LArOnlineIDHelper->barrel_ec(FTId),m_LArOnlineIDHelper->pos_neg(FTId),m_LArOnlineIDHelper->feedthrough(FTId),slotNumber);
//    Identifier offlineId;
//    try {
//    offlineId = m_LArCablingService->cnvToIdentifier(febID); 
//    }
//    catch(LArID_Exception& except) {
//    msg(MSG::ERROR) << "LArID_Exception " << (std::string) except << endreq ;
//    }
//    m_nbMonitoredChanPerPartSampling[partitionNb*4+m_LArEM_IDHelper->sampling(offlineId)]++;
    m_nbMonitoredTotal++;

    switch (partitionNb){
    case 0:
      m_barrelCSummary.CellNbPerSlot->Fill(slotNumber,feedthrough,weight);
      break;
    case 1:
      m_barrelASummary.CellNbPerSlot->Fill(slotNumber,feedthrough,weight);
      break;
    case 2:
    case 4:
    case 6:
      m_endcapCSummary.CellNbPerSlot->Fill(slotNumber,feedthrough,weight);
      if (m_detailEndCap){
	if (partitionNb == 2) m_emecCSummary.CellNbPerSlot->Fill(slotNumber,feedthrough,weight);
	if (partitionNb == 4) m_hecCSummary.CellNbPerSlot->Fill(slotNumber,feedthrough,weight);
	if (partitionNb == 6) m_fcalCSummary.CellNbPerSlot->Fill(slotNumber,feedthrough,weight);
      }
      m_nbMonitoredChan[8]++; // Line here to compute the total nb of channels in EndcapC
      break;
    case 3:
    case 5:
    case 7:
      m_endcapASummary.CellNbPerSlot->Fill(slotNumber,feedthrough,weight);
      if (m_detailEndCap){
	if (partitionNb == 3) m_emecASummary.CellNbPerSlot->Fill(slotNumber,feedthrough,weight);
	if (partitionNb == 5) m_hecASummary.CellNbPerSlot->Fill(slotNumber,feedthrough,weight);
	if (partitionNb == 7) m_fcalASummary.CellNbPerSlot->Fill(slotNumber,feedthrough,weight);
      }
      m_nbMonitoredChan[9]++; // Line here to compute the total nb of channels in EndcapA
      break;
    }
  }
  else{
// Here fill the nb of odd events per FEB
    switch (partitionNb){
    case 0:
      m_barrelCSummary.NbOddPerFEB->Fill(slotNumber,feedthrough,weight);
      break;
    case 1:
      m_barrelASummary.NbOddPerFEB->Fill(slotNumber,feedthrough,weight);
      break;
    case 2:
    case 4:
    case 6:
      m_endcapCSummary.NbOddPerFEB->Fill(slotNumber,feedthrough,weight);
      if (m_detailEndCap){
	if (partitionNb == 2) m_emecCSummary.NbOddPerFEB->Fill(slotNumber,feedthrough,weight);
	if (partitionNb == 4) m_hecCSummary.NbOddPerFEB->Fill(slotNumber,feedthrough,weight);
	if (partitionNb == 6) m_fcalCSummary.NbOddPerFEB->Fill(slotNumber,feedthrough,weight);
      }
      break;
    case 3:
    case 5:
    case 7:
      m_endcapASummary.NbOddPerFEB->Fill(slotNumber,feedthrough,weight);
      if (m_detailEndCap){
	if (partitionNb == 3) m_emecASummary.NbOddPerFEB->Fill(slotNumber,feedthrough,weight);
	if (partitionNb == 5) m_hecASummary.NbOddPerFEB->Fill(slotNumber,feedthrough,weight);
	if (partitionNb == 7) m_fcalASummary.NbOddPerFEB->Fill(slotNumber,feedthrough,weight);
      }
      break;
    }
  }
  return;
}

/*---------------------------------------------------------*/
std::string LArOddCellsMonTool::gain_str(CaloGain::CaloGain gain)
{
  // return a string for the gain
  std::string gStr = "";
  if (gain == CaloGain::LARHIGHGAIN) {
    gStr = "High";
  } else if (gain == CaloGain::LARMEDIUMGAIN) {
    gStr = "Medium";
  } else if (gain == CaloGain::LARLOWGAIN) {
    gStr = "Low";
  } else if (gain == CaloGain::UNKNOWNGAIN) {
    gStr = "Unknown";  
  }
  return gStr;
}

/*---------------------------------------------------------*/
void LArOddCellsMonTool::fillYieldHistos(TH2I_LW* summaryHisto,TH2F_LW* statusHisto,TH2I_LW* cellCountingHisto)
// Compute the yield of odd events per channel or FEB or odd sum
// The number of odd events is stored in summaryHisto and the yield is stored in statusHisto
// When normalizing the nb of odd events per FEB, one has to yake into accout the number of monitored cells 
// (information in cellCountingHisto)
// Otherwise for a single channel or odd sums, only normalize by NEvents * NSamples
{
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in fillYieldHistos() - " << m_eventsCounter << endreq;

  if (m_eventsCounter != 0){
    const unsigned nx(summaryHisto->GetNbinsX());
    const unsigned ny(summaryHisto->GetNbinsY());
    for (unsigned ix=1;ix<=nx;++ix){
      for (unsigned iy=1;iy<=ny;++iy){
	// Initialize normalization factor
	float normFactor = (float)m_eventsCounter;
	if (!m_coolRetrieval) normFactor = m_eventsCounter - m_computePeds;
	// Take into account the number of samples
	if ((m_sampleNumber>m_nbOfSamples || m_sampleNumber<0) && (m_nbOfSamples != 0)) {
	   normFactor = normFactor * m_nbOfSamples;	  
	}
	// Take into account the number of channels monitored
	if (cellCountingHisto != NULL){
	  const double bincont(cellCountingHisto->GetBinContent(ix,iy));
	  if (bincont)  normFactor *= bincont;
	}

	float numer = (float) (summaryHisto->GetBinContent(ix,iy));
	float propOfOddCells = numer/normFactor*100;
	float propOfOddCellsError = numer ? sqrt(numer*normFactor*(normFactor-numer))/normFactor/normFactor*100 : 0;

	statusHisto->SetBinContentAndError(ix,iy,propOfOddCells,propOfOddCellsError);
      }// End of loop on y axis
    }//End of loop on x axis
  }//End of test on nb of events

  return;
}

/*---------------------------------------------------------*/
void LArOddCellsMonTool::bookHistsPartition(summaryHistos& summary,std::string partitionName)
// Book histograms related to offline id (FT/Slots...) per partition
{
  if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "in bookHistsPartition()" << endreq;
 
  StatusCode sc;
 
  MonGroup generalGroup( this, "LAr/OddCells/General", run, ATTRIB_MANAGED );
  MonGroup OddChannelsGroup( this, "LAr/OddCells/OddChannels", run, ATTRIB_MANAGED, "", "effAsPerCent" );
  MonGroup OddFEBsDebugGroup( this, "LAr/OddCells/OddFEBs", run, ATTRIB_MANAGED );
  MonGroup OddFEBsShiftGroup( this, "LAr/OddCells/OddFEBs", run, ATTRIB_MANAGED, "", "effAsPerCent" );
  MonGroup OddSumFEBsDebugGroup( this, "LAr/OddCells/OddSumFEBs", run, ATTRIB_MANAGED );
  MonGroup OddSumFEBsShiftGroup( this, "LAr/OddCells/OddSumFEBs", run, ATTRIB_MANAGED, "", "effAsPerCent" );
  MonGroup cellNumberGroup( this, "LAr/OddCells/cellNumber", run, ATTRIB_MANAGED, "", "weightedAverage" );

  // Extract characteristics of the partition
  int nbOfSlots = 15;
  int nbOfFt = 25;
  int nbOfFtSlots = 375;
  if (partitionName.find("Barrel",0) != std::string::npos){
    nbOfSlots = 14;
    nbOfFt = 32;
    nbOfFtSlots = 448;
  }
  float slotMax = nbOfSlots + 0.5;
  float ftMax = nbOfFt - 0.5;  
  float ftSlotsMax = nbOfFtSlots + 0.5;

  int refIndex = 0;
  if (partitionName.find("Hec",0) != std::string::npos) refIndex = 1;  

  std::string suffix = "(" + format("%.0f",m_oddnessThreshold) + " sigma - ";
  if (m_sampleNumber<0) suffix = suffix +" - All samples) - ";
  else suffix = suffix +" - Sample " + format("%1d only) - ",m_sampleNumber);
  if (partitionName.find("Endcap",0) != std::string::npos) suffix = suffix + m_gainMonitored[0] + " / " + m_gainMonitored[1];
  else suffix = suffix + m_gainMonitored[refIndex];  
  for (uint i=0;i<m_noise_streams.size();i++) suffix = suffix + " - " + m_noise_streams[i];

  // Book yield of odd events per channel
  // Currently, this histo is just a copy of FT yield histo
  std::string hName = partitionName + "PropOddEventsPerChan";
  std::string hTitle = partitionName + " : prop. of odd events per channel " + suffix;
  summary.YieldPerChan = TH2F_LW::create(hName.c_str(), hTitle.c_str(),nbOfFtSlots,0.5,ftSlotsMax,128,-0.5,127.5);
  defineAxisTitle(summary.YieldPerChan,3);
  sc= sc && OddChannelsGroup.regHist(summary.YieldPerChan);

  // Book nb of odd events per FEB
  hName = partitionName + "FeedthroughSummaryOddChannels";
  hTitle = partitionName + " : nb of odd events per FEB " + suffix ;
  summary.NbOddPerFEB = TH2I_LW::create(hName.c_str(), hTitle.c_str(),nbOfSlots,0.5,slotMax,nbOfFt,-0.5,ftMax);
  defineAxisTitle(summary.NbOddPerFEB,0);
  sc= sc && OddFEBsDebugGroup.regHist(summary.NbOddPerFEB);

  // Book yield of odd events per FEB
  hName = partitionName + "FeedthroughStatusOddChannels";
  hTitle = partitionName + " : prop. of odd events per FEB " + suffix;
  summary.YieldPerFEB = TH2F_LW::create(hName.c_str(), hTitle.c_str(),nbOfSlots,0.5,slotMax,nbOfFt,-0.5,ftMax);
  defineAxisTitle(summary.YieldPerFEB,0);
  sc = sc && OddFEBsShiftGroup.regHist(summary.YieldPerFEB);

  // Book the nb of monitored cell per FEB
  hName = partitionName + "FeedthroughCellCounting";
  hTitle = partitionName + " : nb of monitored cells (with available pedestal/noise)";
  summary.CellNbPerSlot = TH2I_LW::create(hName.c_str(), hTitle.c_str(),nbOfSlots,0.5,slotMax,nbOfFt,-0.5,ftMax);
  defineAxisTitle(summary.CellNbPerSlot,0);
  sc = sc && cellNumberGroup.regHist(summary.CellNbPerSlot);

  // Book yield of odd channels per events
  hName = partitionName + "YieldPerEvent";
  hTitle = partitionName + " : odd channels yield per event " + suffix;
//  if (m_oddnessThreshold == 3 && m_posNeg == 0) hTitle = hTitle + " (Gauss : 0.27% exp.)";
//  if (m_oddnessThreshold == 4 && m_posNeg == 0) hTitle = hTitle + " (Gauss : 0.063% exp.)";
//  if (m_oddnessThreshold == 3 && m_posNeg != 0) hTitle = hTitle + " (Gauss : 0.14% exp.)";
//  if (m_oddnessThreshold == 4 && m_posNeg != 0) hTitle = hTitle + " (Gauss : 0.032% exp.)";
  summary.YieldPerEvent = TH1D_LW::create(hName.c_str(), hTitle.c_str(),500,-0.2,2.3);
  sc = sc && generalGroup.regHist(summary.YieldPerEvent);

  // Book yield of odd channels per events
  hName = partitionName + "YieldPerEventCleanQ";
  hTitle = partitionName + " : odd channels yield per event (No Q factor burst)" + suffix;
//  if (m_oddnessThreshold == 3 && m_posNeg == 0) hTitle = hTitle + " (Gauss : 0.27% exp.)";
//  if (m_oddnessThreshold == 4 && m_posNeg == 0) hTitle = hTitle + " (Gauss : 0.063% exp.)";
//  if (m_oddnessThreshold == 3 && m_posNeg != 0) hTitle = hTitle + " (Gauss : 0.14% exp.)";
//  if (m_oddnessThreshold == 4 && m_posNeg != 0) hTitle = hTitle + " (Gauss : 0.032% exp.)";
  summary.YieldPerEventCleanQ = TH1D_LW::create(hName.c_str(), hTitle.c_str(),500,-0.2,2.3);
  sc = sc && generalGroup.regHist(summary.YieldPerEventCleanQ);

  // Book histogram of time of bursty events
  hName = partitionName + "TimeBurstyEvents";
  hTitle = partitionName + " : time of bursty events (yield of channels > " + format("%.1f",m_burstyThreshold) + "%) ";
  summary.TimeBursty = TH1I_LW::create(hName.c_str(), hTitle.c_str(),1440,0,1440);
  for (int iLab = 1; iLab < 24 ; iLab++){
    int label = iLab * 60;
    (summary.TimeBursty->GetXaxis())->SetBinLabel(label,Form("%.0d",iLab));
    //Not supported in LWHists: (summary.TimeBursty->GetXaxis())->SetTickLength(0);
    (summary.TimeBursty->GetXaxis())->SetTitle("UTC");
  }
  sc = sc && generalGroup.regHist(summary.TimeBursty);

  // Book coh noise per FEB
  hName = partitionName + "oddSumsNb";
  hTitle = partitionName + " : nb of odd sums over 128 ch. of  FEB " + suffix;
  summary.NbOddSumPerFEB = TH2I_LW::create(hName.c_str(), hTitle.c_str(),nbOfSlots,0.5,slotMax,nbOfFt,-0.5,ftMax);
  defineAxisTitle(summary.NbOddSumPerFEB,0);
  sc = sc && OddSumFEBsDebugGroup.regHist(summary.NbOddSumPerFEB);

  hName = partitionName + "oddSumsYield";
  hTitle = partitionName + " : prop. of odd sums over 128 ch. of  FEB " + suffix;
  summary.YieldSumPerFEB = TH2F_LW::create(hName.c_str(), hTitle.c_str(),nbOfSlots,0.5,slotMax,nbOfFt,-0.5,ftMax);
  defineAxisTitle(summary.YieldSumPerFEB,0);
  sc = sc && OddSumFEBsShiftGroup.regHist(summary.YieldSumPerFEB);

  // Book histogram of proportion of bursty events
  // 0=ok 1=ADC burst 2=LArNoisyRO alg 3=1+2
  hName = partitionName + "BurstStatus";
  hTitle = partitionName + " : BurstStatus";
  summary.BurstStatus = TH1I_LW::create(hName.c_str(), hTitle.c_str(),4,-0.5,3.5);
  (summary.BurstStatus->GetXaxis())->SetBinLabel(1,"No burst");
  (summary.BurstStatus->GetXaxis())->SetBinLabel(2,"ADC");
  (summary.BurstStatus->GetXaxis())->SetBinLabel(3,"Q factor");
  (summary.BurstStatus->GetXaxis())->SetBinLabel(4,"ADC + q factor");
  (summary.BurstStatus->GetXaxis())->SetTitle("UTC");
  sc = sc && generalGroup.regHist(summary.BurstStatus);

  if (sc.isFailure()) {
    msg(MSG::ERROR) << "BookPerPartition failed" << endreq;
  }

  return;
}

//********************************************************************
void LArOddCellsMonTool::defineAxisTitle(LWHist2D* hist,int typeH)
{
  switch (typeH){
  case 0:  
    (m_strHelper->definePartitionSummProp(hist)).ignore();
    break;
  case 3:  
    (m_strHelper->definePartitionSummProp2(hist)).ignore();
    break;
  }
  return;
}

//********************************************************************
void LArOddCellsMonTool::copyChanYield(feedthroughInfo ftInfo)
{
  // partitionNb = 0 : EMBC / 1 : EMBA / 2 : EndcapC / 3 : EndcapA 
  int partitionNb = ftInfo.barrel_ec*2 + ftInfo.pos_neg;
  double tmp_cont, tmp_err;

  switch(partitionNb){
  case 0:
    for (int iSlot = 1;iSlot < 15; iSlot++){
      int iSFT = ftInfo.feedthrough*14+iSlot;
      for (int iCh = 0; iCh<128;iCh++){
	ftInfo.YieldPerChan->GetBinContentAndError(iSlot,iCh+1,tmp_cont,tmp_err);
	m_barrelCSummary.YieldPerChan->SetBinContentAndError(iSFT,iCh+1,tmp_cont,tmp_err);
      }
    }   
    break;
  case 1:
    for (int iSlot = 1;iSlot < 15; iSlot++){
      int iSFT = ftInfo.feedthrough*14+iSlot;
      for (int iCh = 0; iCh<128;iCh++){
	ftInfo.YieldPerChan->GetBinContentAndError(iSlot,iCh+1,tmp_cont,tmp_err);
	m_barrelASummary.YieldPerChan->SetBinContentAndError(iSFT,iCh+1,tmp_cont,tmp_err);
      }
    }    
    break;
  case 2:
    for (int iSlot = 1;iSlot < 16; iSlot++){
      int iSFT = ftInfo.feedthrough*15+iSlot;
      
      for (int iCh = 0; iCh<128;iCh++){
	ftInfo.YieldPerChan->GetBinContentAndError(iSlot,iCh+1,tmp_cont,tmp_err);
	m_endcapCSummary.YieldPerChan->SetBinContentAndError(iSFT,iCh+1,tmp_cont,tmp_err);

	if (m_detailEndCap){
	  // FCAL
	  if ( ftInfo.feedthrough == 6 ){
	    m_fcalCSummary.YieldPerChan->SetBinContentAndError(iSFT,iCh+1,tmp_cont,tmp_err);
	  }
	  // HEC
	  else if ((ftInfo.feedthrough == 3 || ftInfo.feedthrough == 10 || ftInfo.feedthrough == 16 || ftInfo.feedthrough == 22) && (iSlot > 2)){
	    m_hecCSummary.YieldPerChan->SetBinContentAndError(iSFT,iCh+1,tmp_cont,tmp_err);
	  } 
	  //EMEC
	  else{
	    m_emecCSummary.YieldPerChan->SetBinContentAndError(iSFT,iCh+1,tmp_cont,tmp_err);
	  } 
	}
      }
    }
    break;
  case 3:
    for (int iSlot = 1;iSlot < 16; iSlot++){
      int iSFT = ftInfo.feedthrough*15+iSlot;
      for (int iCh = 0; iCh<128;iCh++){
	ftInfo.YieldPerChan->GetBinContentAndError(iSlot,iCh+1,tmp_cont,tmp_err);
	m_endcapASummary.YieldPerChan->SetBinContentAndError(iSFT,iCh+1,tmp_cont,tmp_err);
	if (m_detailEndCap){
	  // FCAL
	  if ( ftInfo.feedthrough == 6 ){
	    m_fcalASummary.YieldPerChan->SetBinContentAndError(iSFT,iCh+1,tmp_cont,tmp_err);
	  }
	  // HEC
	  else if ((ftInfo.feedthrough == 3 || ftInfo.feedthrough == 10 || ftInfo.feedthrough == 16 || ftInfo.feedthrough == 22) && (iSlot > 2)){
	    m_hecASummary.YieldPerChan->SetBinContentAndError(iSFT,iCh+1,tmp_cont,tmp_err);
	  } 
	  //EMEC
	  else{
	    m_emecASummary.YieldPerChan->SetBinContentAndError(iSFT,iCh+1,tmp_cont,tmp_err);
	  } 
	}
      }
    }    
    break;
  }

  return;
}

//********************************************************************
void LArOddCellsMonTool::fillYieldPerEvent(summaryHistos& summaryPart,double yieldEvent,int typeOfBursty)
{
  msg(MSG::DEBUG) << "In fillYieldPerEvent " << yieldEvent << endreq ;
  summaryPart.YieldPerEvent->Fill(yieldEvent);
  if (typeOfBursty==0 || typeOfBursty==1)summaryPart.YieldPerEventCleanQ->Fill(yieldEvent);
  if (typeOfBursty != 0) m_eventToBeStored = true;
  if (typeOfBursty == 1 || typeOfBursty == 3){
    msg(MSG::DEBUG) << "This is bursty " << endreq ;
    summaryPart.TimeBursty->Fill(m_evtTime);
    // Retrieve event info to get event time, event number, trigger type...
//    EventID *thisEvent;
//    const EventInfo* thisEventInfo;
//    StatusCode sc = evtStore()->retrieve(thisEventInfo);    
//    if (sc!=StatusCode::SUCCESS)
//      msg(MSG::WARNING) << "No EventInfo object found! Can't read run number!" << endreq;
//    else{
//      thisEvent=thisEventInfo->event_ID();
//      int Offset = thisEvent->time_stamp()/(24*3600); 
//      float runningTime = float(thisEvent->time_stamp() - (Offset*24*3600))/60;
//      summaryPart.TimeBursty->Fill(runningTime);
//    }
//    msg(MSG::DEBUG) << "This was bursty " << endreq ;
  }
  
  summaryPart.BurstStatus->Fill(typeOfBursty);
    
  return;
}

//********************************************************************
void LArOddCellsMonTool::fillSumPerFEBHistos(feedthroughInfo& ftInfo)
{
  const unsigned nx(ftInfo.hTempSumComput->GetNbinsX());
  const unsigned ny(ftInfo.hTempSumComput->GetNbinsY());
  for (unsigned iSlot=1;iSlot<=nx;++iSlot){
    for (unsigned iSample=1;iSample<=ny;++iSample){
      if (fabs(ftInfo.hTempSumComput->GetBinContent(iSlot-1,iSample-1))>m_oddnessThreshold*ftInfo.hReferenceCool_sum2->GetBinContent(iSlot)){
	// no detailedEndcap : partitionNb = 0 : EMBC / 1 : EMBA / 2 : EndcapC / 3 : EndcapA 
	// detailedEndcap :    partitionNb = 0 : EMBC / 1 : EMBA / 2 : EMECC / 3 : EMECA / 4 : HECC / 5 : HECA / 6 : FCALC / 7 : FCALA
	int partitionNb = ftInfo.barrel_ec*2+ftInfo.pos_neg;
	if (m_detailEndCap && ftInfo.barrel_ec  == 1){
	  if ((ftInfo.feedthrough == 3 || ftInfo.feedthrough == 10 || ftInfo.feedthrough == 16 || ftInfo.feedthrough == 22) && (iSlot > 2)) partitionNb = ftInfo.barrel_ec*2+ftInfo.pos_neg + 2; // This is HEC FEB
	  if (ftInfo.feedthrough == 6) partitionNb = ftInfo.barrel_ec*2+ftInfo.pos_neg + 4; // This is FCAL FEB
	}
	switch (partitionNb){
	case 0:
	  m_barrelCSummary.NbOddSumPerFEB->Fill(iSlot,ftInfo.feedthrough,1);
	  break;
	case 1:
	  m_barrelASummary.NbOddSumPerFEB->Fill(iSlot,ftInfo.feedthrough,1);
	  break;
	case 2:
	case 4:
	case 6:
	  m_endcapCSummary.NbOddSumPerFEB->Fill(iSlot,ftInfo.feedthrough,1);
	  if (m_detailEndCap){
	    if (partitionNb == 2) m_emecCSummary.NbOddSumPerFEB->Fill(iSlot,ftInfo.feedthrough,1);
	    if (partitionNb == 4) m_hecCSummary.NbOddSumPerFEB->Fill(iSlot,ftInfo.feedthrough,1);
	    if (partitionNb == 6) m_fcalCSummary.NbOddSumPerFEB->Fill(iSlot,ftInfo.feedthrough,1);
	  }
	  break;
	case 3:
	case 5:
	case 7:
	  m_endcapASummary.NbOddSumPerFEB->Fill(iSlot,ftInfo.feedthrough,1);
	  if (m_detailEndCap){
	    if (partitionNb == 3) m_emecASummary.NbOddSumPerFEB->Fill(iSlot,ftInfo.feedthrough,1);
	    if (partitionNb == 5) m_hecASummary.NbOddSumPerFEB->Fill(iSlot,ftInfo.feedthrough,1);
	    if (partitionNb == 7) m_fcalASummary.NbOddSumPerFEB->Fill(iSlot,ftInfo.feedthrough,1);
	  }
	  break;
	}
      }
    }
  }

  return;
}

//********************************************************************
void LArOddCellsMonTool::deregisterHisto(LWHist* hist)
{
  if (hist){
    (deregHist(hist)).ignore();
    LWHist::safeDelete(hist);
  }
}




LArOddCellsMonTool::summaryHistos::summaryHistos()
  : YieldPerChan(0),
    NbOddPerFEB(0),
    YieldPerFEB(0),
    NbOddSumPerFEB(0),
    YieldSumPerFEB(0),
    YieldPerEvent(0),
    YieldPerEventCleanQ(0),
    TimeBursty(0),
    BurstStatus(0),
    CellNbPerSlot(0)
{
}
