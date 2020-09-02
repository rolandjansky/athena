/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include <map>
#include <utility>
#include <set>
#include <tuple>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <sstream>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"

#include "EventInfo/EventID.h"
#include "EventInfo/EventInfo.h"

#include "LWHists/LWHist.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"

#include "TrigConfInterfaces/ITrigConfigSvc.h"
#include "TrigConfL1Data/ThresholdConfig.h"
#include "TrigT1CaloMonitoringTools/ITrigT1CaloMonErrorTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1Interfaces/FrontPanelCTP.h"
#include "TrigT1Interfaces/TrigT1StoreGateKeys.h"
#include "TrigT1Interfaces/CoordinateRange.h"
#include "TrigT1Interfaces/CPRoIDecoder.h"
#include "TrigT1Interfaces/JEPRoIDecoder.h"

#include "xAODTrigL1Calo/CMXJetTobContainer.h"
#include "xAODTrigL1Calo/CMXCPTobContainer.h"
#include "TrigT1Result/CTP_RDO.h"
#include "TrigT1Result/CTP_Decoder.h"
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
    m_debug(false), m_histBooked(false),m_lumiNo(0),
    m_h_l1topo_1d_CMXTobs(0),
    m_h_l1topo_1d_Simulation(0),
    m_h_l1topo_1d_JetTobs_EnergyLg(0),
    m_h_l1topo_2d_Tobs_Hitmap_mismatch{},
    m_h_l1topo_2d_Tobs_Hitmap_match{},
    m_h_l1topo_2d_Tobs_etaPhi_mismatch{},
    m_h_l1topo_2d_Tobs_etaPhi_match{},
    m_h_l1topo_1d_Errors(0),
    m_h_l1topo_1d_Overflows(0),
    m_h_l1topo_1d_DAQTobs(0),
    m_h_l1topo_1d_DAQJetTobs(0),
    m_h_l1topo_1d_DAQTauTobs(0),
    m_h_l1topo_1d_DAQEMTobs(0),
    m_h_l1topo_1d_DAQMuonTobs(0),
    m_h_l1topo_1d_DAQTriggerBits(0),
    m_h_l1topo_1d_DAQMismatchTriggerBits(0),
    m_h_l1topo_1d_DAQOverflowBits(0),
    m_h_l1topo_1d_ROITobs(0),
    m_h_l1topo_1d_ErrorsByLumiblock(0),
    m_h_l1topo_2d_ItemsBC{},
    m_h_l1topo_2d_ItemsBC_ratio{}
  

    /*---------------------------------------------------------*/
{
  // This is how you declare the parameters to Gaudi so that
  // they can be over-written via the job options file
  
  declareProperty( "TrigConfigSvc", m_configSvc, "Trig Config Service");
  declareProperty( "PathInRootFile",
		   m_PathInRootFile= "LVL1_Interfaces/L1Topo");
  declareProperty( "CMXJetTobLocation", m_CMXJetTobLocation
		   = LVL1::TrigT1CaloDefs::CMXJetTobLocation);
  declareProperty( "CMXCPTobLocation", m_CMXCPTobLocation
		   = LVL1::TrigT1CaloDefs::CMXCPTobLocation);
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
                 << PACKAGE_VERSION << endmsg;
  m_debug = msgLvl(MSG::DEBUG);

  StatusCode sc = ManagedMonitorToolBase::initialize();
  if (sc.isFailure()) return sc;

  sc = m_configSvc.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Couldn't connect to " << m_configSvc.typeAndName()
                    << endmsg;
    return sc;
  } else {
    msg(MSG::INFO) << "Connected to " << m_configSvc.typeAndName() << endmsg;
  }

  sc = m_errorTool.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloMonErrorTool"
                  << endmsg;
    return sc;
  }

  sc = m_histTool.retrieve();
  if ( sc.isFailure() ) {
    msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloLWHistogramTool"
                    << endmsg;
    return sc;
  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode L1CaloL1TopoMon::bookHistogramsRecurrent()
/*---------------------------------------------------------*/
{


  msg(MSG::DEBUG) << "in L1CaloL1TopoMon::bookHistograms" << endmsg;

  if ( m_environment == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
  }

  if ( m_dataType == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
  }

  if (newRunFlag() || newLumiBlockFlag()) {
    const EventInfo *evtInfo = 0;
    StatusCode sc = evtStore()->retrieve(evtInfo);
    if (sc.isSuccess()) {
      m_lumiNo = evtInfo->event_ID()->lumi_block();
    }
  }

  //if ( newLumiBlockFlag() ) {}

  if ( newRunFlag() ) {

    MgmtAttr_t attr = ATTRIB_UNMANAGED;
    MonGroup L1CaloL1Topo( this, m_PathInRootFile, run, attr, "", "merge" );

    m_histTool->setMonGroup(&L1CaloL1Topo);

    m_h_l1topo_1d_Errors =
      m_histTool->book1F("l1topo_1d_Errors",
			 "L1Topo error summary;;Number of Events",
			 ERROR_BIT::NUMBEROFBITS, 0, ERROR_BIT::NUMBEROFBITS);

    m_h_l1topo_1d_Overflows =
      m_histTool->book1F("l1topo_1d_Overflow",
			 "L1Topo overflows;Overflow;Number of Events",1,0,1);

    for (int i=0; i<ERROR_BIT::NUMBEROFBITS;++i)
      m_h_l1topo_1d_Errors->GetXaxis()->
	SetBinLabel(i+1,ERROR_LABELS[i].c_str());

    m_h_l1topo_1d_ErrorsByLumiblock =
      m_histTool->book1F("l1topo_1d_ErrorsByLumiblock",
			 "Events with Errors by Lumiblock;"
			 "Lumi Block;Number of Events",2500,0,2500);

    m_h_l1topo_1d_CMXTobs =
      m_histTool->book1F("l1topo_1d_CMXTobs",
			 "Number of CMX TOBs",
			 MAXTOBS, 0, MAXTOBS);

    m_h_l1topo_1d_Simulation =
      m_histTool->book1F("l1topo_1d_Simulation",
			 "Simulated L1Topo trigger bits", 128, 0, 128);

    //---- eta phi ----
    
    m_h_l1topo_2d_Tobs_etaPhi_mismatch[JETS_TOB] = m_histTool->
      bookJEMRoIEtaVsPhi("l1topo_2d_JetSTobs_etaPhi_mismatch",
			 "CMX-L1Topo mismatched small jet TOBs hit map");
    m_h_l1topo_2d_Tobs_etaPhi_match[JETS_TOB] = m_histTool->
      bookJEMRoIEtaVsPhi("l1topo_2d_JetSTobs_etaPhi_match",
			 "CMX-L1Topo matched small jet TOBs hit map");
    m_h_l1topo_2d_Tobs_etaPhi_mismatch[JETL_TOB] = m_histTool->
      bookJEMRoIEtaVsPhi("l1topo_2d_JetLTobs_etaPhi_mismatch",
			 "CMX-L1Topo mismatched large jet TOBs hit map");
    m_h_l1topo_2d_Tobs_etaPhi_match[JETL_TOB] = m_histTool->
      bookJEMRoIEtaVsPhi("l1topo_2d_JetLTobs_etaPhi_match",
			 "CMX-L1Topo matched large jet TOBs hit map");
    m_h_l1topo_2d_Tobs_etaPhi_mismatch[TAU_TOB] = m_histTool->
      bookCPMEtaVsPhi("l1topo_2d_TauTobs_etaPhi_mismatch",
		      "CMX-L1Topo mismatched tau TOBs hit map");
    m_h_l1topo_2d_Tobs_etaPhi_match[TAU_TOB] = m_histTool->
      bookCPMEtaVsPhi("l1topo_2d_TauTobs_etaPhi_match",
		      "CMX-L1Topo matched tau TOBs hit map");
    m_h_l1topo_2d_Tobs_etaPhi_mismatch[EM_TOB] = m_histTool->
      bookCPMEtaVsPhi("l1topo_2d_EMTobs_etaPhi_mismatch",
		      "CMX-L1Topo mismatched EM TOBs hit map");
    m_h_l1topo_2d_Tobs_etaPhi_match[EM_TOB] = m_histTool->
      bookCPMEtaVsPhi("l1topo_2d_EMTobs_etaPhi_match",
		      "CMX-L1Topo matched EM TOBs hit map");    
    m_h_l1topo_2d_Tobs_etaPhi_mismatch[MU_TOB] = m_histTool->
      bookCPMEtaVsPhi("l1topo_2d_MuTobs_etaPhi_mismatch",
		      "CMX-L1Topo mismatched muon TOBs hit map");
    m_h_l1topo_2d_Tobs_etaPhi_match[MU_TOB] = m_histTool->
      bookCPMEtaVsPhi("l1topo_2d_MuTobs_etaPhi_match",
		      "CMX-L1Topo matched muon TOBs hit map");
    
    // ---- hardware coordinates ----

    m_h_l1topo_2d_Tobs_Hitmap_mismatch[JETS_TOB] = m_histTool->
      bookJEMCrateModuleVsFrameLoc("l1topo_2d_JetSTobs_Hitmap_mismatch",
				   "CMX-L1Topo mismatched small jet TOBs hit map");
    m_h_l1topo_2d_Tobs_Hitmap_match[JETS_TOB] = m_histTool->
      bookJEMCrateModuleVsFrameLoc("l1topo_2d_JetSTobs_Hitmap_match",
				   "CMX-L1Topo matched small jet TOBs hit map");
    m_h_l1topo_2d_Tobs_Hitmap_mismatch[JETL_TOB] = m_histTool->
      bookJEMCrateModuleVsFrameLoc("l1topo_2d_JetLTobs_Hitmap_mismatch",
				   "CMX-L1Topo mismatched large jet TOBs hit map");
    m_h_l1topo_2d_Tobs_Hitmap_match[JETL_TOB] = m_histTool->
      bookJEMCrateModuleVsFrameLoc("l1topo_2d_JetLTobs_Hitmap_match",
				   "CMX-L1Topo matched large jet TOBs hit map");
    m_h_l1topo_2d_Tobs_Hitmap_mismatch[TAU_TOB] = m_histTool->
      bookCPMCrateModuleVsTobChipLocalCoord("l1topo_2d_TauTobs_Hitmap_mismatch",
					    "CMX-L1Topo mismatched tau TOBs hit map");
    m_h_l1topo_2d_Tobs_Hitmap_match[TAU_TOB] = m_histTool->
      bookCPMCrateModuleVsTobChipLocalCoord("l1topo_2d_TauTobs_Hitmap_match",
					    "CMX-L1Topo matched tau TOBs hit map");
    m_h_l1topo_2d_Tobs_Hitmap_mismatch[EM_TOB] = m_histTool->
      bookCPMCrateModuleVsTobChipLocalCoord("l1topo_2d_EMTobs_Hitmap_mismatch",
					    "CMX-L1Topo mismatched EM TOBs hit map");
    m_h_l1topo_2d_Tobs_Hitmap_match[EM_TOB] = m_histTool->
      bookCPMCrateModuleVsTobChipLocalCoord("l1topo_2d_EMTobs_Hitmap_match",
					    "CMX-L1Topo matched EM TOBs hit map");
    m_h_l1topo_2d_Tobs_Hitmap_mismatch[MU_TOB] = m_histTool->
      bookCPMCrateModuleVsTobChipLocalCoord("l1topo_2d_MuTobs_Hitmap_mismatch",
					    "CMX-L1Topo mismatched muon TOBs hit map");
    m_h_l1topo_2d_Tobs_Hitmap_match[MU_TOB] = m_histTool->
      bookCPMCrateModuleVsTobChipLocalCoord("l1topo_2d_MuTobs_Hitmap_match",
					    "CMX-L1Topo matched muon TOBs hit map");

    // --------
    
    m_h_l1topo_1d_JetTobs_EnergyLg = m_histTool->
      book1F("l1topo_1d_JetTobs_EnergyLg",
	     "L1Topo-Jet TOB Energy Large Window Size", 256, 0., 1024);
    m_h_l1topo_1d_JetTobs_EnergyLg->SetOption("HIST");

    m_h_l1topo_1d_DAQTobs =
      m_histTool->book1F("l1topo_1d_DAQTobs",
                         "Number of L1Topo DAQ L1Topo TOBs",
                         20, 0, 20);
    m_h_l1topo_1d_DAQJetTobs =
      m_histTool->book1F("l1topo_1d_DAQJetTobs",
                         "Number of L1Topo DAQ Jet TOBs",
                         MAXTOBS, 0, MAXTOBS);
    m_h_l1topo_1d_DAQTauTobs =
      m_histTool->book1F("l1topo_1d_DAQTauTobs",
			 "Number of L1Topo DAQ Tau TOBs",
			 MAXTOBS, 0, MAXTOBS);
    m_h_l1topo_1d_DAQEMTobs =
      m_histTool->book1F("l1topo_1d_DAQEMTobs",
			 "Number of L1Topo DAQ EM TOBs",
			 MAXTOBS, 0, MAXTOBS);
    m_h_l1topo_1d_DAQMuonTobs =
      m_histTool->book1F("l1topo_1d_DAQMuonTobs",
			 "Number of L1Topo DAQ Muon TOBs",
			 MAXTOBS, 0, MAXTOBS);
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
    for (int i=0; i<4; ++i) {
      const std::string textFPGA[4]=
	{"L1Topo_00_U1","L1Topo_00_U2","L1Topo_01_U1","L1Topo_01_U2"};
      m_h_l1topo_2d_ItemsBC[i] =
	m_histTool->bookTH2F(std::string("l1topo_2d_ItemsBC")+
			     std::to_string(i),
			     std::string("Timing vs "
					 "Algorithm Number ")+textFPGA[i],
			     32, i*32, (i+1)*32, 5, -2.5, 2.5);
      m_h_l1topo_2d_ItemsBC_ratio[i][0] =
	m_histTool->bookTH2F(std::string("l1topo_2d_ItemsBC_online_ratio")+
			     std::to_string(i),
			     std::string("(online) Timing Ratio vs "
					 "Algorithm Number ")+textFPGA[i],
			     32, i*32, (i+1)*32, 5, -2.5, 2.5);
      m_h_l1topo_2d_ItemsBC_ratio[i][1] =
	m_histTool->bookTH2F(std::string("l1topo_2d_ItemsBC_ratio")+
			     std::to_string(i),
			     std::string("Timing Ratio vs "
					 "Algorithm Number ")+textFPGA[i],
			     32, i*32, (i+1)*32, 5, -2.5, 2.5);
    }

    // Get L1Topo output bit names from the LVL1 menu thresholds
    std::map<unsigned int, std::string> topoCounterToName;
    const std::vector<TrigConf::TriggerThreshold*>& thrVec = 
      m_configSvc->thresholdConfig()->
      getThresholdVector(TrigConf::L1DataDef::TOPO);
    ATH_MSG_DEBUG("L1Topo bits found in the LVL1 menu = " << thrVec.size());
    if (thrVec.size()>0) {
      for (auto thr : thrVec) {
	if (thr) {
	  ATH_MSG_DEBUG("Looping L1Topo mapping " << thr->mapping());
	  ATH_MSG_DEBUG("Looping L1Topo name " << thr->name());
	  topoCounterToName.emplace(thr->mapping(),thr->name());
	}
	else
	  ATH_MSG_DEBUG("Looping L1Topo bit inactive");
      }
      ATH_MSG_DEBUG("L1Topo bits configured in the LVL1 menu = "
		    << topoCounterToName.size());
      for (unsigned int binIndex=0; binIndex<128; ++binIndex){ 
	std::string label(std::to_string(binIndex));
	auto it = topoCounterToName.find(binIndex);
	if (it != topoCounterToName.end()){
	  const bool USE_ALPHANUM=true;
	  if (USE_ALPHANUM && it->second.length()>2) {
	    label=it->second;
	    for (uint32_t bc=1; bc<=5; ++bc)
	      m_h_l1topo_2d_ItemsBC[binIndex/32]->
		SetBinContent(binIndex%32+1,bc,0.0001); // bins>0 active bins
	  }
	}
	m_h_l1topo_2d_ItemsBC[binIndex/32]->GetXaxis()->
	  SetBinLabel(binIndex%32+1,label.c_str());
      }
    }

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
			       << endmsg;

  if (!m_histBooked) {
    if (m_debug) msg(MSG::DEBUG) << "Histogram(s) not booked" << endmsg;
    return StatusCode::SUCCESS;
  }

  StatusCode sc = StatusCode::SUCCESS;

  int topo_error=0;

  typedef std::tuple<int,int,int,int,int,int> TobKey;
  std::set<TobKey> cmxKeys[TOB_TYPES],topoKeys[TOB_TYPES],
    keyDiff[TOB_TYPES],keyIntersect[TOB_TYPES];
    
  // Validate properly unpacked input from L1Calo
  if (m_errorTool->corrupt() || m_errorTool->robOrUnpackingError()) {
    if (m_debug) msg(MSG::DEBUG) << "Corrupt L1Calo event" << endmsg;
    m_h_l1topo_1d_Errors->Fill(CALO_CONV);
    topo_error|=(1<<CALO_CONV);
  }

  const CTP_RDO* ctpRDO = 0;
  sc = evtStore()->retrieve(ctpRDO,"CTP_RDO");
  if (sc.isFailure()) {
    ATH_MSG_WARNING("Could not find CTP_RDO in StoreGate");
  }
  else {
    CTP_Decoder ctp;
    ctp.setRDO(ctpRDO);
    const uint16_t l1aPos = ctpRDO->getL1AcceptBunchPosition();
    const uint32_t bx=ctp.getBunchCrossings().size();
    const int dbx=bx/2;
    if (l1aPos >= bx) {
      ATH_MSG_INFO( "CTP_RDO gave invalid l1aPos." );
    }
    else {
      ATH_MSG_DEBUG( "CTP l1aPos, size : " << l1aPos << ", " << bx );
      if ( bx>=3 && l1aPos>0) {
	for (int bc=-dbx; bc<=dbx; ++bc) {
	  const CTP_BC& ctp_bc = ctp.getBunchCrossings().at(l1aPos+bc);
	  std::bitset<512> tip = ctp_bc.getTIP();
	  ATH_MSG_DEBUG( "got CTP TIP bits: " << tip.to_string() );
	  const unsigned int topoTipStart(384);
	  const unsigned int nTopoCTPOutputs(128);
	  for (unsigned int item=0; item<nTopoCTPOutputs;++item) {
	    // Check if algorithms fired
	    if (tip.test(item+topoTipStart)) {
	      m_h_l1topo_2d_ItemsBC[item/32]->Fill(item,bc);
	      ATH_MSG_DEBUG("  Fired (item, bc) =" << item << ", " << bc);
	    }
	  }
	}
      }
    }
  }
  
  // Retrieve CMX CP tobs
  const xAOD::CMXCPTobContainer* cmxcptob = 0;
  sc = evtStore()->retrieve(cmxcptob);
  if (sc.isFailure() || !cmxcptob) {
    ATH_MSG_DEBUG ("No CMX CP tobs found in TES");
    // tot_error|=NO_CMX_CP;
  }   
  else {
    ATH_MSG_DEBUG( "Found CMXCPTobCollection, looping on TOBs ..." );
    for (const xAOD::CMXCPTob* t : *cmxcptob) {
      if (t->energy()) {
	for (int clone=0;clone<4;++clone) {
	  if (t->cmx()==0)
	    cmxKeys[TAU_TOB].insert(std::make_tuple(t->crate(),t->cpm(),
						    t->chip(),t->location(),
						    t->energy(),clone));
	  else
	    cmxKeys[EM_TOB].insert(std::make_tuple(t->crate(),t->cpm(),
						    t->chip(),t->location(),
						    t->energy(),clone));
	}
      }
    }
    //m_h_l1topo_1d_CMXCPTobs->Fill(std::min((int)cmxtobs.size(),MAXTOBS-1));
  }
  
  std::vector<const xAOD::CMXJetTob*> cmxtobs;  
  // Retrieve CMX jet tobs
  const xAOD::CMXJetTobContainer* cmxtob = 0;
  sc = evtStore()->retrieve(cmxtob);
  if (sc.isFailure() || !cmxtob) {
    ATH_MSG_DEBUG ("No CMX tobs found in TES");
    m_h_l1topo_1d_Errors->Fill(NO_CMX);
    topo_error|=(1<<NO_CMX);
  }   
  else {
    ATH_MSG_DEBUG( "Found CMXJetTobCollection, looping on TOBs ..." );
    for (const xAOD::CMXJetTob* t : *cmxtob) {
      if (t->energyLarge()) cmxtobs.push_back(t);
      for (int clone=0;clone<4;++clone) {
	if (t->energyLarge())
	  cmxKeys[JETL_TOB].insert(std::make_tuple(t->crate(),t->jem(),
						   t->frame(),t->location(),
						   t->energyLarge(),clone));
	if (t->energySmall())
	  cmxKeys[JETS_TOB].insert(std::make_tuple(t->crate(),t->jem(),
						   t->frame(),t->location(),
						   t->energySmall(),clone));
      }
    }
    m_h_l1topo_1d_CMXTobs->Fill(std::min((int)cmxtobs.size(),MAXTOBS-1));
  }
  
  // Retrieve L1Topo CTP simulted decision if present
  if (!evtStore()->contains<LVL1::FrontPanelCTP>(m_topoCTPLoc.value())){
    ATH_MSG_DEBUG("Could not retrieve LVL1::FrontPanelCTP with key "
		  << m_topoCTPLoc.value());
  }
  else {
    const LVL1::FrontPanelCTP* topoCTP = nullptr;
    CHECK_RECOVERABLE(evtStore()->retrieve(topoCTP,m_topoCTPLoc.value()));
    if (!topoCTP){
      ATH_MSG_INFO( "Retrieve of LVL1::FrontPanelCTP failed." );
    }
    else {
      for(unsigned int i=0; i<32; ++i) {
	uint64_t topores0=topoCTP->cableWord0(1);
	topores0<<=32;
	topores0+=topoCTP->cableWord0(0);
	uint64_t topores1=topoCTP->cableWord1(1);
	topores1<<=32;
	topores1+=topoCTP->cableWord1(0);
	if( (topores0 & (0x1UL << (2*i+0))))
	  m_h_l1topo_1d_Simulation->Fill(i); // cable 0, clock 0
	if( (topores0 & (0x1UL << (2*i+1))))
	  m_h_l1topo_1d_Simulation->Fill(32 + i); // cable 0, clock 1
	if( (topores1 & (0x1UL << (2*i+0))))
	  m_h_l1topo_1d_Simulation->Fill(64 + i); // cable 1, clock 0
	if( (topores1 & (0x1UL << (2*i+1))))
	  m_h_l1topo_1d_Simulation->Fill(96 + i); // cable 1, clock 1
      }
      ATH_MSG_DEBUG("Simulated output from L1Topo from StoreGate with key "
		    << m_topoCTPLoc);
      ATH_MSG_DEBUG("L1Topo word 1 at clock 0 is: 0x"
		    << std::hex << std::setw( 8 ) << std::setfill( '0' )
		    << topoCTP->cableWord0(0));
      ATH_MSG_DEBUG("L1Topo word 2 at clock 0 is: 0x"
		    << std::hex << std::setw( 8 ) << std::setfill( '0' )
		    << topoCTP->cableWord1(0));
      ATH_MSG_DEBUG("L1Topo word 1 at clock 1 is: 0x"
		    << std::hex << std::setw( 8 ) << std::setfill( '0' )
		    << topoCTP->cableWord0(1));
      ATH_MSG_DEBUG("L1Topo word 2 at clock 1 is: 0x"
		    << std::hex << std::setw( 8 ) << std::setfill( '0' )
		    << topoCTP->cableWord1(1));
    }       
  }

  // Retrieve the L1Topo RDOs from the DAQ RODs
  const int NFPGA=4;
  const L1TopoRDOCollection* rdos = 0;
  sc = evtStore()->retrieve(rdos);
  if (sc.isFailure() or 0 == rdos) {
    m_h_l1topo_1d_Errors->Fill(NO_DAQ);
    topo_error|=(1<<NO_DAQ);
    ATH_MSG_DEBUG ( "Could not retrieve L1Topo DAQ RDO collection "
		    "from StoreGate" );
  }
  else {
    // initialise collections filled for all blocks
    std::vector<L1Topo::L1TopoTOB> daqTobs;
    std::vector<uint32_t> daqJetTobs;
    std::vector<uint32_t> daqTauTobs;
    std::vector<uint32_t> daqEMTobs;
    std::vector<uint32_t> daqMuonTobs;
    std::vector<uint32_t> vFibreSizes;
    std::vector<uint32_t> vFibreStatus;

    auto triggerBits = L1Topo::getDecisionAndOverflowBits(*rdos);
    ATH_MSG_DEBUG( "Trigger  bits from L1Topo RDOs 0b" << triggerBits.first );
    ATH_MSG_DEBUG( "Overflow bits from L1Topo RDOs 0b" << triggerBits.second );
    
    for (const L1TopoRDO* rdo : *rdos) {
      ATH_MSG_VERBOSE( *rdo );
      ATH_MSG_DEBUG( "Found DAQ RDO with source ID "
                     << L1Topo::formatHex8(rdo->getSourceID()) );
      std::vector<L1Topo::Error> errors = rdo->getErrors();
      if (! errors.empty()){
	ATH_MSG_WARNING( "Converter errors reported: " << errors );
	m_h_l1topo_1d_Errors->Fill(DAQ_CONV);
	topo_error|=(1<<DAQ_CONV);
      }
      const std::vector<uint32_t> cDataWords = rdo->getDataWords();

      if ( cDataWords.size() == 0 ) {
        ATH_MSG_DEBUG ( "L1TopoRDO DAQ is empty" );
	m_h_l1topo_1d_Errors->Fill(NO_DAQ);
	topo_error|=(1<<NO_DAQ);
      }

      // initialise header: beware, this can make a valid-looking header
      //   set version 15, BCN -7, which is unlikely:
      L1Topo::Header header(0xf, 0, 0, 0, 0, 1, 0x7);
      int i_fpga=-1;
      for (auto word : cDataWords){
	switch (L1Topo::blockType(word)){
	case L1Topo::BlockTypes::HEADER:
	  {
	    header = L1Topo::Header(word);
	    if (header.payload_crc()!=0) {
	      m_h_l1topo_1d_Errors->Fill(PAYL_CRC);
	      topo_error|=(1<<PAYL_CRC);
	    }
	    i_fpga=(((rdo->getSourceID())>>3)&2)+header.fpga();
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
	    if (status.overflow()) m_h_l1topo_1d_Overflows->Fill(0.5);
	    if (status.crc()) {
	      m_h_l1topo_1d_Errors->Fill(F_CRC);
	      topo_error|=(1<<F_CRC);
	    }
	    break;
	  }
	case L1Topo::BlockTypes::L1TOPO_TOB:
	  {
	    if (header.bcn_offset()==0){
	      auto tob = L1Topo::L1TopoTOB(word);
	      ATH_MSG_DEBUG(tob);
	      daqTobs.push_back(tob);
	      auto index = L1Topo::triggerBitIndex(rdo->getSourceID(),tob);
	      for (unsigned int i=0; i<8; ++i){
		if ((tob.trigger_bits() >>i)&1)
		  m_h_l1topo_1d_DAQTriggerBits->Fill(index+i);
		if ((tob.overflow_bits()>>i)&1)
		  m_h_l1topo_1d_DAQOverflowBits->Fill(index+i);
	      }
	    }
	    break;
	  }
	case L1Topo::BlockTypes::JET1_TOB:
	case L1Topo::BlockTypes::JET2_TOB:
	  {
	    if (header.bcn_offset()==0) {
	      const int crate    = (word >> 28) & 0x1;
	      const int jem      = (word >> 24) & 0xF;
	      const int frame    = (word >> 21) & 0x7;
	      const int location = (word >> 19) & 0x3;
	      const int energyS  = (word >> 10) & 0x1FF;
	      const int energyL  = (word & 0x3FF);	      
	      if (energyL) {
		topoKeys[JETL_TOB].insert(std::make_tuple(crate,jem,frame,
							  location,energyL,i_fpga));
		//auto tob = L1Topo::JetTOB(word);
		int tob = 1; // Fake object until defined
		daqJetTobs.push_back(tob);
		m_h_l1topo_1d_JetTobs_EnergyLg->Fill(energyL,1./NFPGA);
	      }
	      if (energyS) {
		topoKeys[JETS_TOB].insert(std::make_tuple(crate,jem,frame,
							  location,energyS,i_fpga));
	      }
	    }
	    break;
	  }
	case L1Topo::BlockTypes::TAU_TOB:
	  {
	    if (header.bcn_offset()==0) {
	      int tob = 1; // Fake object until defined
	      daqTauTobs.push_back(tob);
	      const int crate    = (word >> 26) & 0x3;
	      const int cpm      = (word >> 20) & 0xF;
	      const int chip     = (word >> 15) & 0xF;
	      const int location = (word >> 13) & 0x3;
	      const int energy   = (word & 0xFF);	      
	      if (energy)
		topoKeys[TAU_TOB].insert(std::make_tuple(crate,cpm,chip,location,
							 energy,i_fpga));
	    }
	    break;
	  }	  
	case L1Topo::BlockTypes::EM_TOB:
	  {
	    if (header.bcn_offset()==0) {
	      int tob = 1; // Fake object until defined
	      daqEMTobs.push_back(tob);
	      const int crate    = (word >> 26) & 0x3;
	      const int cpm      = (word >> 20) & 0xF;
	      const int chip     = (word >> 15) & 0xF;
	      const int location = (word >> 13) & 0x3;
	      const int energy   = (word & 0xFF);
	      if (energy)
		topoKeys[EM_TOB].insert(std::make_tuple(crate,cpm,chip,
							location,energy,i_fpga));
	    }
	    break;
	  }
	case L1Topo::BlockTypes::MUON_TOB:
	  {
	    if (header.bcn_offset()==0){
	      int tob = 1; // Fake object until defined
	      daqMuonTobs.push_back(tob);
	    }
	    break;
	  }
	case L1Topo::BlockTypes::ENERGY_TOB:
	  {
	    if (header.bcn_offset()==0){
	    }
	    break;
	  }
	default:
	  {
	    break;
	  }
	}
      } // for word
    }
    m_h_l1topo_1d_DAQTobs->Fill(std::min((int)daqTobs.size()/NFPGA,19));
    m_h_l1topo_1d_DAQJetTobs->Fill(std::min((int)daqJetTobs.size()/NFPGA,
					    MAXTOBS-1));
    m_h_l1topo_1d_DAQTauTobs->Fill(std::min((int)daqTauTobs.size()/NFPGA,
					    MAXTOBS-1));
    m_h_l1topo_1d_DAQMuonTobs->Fill(std::min((int)daqMuonTobs.size()/NFPGA,
					     MAXTOBS-1));
    m_h_l1topo_1d_DAQEMTobs->Fill(std::min((int)daqEMTobs.size()/NFPGA,
					   MAXTOBS-1));
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
    const auto& rdo = r.rdo();
    ATH_MSG_DEBUG( "Found ROI RDO with source ID "
		   << L1Topo::formatHex8(rdo.getSourceID()) );
    auto errors = rdo.getErrors();
    if (! errors.empty()) {
      ATH_MSG_WARNING( "Converter errors reported: " << errors );
      m_h_l1topo_1d_Errors->Fill(ROI_CONV);
      topo_error|=(1<<ROI_CONV);
    }
    const std::vector<uint32_t>& cDataWords = rdo.getDataWords();
    if ( cDataWords.size() == 0 ) {
      ATH_MSG_DEBUG ( "L1TopoRDO ROI is empty" );
      m_h_l1topo_1d_Errors->Fill(NO_ROI);
      topo_error|=(1<<NO_ROI);
    }
    for (auto word : cDataWords) {
      switch (L1Topo::blockType(word)) {
      case L1Topo::BlockTypes::L1TOPO_TOB:
      {
        auto tob = L1Topo::L1TopoTOB(word);
        ATH_MSG_DEBUG( tob );
        roiTobs.push_back(tob);
        //auto index = L1Topo::triggerBitIndex(rdo.getSourceID(),tob);
        for (unsigned int i = 0; i < 8; ++i) {
          //m_histTriggerBitsFromROIConv->
	  // Fill(index+i,(tob.trigger_bits() >>i)&1);
          //m_histOverflowBitsFromROIConv->
	  // Fill(index+i,(tob.overflow_bits()>>i)&1);
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

  // look in both directions for CMX and Topo TOBs (mis)matches
  for (int t=0; t<TOB_TYPES; ++t) { 
    set_symmetric_difference(cmxKeys[t].begin(),cmxKeys[t].end(),
			     topoKeys[t].begin(),topoKeys[t].end(),
			     inserter(keyDiff[t],keyDiff[t].begin()));
    if (keyDiff[t].size()>0) {
      m_h_l1topo_1d_Errors->Fill(CMX_MATCH);
      topo_error|=(1<<CMX_MATCH);
    }
    for (auto& tob : keyDiff[t]) {
      int x,y;
      double eta,phi;
      if (t==JETS_TOB || t==JETL_TOB) {
	jem2Coord(std::get<0>(tob),std::get<1>(tob),std::get<2>(tob),std::get<3>(tob),
		  x,y,eta,phi);
	m_histTool->fillJEMRoIEtaVsPhi(m_h_l1topo_2d_Tobs_etaPhi_mismatch[t],eta,phi);
      }
      else {
	cpm2Coord(std::get<0>(tob),std::get<1>(tob),std::get<2>(tob),std::get<3>(tob),
		  x,y,eta,phi);
	m_histTool->fillCPMRoIEtaVsPhi(m_h_l1topo_2d_Tobs_etaPhi_mismatch[t],eta,phi);
      }
      m_h_l1topo_2d_Tobs_Hitmap_mismatch[t]->Fill(x,y);
    }
    set_intersection(cmxKeys[t].begin(),cmxKeys[t].end(),
		     topoKeys[t].begin(),topoKeys[t].end(),
		     inserter(keyIntersect[t],keyIntersect[t].begin()));
    for (auto& tob : keyIntersect[t]) {
      int x,y;
      double eta,phi;
      if (t==JETS_TOB || t==JETL_TOB) {
	jem2Coord(std::get<0>(tob),std::get<1>(tob),std::get<2>(tob),std::get<3>(tob),
		  x,y,eta,phi);
	m_histTool->fillJEMRoIEtaVsPhi(m_h_l1topo_2d_Tobs_etaPhi_match[t],eta,phi);
      }
      else {
	cpm2Coord(std::get<0>(tob),std::get<1>(tob),std::get<2>(tob),std::get<3>(tob),
		  x,y,eta,phi);
	m_histTool->fillCPMRoIEtaVsPhi(m_h_l1topo_2d_Tobs_etaPhi_match[t],eta,phi);
      }
      m_h_l1topo_2d_Tobs_Hitmap_match[t]->Fill(x,y);
    }
  }

  if (topo_error) {
    m_h_l1topo_1d_ErrorsByLumiblock->Fill(m_lumiNo);
  }
  
  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode L1CaloL1TopoMon::procHistograms()
/*---------------------------------------------------------*/
{
  ATH_MSG_DEBUG("Enter procHistograms()");

  //if ( endOfLumiBlockFlag() ) { }  
  //if ( endOfRunFlag() ) { }
  
  int eor=(endOfRunFlag() ? 1 : 0);
  for (int cpu=0; cpu<=3; ++cpu) {
    for (int item=1; item<=32; ++item) {
      for (int bc=1; bc<=5; ++bc) {
	if (m_h_l1topo_2d_ItemsBC[cpu]->GetBinContent(item,bc)==0);
	else if (bc==3)
	  m_h_l1topo_2d_ItemsBC_ratio[cpu][eor]->SetBinContent(item,bc,1);
	else {
	  float binval=m_h_l1topo_2d_ItemsBC[cpu]->GetBinContent(item,bc);
	  float centerbin=m_h_l1topo_2d_ItemsBC[cpu]->GetBinContent(item,3);
	  float ratio=(binval<1 || centerbin<1 ? 0 : binval/centerbin);
	  m_h_l1topo_2d_ItemsBC_ratio[cpu][eor]->
	    SetBinContent(item,bc,fmax(0.05,ratio));
	}
      }
    }
  }

  return StatusCode::SUCCESS;
}

// *********************************************************************
// Private Methods
// *********************************************************************

void L1CaloL1TopoMon::jem2Coord(const int crate, const int jem,
	       const int frame, const int location,
	       int &x, int &y, double &eta, double &phi)
{
  x = crate*16 + jem;
  y = frame*4 + location;
  const uint32_t roiWord = // formula from JEPSimMon
    ((((((crate << 4) + jem) << 3) + frame) << 2) + location) << 19;
  LVL1::JEPRoIDecoder decoder;
  const LVL1::CoordinateRange coord(decoder.coordinate(roiWord));
  eta = coord.eta();
  phi = coord.phi();
}

void L1CaloL1TopoMon::cpm2Coord(const int crate, const int cpm,
	       const int chip, const int location,
	       int &x, int &y, double &eta, double &phi)
{
  x = crate*14 + cpm - 1;
  y = chip*4 + location;
  const uint32_t roiWord = // formula from CPSimMon
    ((((((crate << 4) + cpm) << 4) + chip) << 2)
     + location) << 18;
  LVL1::CPRoIDecoder decoder;
    const LVL1::CoordinateRange coord(decoder.coordinate(roiWord));
    eta = coord.eta();
    phi = coord.phi();
}    

}
