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
#include "LWHists/TH1I_LW.h"
#include "LWHists/TH2I_LW.h"

#include "TrigConfInterfaces/ILVL1ConfigSvc.h"
#include "TrigT1CaloMonitoringTools/ITrigT1CaloMonErrorTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"

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
    m_h_l1topo_1d_L1CaloL1TopoDAQCnvErrors(0),
    m_h_l1topo_1d_L1CaloL1TopoDAQPayloadCRCErrors(0),
    m_h_l1topo_1d_L1CaloL1TopoDAQTobs(0)

    /*---------------------------------------------------------*/
{
  // This is how you declare the parameters to Gaudi so that
  // they can be over-written via the job options file

  declareProperty( "LVL1ConfigSvc", m_configSvc, "LVL1 Config Service");
  declareProperty( "PathInRootFile", m_PathInRootFile = "LVL1_Interfaces/L1Topo");
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

    m_h_l1topo_1d_L1CaloL1TopoDAQCnvErrors =
      m_histTool->book1F("l1topo_1d_L1CaloL1TopoDAQCnvErrors",
                         "L1Topo DAQ converter errors",
                         1, 0, 1);
    setLabels(m_h_l1topo_1d_L1CaloL1TopoDAQCnvErrors);

    m_h_l1topo_1d_L1CaloL1TopoDAQPayloadCRCErrors =
      m_histTool->book1F("l1topo_1d_L1CaloL1TopoDAQPayloadCRCErrors",
                         "L1Topo DAQ payload CRC errors",
                         1, 0, 1);
    setLabels(m_h_l1topo_1d_L1CaloL1TopoDAQPayloadCRCErrors);

    m_h_l1topo_1d_L1CaloL1TopoDAQTobs =
      m_histTool->book1F("l1topo_1d_L1CaloL1TopoDAQTobs",
                         "Number of L1Topo DAQ L1Topo TOBs",
                         20, 0, 20);
    setLabels(m_h_l1topo_1d_L1CaloL1TopoDAQTobs);

    m_h_l1topo_1d_L1CaloL1TopoDAQJetTobs =
      m_histTool->book1F("l1topo_1d_L1CaloL1TopoDAQJetTobs",
                         "Number of L1Topo DAQ Jet TOBs",
                         200, 0, 200);
    setLabels(m_h_l1topo_1d_L1CaloL1TopoDAQJetTobs);

    m_histTool->unsetMonGroup();
    m_histBooked = true;

  }

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode L1CaloL1TopoMon::fillHistograms()
/*---------------------------------------------------------*/
{
  if (m_debug) msg(MSG::DEBUG) << "in L1CaloL1TopoMon::fillHistograms" << endreq;

  if (!m_histBooked) {
    if (m_debug) msg(MSG::DEBUG) << "Histogram(s) not booked" << endreq;
    return StatusCode::SUCCESS;
  }

  if (m_errorTool->corrupt() || m_errorTool->robOrUnpackingError()) {
    if (m_debug) msg(MSG::DEBUG) << "Skipping corrupt event" << endreq;
    return StatusCode::SUCCESS;
  }

  // Retrieve the L1Topo RDOs from the DAQ RODs
  const DataHandle<L1TopoRDOCollection> rdos = 0;
  StatusCode sc = StatusCode::SUCCESS;
  sc = evtStore()->retrieve(rdos);
  if (sc.isFailure() or 0 == rdos) {
    ATH_MSG_WARNING ( "Could not retrieve L1Topo DAQ RDO collection from StoreGate" );
  }
  else {
    for (auto & rdo : *rdos) {
      ATH_MSG_VERBOSE( *rdo );
      ATH_MSG_DEBUG( "Found DAQ RDO with source ID "
                     << L1Topo::formatHex8(rdo->getSourceID()) );
      /*
      m_histSIDsViaConverters->
      Fill((L1Topo::formatHex8(rdo->getSourceID())).c_str(), 1.);
      m_histSIDsViaConverters->LabelsDeflate("X");
      */
      auto errors = rdo->getErrors();
      if (! errors.empty()) {
        ATH_MSG_WARNING( "Converter errors reported: " << errors );
        m_h_l1topo_1d_L1CaloL1TopoDAQCnvErrors->Fill(1.);
      }
      const std::vector<uint32_t> cDataWords = rdo->getDataWords();

      if ( cDataWords.size() == 0 ) {
        ATH_MSG_WARNING ( "L1TopoRDO DAQ is empty" );
      }

      // initialise collections filled for all blocks
      std::vector<L1Topo::L1TopoTOB> daqTobs;
      std::vector<L1Topo::L1TopoTOB> daqTobsBC0;
      //std::vector<L1Topo::L1TopoTOB> daqJetTobs;
      std::vector<int> daqJetTobs;
      std::vector<uint32_t> vFibreSizes;
      std::vector<uint32_t> vFibreStatus;

      // initialise header: beware, this can make a valid-looking header
      //   set version 15, BCN -7, which is unlikely:
      L1Topo::Header header(0xf, 0, 0, 0, 0, 1, 0x7);
      // reset containers
      vFibreSizes.clear();
      vFibreStatus.clear();
      daqTobs.clear();
      daqTobsBC0.clear();
      daqJetTobs.clear();
      for (auto word : cDataWords) {
        switch (L1Topo::blockType(word)) {
        case L1Topo::BlockTypes::HEADER:
        {
          header = L1Topo::Header(word);
          if (header.payload_crc() != 0) {
            m_h_l1topo_1d_L1CaloL1TopoDAQPayloadCRCErrors->Fill(1.);
          }
          break;
        }
        case L1Topo::BlockTypes::FIBRE:
        {
          auto fibreBlock = L1Topo::Fibre(word);
          for (auto fsize : fibreBlock.count()) {
            vFibreSizes.push_back(fsize);
          }
          for (auto fstatus : fibreBlock.status()) {
            vFibreStatus.push_back(fstatus);
          }
          break;
        }
        case L1Topo::BlockTypes::STATUS:
        {
          auto status = L1Topo::Status(word);
          ATH_MSG_WARNING( "fibre overflow: " << status.overflow()
                           << " fibre crc: " << status.crc() );
        }
        case L1Topo::BlockTypes::L1TOPO_TOB:
        {
          auto tob = L1Topo::L1TopoTOB(word);
          daqTobs.push_back(tob);
          if (header.bcn_offset() == 0) {
            daqTobsBC0.push_back(tob);
          }
          break;
        }
        case L1Topo::BlockTypes::JET1_TOB:
        case L1Topo::BlockTypes::JET2_TOB:
        {
          //auto tob = L1Topo::JetTOB(word);
          //daqJetTobs.push_back(tob);
          int tob = 1; // Fake object until defined
          daqJetTobs.push_back(tob);
          break;
        }
        default:
        {
          // perhaps count other TOB types?
          break;
        }
        }
      } // for word
      m_h_l1topo_1d_L1CaloL1TopoDAQTobs->Fill(std::min((int)daqTobs.size(), 19));
      m_h_l1topo_1d_L1CaloL1TopoDAQJetTobs->Fill(std::min((int)daqJetTobs.size(), 199));
    }
  }

  // Retrieve and print the L1Topo RDOs from the ROI RODs
  std::vector<L1Topo::L1TopoTOB> roiTobs;
  const ROIB::RoIBResult* roibresult = 0;
  CHECK (evtStore()->retrieve(roibresult) );
  const std::vector< ROIB::L1TopoResult > l1TopoResults = roibresult->l1TopoResult();
  ATH_MSG_DEBUG( "Number of L1Topo ROI RODs found: " << l1TopoResults.size() );
  for (auto & r : l1TopoResults) {
    //ATH_MSG_VERBOSE( r.dump() );
    auto rdo = r.rdo();
    ATH_MSG_DEBUG( "Found ROI RDO with source ID " << L1Topo::formatHex8(rdo.getSourceID()) );
    //m_histSIDsViaConverters->Fill((L1Topo::formatHex8(rdo.getSourceID())).c_str(), 1.);
    //m_histSIDsViaConverters->LabelsDeflate("X");
    auto errors = rdo.getErrors();
    if (! errors.empty()) {
      ATH_MSG_WARNING( "Converter errors reported: " << errors );
    }
    const std::vector<uint32_t> cDataWords = rdo.getDataWords();
    if ( cDataWords.size() == 0 ) {
      ATH_MSG_WARNING ( "L1TopoRDO ROI is empty" );
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
        ATH_MSG_WARNING( "unexpected TOB type in ROI: " <<  L1Topo::formatHex8(word) );
        break;
      }
      }
    }
  }

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

//void L1CaloL1TopoMon::setLabels(LWHist* hist, bool xAxis)
void L1CaloL1TopoMon::setLabels(LWHist*, bool)
{
  /*
  LWHist::LWHistAxis* axis = (xAxis) ? hist->GetXaxis() : hist->GetYaxis();
  axis->SetBinLabel(1+EMType,           "EMHits");
  axis->SetBinLabel(1+EMTauType,        "EMTauHits");
  axis->SetBinLabel(1+MainType,         "MainHits");
  axis->SetBinLabel(1+ForwardType,      "ForwardHits");
  axis->SetBinLabel(1+BackwardType,     "BackwardHits");
  axis->SetBinLabel(1+JetEtType,        "JetEtHits");
  axis->SetBinLabel(1+MissingEtType,    "MissingEtHits");
  axis->SetBinLabel(1+SumEtType,        "SumEtHits");
  axis->SetBinLabel(1+MissingEtSigType, "MissEtSigHits");
  */
}
// ============================================================================
} // end namespace
// ============================================================================
