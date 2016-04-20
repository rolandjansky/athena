/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigL1TopoROBMonitor.h"
#include "L1TopoRDO/L1TopoRDOCollection.h"
#include "L1TopoRDO/Helpers.h"
#include "L1TopoRDO/Header.h"
#include "L1TopoRDO/Fibre.h"
#include "L1TopoRDO/Status.h"
#include "L1TopoRDO/L1TopoTOB.h"
#include "L1TopoRDO/BlockTypes.h"
#include "TrigT1Result/CTP_RDO.h"
#include "TrigT1Result/CTP_Decoder.h"
#include "TrigT1Result/RoIBResult.h"
#include "TrigT1Interfaces/FrontPanelCTP.h"
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"
#include "TrigConfInterfaces/IL1TopoConfigSvc.h"
#include "L1TopoConfig/L1TopoMenu.h"
#include "EventInfo/EventInfo.h"
#include "EventInfo/EventID.h"

#include "GaudiKernel/ThreadGaudi.h"
#include "AthenaKernel/Timeout.h"
#include "ByteStreamCnvSvcBase/IROBDataProviderSvc.h"
//#include "TrigROBDataProviderSvc/ITrigROBDataProviderSvc.h"
#include "GaudiKernel/ITHistSvc.h"
#include <TH1F.h>
//#include <TH2F.h>
//#include <TProfile.h>

#include "TrigMonitorBase/TrigLockedHist.h"
#include "eformat/eformat.h"
//#include "eformat/index.h"
#include "eformat/DetectorMask.h"

#include "TrigSteering/Scaler.h"
#include "TrigSteering/PeriodicScaler.h"
#include "TrigSteeringEvent/HLTResult.h"
#include "TrigSteeringEvent/HLTExtraData.h"

#include <algorithm>
#include <vector>
#include <map>
#include <iomanip>
#include <sstream>
#include <string>

// To be moved to L1TopoRDO Helpers.cxx
namespace L1Topo{
  const std::string formatVecHex8(const std::vector<uint32_t>& vec)
  {
    std::ostringstream s;
    s << "[ ";
    for (auto elem: vec){
      s << std::hex << std::showbase << std::setfill('0') << std::setw(10) 
        << elem << " " << std::dec << std::noshowbase;
    }
    s << "]";
    return s.str();
  }
}
namespace L1Topo{
  const std::string formatHex1(const uint32_t word)
  {
    std::ostringstream s;
    s << std::hex << std::showbase << std::setfill('0') << std::setw(3) 
        << word << std::dec << std::noshowbase;
    return s.str();
  }
}


TrigL1TopoROBMonitor::TrigL1TopoROBMonitor(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), 
  m_robDataProviderSvc( "ROBDataProviderSvc", name ),
  m_l1topoConfigSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name),
  //m_histPropNoBins(Gaudi::Histo1DDef("",0,1,1)), //! generic for labelled bins set at fill time
  m_histSIDsViaConverters(0),
  m_histSIDsDirectFromROBs(0),
  m_histCTPSignalPartFromROIConv(0),
  m_histOverflowBitsFromROIConv(0),
  m_histTriggerBitsFromROIConv(0),
  m_histPayloadCRCFromDAQConv(0),
  m_histFibreStatusFlagsFromDAQConv(0),
  m_histTOBCountsFromROIROB(0),
  m_histTOBCountsFromDAQROB(0),
  m_histPayloadSizeDAQROB(0),
  m_histPayloadSizeROIROB(0),
  m_histBCNsFromDAQConv(0),
  m_histTopoSimHdwStatComparison(0),
  m_histTopoSimHdwEventComparison(0),
  m_histTopoCtpSimHdwEventComparison(0),
  m_histTopoCtpHdwEventComparison(0),
  m_histTopoSimResult(0),
  m_histTopoHdwResult(0),
  m_histTopoProblems(0),
  m_histInputLinkCRCfromROIConv(0),
  m_setTopoSimResult(false)
{
  m_scaler = new HLT::PeriodicScaler();
  declareProperty("L1TopoDAQROBIDs", m_vDAQROBIDs = {0x00910000, 0x00910010, 0x00910020}, "L1TOPO DAQ ROB IDs");
  declareProperty("L1TopoROIROBIDs", m_vROIROBIDs = {0x00910081, 0x00910082, 0x00910091, 0x00910092, 0x009100a1, 0x009100a2}, "L1Topo ROI ROB IDs");
  declareProperty("PrescaleDAQROBAccess", m_prescaleForDAQROBAccess = 4, "Prescale factor for requests for DAQ ROBs: can be used to avoid overloading ROS. Zero means disabled, 1 means always, N means sample only 1 in N events");
  declareProperty("doRawMon", m_doRawMon = true, "enable L1Topo monitoring direct from ROB fragments");
  declareProperty("doCnvMon", m_doCnvMon = true, "enable L1Topo monitoring via converters");
  declareProperty("doSimMon", m_doSimMon = true, "enable L1Topo hardware vs simulation comparison");
  declareProperty("doWriteValData", m_doWriteValData = true, "write L1Topo simulation/validation data into HLTResult"); 
  declareProperty("useDetMask", m_useDetMask = true, "only monitor if L1Topo is included in the event according to the detector mask; this can disable monitoring automatically in spite of other options");
  declareProperty("SimTopoCTPLocation", m_simTopoCTPLocation = LVL1::DEFAULT_L1TopoCTPLocation, "StoreGate key of simulated topo decision output for CTP, defaults to default output key of L1TopoSimulation" );
  declareProperty("HLTResultName", m_HltResultName = "HLTResult_HLT", "StoreGate key of HLT result" );
}

StatusCode TrigL1TopoROBMonitor::initialize(){
  ATH_MSG_INFO ("initialize");
  CHECK( m_robDataProviderSvc.retrieve() );
  ATH_MSG_DEBUG ("Properties:" );
  ATH_MSG_DEBUG ( m_doRawMon );
  ATH_MSG_DEBUG ( m_doCnvMon );
  ATH_MSG_DEBUG ( m_doSimMon );
  ATH_MSG_DEBUG ( m_doWriteValData );
  ATH_MSG_DEBUG ( m_useDetMask );
  ATH_MSG_DEBUG ( m_vDAQROBIDs );
  ATH_MSG_DEBUG ( m_vROIROBIDs );
  ATH_MSG_DEBUG ( m_prescaleForDAQROBAccess );
  ATH_MSG_DEBUG ( m_simTopoCTPLocation );
  return StatusCode::SUCCESS;
}

/* The execute method just decides whether to run any monitoring at
   all, requiring that L1Topo is included in the run via the detector
   mask, and if so, calls methods to run monitoring raw (direct via
   ROBs) and via converters, if they are enabled by properties of this
   algorithm.
 */
StatusCode TrigL1TopoROBMonitor::execute() {

  ATH_MSG_DEBUG ("execute");

  //--------------------------------------------------------------------------
  // check that there is still time left
  //--------------------------------------------------------------------------
  if (Athena::Timeout::instance().reached()) {
    ATH_MSG_INFO( " Time out reached in entry to execute." );
    return StatusCode::SUCCESS;
  }

  if (m_useDetMask){
    ATH_MSG_VERBOSE( "Using DetMask to decide whether to monitor this event" );

    // Check L1Topo detector is present
    // get event information
    const EventInfo* event;
    CHECK_RECOVERABLE( evtStore()->retrieve(event) );
    if(! event){
      ATH_MSG_WARNING( "Could not retrieve EventInfo object" );
      return StatusCode::SUCCESS;
    }
    // get event ID
    EventID* eventId;
    if ( !(eventId = event->event_ID())){
      ATH_MSG_WARNING( "Could not find EventID object" );
      return StatusCode::SUCCESS;
    }
    // check detector mask
    uint64_t mask64 = ((uint64_t)eventId->detector_mask0()) | (((uint64_t)eventId->detector_mask1()) << 32);
    eformat::helper::DetectorMask detMask(mask64);
    ATH_MSG_VERBOSE ( "DetectorMask " << detMask.string() << " is_set(TDAQ_CALO_TOPO_PROC): " << detMask.is_set(eformat::TDAQ_CALO_TOPO_PROC) );
    if (! detMask.is_set(eformat::TDAQ_CALO_TOPO_PROC) ){
      ATH_MSG_INFO( "SubDetector::TDAQ_CALO_TOPO_PROC not included so do nothing and return" );
      return StatusCode::SUCCESS;
    }
  }
  else {
    ATH_MSG_VERBOSE( "Not using DetMask - monitor this event regardless" );
  }


  bool prescaleForDAQROBAccess=false;
  if (m_prescaleForDAQROBAccess>0){
    prescaleForDAQROBAccess = m_scaler->decision(m_prescaleForDAQROBAccess);
  }
  ATH_MSG_DEBUG( "Prescale for DAQ ROB access: " << std::boolalpha << prescaleForDAQROBAccess << std::noboolalpha );

  if (m_doRawMon){
    CHECK( doRawMon(prescaleForDAQROBAccess) );
  }

  if (m_doCnvMon){
    CHECK( doCnvMon(prescaleForDAQROBAccess) );
  }

  if (m_doSimMon){
    CHECK( doSimMon(prescaleForDAQROBAccess) );
  }
 
  if (m_doWriteValData){
    CHECK( doWriteValData() );
  }
 
  return StatusCode::SUCCESS;
}

StatusCode TrigL1TopoROBMonitor::finalize() {
  ATH_MSG_INFO ("finalize");
  delete m_scaler;
  m_scaler=0;
  return StatusCode::SUCCESS;
}

StatusCode TrigL1TopoROBMonitor::bookAndRegisterHist(ServiceHandle<ITHistSvc>& rootHistSvc, TH1F*& hist, const Histo1DProperty& prop, std::string extraName, std::string extraTitle){
  auto p = prop.value();
  return bookAndRegisterHist(rootHistSvc, hist, p.title()+extraName, p.title()+extraTitle, p.bins(), p.lowEdge(), p.highEdge());
}

StatusCode TrigL1TopoROBMonitor::bookAndRegisterHist(ServiceHandle<ITHistSvc>& rootHistSvc, TH1F*& hist, std::string hName, std::string hTitle, int bins, float lowEdge, float highEdge){

  // *-- booking path
  std::string path = std::string("/EXPERT/")+getGaudiThreadGenericName(name())+"/";
  ATH_MSG_VERBOSE( "Booking monitoring histogram " << hName );
  hist = new TH1F(hName.c_str(), hTitle.c_str(), bins, lowEdge, highEdge);
  if (hist) {
    if ( rootHistSvc->regHist(path + hist->GetName(), hist).isFailure() ){
      ATH_MSG_WARNING( "Can not register monitoring histogram" <<  hist->GetName() );
      return StatusCode::RECOVERABLE;
    }
  }
  else{
    ATH_MSG_WARNING( "Failed to create new monitoring histogram" << hName );
    return StatusCode::RECOVERABLE;
  }
  return StatusCode::SUCCESS;
}

StatusCode TrigL1TopoROBMonitor::beginRun() {

  ATH_MSG_INFO ("beginRun");
  ServiceHandle<ITHistSvc> rootHistSvc("THistSvc", name());
  CHECK( rootHistSvc.retrieve() );
  
  m_scaler->reset();

  // fill map of all ROB SIDs from properties, mapping to an integer counter for use as a bin index.
  unsigned int nROBs = 0;
  m_allSIDLabelsToInts.clear();
  for (auto sid: m_vDAQROBIDs.value()){
    ATH_MSG_VERBOSE( "sid=" << L1Topo::formatHex8(sid) << " nROBs=" << nROBs);
    m_allSIDLabelsToInts.insert(std::pair<unsigned int,unsigned int>(sid,nROBs));
    ++nROBs;
  }
  for (auto sid: m_vROIROBIDs.value()){
    ATH_MSG_VERBOSE( "sid=" << L1Topo::formatHex8(sid) << " nROBs=" << nROBs);
    m_allSIDLabelsToInts.insert(std::pair<unsigned int,unsigned int>(sid,nROBs));
    ++nROBs;
  }
  ATH_MSG_VERBOSE( "allSIDLabelsToInts.size() and nROBs " << m_allSIDLabelsToInts.size() << " " << nROBs );
  ATH_MSG_VERBOSE( "ROB source ID labels for histograms: " << m_allSIDLabelsToInts );
  

  CHECK( bookAndRegisterHist(rootHistSvc, m_histSIDsViaConverters, "ROB_src_IDs_fromCnv", "L1Topo ROB source IDs received via converters;ROB ID", nROBs, 0, nROBs ) );
  CHECK( bookAndRegisterHist(rootHistSvc, m_histSIDsDirectFromROBs, "ROB_src_IDs_fromROB", "L1Topo ROB source IDs received direct from ROBs;ROB ID", nROBs, 0, nROBs ) );
  CHECK( bookAndRegisterHist(rootHistSvc, m_histCTPSignalPartFromROIConv, "CTP_sig_part_fromROICnv", "4-bit CTP signal part from ROI via converter;CTP signal part", 16, 0, 16) ); // 4 bits
  CHECK( bookAndRegisterHist(rootHistSvc, m_histOverflowBitsFromROIConv, "CTP_Overflow_fromROICnv", "L1Topo CTP signal overflow bits from ROI via converter;overflow bits", m_nTopoCTPOutputs, 0, m_nTopoCTPOutputs) );
  CHECK( bookAndRegisterHist(rootHistSvc, m_histTriggerBitsFromROIConv, "CTP_Trigger_fromROICnv", "L1Topo CTP signal trigger bits from ROI via converter;trigger bits", m_nTopoCTPOutputs, 0, m_nTopoCTPOutputs) );
  CHECK( bookAndRegisterHist(rootHistSvc, m_histPayloadCRCFromDAQConv, "CRC_fromDAQCnv", "non zero payload CRCs via converter;Payload CRC", 256, 0, 256 ) ); // CRC is 8 bits
  CHECK( bookAndRegisterHist(rootHistSvc, m_histFibreStatusFlagsFromDAQConv, "NonZero_FibreStatus_fromDAQCnv", "L1Topo Non-zero Fibre status flags from DAQ via converter; fibre status flags", 70, 0, 70) );
  CHECK( bookAndRegisterHist(rootHistSvc, m_histTOBCountsFromROIROB, "TOBtype_fromROIROB", "4-bit TOB type via ROI ROB;TOB type", 16, 0, 16) );
  CHECK( bookAndRegisterHist(rootHistSvc, m_histTOBCountsFromDAQROB, "TOBtype_fromDAQROB", "4-bit TOB type via DAQ ROB;TOB type", 16, 0, 16) );
  CHECK( bookAndRegisterHist(rootHistSvc, m_histPayloadSizeDAQROB, "DAQ_ROB_payload_size", "L1Topo DAQ ROB payload size;number of words", 300, 0, 300) );
  CHECK( bookAndRegisterHist(rootHistSvc, m_histPayloadSizeROIROB, "ROI_ROB_payload_size", "L1Topo ROI ROB payload size;number of words", 300, 0, 300) );
  CHECK( bookAndRegisterHist(rootHistSvc, m_histBCNsFromDAQConv, "DAQ_ROB_rel_bx_fromCnv", "L1Topo DAQ ROB relative bunch crossings sent via converter;relative bunch crossing", 10,-5,5) ); 
  CHECK( bookAndRegisterHist(rootHistSvc, m_histTopoSimHdwStatComparison, "Hdw_vs_Sim_Stat", "L1Topo decisions hardware - simulation statistical differences, events with no overflows", m_nTopoCTPOutputs, 0, m_nTopoCTPOutputs) ) ;
  CHECK( bookAndRegisterHist(rootHistSvc, m_histTopoSimHdwEventComparison, "Hdw_vs_Sim_Events", "L1Topo decisions hardware XOR simulation event-by-event differences, events with no overflows", m_nTopoCTPOutputs, 0, m_nTopoCTPOutputs) ) ;
  CHECK( bookAndRegisterHist(rootHistSvc, m_histTopoCtpSimHdwEventComparison, "CTP_Hdw_vs_Sim_Events", "L1Topo decisions CTP hardware XOR simulation event-by-event differences, events with no overflows", m_nTopoCTPOutputs, 0, m_nTopoCTPOutputs) ) ;
  CHECK( bookAndRegisterHist(rootHistSvc, m_histTopoCtpHdwEventComparison, "CTP_Hdw_vs_L1Topo_Hdw_Events", "L1Topo decisions hardware (trigger|overflow) XOR CTP TIP hardware event-by-event differences, events with no overflows", m_nTopoCTPOutputs, 0, m_nTopoCTPOutputs) ) ;
  CHECK( bookAndRegisterHist(rootHistSvc, m_histTopoSimResult, "SimResults", "L1Topo simulation accepts, events with no overflows", m_nTopoCTPOutputs, 0, m_nTopoCTPOutputs) );
  CHECK( bookAndRegisterHist(rootHistSvc, m_histTopoHdwResult, "HdwResults", "L1Topo hardware accepts, events with no overflows", m_nTopoCTPOutputs, 0, m_nTopoCTPOutputs) ) ;
  unsigned int nProblems=m_problems.size();
  CHECK( bookAndRegisterHist(rootHistSvc, m_histTopoProblems, "Problems", "Counts of various problems", nProblems, 0, nProblems) ) ;
  CHECK( bookAndRegisterHist(rootHistSvc, m_histInputLinkCRCfromROIConv, "InputLinkCRCs","CRC flags for input links, from ROI via converter", 5, 0, 5) );

  // Next, apply x-bin labels to some histograms

  for (unsigned int i=0; i<nProblems; ++i){
    m_histTopoProblems->GetXaxis()->SetBinLabel(i+1,m_problems.at(i).c_str());
  }

  std::vector<std::string> tobLabels = {"EM","TAU","MU","0x3","JETc1","JETc2","ENERGY","0x7","L1TOPO","0x9","0xa","0xb","HEADER","FIBRE","STATUS","0xf"};  
  for (unsigned int i=0; i<tobLabels.size(); ++i){
    //ATH_MSG_VERBOSE ("bin " << i+1 << " " << tobLabels.at(i));
    m_histTOBCountsFromROIROB->GetXaxis()->SetBinLabel(i+1,tobLabels.at(i).c_str());
    m_histTOBCountsFromDAQROB->GetXaxis()->SetBinLabel(i+1,tobLabels.at(i).c_str());
  }

  std::vector<std::string> crcLabels = {"EM", "Tau", "Muon", "Jet", "Energy"};
  for (unsigned int i=0; i<crcLabels.size(); ++i){
    m_histInputLinkCRCfromROIConv->GetXaxis()->SetBinLabel(i+1,crcLabels.at(i).c_str());
  }

  for( const auto & it : m_allSIDLabelsToInts){
    std::string label = L1Topo::formatHex8(it.first);
    unsigned int binIndex = it.second+1;
    m_histSIDsViaConverters->GetXaxis()->SetBinLabel(binIndex,label.c_str());
    m_histSIDsDirectFromROBs->GetXaxis()->SetBinLabel(binIndex,label.c_str());
    ATH_MSG_VERBOSE( "bin " << binIndex << " label " << label );
  }
  
  // Get L1Topo output bit names
  const TXC::L1TopoMenu *topoMenu = m_l1topoConfigSvc->menu();
  if(topoMenu == nullptr) {
    ATH_MSG_WARNING("No L1Topo menu from " << m_l1topoConfigSvc->name());
  } else {
    const std::vector<TXC::TriggerLine> & topoTriggers = topoMenu->getL1TopoConfigOutputList().getTriggerLines();
    ATH_MSG_DEBUG( "Number of trigger lines configured " << topoTriggers.size() );
    // Label histogram bins
    std::map<unsigned int, std::string> topoCounterToName;
    for(const TXC::TriggerLine tl : topoTriggers) {
      ATH_MSG_VERBOSE( tl.name() << " " << tl.counter() );
      topoCounterToName.emplace(tl.counter(), tl.name());
    }
    for (unsigned int binIndex=0; binIndex<m_nTopoCTPOutputs; ++binIndex){ 
      auto it = topoCounterToName.find(binIndex);
      std::string label;
      if (it != topoCounterToName.end()){
        label=it->second;
      }
      else {
        label=std::to_string(binIndex); // do not add 1 because TriggerLines are numbered from zero. The bin label is thus in effect the counter value that is not assigned to any TriggerLine.
      }
      m_histTriggerBitsFromROIConv->GetXaxis()->SetBinLabel(binIndex+1,label.c_str());
      m_histOverflowBitsFromROIConv->GetXaxis()->SetBinLabel(binIndex+1,label.c_str());
      m_histTopoSimResult->GetXaxis()->SetBinLabel(binIndex+1,label.c_str());
      m_histTopoHdwResult->GetXaxis()->SetBinLabel(binIndex+1,label.c_str());
      m_histTopoSimHdwStatComparison->GetXaxis()->SetBinLabel(binIndex+1,label.c_str());
      m_histTopoSimHdwEventComparison->GetXaxis()->SetBinLabel(binIndex+1,label.c_str());
      m_histTopoCtpSimHdwEventComparison->GetXaxis()->SetBinLabel(binIndex+1,label.c_str());
      m_histTopoCtpHdwEventComparison->GetXaxis()->SetBinLabel(binIndex+1,label.c_str());
    }
  }

  // release histogramming service
  rootHistSvc.release().ignore();

  return StatusCode::SUCCESS;
}

StatusCode TrigL1TopoROBMonitor::endRun() {

  ATH_MSG_INFO ("endRun");

  return StatusCode::SUCCESS;
}

StatusCode TrigL1TopoROBMonitor::doRawMon(bool prescalForDAQROBAccess) {
  ATH_MSG_DEBUG( "doRawMon" );
  
  CHECK( monitorROBs(m_vROIROBIDs.value(),true) ); //isROIROB=true
  
  if (prescalForDAQROBAccess){
    CHECK( monitorROBs(m_vDAQROBIDs.value(),false) ); //isROIROB=false
  }
  return StatusCode::SUCCESS;
}

StatusCode TrigL1TopoROBMonitor::monitorROBs(const std::vector<uint32_t>& vROBIDs, bool isROIROB){

  // Iterate over the ROB fragments and histogram their source IDs
  ATH_MSG_VERBOSE( "ROB IDs of type " << (isROIROB?"ROI":"DAQ") << " requested: " << L1Topo::formatVecHex8(vROBIDs));

  // Fetch the ROB fragments
  std::vector<const OFFLINE_FRAGMENTS_NAMESPACE::ROBFragment*> vRobFrags;
  vRobFrags.reserve(vROBIDs.size());
  m_robDataProviderSvc->getROBData(vROBIDs,vRobFrags);
  if (vRobFrags.empty()) {
    ATH_MSG_INFO( "None of these ROBs found" );
  }

  // loop over ROB fragments
  for (auto & rob : vRobFrags){

    ATH_MSG_DEBUG( "RawMon: found ROB ID " << L1Topo::formatHex8(rob->source_id()) << " hist bin " << m_allSIDLabelsToInts.at(rob->source_id()) );
    m_histSIDsDirectFromROBs->Fill(m_allSIDLabelsToInts.at(rob->source_id()));
    
    // Go through the data words in the ROB and histogram the word types 
    // (TOB type etc.).
    OFFLINE_FRAGMENTS_NAMESPACE::PointerType it_data = rob->rod_data();
    const uint32_t ndata = rob->rod_ndata();
    ATH_MSG_VERBOSE( "L1Topo data words: " << MSG::dec << ndata);

    // loop over data words
    for ( unsigned int i = 0; i < ndata; ++i, ++it_data ) {
      ATH_MSG_VERBOSE( L1Topo::formatHex8(*it_data) );
      auto blockType = L1Topo::blockType(static_cast<uint32_t>(*it_data));
      if (isROIROB){
        m_histTOBCountsFromROIROB->Fill(static_cast<float>(blockType),1.);
      } 
      else {
        m_histTOBCountsFromDAQROB->Fill(static_cast<float>(blockType),1.);
      }
    } // loop over data words

    // histogram the data size
    if (isROIROB){
      m_histPayloadSizeROIROB->Fill(ndata);
    }
    else {
      m_histPayloadSizeDAQROB->Fill(ndata);
    }

  }//  loop over rob fragments
  
  return StatusCode::SUCCESS;
}
  
StatusCode TrigL1TopoROBMonitor::doCnvMon(bool prescalForDAQROBAccess) {

  ATH_MSG_DEBUG( "doCnvMon" );
  
  // reset bitsets holding trigger and overflow bits
  m_triggerBits.reset();
  m_overflowBits.reset();
  m_topoSimResult.reset();
  m_topoCtpResult.reset();
  m_setTopoSimResult=false;

  // Retrieve and print the L1Topo RDOs from the ROI RODs
  std::vector<L1Topo::L1TopoTOB> roiTobs;
  const ROIB::RoIBResult* roibresult = 0;
  CHECK (evtStore()->retrieve(roibresult) );
  const std::vector< ROIB::L1TopoResult > l1TopoResults = roibresult->l1TopoResult();
  ATH_MSG_DEBUG( "Number of L1Topo ROI RODs found: " << l1TopoResults.size() );
  if (l1TopoResults.size()==0){
      m_histTopoProblems->Fill(static_cast<float>(Problems::ROI_NO_RDO));
  }
  for (auto & r : l1TopoResults){
    //ATH_MSG_VERBOSE( r.dump() );
    auto rdo=r.rdo();
    ATH_MSG_DEBUG( "Found ROI RDO with source ID " << L1Topo::formatHex8(rdo.getSourceID()) );
    m_histSIDsViaConverters->Fill(m_allSIDLabelsToInts.at(rdo.getSourceID()));
    auto errors = rdo.getErrors();
    if (! errors.empty()){
      ATH_MSG_INFO( "ROI Converter errors reported: " << errors );
      m_histTopoProblems->Fill(static_cast<float>(Problems::ROI_CNV_ERR));
    }
    const std::vector<uint32_t> cDataWords = rdo.getDataWords();
    if ( cDataWords.empty() ) {
      ATH_MSG_INFO( "L1TopoRDO ROI payload is empty" );
      m_histTopoProblems->Fill(static_cast<float>(Problems::ROI_PAYLOAD_EMPTY));
    }
    for (auto word : cDataWords){
      ATH_MSG_VERBOSE( "got ROI word: " << L1Topo::formatHex8(word) );
      switch (L1Topo::blockType(word)){
      case L1Topo::BlockTypes::L1TOPO_TOB:
	{
	  auto tob = L1Topo::L1TopoTOB(word);
	  ATH_MSG_DEBUG( tob );
	  roiTobs.push_back(tob);
	  m_histCTPSignalPartFromROIConv->Fill(tob.ctp_signal());
	  // Check for CRC errors on input links which are flagged in header
	  int ibin(1);
	  for (bool crc: {tob.crc_EM(), tob.crc_Tau(), tob.crc_Muon(), tob.crc_Jet(), tob.crc_Energy()}){
	    if (crc){
	      m_histInputLinkCRCfromROIConv->Fill(ibin++);
	    }
	  }
	  // collect trigger and overflow bits in bitsets
	  for (unsigned int i=0; i<8; ++i){
            unsigned int index = L1Topo::triggerBitIndexNew(rdo.getSourceID(),tob,i);
            //m_histTriggerBitsFromROIConv->Fill(index+i,(tob.trigger_bits()>>i)&1);
            //m_histOverflowBitsFromROIConv->Fill(index+i,(tob.overflow_bits()>>i)&1);
            ATH_MSG_VERBOSE( "filling index " << index << " with value " << ((tob.trigger_bits()>>i)&1) << " sourceID " << L1Topo::formatHex8(rdo.getSourceID()) << " tob: idx clk fpga " << tob.index() << " " << tob.clock() << " " << tob.fpga() << " bit " << i);
            m_triggerBits[index]  = (tob.trigger_bits()>>i)&1;
            m_overflowBits[index] = (tob.overflow_bits()>>i)&1;
	  }
          break;
	}
      default:
	{
	  ATH_MSG_WARNING( "unexpected TOB type in ROI: " << L1Topo::formatHex8(word) );
          m_histTopoProblems->Fill(static_cast<float>(Problems::ROI_BAD_TOB));
	  break;
	}
      }
    }
  }

  // record any set trigger and overflow bits in histograms
  for (unsigned int i=0; i<m_nTopoCTPOutputs; ++i){
    if (m_triggerBits.test(i)!=0){
      m_histTriggerBitsFromROIConv->Fill(i,1.0);
    }
    if (m_overflowBits.test(i)!=0){
      m_histOverflowBitsFromROIConv->Fill(i,1.0);
    }
  }

  ATH_MSG_VERBOSE( "trigger  bits from RoI Cnv: " << m_triggerBits );
  ATH_MSG_VERBOSE( "overflow bits from RoI Cnv: " << m_overflowBits );

  if (m_overflowBits.any())
    ATH_MSG_DEBUG("There is at least one item with overflow");
  

  // Only check DAQ ROBs if prescaler said yes
  if (prescalForDAQROBAccess){

    std::vector<L1Topo::L1TopoTOB> daqTobsBC0;
    
    // Retrieve the L1Topo RDOs from the DAQ RODs
    const DataHandle<L1TopoRDOCollection> rdos = 0;
    StatusCode sc = StatusCode::SUCCESS;
    sc = evtStore()->retrieve(rdos);
    if (sc.isFailure() or 0 == rdos) {
      ATH_MSG_INFO ( "Could not retrieve L1Topo DAQ RDO collection from StoreGate" );
      m_histTopoProblems->Fill(static_cast<float>(Problems::DAQ_NO_RDO));
    }
    else if (rdos->empty()) {
      ATH_MSG_INFO ( "L1Topo DAQ RDO collection is empty" );
      m_histTopoProblems->Fill(static_cast<float>(Problems::DAQ_COLL_EMPTY));
    }
    else {
      // loop over and print RDOs
      for (auto & rdo : *rdos){
        ATH_MSG_VERBOSE( *rdo );
        ATH_MSG_DEBUG( "CnvMon: Found DAQ RDO with source ID " << L1Topo::formatHex8(rdo->getSourceID()) );
        m_histSIDsViaConverters->Fill(m_allSIDLabelsToInts.at(rdo->getSourceID()));
        auto errors = rdo->getErrors();
        if (! errors.empty()){
          ATH_MSG_INFO( "DAQ Converter errors reported: " << errors );
          m_histTopoProblems->Fill(static_cast<float>(Problems::DAQ_CNV_ERR));
        }
        const std::vector<uint32_t> cDataWords = rdo->getDataWords();
      
        if ( cDataWords.empty() ) {
          ATH_MSG_INFO ( "L1TopoRDO DAQ payload is empty" );
          m_histTopoProblems->Fill(static_cast<float>(Problems::DAQ_PAYLOAD_EMPTY));
        }
        // initialise collections filled for each block
        std::vector<L1Topo::L1TopoTOB> daqTobs;
        std::vector<uint32_t> vFibreSizes;
        std::vector<uint32_t> vFibreStatus;
        // initialise header: beware, this can make a valid-looking header and be misinterpreted; set version 15, BCN -7, which is unlikely:
        L1Topo::Header header(0xf,0,0,0,0,1,0x7); 
        bool firstWord=true;
        for (auto word : cDataWords){
          switch (L1Topo::blockType(word)){
          case L1Topo::BlockTypes::HEADER:
            {
              // New block detected, so send the one just completed for monitoring
              if (! firstWord){
                CHECK( monitorBlock(rdo->getSourceID(),header,vFibreSizes,vFibreStatus,daqTobs) );
              }
              header = L1Topo::Header(word);
              // reset containers
              vFibreSizes.clear();
              vFibreStatus.clear();
              daqTobs.clear();
              break;
            }
          case L1Topo::BlockTypes::FIBRE:
            {
              auto fibreBlock = L1Topo::Fibre(word);
              //unsigned int nFibres = fibreBlock.count().size();
              for (auto fsize: fibreBlock.count()){
                vFibreSizes.push_back(fsize);
              }
              for (auto fstatus: fibreBlock.status()){
                vFibreStatus.push_back(fstatus);
              }
              break;
            }
          case L1Topo::BlockTypes::STATUS:
            {
              auto status = L1Topo::Status(word);
	      //IR  Fill overflow variable but it doesn-t seem it works as it does not run
	      //IR  if (status.overflow()) m_nTopoOverflow = 1;
              ATH_MSG_DEBUG( "fibre overflow: " << status.overflow() << " fibre crc: " << status.crc() );
              if (status.overflow()){
                m_histTopoProblems->Fill(static_cast<float>(Problems::FIBRE_OVERFLOW));
              }
              if (status.crc()){
                m_histTopoProblems->Fill(static_cast<float>(Problems::FIBRE_CRC));
              }
              break;
            }
          case L1Topo::BlockTypes::L1TOPO_TOB:
            {
              auto tob = L1Topo::L1TopoTOB(word);
              daqTobs.push_back(tob);
              if (header.bcn_offset()==0){
                daqTobsBC0.push_back(tob);
	    }
              break;
            }
          default:
            {
              // perhaps count other TOB types?
              break;
            }
          }
          firstWord=false;
        } // for word
        // monitor last block
        
	  CHECK( monitorBlock(rdo->getSourceID(),header,vFibreSizes,vFibreStatus,daqTobs) );
      }
    }
    
  // Compare ROI and DAQ L1Topo TOBS

  // need to sort them first? Note zero supression possible

      if (roiTobs.empty() and daqTobsBC0.empty()){
	ATH_MSG_DEBUG( "L1Topo TOBs from both ROI and DAQ via converters are empty: zero supression or problem?" );
      } 

    
    /*
      if (daqTobsBC0==roiTobs){
      ATH_MSG_DEBUG( "DAQ L1Topo TOBs from BC0 are the same as ROI L1Topo TOBs" );
      }
      else {
      ATH_MSG_WARNING( "DAQ L1Topo TOBs from BC0 are NOT the same as ROI L1Topo TOBs" );
      }
      //compareL1TopoTOBs(daqTobsBC0,roiTobs);
    */

  }
  else {
    ATH_MSG_DEBUG( "L1Topo DAQ ROB access via converter skipped due to prescale" );
  }

  return StatusCode::SUCCESS;

}

  StatusCode TrigL1TopoROBMonitor::monitorBlock(uint32_t sourceID, L1Topo::Header& header, std::vector<uint32_t>& /* vFibreSizes */, std::vector<uint32_t>& vFibreStatus, std::vector<L1Topo::L1TopoTOB>& /* daqTobs */) {
  ATH_MSG_DEBUG( "monitorBlock" );
  ATH_MSG_DEBUG( header );
  if (header.payload_crc()!=0){
    ATH_MSG_INFO( "header payload CRC not zero: " << L1Topo::formatHex8(header.payload_crc()) );
    m_histPayloadCRCFromDAQConv->Fill(header.payload_crc(), 1. );
  }
  // This is to be expected; not an error.
  //if (vFibreStatus.size()!=header.active_fibres()){
  //  ATH_MSG_WARNING( "Mismatch between number of fibres declared in header " << header.active_fibres() << " and number found " << vFibreStatus.size() );
  //}
  for (unsigned int i=0; i<vFibreStatus.size(); ++i){
    if (vFibreStatus.at(i)!=0){
      ATH_MSG_DEBUG( " Warning: Fibre status set for fibre " << i << " of ROB " << L1Topo::formatHex8(sourceID) << " header " << header );
    }
    m_histFibreStatusFlagsFromDAQConv->Fill(i,vFibreStatus.at(i));
  }
  // bcns
  m_histBCNsFromDAQConv->Fill(header.bcn(),1.);
  return StatusCode::SUCCESS;
}


StatusCode TrigL1TopoROBMonitor::doSimMon(bool prescalForDAQROBAccess){
  ATH_MSG_DEBUG( "doSimMon" );
  // Retrieve L1Topo CTP simulated decision if present
  if ( ! evtStore()->contains<LVL1::FrontPanelCTP>(m_simTopoCTPLocation.value()) ){
    ATH_MSG_INFO("Could not retrieve LVL1::FrontPanelCTP with key " << m_simTopoCTPLocation.value() << ". Perhaps it was prescaled? Skipping simulation comparison." );
  }
  else {
    const DataHandle< LVL1::FrontPanelCTP > simTopoCTP; ///! simulation output
    CHECK_RECOVERABLE( evtStore()->retrieve(simTopoCTP,m_simTopoCTPLocation.value()) );
    if (!simTopoCTP){
      ATH_MSG_INFO( "Retrieve of LVL1::FrontPanelCTP failed. Skipping simulation comparison." );
    }
    else {


      // New code to fill histograms of simulated L1Topo decisions bits and comparison
      
      // Do the comparison and fill histograms only if the L1Topo items did not overflow
      if (m_overflowBits.none()){

        m_setTopoSimResult=true;
        // From L1CaloL1TopoMon code (simulated result)
        for(unsigned int i=0; i<32; ++i) {
          uint32_t mask = 0x1; mask <<= i;
          if( (simTopoCTP->cableWord1(0) & mask) != 0 )
            m_topoSimResult[i] = 1; // cable 1, clock 0
          if( (simTopoCTP->cableWord1(1) & mask) != 0 )
            m_topoSimResult[32 + i] = 1; // cable 1, clock 1
          if( (simTopoCTP->cableWord2(0) & mask) != 0 )
            m_topoSimResult[64 + i] = 1; // cable 2, clock 0
          if( (simTopoCTP->cableWord2(1) & mask) != 0 )
            m_topoSimResult[96 + i] = 1; // cable 2, clock 1
        }

        // fill histograms
        for (unsigned int i=0; i< m_nTopoCTPOutputs; ++i){
          m_histTopoHdwResult->Fill(i,m_triggerBits.test(i)); 
        }
        for (unsigned int i=0; i< m_nTopoCTPOutputs; ++i){
          m_histTopoSimResult->Fill(i,m_topoSimResult.test(i)); 
        }

        // debug printout
        ATH_MSG_DEBUG("Simulated output from L1Topo from StoreGate with key "
                      << m_simTopoCTPLocation);
        ATH_MSG_DEBUG("L1Topo word 1 at clock 0 is: 0x"
                      << std::hex << std::setw( 8 ) << std::setfill( '0' )
                      << simTopoCTP->cableWord1(0));
        ATH_MSG_DEBUG("L1Topo word 2 at clock 0 is: 0x"
                      << std::hex << std::setw( 8 ) << std::setfill( '0' )
                      << simTopoCTP->cableWord2(0));
        ATH_MSG_DEBUG("L1Topo word 1 at clock 1 is: 0x"
                      << std::hex << std::setw( 8 ) << std::setfill( '0' )
                      << simTopoCTP->cableWord1(1));
        ATH_MSG_DEBUG("L1Topo word 2 at clock 1 is: 0x"
                      << std::hex << std::setw( 8 ) << std::setfill( '0' )
                      << simTopoCTP->cableWord2(1));
      } 


      // Perform the hardware versus simulation comparison
      for (unsigned int i=1; i<=m_nTopoCTPOutputs;++i) {
        float diff=fabs(m_histTopoHdwResult->GetBinContent(i)-
                        m_histTopoSimResult->GetBinContent(i));
        if (diff>0.1){
          m_histTopoSimHdwStatComparison->SetBinContent(i,diff);
        }
      }
      compBitSets("L1Topo hardware", "L1Topo simulation",
                  m_triggerBits, m_topoSimResult,
                  m_histTopoSimHdwEventComparison);

    }
  }

  // Retrieve CTP DAQ data for comparison, if ROB access not prescaled and no overflows
  // Do the comparison and fill histograms only if the L1Topo items did not overflow
  if (prescalForDAQROBAccess){
    const CTP_RDO* ctpRDO; 
    ctpRDO = evtStore()->tryConstRetrieve<CTP_RDO>();
    if (!ctpRDO){
      ATH_MSG_INFO( "Retrieve of CTP_RDO (converted from CTP DAQ ROB) failed. Skipping CTP hardware comparison." );
    }
    else {
      // CTP RDO contains 17 TIP words for a number of BCs, so use CTP_Decoder to access accepted BC
      CTP_Decoder ctp;
      ctp.setRDO(ctpRDO);
      const uint16_t l1aPos = ctpRDO->getL1AcceptBunchPosition();
      if (l1aPos >= ctp.getBunchCrossings().size()) {
        ATH_MSG_INFO( "CTP_RDO gave invalid l1aPos. Skipping CTP hardware comparison" );
      }
      else {
        ATH_MSG_DEBUG( "CTP l1aPos, size : " << l1aPos << ", " << ctp.getBunchCrossings().size() );
        const CTP_BC& ctpL1a = ctp.getBunchCrossings().at(l1aPos);
        std::bitset<512> tip = ctpL1a.getTIP();
        ATH_MSG_VERBOSE( "got CTP TIP bits: " << tip.to_string() );
        const unsigned int topoTipStart(384);
        for (unsigned int i=0; i<m_nTopoCTPOutputs; ++i){
          m_topoCtpResult[i]=tip.test(i+topoTipStart);
        }
     
	// Compare L1Topo trigger bits 
        compBitSets("L1Topo hardware trigger|overflow", "CTP TIP hardware",
                    m_triggerBits|m_overflowBits, m_topoCtpResult,
                    m_histTopoCtpHdwEventComparison);

	if (m_overflowBits.none() && m_setTopoSimResult){
	  // Compare L1Topo outputs from simulation with those at the CTP
	  compBitSets("L1Topo CTP TIP hardware", "L1Topo CTP simulation",
		      m_topoCtpResult, m_topoSimResult,
		      m_histTopoCtpSimHdwEventComparison);
	}
      }
    }
  }
  else {
    ATH_MSG_DEBUG( "CTP DAQ ROB access via converter skipped due to prescale" );
  }

  return StatusCode::SUCCESS;

}

StatusCode TrigL1TopoROBMonitor::doWriteValData(){
  ATH_MSG_DEBUG( "doWriteValData" );

  // Retrieve HLTResuly
  DataHandle<HLT::HLTResult> hltResult; ///! HLTResult object
  if ( ! evtStore()->transientContains<HLT::HLTResult>(m_HltResultName.value()) ) {
    ATH_MSG_INFO("Could not find HLTResult with key " << m_HltResultName.value() << "in SG." );
  } else {
    CHECK_RECOVERABLE( evtStore()->retrieve(hltResult,m_HltResultName.value()) );
    if (!hltResult){
      ATH_MSG_INFO( "Retrieve of HLT::HLTResult failed. No data are written to HLTResult" );
      return StatusCode::RECOVERABLE;
    }
  }

  // Retrieve L1Topo CTP simulated decision if present
  const DataHandle< LVL1::FrontPanelCTP > simTopoCTP; ///! simulation output
  if ( ! evtStore()->contains<LVL1::FrontPanelCTP>(m_simTopoCTPLocation.value()) ){
    ATH_MSG_INFO("Could not find LVL1::FrontPanelCTP with key " << m_simTopoCTPLocation.value() << "in SG." );
  } else {
    CHECK_RECOVERABLE( evtStore()->retrieve(simTopoCTP,m_simTopoCTPLocation.value()) );
    if (!simTopoCTP){
      ATH_MSG_INFO( "Retrieve of LVL1::FrontPanelCTP failed. No data are written to HLTResult" );
      return StatusCode::RECOVERABLE;
    }
  }

  // Write the L1Topo simulation data into the HLTResult
  hltResult->getExtraData().anonymous.push_back( 4 );                         // number of words to be written
  hltResult->getExtraData().anonymous.push_back( simTopoCTP->cableWord1(0) ); // L1Topo simulation words
  hltResult->getExtraData().anonymous.push_back( simTopoCTP->cableWord1(1) );
  hltResult->getExtraData().anonymous.push_back( simTopoCTP->cableWord2(0) );
  hltResult->getExtraData().anonymous.push_back( simTopoCTP->cableWord2(1) );

  return StatusCode::SUCCESS;
}

// compare two bitsets and histogram the differences
// return true if the same, otherwise false
void TrigL1TopoROBMonitor::compBitSets(std::string leftLabel, std::string rightLabel,
                                       const std::bitset<m_nTopoCTPOutputs>& left, 
                                       const std::bitset<m_nTopoCTPOutputs>& right, 
                                       TH1F*& hist){
  // Compare L1Topo outputs from simulation with those at the CTP
  if (left!=right){
    std::bitset<m_nTopoCTPOutputs> diff = (left ^ right);
    ATH_MSG_DEBUG( "compBitSets: " << leftLabel << " and " << rightLabel << " differ in this event:\n" << left << "\n" << right);
    ATH_MSG_VERBOSE("Differences (XOR):\n" << diff);
    for (unsigned int i=0; i<m_nTopoCTPOutputs;++i) {
      if (diff.test(i)){
        ATH_MSG_VERBOSE("Bit " << i << " differs");
        if (hist !=0){
          hist->Fill(i,1.);
        }
      }
    }
  }
  else {
    ATH_MSG_DEBUG( "compBitSets: " << leftLabel << " and " << rightLabel << " are the same" );
  }
  return;
}
