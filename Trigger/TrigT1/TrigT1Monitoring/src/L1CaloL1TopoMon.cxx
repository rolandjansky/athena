/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:        L1CaloL1TopoMon.cxx
// PACKAGE:     TrigT1CaloMonitoring
//
// AUTHOR:      Joergen Sjoelin (sjolin@fysik.su.se)
//
// DESCRIPTION: Monitoring of L1Calo --> L1Topo transmission
//
// ********************************************************************

#include <map>
#include <utility>

#include <algorithm>
#include <vector>
#include <iomanip>
#include <sstream>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "LWHists/LWHist.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"

#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigT1CaloMonitoringTools/ITrigT1CaloMonErrorTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1Interfaces/FrontPanelCTP.h"
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"

#include "TrigT1CaloEvent/CMXJetTobCollection.h"
#include "TrigT1Result/RoIBResult.h"
#include "L1TopoRDO/L1TopoRDOCollection.h"
#include "L1TopoRDO/Helpers.h"
#include "L1TopoRDO/Header.h"
#include "L1TopoRDO/Fibre.h"
#include "L1TopoRDO/Status.h"
#include "L1TopoRDO/L1TopoTOB.h"
#include "L1TopoRDO/BlockTypes.h"
#include "L1CaloL1TopoMon.h"

// ============================================================================
// To be moved to L1TopoRDO Helpers.cxx
namespace L1Topo {
// ============================================================================
const std::string formatVecHex8(const std::vector<uint32_t>& vec)
{
  std::ostringstream s;
  s << "[ ";
  for (auto elem : vec) {
    s << std::hex << std::showbase << std::setfill('0') << std::setw(8)
      << elem << " " << std::dec << std::noshowbase;
  }
  s << "]";
  return s.str();
}
// ============================================================================
}  // end namespace
// ============================================================================

// ============================================================================
namespace LVL1 {
// ============================================================================

// *********************************************************************
// Public Methods
// *********************************************************************

/*---------------------------------------------------------*/
L1CaloL1TopoMon::L1CaloL1TopoMon( const std::string & type,
                                  const std::string & name,
                                  const IInterface* parent )
  : ManagedMonitorToolBase( type, name, parent ),
    m_configSvc("TrigConf::TrigConfigSvc/TrigConfigSvc", name),
    m_errorTool("LVL1::TrigT1CaloMonErrorTool/TrigT1CaloMonErrorTool"),
    m_histTool("LVL1::TrigT1CaloLWHistogramTool/TrigT1CaloLWHistogramTool"),
    m_debug(false), m_histBooked(false),
    m_h_l1topo_1d_CMXTobs(0),
    m_h_l1topo_1d_Simulation(0),
    m_h_l1topo_1d_JetTobs_EnergyLg(0),
    m_h_l1topo_2d_JetTobs_Hitmap_mismatch(0),
    m_h_l1topo_2d_JetTobs_Hitmap_match(0),
    m_h_l1topo_1d_Errors(0),
    m_h_l1topo_1d_DAQTobs(0),
    m_h_l1topo_1d_DAQJetTobs_no0(0),
    m_h_l1topo_1d_DAQJetTobs(0),
    m_h_l1topo_1d_DAQTauTobs_no0(0),
    m_h_l1topo_1d_DAQTauTobs(0),
    m_h_l1topo_1d_DAQEMTobs_no0(0),
    m_h_l1topo_1d_DAQEMTobs(0),
    m_h_l1topo_1d_DAQMuonTobs_no0(0),
    m_h_l1topo_1d_DAQMuonTobs(0),
    m_h_l1topo_1d_DAQEnergyTobs(0),
    m_h_l1topo_1d_DAQCTPSignal(0),
    m_h_l1topo_1d_DAQTriggerBits(0),
    m_h_l1topo_1d_DAQMismatchTriggerBits(0),
    m_h_l1topo_1d_DAQOverflowBits(0),
    m_h_l1topo_1d_ROITobs(0)

    /*---------------------------------------------------------*/
{
  // This is how you declare the parameters to Gaudi so that
  // they can be over-written via the job options file
  
  declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
  declareProperty( "PathInRootFile",
		   m_PathInRootFile= "LVL1_Interfaces/L1Topo");
  declareProperty( "CMXJetTobLocation", m_CMXJetTobLocation
		   = LVL1::TrigT1CaloDefs::CMXJetTobLocation);
  declareProperty( "TopoCTPLocation", m_topoCTPLoc
		   = LVL1::DEFAULT_L1TopoCTPLocation, 
		   "StoreGate location of topo inputs" );
}
  
/*---------------------------------------------------------*/
L1CaloL1TopoMon::~L1CaloL1TopoMon()
/*---------------------------------------------------------*/
{
}
  
#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

/*---------------------------------------------------------*/
StatusCode L1CaloL1TopoMon::initialize()
/*---------------------------------------------------------*/
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << PACKAGE_VERSION << endreq;
  m_debug = msgLvl(MSG::DEBUG);

  StatusCode sc = ManagedMonitorToolBase::initialize();
  if (sc.isFailure()) return sc;

  sc = m_configSvc.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Couldn't connect to " << m_configSvc.typeAndName()
                    << endreq;
    return sc;
  } else {
    msg(MSG::INFO) << "Connected to " << m_configSvc.typeAndName() << endreq;
  }
  
  sc = m_errorTool.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloMonErrorTool"
                  << endreq;
    return sc;
  }

  sc = m_histTool.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloLWHistogramTool"
                    << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode L1CaloL1TopoMon::bookHistogramsRecurrent()
/*---------------------------------------------------------*/
{
  msg(MSG::DEBUG) << "in L1CaloL1TopoMon::bookHistograms" << endreq;

  if ( m_environment == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
  }

  if ( m_dataType == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
  }

  if ( newLumiBlock ) {}

  if ( newRun ) {

    MgmtAttr_t attr = ATTRIB_UNMANAGED;
    MonGroup L1CaloL1Topo( this, m_PathInRootFile, run, attr );
    MonGroup L1CaloL1TopoEvents( this, m_PathInRootFile, run, attr,
                                 "", "eventSample" );

    m_histTool->setMonGroup(&L1CaloL1Topo);

    m_h_l1topo_1d_Errors =
      m_histTool->book1F("l1topo_1d_Errors",
			 "L1Topo error summary",
			 ERROR_BIT::NUMBEROFBITS, 0, ERROR_BIT::NUMBEROFBITS);
    for (int i=0; i<ERROR_BIT::NUMBEROFBITS;++i)
      m_h_l1topo_1d_Errors->GetXaxis()->
	SetBinLabel(i+1,ERROR_LABELS[i].c_str());

    m_h_l1topo_1d_CMXTobs =
      m_histTool->book1F("l1topo_1d_CMXTobs",
			 "Number of CMX TOBs",
			 200, 0, 200);

    m_h_l1topo_1d_Simulation =
      m_histTool->book1F("l1topo_1d_Simulation",
			 "Simulated L1Topo trigger bits", 128, 0, 128);

    m_h_l1topo_2d_JetTobs_Hitmap_mismatch = m_histTool->
      bookJEMCrateModuleVsFrameLoc("l1topo_2d_JetTobs_Hitmap_mismatch",
				   "Mismatched L1Topo-Jet TOBs Hit Map");
    m_h_l1topo_2d_JetTobs_Hitmap_match = m_histTool->
      bookJEMCrateModuleVsFrameLoc("l1topo_2d_JetTobs_Hitmap_match",
				   "CMX-matched L1Topo-Jet TOBs Hit Map");
    m_h_l1topo_1d_JetTobs_EnergyLg = m_histTool->
      book1F("l1topo_1d_JetTobs_EnergyLg",
	     "L1Topo-Jet TOB Energy Large Window Size", 256, 0., 1024);
    m_h_l1topo_1d_DAQTobs =
      m_histTool->book1F("l1topo_1d_DAQTobs",
                         "Number of L1Topo DAQ L1Topo TOBs",
                         20, 0, 20);
    m_h_l1topo_1d_DAQJetTobs_no0 =
      m_histTool->book1F("l1topo_1d_DAQJetTobs_no0",
                         "Number of L1Topo DAQ Jet TOBs (zero supp.)",
                         200, 0, 200);
    m_h_l1topo_1d_DAQJetTobs =
      m_histTool->book1F("l1topo_1d_DAQJetTobs",
                         "Number of L1Topo DAQ Jet TOBs",
                         200, 0, 200);
    m_h_l1topo_1d_DAQTauTobs_no0 =
      m_histTool->book1F("l1topo_1d_DAQTauTobs_no0",
			 "Number of L1Topo DAQ Tau TOBs (zero supp.)",
			 200, 0, 200);
    m_h_l1topo_1d_DAQTauTobs =
      m_histTool->book1F("l1topo_1d_DAQTauTobs",
			 "Number of L1Topo DAQ Tau TOBs",
			 200, 0, 200);
    m_h_l1topo_1d_DAQEMTobs_no0 =
      m_histTool->book1F("l1topo_1d_DAQEMTobs_no0",
			 "Number of L1Topo DAQ EM TOBs (zero supp.)",
			 200, 0, 200);
    m_h_l1topo_1d_DAQEMTobs =
      m_histTool->book1F("l1topo_1d_DAQEMTobs",
			 "Number of L1Topo DAQ EM TOBs",
			 200, 0, 200);
    m_h_l1topo_1d_DAQMuonTobs_no0 =
      m_histTool->book1F("l1topo_1d_DAQMuonTobs_no0",
			 "Number of L1Topo DAQ Muon TOBs (zero supp.)",
			 200, 0, 200);
    m_h_l1topo_1d_DAQMuonTobs =
      m_histTool->book1F("l1topo_1d_DAQMuonTobs",
			 "Number of L1Topo DAQ Muon TOBs",
			 200, 0, 200);
    m_h_l1topo_1d_DAQEnergyTobs =
      m_histTool->book1F("l1topo_1d_DAQEnergyTobs",
			 "Number of L1Topo DAQ Energy TOBs",
			 20, 0, 20);
    m_h_l1topo_1d_DAQCTPSignal =
      m_histTool->bookTH1F("l1topo_1d_DAQCTPSignal",
			 "L1Topo DAQ CTP signal",
			 1, 0, 1);
    m_h_l1topo_1d_DAQTriggerBits =
      m_histTool->book1F("l1topo_1d_DAQTriggerBits",
			 "L1Topo DAQ trigger bits",
			 128, 0, 128);
    m_h_l1topo_1d_DAQMismatchTriggerBits =
      m_histTool->book1F("l1topo_1d_DAQMismatchTriggerBits",
			 "L1Topo DAQ-Simulation trigger bits mismatch",
			 128, 0, 128);
    m_h_l1topo_1d_DAQOverflowBits =
      m_histTool->book1F("l1topo_1d_DAQOverflowBits",
			 "L1Topo DAQ overflow bits",
			 128, 0, 128);
    m_h_l1topo_1d_ROITobs =
      m_histTool->book1F("l1topo_1d_ROITobs",
			 "Number of L1Topo ROI L1Topo TOBs",
			 20, 0, 20);

    m_histTool->unsetMonGroup();
    m_histBooked = true;

  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode L1CaloL1TopoMon::fillHistograms()
/*---------------------------------------------------------*/
{
  if (m_debug) msg(MSG::DEBUG) << "in L1CaloL1TopoMon::fillHistograms"
			       << endreq;

  if (!m_histBooked) {
    if (m_debug) msg(MSG::DEBUG) << "Histogram(s) not booked" << endreq;
    return StatusCode::SUCCESS;
  }

  StatusCode sc = StatusCode::SUCCESS;
  std::vector<LVL1::CMXJetTob*> cmxtobs;  

  // Validate properly unpacked input from L1Calo
  if (m_errorTool->corrupt() || m_errorTool->robOrUnpackingError()) {
    if (m_debug) msg(MSG::DEBUG) << "Corrupt L1Calo event" << endreq;
    m_h_l1topo_1d_Errors->Fill(CALO_CONV);
  }

  // Retrieve CMX tobs
  bool cmx_ematch=true;
  const DataHandle<CMXJetTobCollection> cmxtob = 0;
  sc = evtStore()->retrieve(cmxtob);
  if (sc.isFailure() || !cmxtob) {
    ATH_MSG_DEBUG ("No CMX tobs found in TES");
    m_h_l1topo_1d_Errors->Fill(NO_CMX);
  }   
  else {
    ATH_MSG_DEBUG( "Found CMXJetTobCollection, looping on TOBs ..." );
    for (auto & tob : *cmxtob) {
      if (tob->energyLarge()) {
	cmxtobs.push_back(tob);
      }
    }
    m_h_l1topo_1d_CMXTobs->Fill(std::min((int)cmxtobs.size(),199));
  }

  // Retrieve L1Topo CTP simulted decision if present
  if ( evtStore()->retrieve( m_topoCTP, m_topoCTPLoc ).isSuccess() ) {
    for(unsigned int i=0; i<32; ++i) {
      uint32_t mask = 0x1; mask <<= i;
      if( (m_topoCTP->cableWord0(0) & mask) != 0 )
	m_h_l1topo_1d_Simulation->Fill(i); // cable 0, clock 0
      if( (m_topoCTP->cableWord0(1) & mask) != 0 )
	m_h_l1topo_1d_Simulation->Fill(32 + i); // cable 0, clock 1
      if( (m_topoCTP->cableWord1(0) & mask) != 0 )
	m_h_l1topo_1d_Simulation->Fill(64 + i); // cable 1, clock 0
      if( (m_topoCTP->cableWord1(1) & mask) != 0 )
	m_h_l1topo_1d_Simulation->Fill(96 + i); // cable 1, clock 1
    }
    ATH_MSG_DEBUG("Simulated output from L1Topo from StoreGate with key "
		  << m_topoCTPLoc);
    ATH_MSG_DEBUG("L1Topo word 1 at clock 0 is: 0x"
		  << std::hex << std::setw( 8 ) << std::setfill( '0' )
		  << m_topoCTP->cableWord0(0));
    ATH_MSG_DEBUG("L1Topo word 2 at clock 0 is: 0x"
		  << std::hex << std::setw( 8 ) << std::setfill( '0' )
		  << m_topoCTP->cableWord1(0));
    ATH_MSG_DEBUG("L1Topo word 1 at clock 1 is: 0x"
		  << std::hex << std::setw( 8 ) << std::setfill( '0' )
		  << m_topoCTP->cableWord0(1));
    ATH_MSG_DEBUG("L1Topo word 2 at clock 1 is: 0x"
		  << std::hex << std::setw( 8 ) << std::setfill( '0' )
		  << m_topoCTP->cableWord1(1));
  } else {
    ATH_MSG_DEBUG("No simulated output from L1Topo from StoreGate");
  }       
  
  // Retrieve the L1Topo RDOs from the DAQ RODs
  const DataHandle<L1TopoRDOCollection> rdos = 0;
  sc = evtStore()->retrieve(rdos);
  if (sc.isFailure() or 0 == rdos) {
    m_h_l1topo_1d_Errors->Fill(NO_DAQ);
    ATH_MSG_DEBUG ( "Could not retrieve L1Topo DAQ RDO collection "
		    "from StoreGate" );
  }
  else {
    // initialise collections filled for all blocks
    std::vector<L1Topo::L1TopoTOB> daqTobs;
    std::vector<L1Topo::L1TopoTOB> daqTobsBC0;
    std::vector<uint32_t> daqJetTobs_no0,daqJetTobs;
    std::vector<uint32_t> daqTauTobs_no0,daqTauTobs;
    std::vector<uint32_t> daqEMTobs_no0,daqEMTobs;
    std::vector<uint32_t> daqMuonTobs_no0,daqMuonTobs;
    std::vector<uint32_t> daqEnergyTobs;
    std::vector<uint32_t> vFibreSizes;
    std::vector<uint32_t> vFibreStatus;
    
    for (auto & rdo : *rdos){
      ATH_MSG_VERBOSE( *rdo );
      ATH_MSG_DEBUG( "Found DAQ RDO with source ID "
                     << L1Topo::formatHex8(rdo->getSourceID()) );
      /*
      m_histSIDsViaConverters->
      Fill((L1Topo::formatHex8(rdo->getSourceID())).c_str(), 1.);
      m_histSIDsViaConverters->LabelsDeflate("X");
      */
      auto errors = rdo->getErrors();
      if (! errors.empty()){
	ATH_MSG_WARNING( "Converter errors reported: " << errors );
	m_h_l1topo_1d_Errors->Fill(DAQ_CONV);
      }
      const std::vector<uint32_t> cDataWords = rdo->getDataWords();

      if ( cDataWords.size() == 0 ) {
        ATH_MSG_DEBUG ( "L1TopoRDO DAQ is empty" );
	m_h_l1topo_1d_Errors->Fill(NO_DAQ);
      }

      // initialise header: beware, this can make a valid-looking header
      //   set version 15, BCN -7, which is unlikely:
      L1Topo::Header header(0xf, 0, 0, 0, 0, 1, 0x7);
      for (auto word : cDataWords){
	switch (L1Topo::blockType(word)){
	case L1Topo::BlockTypes::HEADER:
	  {
	    header = L1Topo::Header(word);
	    if (header.payload_crc()!=0) {
	      m_h_l1topo_1d_Errors->Fill(PAYL_CRC);
	    }
	    break;
	  }
	case L1Topo::BlockTypes::FIBRE:
	  {
	    auto fibreBlock = L1Topo::Fibre(word);
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
	    ATH_MSG_WARNING( "fibre overflow: " << status.overflow()
			     << " fibre crc: " << status.crc() );
	    if (status.overflow()) m_h_l1topo_1d_Errors->Fill(F_OVERFLOW);
	    if (status.crc()) m_h_l1topo_1d_Errors->Fill(F_CRC);
	    break;
	  }
	case L1Topo::BlockTypes::L1TOPO_TOB:
	  {
	    auto tob = L1Topo::L1TopoTOB(word);
	    ATH_MSG_DEBUG(tob);
	    daqTobs.push_back(tob);
	    if (header.bcn_offset()==0){
	      daqTobsBC0.push_back(tob);
	    }
	    auto index = L1Topo::triggerBitIndex(rdo->getSourceID(),tob);
	    m_h_l1topo_1d_DAQCTPSignal->
	      Fill(L1Topo::formatHex4(tob.ctp_signal()).c_str(),1.);
	    m_h_l1topo_1d_DAQCTPSignal->LabelsDeflate("X");
	    for (unsigned int i=0; i<8; ++i){
	      m_h_l1topo_1d_DAQTriggerBits->
		Fill(index+i,(tob.trigger_bits() >>i)&1);
	      m_h_l1topo_1d_DAQOverflowBits->
		Fill(index+i,(tob.overflow_bits()>>i)&1);
	    }
	    break;
	  }
	case L1Topo::BlockTypes::JET1_TOB:
	case L1Topo::BlockTypes::JET2_TOB:
	  {
	    const int crate    = (word >> 28) & 0x1;
	    const int jem      = (word >> 24) & 0xF;
	    const int frame    = (word >> 21) & 0x7;
	    const int location = (word >> 19) & 0x3;
	    const int energyL  = (word & 0x2FF);
	    const int x = crate*16 + jem;
	    const int y = frame*4 + location;
	    //auto tob = L1Topo::JetTOB(word);
	    //daqJetTobs.push_back(tob);
	    int tob = 1; // Fake object until defined
	    if (energyL) {
	      m_h_l1topo_1d_JetTobs_EnergyLg->Fill(energyL);
	      daqJetTobs_no0.push_back(tob);
	      bool match=false;
	      bool ematch=false;
	      for (auto & t : cmxtobs) {
		const int cmx_x = t->crate()*16 + t->jem();
		const int cmx_y = t->frame()*4 + t->location();
		if (x==cmx_x && y==cmx_y && energyL==t->energyLarge())
		  match=true;
		if (energyL==t->energyLarge())
		  ematch=true;
	      }
	      if (!ematch) cmx_ematch=false;
	      if (match)
		m_h_l1topo_2d_JetTobs_Hitmap_match->Fill(x, y);
	      else
		m_h_l1topo_2d_JetTobs_Hitmap_mismatch->Fill(x, y);
	    }
	    else
	      daqJetTobs.push_back(tob);
	    break;
	  }
	case L1Topo::BlockTypes::TAU_TOB:
	  {
	    //auto tob = L1Topo::TauTOB(word);
	    //daqTauTobs.push_back(tob);
	    int tob = 1; // Fake object until defined
	    if (word & 0xFF)
	      daqTauTobs_no0.push_back(tob);
	    else
	      daqTauTobs.push_back(tob);
	    break;
	  }
	case L1Topo::BlockTypes::EM_TOB:
	  {
	    //auto tob = L1Topo::EMTOB(word);
	    //daqEMTobs.push_back(tob);
	    int tob = 1; // Fake object until defined
	    if (word & 0xFF)
	      daqEMTobs_no0.push_back(tob);
	    else
	      daqEMTobs.push_back(tob);
	    break;
	  }
	case L1Topo::BlockTypes::MUON_TOB:
	  {
	    //auto tob = L1Topo::MuonTOB(word);
	    //daqMuonTobs.push_back(tob);
	    int tob = 1; // Fake object until defined
	    if (word & 0x202)
	      daqMuonTobs_no0.push_back(tob);
	    else
	      daqMuonTobs.push_back(tob);
	    break;
	  }
	case L1Topo::BlockTypes::ENERGY_TOB:
	  {
	    //auto tob = L1Topo::EnergyTOB(word);
	    //daqEnergyTobs.push_back(tob);
	    int tob = 1; // Fake object until defined
	    daqEnergyTobs.push_back(tob);
	    break;
	  }
	default:
	  {
	    break;
	  }
	}
      } // for word
    }
    m_h_l1topo_1d_DAQTobs->Fill(std::min((int)daqTobs.size(),19));
    m_h_l1topo_1d_DAQJetTobs_no0->Fill(std::min((int)daqJetTobs_no0.size(),
						199));
    m_h_l1topo_1d_DAQJetTobs->Fill(std::min((int)daqJetTobs.size(),199));
    m_h_l1topo_1d_DAQTauTobs_no0->Fill(std::min((int)daqTauTobs_no0.size(),
						199));
    m_h_l1topo_1d_DAQTauTobs->Fill(std::min((int)daqTauTobs.size(),199));
    m_h_l1topo_1d_DAQMuonTobs_no0->Fill(std::min((int)daqMuonTobs_no0.size(),
						 199));
    m_h_l1topo_1d_DAQMuonTobs->Fill(std::min((int)daqMuonTobs.size(),199));
    m_h_l1topo_1d_DAQEMTobs->Fill(std::min((int)daqEMTobs.size(),199));
    m_h_l1topo_1d_DAQEnergyTobs->Fill(std::min((int)daqEnergyTobs.size(),19));
  }

  // Retrieve and print the L1Topo RDOs from the ROI RODs
  std::vector<L1Topo::L1TopoTOB> roiTobs;
  const ROIB::RoIBResult* roibresult = 0;
  CHECK (evtStore()->retrieve(roibresult) );
  const std::vector< ROIB::L1TopoResult > l1TopoResults =
    roibresult->l1TopoResult();
  ATH_MSG_DEBUG( "Number of L1Topo ROI RODs found: " << l1TopoResults.size() );
  for (auto & r : l1TopoResults) {
    //ATH_MSG_VERBOSE( r.dump() );
    auto rdo = r.rdo();
    ATH_MSG_DEBUG( "Found ROI RDO with source ID "
		   << L1Topo::formatHex8(rdo.getSourceID()) );
    //m_histSIDsViaConverters->Fill((L1Topo::formatHex8(rdo.getSourceID())).c_str(), 1.);
    //m_histSIDsViaConverters->LabelsDeflate("X");
    auto errors = rdo.getErrors();
    if (! errors.empty()) {
      ATH_MSG_WARNING( "Converter errors reported: " << errors );
      m_h_l1topo_1d_Errors->Fill(ROI_CONV);
    }
    const std::vector<uint32_t> cDataWords = rdo.getDataWords();
    if ( cDataWords.size() == 0 ) {
      ATH_MSG_DEBUG ( "L1TopoRDO ROI is empty" );
      m_h_l1topo_1d_Errors->Fill(NO_ROI);
    }
    for (auto word : cDataWords) {
      switch (L1Topo::blockType(word)) {
      case L1Topo::BlockTypes::L1TOPO_TOB:
      {
        auto tob = L1Topo::L1TopoTOB(word);
        ATH_MSG_DEBUG( tob );
        roiTobs.push_back(tob);
        //auto index = L1Topo::triggerBitIndex(rdo.getSourceID(),tob);
        //m_histCTPSignalPartFromROIConv->Fill(L1Topo::formatHex4(tob.ctp_signal()).c_str(), 1.);
        //m_histCTPSignalPartFromROIConv->LabelsDeflate("X");
        for (unsigned int i = 0; i < 8; ++i) {
          //m_histTriggerBitsFromROIConv->Fill (index+i,(tob.trigger_bits() >>i)&1);
          //m_histOverflowBitsFromROIConv->Fill(index+i,(tob.overflow_bits()>>i)&1);
        }
        // histogram trigger and overflow bits
        break;
      }
      default:
      {
        ATH_MSG_WARNING( "unexpected TOB type in ROI: "
			 <<  L1Topo::formatHex8(word) );
        break;
      }
      }
    }
  }
  m_h_l1topo_1d_ROITobs->Fill(std::min((int)roiTobs.size(),19));

  for (unsigned int i=1; i<=128;++i) {
    float diff=fabs(m_h_l1topo_1d_DAQTriggerBits->GetBinContent(i)-
		    m_h_l1topo_1d_Simulation->GetBinContent(i));
    if (diff>0.1)
      m_h_l1topo_1d_DAQMismatchTriggerBits->SetBinContent(i,diff);
  }

  if (!cmx_ematch) m_h_l1topo_1d_Errors->Fill(CMX_EMATCH);

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode L1CaloL1TopoMon::procHistograms()
/*---------------------------------------------------------*/
{
  if ( endOfLumiBlock ) { }

  if ( endOfRun ) { }

  return StatusCode::SUCCESS;
}

// *********************************************************************
// Private Methods
// *********************************************************************

}
