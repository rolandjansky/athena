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
#include "TrigT1Result/RoIBResult.h"

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

#include "TrigSteering/Scaler.h"
#include "TrigSteering/PeriodicScaler.h"

#include <algorithm>
#include <vector>
#include <iomanip>
#include <sstream>


// To be moved to L1TopoRDO Helpers.cxx
namespace L1Topo{
  const std::string formatVecHex8(const std::vector<uint32_t>& vec)
  {
    std::ostringstream s;
    s << "[ ";
    for (auto elem: vec){
      s << std::hex << std::showbase << std::setfill('0') << std::setw(8) 
        << elem << " " << std::dec << std::noshowbase;
    }
    s << "]";
    return s.str();
  }
}


TrigL1TopoROBMonitor::TrigL1TopoROBMonitor(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator), 
  m_robDataProviderSvc( "ROBDataProviderSvc", name ),
  m_histPropNoBins(Gaudi::Histo1DDef("",0,1,1)), //! generic for labelled bins set at fill time
  m_histSIDsViaConverters(0),
  m_histSIDsDirectFromROBs(0),
  m_histCTPSignalPartFromROIConv(0),
  m_histOverflowBitsFromROIConv(0),
  m_histTriggerBitsFromROIConv(0),
  m_histPayloadCRCFromDAQConv(0),
  m_histFibreStatusFlagsFromDAQConv(0),
  //m_histFibreSizesFromDAQConv(0),
  m_histTOBCountsFromROIROB(0),
  m_histTOBCountsFromDAQROB(0)
{
  m_scaler = new HLT::PeriodicScaler();
  // 0x00910000 was used for tests; the others are the planned source IDs
  declareProperty("L1TopoDAQROBIDs", m_vDAQROBIDs = {0x00910000, 0x00910001, 0x00910011}, "L1TOPO DAQ ROB IDs");
  declareProperty("L1TopoROIROBIDs", m_vROIROBIDs = {0x00910080, 0x00910090},"L1Topo ROI ROB IDs");
  declareProperty("PrescaleDAQROBAccess", m_prescaleForDAQROBAccess = 2, "Prescale factor for requests for DAQ ROBs: can be used to avoid overloading ROS. Zero means disabled, 1 means always, N means sample only 1 in N events");
  declareProperty("doRawMon", m_doRawMon = true, "enable L1Topp monitoring direct from ROB fragments");
  declareProperty("doCnvMon", m_doCnvMon = true, "enable L1Topo monitoring via converters");
}


StatusCode TrigL1TopoROBMonitor::initialize(){
  ATH_MSG_INFO ("initialize");
  CHECK( m_robDataProviderSvc.retrieve() );
  ATH_MSG_DEBUG ("Properties:" );
  ATH_MSG_DEBUG ( m_doRawMon );
  ATH_MSG_DEBUG ( m_doCnvMon );
  ATH_MSG_DEBUG ( m_vDAQROBIDs );
  ATH_MSG_DEBUG ( m_vROIROBIDs );
  ATH_MSG_DEBUG ( m_prescaleForDAQROBAccess );
  return StatusCode::SUCCESS;
}

StatusCode TrigL1TopoROBMonitor::execute() {

  ATH_MSG_DEBUG ("execute");

  //--------------------------------------------------------------------------
  // check that there is still time left
  //--------------------------------------------------------------------------
  if (Athena::Timeout::instance().reached()) {
    ATH_MSG_INFO( " Time out reached in entry to execute." );
    return StatusCode::SUCCESS;
  }

  bool prescalForDAQROBAccess=false;
  if (m_prescaleForDAQROBAccess>0){
    prescalForDAQROBAccess = m_scaler->decision(m_prescaleForDAQROBAccess);
  }
  ATH_MSG_DEBUG( "Prescale for DAQ ROB access: " << std::boolalpha << prescalForDAQROBAccess << std::noboolalpha );


  if (m_doRawMon){
    CHECK( doRawMon(prescalForDAQROBAccess) );
  }

  if (m_doCnvMon){
    CHECK( doCnvMon(prescalForDAQROBAccess) );
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

  CHECK( bookAndRegisterHist(rootHistSvc, m_histSIDsViaConverters, m_histPropNoBins, "L1Topo ROB source IDs received via converters", " via converters;ROB ID") );
  CHECK( bookAndRegisterHist(rootHistSvc, m_histSIDsDirectFromROBs, m_histPropNoBins, "L1Topo ROB source IDs received direct from ROBs", " direct from ROBs;ROB ID") );
  CHECK( bookAndRegisterHist(rootHistSvc, m_histCTPSignalPartFromROIConv, m_histPropNoBins, "L1Topo CTP signal part ROI via converter", "4-bit CTP signal part via converter;CTP signal part") );
  CHECK( bookAndRegisterHist(rootHistSvc, m_histOverflowBitsFromROIConv, "L1Topo CTP signal overflow bits from ROI via converter", "L1Topo CTP signal overflow bits from ROI via converter;overflow bits", 128, 0, 128) );
  CHECK( bookAndRegisterHist(rootHistSvc, m_histTriggerBitsFromROIConv, "L1Topo CTP signal trigger bits from ROI via converter", "L1Topo CTP signal trigger bits from ROI via converter;overflow bits", 128, 0, 128) );
  CHECK( bookAndRegisterHist(rootHistSvc, m_histPayloadCRCFromDAQConv, m_histPropNoBins, "L1Topo payload CRC from DAQ via converter", "non zero payload CRCs via converter;Payload CRC") );
  CHECK( bookAndRegisterHist(rootHistSvc, m_histFibreStatusFlagsFromDAQConv, "L1Topo Fibre status flags from DAQ via converter", "L1Topo Fibre status flags from DAQ via converter; fibre status flags", 70, 0, 70) );
  CHECK( bookAndRegisterHist(rootHistSvc, m_histTOBCountsFromROIROB, "L1Topo TOB types from ROI direct from ROBs", "4-bit TOB type via ROI ROB;TOB type", 16, 0, 16) );
  CHECK( bookAndRegisterHist(rootHistSvc, m_histTOBCountsFromDAQROB, "L1Topo TOB types from DAQ direct from ROBs", "4-bit TOB type via DAQ ROB;TOB type", 16, 0, 16) );
  std::vector<std::string> labels = {"EM","TAU","MU","0x3","JETc1","JETc2","ENERGY","0x7","L1TOPO","0x9","0xa","0xb","HEADER","FIBRE","STATUS","0xf"};
  
  for (unsigned int i=0; i<16; ++i){
    //ATH_MSG_VERBOSE ("bin " << i+1 << " " << labels.at(i));
    m_histTOBCountsFromROIROB->GetXaxis()->SetBinLabel(i+1,labels.at(i).c_str());
    m_histTOBCountsFromDAQROB->GetXaxis()->SetBinLabel(i+1,labels.at(i).c_str());
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
    ATH_MSG_WARNING( "None of these ROBs found" );
  }

  // loop over ROB fragments
  for (auto & rob : vRobFrags){

    ATH_MSG_DEBUG( "RawMon: found ROB ID " << L1Topo::formatHex8(rob->source_id()) );
    scoped_lock_histogram lock;
    m_histSIDsDirectFromROBs->Fill(L1Topo::formatHex8(rob->source_id()).c_str(), 1.);
    m_histSIDsDirectFromROBs->LabelsDeflate("X");
    
    // Go through the data words in the ROB and histogram the word types 
    // (TOB type etc.).
    OFFLINE_FRAGMENTS_NAMESPACE::PointerType it_data = rob->rod_data();
    const uint32_t ndata = rob->rod_ndata();
    ATH_MSG_VERBOSE( "L1Topo data words:");

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

  }//  loop over rob fragments
  
  return StatusCode::SUCCESS;
}
  
StatusCode TrigL1TopoROBMonitor::doCnvMon(bool prescalForDAQROBAccess) {

  ATH_MSG_DEBUG( "doCnvMon" );
  
  // Retrieve and print the L1Topo RDOs from the ROI RODs
  std::vector<L1Topo::L1TopoTOB> roiTobs;
  const ROIB::RoIBResult* roibresult = 0;
  CHECK (evtStore()->retrieve(roibresult) );
  const std::vector< ROIB::L1TopoResult > l1TopoResults = roibresult->l1TopoResult();
  ATH_MSG_DEBUG( "Number of L1Topo ROI RODs found: " << l1TopoResults.size() );
  for (auto & r : l1TopoResults){
    //ATH_MSG_VERBOSE( r.dump() );
    auto rdo=r.rdo();
    ATH_MSG_DEBUG( "Found ROI RDO with source ID " << L1Topo::formatHex8(rdo.getSourceID()) );
    scoped_lock_histogram lock;
    m_histSIDsViaConverters->Fill((L1Topo::formatHex8(rdo.getSourceID())).c_str(), 1.);
    m_histSIDsViaConverters->LabelsDeflate("X");
    auto errors = rdo.getErrors();
    if (! errors.empty()){
      ATH_MSG_WARNING( "Converter errors reported: " << errors );
    }
    const std::vector<uint32_t> cDataWords = rdo.getDataWords();
    if ( cDataWords.size() == 0 ) {
      ATH_MSG_WARNING ( "L1TopoRDO ROI is empty" );
    }
    for (auto word : cDataWords){
      switch (L1Topo::blockType(word)){
      case L1Topo::BlockTypes::L1TOPO_TOB:
	{
	  auto tob = L1Topo::L1TopoTOB(word);
	  ATH_MSG_DEBUG( tob );
	  roiTobs.push_back(tob);
	  auto index = L1Topo::triggerBitIndex(rdo.getSourceID(),tob);
          scoped_lock_histogram lock;
	  m_histCTPSignalPartFromROIConv->Fill(L1Topo::formatHex4(tob.ctp_signal()).c_str(), 1.);
	  m_histCTPSignalPartFromROIConv->LabelsDeflate("X");
	  for (unsigned int i=0; i<8; ++i){
	    m_histTriggerBitsFromROIConv->Fill (index+i,(tob.trigger_bits() >>i)&1);
	    m_histOverflowBitsFromROIConv->Fill(index+i,(tob.overflow_bits()>>i)&1);
	  }
	  // histogram trigger and overflow bits
	  break;
	}
      default:
	{
	  ATH_MSG_WARNING( "unexpected TOB type in ROI: " <<  L1Topo::formatHex8(word) );
	  break;
	}
      }
    }
  }

  // Only check DAQ ROBs if prescaler said yes
  if (prescalForDAQROBAccess){

    std::vector<L1Topo::L1TopoTOB> daqTobsBC0;
    
    // Retrieve the L1Topo RDOs from the DAQ RODs
    const DataHandle<L1TopoRDOCollection> rdos = 0;
    StatusCode sc = StatusCode::SUCCESS;
    sc = evtStore()->retrieve(rdos);
    if (sc.isFailure() or 0 == rdos) {
      ATH_MSG_WARNING ( "Could not retrieve L1Topo DAQ RDO collection from StoreGate" );
    }
    else {
      // loop over and print RDOs
      for (auto & rdo : *rdos){
        ATH_MSG_VERBOSE( *rdo );
        ATH_MSG_DEBUG( "CnvMon: Found DAQ RDO with source ID " << L1Topo::formatHex8(rdo->getSourceID()) );
        scoped_lock_histogram lock;
        m_histSIDsViaConverters->Fill((L1Topo::formatHex8(rdo->getSourceID())).c_str(), 1.);
        m_histSIDsViaConverters->LabelsDeflate("X");
        auto errors = rdo->getErrors();
        if (! errors.empty()){
          ATH_MSG_WARNING( "Converter errors reported: " << errors );
        }
        const std::vector<uint32_t> cDataWords = rdo->getDataWords();
      
        if ( cDataWords.size() == 0 ) {
          ATH_MSG_WARNING ( "L1TopoRDO DAQ is empty" );
        }
        // initialise colletions filled for each block
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
              ATH_MSG_WARNING( "fibre overflow: " << status.overflow() << " fibre crc: " << status.crc() );
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

  // need to sort them first
    
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
    ATH_MSG_DEBUG( "DAQ ROB access via converter skipped due to prescale" );
  }

  return StatusCode::SUCCESS;

}

  StatusCode TrigL1TopoROBMonitor::monitorBlock(uint32_t sourceID, L1Topo::Header& header, std::vector<uint32_t>& /* vFibreSizes */, std::vector<uint32_t>& vFibreStatus, std::vector<L1Topo::L1TopoTOB>& /* daqTobs */) {
  ATH_MSG_DEBUG( "monitorBlock" );
  ATH_MSG_DEBUG( header );
  if (header.payload_crc()!=0){
    ATH_MSG_WARNING( "header payload CRC not zero: " << L1Topo::formatHex8(header.payload_crc()) );
    scoped_lock_histogram lock;
    m_histPayloadCRCFromDAQConv->Fill( (L1Topo::formatHex8(header.payload_crc())).c_str(), 1. );
    m_histPayloadCRCFromDAQConv->LabelsDeflate("X");
  }
  if (vFibreStatus.size()!=header.active_fibres()){
    ATH_MSG_WARNING( "Mismatch between number of fibres declared in header " << header.active_fibres() << " and number found " << vFibreStatus.size() );
  }
  for (unsigned int i=0; i<vFibreStatus.size(); ++i){
    if (vFibreStatus.at(i)!=0){
      ATH_MSG_WARNING( "Fibre status set for fibre " << i << " of ROB " << L1Topo::formatHex8(sourceID) << " header " << header );
    }
    scoped_lock_histogram lock;
    m_histFibreStatusFlagsFromDAQConv->Fill(i,vFibreStatus.at(i));
  }
  return StatusCode::SUCCESS;
}

