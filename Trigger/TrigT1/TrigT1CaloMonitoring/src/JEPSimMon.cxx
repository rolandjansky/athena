/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     JEPSimMon.cxx
// PACKAGE:  TrigT1CaloMonitoring
//
// AUTHOR:   Peter Faulkner
//
//
// ********************************************************************

#include <utility>
#include <algorithm>

#include "LWHists/LWHist.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "SGTools/StlVectorClids.h"

#include "AthenaMonitoring/AthenaMonManager.h"

#include "xAODTrigL1Calo/RODHeaderContainer.h"

#include "xAODTrigL1Calo/TriggerTowerAuxContainer.h"
#include "xAODTrigL1Calo/JetElementAuxContainer.h"
#include "xAODTrigL1Calo/CMXJetTobAuxContainer.h"
#include "xAODTrigL1Calo/CMXJetHitsAuxContainer.h"
#include "xAODTrigL1Calo/JEMEtSumsAuxContainer.h"
#include "xAODTrigL1Calo/CMXEtSumsAuxContainer.h"
#include "xAODTrigL1Calo/JetElementAuxContainer.h"
#include "xAODTrigL1Calo/JEMTobRoIAuxContainer.h"

#include "TrigT1CaloUtils/CoordToHardware.h"
#include "TrigT1CaloUtils/JEMJetAlgorithm.h"
#include "TrigT1CaloUtils/DataError.h"
#include "TrigT1CaloToolInterfaces/IL1JetCMXTools.h"
#include "TrigT1CaloToolInterfaces/IL1JEMJetTools.h"
#include "TrigT1CaloToolInterfaces/IL1JetElementTools.h"
#include "TrigT1CaloToolInterfaces/IL1EnergyCMXTools.h"
#include "TrigT1Interfaces/Coordinate.h"
#include "TrigT1Interfaces/JEPRoIDecoder.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1CaloMonitoringTools/ITrigT1CaloMonErrorTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"

#include "JEPSimMon.h"
namespace{
std::string sourceComponent(uint8_t source)
{
    switch (source)
    {
    case xAOD::CMXEtSums::Sources::REMOTE_STANDARD:
        return "REMOTE_STANDARD";
    case xAOD::CMXEtSums::Sources::REMOTE_RESTRICTED:
        return "REMOTE_RESTRICTED";
    case xAOD::CMXEtSums::Sources::LOCAL_STANDARD:
        return "LOCAL_STANDARD";
    case xAOD::CMXEtSums::Sources::LOCAL_RESTRICTED:
        return "LOCAL_RESTRICTED";
    case xAOD::CMXEtSums::Sources::TOTAL_STANDARD:
        return "TOTAL_STANDARD";
    case xAOD::CMXEtSums::Sources::TOTAL_RESTRICTED:
        return "TOTAL_RESTRICTED";
    case xAOD::CMXEtSums::Sources::SUM_ET_STANDARD:
        return "SUM_ET_STANDARD";
    case xAOD::CMXEtSums::Sources::SUM_ET_RESTRICTED:
        return "SUM_ET_RESTRICTED";
    case xAOD::CMXEtSums::Sources::MISSING_ET_STANDARD:
        return "MISSING_ET_STANDARD";
    case xAOD::CMXEtSums::Sources::MISSING_ET_RESTRICTED:
        return "MISSING_ET_RESTRICTED";
    case xAOD::CMXEtSums::Sources::MISSING_ET_SIG_STANDARD:
        return "MISSING_ET_SIG_STANDARD";
    default:
        return std::to_string(source);
    }
}


std::string hitsSourceComponent(uint8_t source)
{
    switch (source)
    {
     case xAOD::CMXJetHits::Sources::REMOTE_MAIN:
        return "REMOTE_MAIN";
    case xAOD::CMXJetHits::Sources::LOCAL_MAIN:
        return "LOCAL_MAIN";
    case xAOD::CMXJetHits::Sources::TOTAL_MAIN:
        return "TOTAL_MAIN";
    case xAOD::CMXJetHits::Sources::REMOTE_FORWARD:
        return "REMOTE_FORWARD";
    case xAOD::CMXJetHits::Sources::LOCAL_FORWARD:
        return "LOCAL_FORWARD";
    case xAOD::CMXJetHits::Sources::TOTAL_FORWARD:
        return "TOTAL_FORWARD";
    case xAOD::CMXJetHits::Sources::TOPO_CHECKSUM:
        return "TOPO_CHECKSUM";
    case xAOD::CMXJetHits::Sources::TOPO_OCCUPANCY_MAP:
        return "TOPO_OCCUPANCY_MAP";
    case xAOD::CMXJetHits::Sources::TOPO_OCCUPANCY_COUNTS:
        return "TOPO_OCCUPANCY_COUNTS";
    default:
        return std::to_string(source);
    }
}



} // end of namespace

std::ostream &operator<<(std::ostream &os, const xAOD::CMXEtSums &el)
{
    os << "xAOD::CMXEtSums crate=" << int(el.crate())
       << " sourceComponent=" << sourceComponent(el.sourceComponent())
       << " peak=" << int(el.peak()) << " et=" << el.et()
       << " ex=" << int(el.ex()) << " ey=" << int(el.ey())
       << " etError=" << int(el.etError()) << " exError="
       << int(el.exError()) << ", eyError=" << int(el.eyError()) << " etVec=[";
    std::for_each(el.etVec().begin(), el.etVec().end(), [&](uint16_t n) { os << " " << int(n); });
    os << "]";
    return os;
}


std::ostream &operator<<(std::ostream &os, const xAOD::CMXJetHits &el)
{
    os << "xAOD::CMXJetHits crate=" << int(el.crate())
       << " sourceComponent=" << hitsSourceComponent(el.sourceComponent())
       << " peak=" << int(el.peak()) << " hits0=0x" << std::hex << el.hits0() << std::dec
       << " hits1=0x" << std::hex << el.hits1() << std::dec << " hits0Vec=[";

    std::for_each(el.hitsVec0().begin(), el.hitsVec0().end(), [&](uint32_t n) { os << " 0x" << std::hex << int(n) << std::dec; });
    os << "]";
    return os;
}

std::ostream &operator<<(std::ostream &os, const xAOD::JEMTobRoI &el)
{
    os << "xAOD::JEMTobRoI crate=" << int(el.crate())
    << " jem=" << el.jem()
    << " frame=" << el.frame()
    << " location=" << el.location()
    << " energyLarge=" << el.energyLarge()
    << " energySmall=" << el.energySmall();
    return os;
}


// ============================================================================
namespace LVL1
{
// ============================================================================
JEPSimMon::JEPSimMon(const std::string &type, const std::string &name,
                     const IInterface *parent)
    : ManagedMonitorToolBase(type, name, parent),
      m_jetCmxTool("LVL1::L1JetCMXTools/L1JetCMXTools"),
      m_jetTool("LVL1::L1JEMJetTools/L1JEMJetTools"),
      m_jetElementTool("LVL1::L1JetElementTools/L1JetElementTools"),
      m_energyCmxTool("LVL1::L1EnergyCMXTools/L1EnergyCMXTools"),
      m_errorTool("LVL1::TrigT1CaloMonErrorTool/TrigT1CaloMonErrorTool"),
      m_histTool("LVL1::TrigT1CaloLWHistogramTool/TrigT1CaloLWHistogramTool"),
      m_debug(false), m_rodTES(0), m_limitedRoi(0), m_histBooked(false),
      m_h_jem_em_2d_etaPhi_jetEl_SimEqCore(0),
      m_h_jem_em_2d_etaPhi_jetEl_SimNeCore(0),
      m_h_jem_em_2d_etaPhi_jetEl_SimNoCore(0),
      m_h_jem_em_2d_etaPhi_jetEl_CoreNoSim(0),
      m_h_jem_had_2d_etaPhi_jetEl_SimEqCore(0),
      m_h_jem_had_2d_etaPhi_jetEl_SimNeCore(0),
      m_h_jem_had_2d_etaPhi_jetEl_SimNoCore(0),
      m_h_jem_had_2d_etaPhi_jetEl_CoreNoSim(0),
      m_h_jem_em_2d_etaPhi_jetEl_SimEqOverlap(0),
      m_h_jem_em_2d_etaPhi_jetEl_SimNeOverlap(0),
      m_h_jem_em_2d_etaPhi_jetEl_SimNoOverlap(0),
      m_h_jem_em_2d_etaPhi_jetEl_OverlapNoSim(0),
      m_h_jem_had_2d_etaPhi_jetEl_SimEqOverlap(0),
      m_h_jem_had_2d_etaPhi_jetEl_SimNeOverlap(0),
      m_h_jem_had_2d_etaPhi_jetEl_SimNoOverlap(0),
      m_h_jem_had_2d_etaPhi_jetEl_OverlapNoSim(0),
      m_h_jem_2d_roi_EnergyLgSimEqData(0), m_h_jem_2d_roi_EnergyLgSimNeData(0),
      m_h_jem_2d_roi_EnergyLgSimNoData(0), m_h_jem_2d_roi_EnergyLgDataNoSim(0),
      m_h_jem_2d_roi_EnergySmSimEqData(0), m_h_jem_2d_roi_EnergySmSimNeData(0),
      m_h_jem_2d_roi_EnergySmSimNoData(0), m_h_jem_2d_roi_EnergySmDataNoSim(0),
      m_h_jem_2d_etaPhi_roi_SimEqData(0), m_h_jem_2d_etaPhi_roi_SimNeData(0),
      m_h_jem_2d_etaPhi_roi_SimNoData(0), m_h_jem_2d_etaPhi_roi_DataNoSim(0),
      m_h_cmx_2d_tob_EnergyLgSimEqData(0), m_h_cmx_2d_tob_EnergyLgSimNeData(0),
      m_h_cmx_2d_tob_EnergyLgSimNoData(0), m_h_cmx_2d_tob_EnergyLgDataNoSim(0),
      m_h_cmx_2d_tob_EnergySmSimEqData(0), m_h_cmx_2d_tob_EnergySmSimNeData(0),
      m_h_cmx_2d_tob_EnergySmSimNoData(0), m_h_cmx_2d_tob_EnergySmDataNoSim(0),
      m_h_cmx_2d_etaPhi_tob_SimEqData(0), m_h_cmx_2d_etaPhi_tob_SimNeData(0),
      m_h_cmx_2d_etaPhi_tob_SimNoData(0), m_h_cmx_2d_etaPhi_tob_DataNoSim(0),
      m_h_cmx_1d_thresh_SumsSimEqData(0), m_h_cmx_1d_thresh_SumsSimNeData(0),
      m_h_cmx_1d_thresh_SumsSimNoData(0), m_h_cmx_1d_thresh_SumsDataNoSim(0),
      m_h_cmx_1d_thresh_SumsOvfSimEqData(0),
      m_h_cmx_1d_thresh_SumsOvfSimNeData(0),
      m_h_cmx_2d_thresh_SumsThreshSimEqData(0),
      m_h_cmx_2d_thresh_SumsThreshSimNeData(0), m_h_cmx_2d_topo_SimEqData(0),
      m_h_cmx_2d_topo_SimNeData(0), m_h_cmx_2d_topo_SimNoData(0),
      m_h_cmx_2d_topo_DataNoSim(0), m_h_jem_2d_energy_SimEqData(0),
      m_h_jem_2d_energy_SimNeData(0), m_h_jem_2d_energy_SimNoData(0),
      m_h_jem_2d_energy_DataNoSim(0), m_h_cmx_2d_energy_JemEqCmx(0),
      m_h_cmx_2d_energy_JemNeCmx(0), m_h_cmx_2d_energy_JemNoCmx(0),
      m_h_cmx_2d_energy_CmxNoJem(0), m_h_cmx_2d_energy_SumsSimEqData(0),
      m_h_cmx_2d_energy_SumsSimNeData(0), m_h_cmx_2d_energy_SumsSimNoData(0),
      m_h_cmx_2d_energy_SumsDataNoSim(0),
      m_h_cmx_2d_energy_EtMapsThreshSimEqData(0),
      m_h_cmx_2d_energy_EtMapsThreshSimNeData(0),
      m_h_jem_2d_SimEqDataOverview(0), m_h_jem_2d_SimNeDataOverview(0),
      m_h_jem_1d_SimNeDataSummary(0), m_v_2d_MismatchEvents(0)
/*---------------------------------------------------------*/
{
    declareProperty("JetCMXTool", m_jetCmxTool);
    declareProperty("JetTool", m_jetTool);
    declareProperty("JetElementTool", m_jetElementTool);
    declareProperty("EnergyCMXTool", m_energyCmxTool);
    declareProperty("ErrorTool", m_errorTool);
    declareProperty("HistogramTool", m_histTool);

    declareProperty("JetElementLocation",
                    m_jetElementLocation =
                        LVL1::TrigT1CaloDefs::JetElementLocation);
    declareProperty("JetElementLocationOverlap",
                    m_jetElementLocationOverlap =
                        LVL1::TrigT1CaloDefs::JetElementLocation + "Overlap");
    declareProperty("CMXJetTobLocation",
                    m_cmxJetTobLocation =
                        LVL1::TrigT1CaloDefs::CMXJetTobLocation);
    declareProperty("CMXJetHitsLocation",
                    m_cmxJetHitsLocation =
                        LVL1::TrigT1CaloDefs::CMXJetHitsLocation);
    declareProperty("JEMTobRoILocation",
                    m_jemRoiLocation = LVL1::TrigT1CaloDefs::JEMTobRoILocation);
    declareProperty("CMXRoILocation",
                    m_cmxRoiLocation = LVL1::TrigT1CaloDefs::CMXRoILocation);
    declareProperty("JEMEtSumsLocation",
                    m_jemEtSumsLocation =
                        LVL1::TrigT1CaloDefs::JEMEtSumsLocation);
    declareProperty("CMXEtSumsLocation",
                    m_cmxEtSumsLocation =
                        LVL1::TrigT1CaloDefs::CMXEtSumsLocation);
    declareProperty("TriggerTowerLocation",
                    m_triggerTowerLocation =
                        LVL1::TrigT1CaloDefs::TriggerTowerLocation);
    declareProperty("RodHeaderLocation",
                    m_rodHeaderLocation =
                        LVL1::TrigT1CaloDefs::RODHeaderLocation);
    declareProperty("ErrorLocation", m_errorLocation = "L1CaloJEMMismatchVector");

    declareProperty("RootDirectory", m_rootDir = "L1Calo/JEM");
}

/*---------------------------------------------------------*/
JEPSimMon::~JEPSimMon()
/*---------------------------------------------------------*/
{
}

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

/*---------------------------------------------------------*/
StatusCode JEPSimMon::initialize()
/*---------------------------------------------------------*/
{
    msg(MSG::INFO) << "Initializing " << name() << " - package version "
                   << PACKAGE_VERSION << endmsg;
    m_debug = msgLvl(MSG::DEBUG);

    StatusCode sc;

    sc = ManagedMonitorToolBase::initialize();
    if (sc.isFailure())
        return sc;

    sc = m_jetElementTool.retrieve();
    if (sc.isFailure())
    {
        msg(MSG::ERROR) << "Unable to locate Tool L1JetElementTools" << endmsg;
        return sc;
    }

    sc = m_jetTool.retrieve();
    if (sc.isFailure())
    {
        msg(MSG::ERROR) << "Unable to locate Tool L1JetTools" << endmsg;
        return sc;
    }

    sc = m_jetCmxTool.retrieve();
    if (sc.isFailure())
    {
        msg(MSG::ERROR) << "Unable to locate Tool L1JetCMXTools" << endmsg;
        return sc;
    }

    sc = m_energyCmxTool.retrieve();
    if (sc.isFailure())
    {
        msg(MSG::ERROR) << "Unable to locate Tool L1EnergyCMXTools" << endmsg;
        return sc;
    }

    sc = m_errorTool.retrieve();
    if (sc.isFailure())
    {
        msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloMonErrorTool" << endmsg;
        return sc;
    }

    sc = m_histTool.retrieve();
    if (sc.isFailure())
    {
        msg(MSG::ERROR) << "Unable to locate Tool TrigT1CaloLWHistogramTool"
                        << endmsg;
        return sc;
    }

    return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode JEPSimMon::bookHistogramsRecurrent()
/*---------------------------------------------------------*/
{
    msg(MSG::DEBUG) << "bookHistograms entered" << endmsg;

    if (m_environment == AthenaMonManager::online)
    {
        // book histograms that are only made in the online environment...
    }

    if (m_dataType == AthenaMonManager::cosmics)
    {
        // book histograms that are only relevant for cosmics data...
    }

    // if (newEventsBlock || newLumiBlock) {
    //}

    if (newRunFlag())
    {
        MgmtAttr_t attr = ATTRIB_UNMANAGED;
        std::string dir1(m_rootDir + "/Errors/Transmission_Simulation");
        MonGroup monShift(this, dir1, run, attr);
        MonGroup monExpert(this, dir1, run, attr);
        MonGroup monElements(this, dir1 + "/PPM2Elements", run, attr);
        MonGroup monRoIs(this, dir1 + "/Elements2RoIs", run, attr);
        MonGroup monEnergy(this, dir1 + "/Elements2Energy", run, attr);
        MonGroup monEvent1(this, dir1 + "/MismatchEventNumbers", run, attr, "",
                           "eventSample");
        std::string dir2(m_rootDir + "_CMX/Errors/Transmission_Simulation");
        MonGroup monTobs(this, dir2 + "/RoIs2TOBs", run, attr);
        MonGroup monHitSums(this, dir2 + "/TOBs2HitSums", run, attr);
        MonGroup monTopo(this, dir2 + "/TOBs2Topo", run, attr);
        MonGroup monEnergy2(this, dir2 + "/JEM2CMXEnergy", run, attr);
        MonGroup monEnergySums(this, dir2 + "/Energy2Sums", run, attr);
        MonGroup monEvent2(this, dir2 + "/MismatchEventNumbers", run, attr, "",
                           "eventSample");

        // JetElements

        m_histTool->setMonGroup(&monElements);

        m_h_jem_em_2d_etaPhi_jetEl_SimEqCore = m_histTool->bookJEMEtaVsPhi(
            "jem_em_2d_etaPhi_jetEl_SimEqCore",
            "Core Jet Elements EM Data/Simulation Non-zero Matches");
        m_h_jem_em_2d_etaPhi_jetEl_SimNeCore = m_histTool->bookJEMEtaVsPhi(
            "jem_em_2d_etaPhi_jetEl_SimNeCore",
            "Core Jet Elements EM Data/Simulation Non-zero Mismatches");
        m_h_jem_em_2d_etaPhi_jetEl_SimNoCore = m_histTool->bookJEMEtaVsPhi(
            "jem_em_2d_etaPhi_jetEl_SimNoCore",
            "Core Jet Elements EM Simulation but no Data");
        m_h_jem_em_2d_etaPhi_jetEl_CoreNoSim = m_histTool->bookJEMEtaVsPhi(
            "jem_em_2d_etaPhi_jetEl_CoreNoSim",
            "Core Jet Elements EM Data but no Simulation");
        m_h_jem_had_2d_etaPhi_jetEl_SimEqCore = m_histTool->bookJEMEtaVsPhi(
            "jem_had_2d_etaPhi_jetEl_SimEqCore",
            "Core Jet Elements HAD Data/Simulation Non-zero Matches");
        m_h_jem_had_2d_etaPhi_jetEl_SimNeCore = m_histTool->bookJEMEtaVsPhi(
            "jem_had_2d_etaPhi_jetEl_SimNeCore",
            "Core Jet Elements HAD Data/Simulation Non-zero Mismatches");
        m_h_jem_had_2d_etaPhi_jetEl_SimNoCore = m_histTool->bookJEMEtaVsPhi(
            "jem_had_2d_etaPhi_jetEl_SimNoCore",
            "Core Jet Elements HAD Simulation but no Data");
        m_h_jem_had_2d_etaPhi_jetEl_CoreNoSim = m_histTool->bookJEMEtaVsPhi(
            "jem_had_2d_etaPhi_jetEl_CoreNoSim",
            "Core Jet Elements HAD Data but no Simulation");
        m_h_jem_em_2d_etaPhi_jetEl_SimEqOverlap = m_histTool->bookJEMEtaVsPhi(
            "jem_em_2d_etaPhi_jetEl_SimEqOverlap",
            "Overlap Jet Elements EM Data/Simulation Non-zero Matches");
        m_h_jem_em_2d_etaPhi_jetEl_SimNeOverlap = m_histTool->bookJEMEtaVsPhi(
            "jem_em_2d_etaPhi_jetEl_SimNeOverlap",
            "Overlap Jet Elements EM Data/Simulation Non-zero Mismatches");
        m_h_jem_em_2d_etaPhi_jetEl_SimNoOverlap = m_histTool->bookJEMEtaVsPhi(
            "jem_em_2d_etaPhi_jetEl_SimNoOverlap",
            "Overlap Jet Elements EM Simulation but no Data");
        m_h_jem_em_2d_etaPhi_jetEl_OverlapNoSim = m_histTool->bookJEMEtaVsPhi(
            "jem_em_2d_etaPhi_jetEl_OverlapNoSim",
            "Overlap Jet Elements EM Data but no Simulation");
        m_h_jem_had_2d_etaPhi_jetEl_SimEqOverlap = m_histTool->bookJEMEtaVsPhi(
            "jem_had_2d_etaPhi_jetEl_SimEqOverlap",
            "Overlap Jet Elements HAD Data/Simulation Non-zero Matches");
        m_h_jem_had_2d_etaPhi_jetEl_SimNeOverlap = m_histTool->bookJEMEtaVsPhi(
            "jem_had_2d_etaPhi_jetEl_SimNeOverlap",
            "Overlap Jet Elements HAD Data/Simulation Non-zero Mismatches");
        m_h_jem_had_2d_etaPhi_jetEl_SimNoOverlap = m_histTool->bookJEMEtaVsPhi(
            "jem_had_2d_etaPhi_jetEl_SimNoOverlap",
            "Overlap Jet Elements HAD Simulation but no Data");
        m_h_jem_had_2d_etaPhi_jetEl_OverlapNoSim = m_histTool->bookJEMEtaVsPhi(
            "jem_had_2d_etaPhi_jetEl_OverlapNoSim",
            "Overlap Jet Elements HAD Data but no Simulation");

        //  RoIs

        m_histTool->setMonGroup(&monRoIs);

        m_h_jem_2d_roi_EnergyLgSimEqData = m_histTool->bookJEMCrateModuleVsFrameLoc(
            "jem_2d_roi_EnergyLgSimEqData",
            "JEM RoI Energy Large Data/Simulation Non-zero Matches");
        m_h_jem_2d_roi_EnergyLgSimNeData = m_histTool->bookJEMCrateModuleVsFrameLoc(
            "jem_2d_roi_EnergyLgSimNeData",
            "JEM RoI Energy Large Data/Simulation Non-zero Mismatches");
        m_h_jem_2d_roi_EnergyLgSimNoData = m_histTool->bookJEMCrateModuleVsFrameLoc(
            "jem_2d_roi_EnergyLgSimNoData",
            "JEM RoI Energy Large Simulation but no Data");
        m_h_jem_2d_roi_EnergyLgDataNoSim = m_histTool->bookJEMCrateModuleVsFrameLoc(
            "jem_2d_roi_EnergyLgDataNoSim",
            "JEM RoI Energy Large Data but no Simulation");
        m_h_jem_2d_roi_EnergySmSimEqData = m_histTool->bookJEMCrateModuleVsFrameLoc(
            "jem_2d_roi_EnergySmSimEqData",
            "JEM RoI Energy Small Data/Simulation Non-zero Matches");
        m_h_jem_2d_roi_EnergySmSimNeData = m_histTool->bookJEMCrateModuleVsFrameLoc(
            "jem_2d_roi_EnergySmSimNeData",
            "JEM RoI Energy Small Data/Simulation Non-zero Mismatches");
        m_h_jem_2d_roi_EnergySmSimNoData = m_histTool->bookJEMCrateModuleVsFrameLoc(
            "jem_2d_roi_EnergySmSimNoData",
            "JEM RoI Energy Small Simulation but no Data");
        m_h_jem_2d_roi_EnergySmDataNoSim = m_histTool->bookJEMCrateModuleVsFrameLoc(
            "jem_2d_roi_EnergySmDataNoSim",
            "JEM RoI Energy Small Data but no Simulation");

        m_h_jem_2d_etaPhi_roi_SimEqData = m_histTool->bookJEMRoIEtaVsPhi(
            "jem_2d_etaPhi_roi_SimEqData",
            "JEM RoI Data/Simulation Non-zero Matches");
        m_h_jem_2d_etaPhi_roi_SimNeData = m_histTool->bookJEMRoIEtaVsPhi(
            "jem_2d_etaPhi_roi_SimNeData",
            "JEM RoI Data/Simulation Non-zero Mismatches");
        m_h_jem_2d_etaPhi_roi_SimNoData = m_histTool->bookJEMRoIEtaVsPhi(
            "jem_2d_etaPhi_roi_SimNoData", "JEM RoI Simulation but no Data");
        m_h_jem_2d_etaPhi_roi_DataNoSim = m_histTool->bookJEMRoIEtaVsPhi(
            "jem_2d_etaPhi_roi_DataNoSim", "JEM RoI Data but no Simulation");

        //  CMX-Jet TOBs

        m_histTool->setMonGroup(&monTobs);

        m_h_cmx_2d_tob_EnergyLgSimEqData = m_histTool->bookJEMCrateModuleVsFrameLoc(
            "cmx_2d_tob_EnergyLgSimEqData",
            "CMX TOB Energy Large Data/Simulation Non-zero Matches");
        m_h_cmx_2d_tob_EnergyLgSimNeData = m_histTool->bookJEMCrateModuleVsFrameLoc(
            "cmx_2d_tob_EnergyLgSimNeData",
            "CMX TOB Energy Large Data/Simulation Non-zero Mismatches");
        m_h_cmx_2d_tob_EnergyLgSimNoData = m_histTool->bookJEMCrateModuleVsFrameLoc(
            "cmx_2d_tob_EnergyLgSimNoData",
            "CMX TOB Energy Large Simulation but no Data");
        m_h_cmx_2d_tob_EnergyLgDataNoSim = m_histTool->bookJEMCrateModuleVsFrameLoc(
            "cmx_2d_tob_EnergyLgDataNoSim",
            "CMX TOB Energy Large Data but no Simulation");
        m_h_cmx_2d_tob_EnergySmSimEqData = m_histTool->bookJEMCrateModuleVsFrameLoc(
            "cmx_2d_tob_EnergySmSimEqData",
            "CMX TOB Energy Small Data/Simulation Non-zero Matches");
        m_h_cmx_2d_tob_EnergySmSimNeData = m_histTool->bookJEMCrateModuleVsFrameLoc(
            "cmx_2d_tob_EnergySmSimNeData",
            "CMX TOB Energy Small Data/Simulation Non-zero Mismatches");
        m_h_cmx_2d_tob_EnergySmSimNoData = m_histTool->bookJEMCrateModuleVsFrameLoc(
            "cmx_2d_tob_EnergySmSimNoData",
            "CMX TOB Energy Small Simulation but no Data");
        m_h_cmx_2d_tob_EnergySmDataNoSim = m_histTool->bookJEMCrateModuleVsFrameLoc(
            "cmx_2d_tob_EnergySmDataNoSim",
            "CMX TOB Energy Small Data but no Simulation");

        m_h_cmx_2d_etaPhi_tob_SimEqData = m_histTool->bookJEMRoIEtaVsPhi(
            "cmx_2d_etaPhi_tob_SimEqData",
            "CMX TOB Data/Simulation Non-zero Matches");
        m_h_cmx_2d_etaPhi_tob_SimNeData = m_histTool->bookJEMRoIEtaVsPhi(
            "cmx_2d_etaPhi_tob_SimNeData",
            "CMX TOB Data/Simulation Non-zero Mismatches");
        m_h_cmx_2d_etaPhi_tob_SimNoData = m_histTool->bookJEMRoIEtaVsPhi(
            "cmx_2d_etaPhi_tob_SimNoData", "CMX TOB Simulation but no Data");
        m_h_cmx_2d_etaPhi_tob_DataNoSim = m_histTool->bookJEMRoIEtaVsPhi(
            "cmx_2d_etaPhi_tob_DataNoSim", "CMX TOB Data but no Simulation");

        m_histTool->setMonGroup(&monHitSums);

        // Local/Remote/Total sums

        m_h_cmx_1d_thresh_SumsSimEqData = m_histTool->book1F(
            "cmx_1d_thresh_SumsSimEqData",
            "CMX Hit Sums Data/Simulation Non-zero Matches", 4, 0, 4);
        setLabelsSH(m_h_cmx_1d_thresh_SumsSimEqData);
        m_h_cmx_1d_thresh_SumsSimNeData = m_histTool->book1F(
            "cmx_1d_thresh_SumsSimNeData",
            "CMX Hit Sums Data/Simulation Non-zero Mismatches", 4, 0, 4);
        setLabelsSH(m_h_cmx_1d_thresh_SumsSimNeData);
        m_h_cmx_1d_thresh_SumsSimNoData =
            m_histTool->book1F("cmx_1d_thresh_SumsSimNoData",
                               "CMX Hit Sums Simulation but no Data", 4, 0, 4);
        setLabelsSH(m_h_cmx_1d_thresh_SumsSimNoData);
        m_h_cmx_1d_thresh_SumsDataNoSim =
            m_histTool->book1F("cmx_1d_thresh_SumsDataNoSim",
                               "CMX Hit Sums Data but no Simulation", 4, 0, 4);
        setLabelsSH(m_h_cmx_1d_thresh_SumsDataNoSim);
        m_h_cmx_1d_thresh_SumsOvfSimEqData = m_histTool->book1F(
            "cmx_1d_thresh_SumsOvfSimEqData",
            "CMX Hit Sums RoI Overflow Bit Data/Simulation Matches", 2, 0, 2);
        LWHist::LWHistAxis *axis = m_h_cmx_1d_thresh_SumsOvfSimEqData->GetXaxis();
        axis->SetBinLabel(1, "Remote");
        axis->SetBinLabel(2, "Total");
        m_h_cmx_1d_thresh_SumsOvfSimNeData = m_histTool->book1F(
            "cmx_1d_thresh_SumsOvfSimNeData",
            "CMX Hit Sums RoI Overflow Bit Data/Simulation Mismatches", 2, 0, 2);
        axis = m_h_cmx_1d_thresh_SumsOvfSimNeData->GetXaxis();
        axis->SetBinLabel(1, "Remote");
        axis->SetBinLabel(2, "Total");
        m_h_cmx_2d_thresh_SumsThreshSimEqData = m_histTool->book2F(
            "cmx_2d_thresh_SumsThreshSimEqData",
            "CMX Hit Sums Data/Simulation Threshold Matches", 4, 0, 4, 25, 0, 25);
        setLabelsSHF(m_h_cmx_2d_thresh_SumsThreshSimEqData);
        m_h_cmx_2d_thresh_SumsThreshSimNeData =
            m_histTool->book2F("cmx_2d_thresh_SumsThreshSimNeData",
                               "CMX Hit Sums Data/Simulation Threshold Mismatches",
                               4, 0, 4, 25, 0, 25);
        setLabelsSHF(m_h_cmx_2d_thresh_SumsThreshSimNeData);

        // Topo output information

        m_histTool->setMonGroup(&monTopo);

        m_h_cmx_2d_topo_SimEqData = m_histTool->book2F(
            "cmx_2d_topo_SimEqData",
            "CMX Topo Output Data/Simulation Non-zero Matches", 2, 0, 2, 3, 0, 3);
        setLabelsTopo(m_h_cmx_2d_topo_SimEqData);
        m_h_cmx_2d_topo_SimNeData = m_histTool->book2F(
            "cmx_2d_topo_SimNeData",
            "CMX Topo Output Data/Simulation Non-zero Mismatches", 2, 0, 2, 3, 0,
            3);
        setLabelsTopo(m_h_cmx_2d_topo_SimNeData);
        m_h_cmx_2d_topo_SimNoData = m_histTool->book2F(
            "cmx_2d_topo_SimNoData", "CMX Topo Output Simulation but no Data", 2, 0,
            2, 3, 0, 3);
        setLabelsTopo(m_h_cmx_2d_topo_SimNoData);
        m_h_cmx_2d_topo_DataNoSim = m_histTool->book2F(
            "cmx_2d_topo_DataNoSim", "CMX Topo Output Data but no Simulation", 2, 0,
            2, 3, 0, 3);
        setLabelsTopo(m_h_cmx_2d_topo_DataNoSim);

        // JEMEtSums

        m_histTool->setMonGroup(&monEnergy);

        m_h_jem_2d_energy_SimEqData = m_histTool->bookJEMCrateModuleVsExEyEt(
            "jem_2d_energy_SimEqData",
            "JEM EtSums Data/Simulation Non-zero Matches");
        m_h_jem_2d_energy_SimNeData = m_histTool->bookJEMCrateModuleVsExEyEt(
            "jem_2d_energy_SimNeData",
            "JEM EtSums Data/Simulation Non-zero Mismatches");
        m_h_jem_2d_energy_SimNoData = m_histTool->bookJEMCrateModuleVsExEyEt(
            "jem_2d_energy_SimNoData", "JEM EtSums Simulation but no Data");
        m_h_jem_2d_energy_DataNoSim = m_histTool->bookJEMCrateModuleVsExEyEt(
            "jem_2d_energy_DataNoSim", "JEM EtSums Data but no Simulation");

        // CMXEtSums

        m_histTool->setMonGroup(&monEnergy2);

        m_h_cmx_2d_energy_JemEqCmx = m_histTool->bookJEMCrateModuleVsExEyEt(
            "cmx_2d_energy_JemEqCmx", "CMX EtSums/JEM EtSums Non-zero Matches");
        m_h_cmx_2d_energy_JemNeCmx = m_histTool->bookJEMCrateModuleVsExEyEt(
            "cmx_2d_energy_JemNeCmx", "CMX EtSums/JEM EtSums Non-zero Mismatches");
        m_h_cmx_2d_energy_JemNoCmx = m_histTool->bookJEMCrateModuleVsExEyEt(
            "cmx_2d_energy_JemNoCmx", "JEM EtSums but no CMX EtSums");
        m_h_cmx_2d_energy_CmxNoJem = m_histTool->bookJEMCrateModuleVsExEyEt(
            "cmx_2d_energy_CmxNoJem", "CMX EtSums but no JEM EtSums");

        m_histTool->setMonGroup(&monEnergySums);

        // Energy Crate/System sums

        m_h_cmx_2d_energy_SumsSimEqData = m_histTool->book2F(
            "cmx_2d_energy_SumsSimEqData",
            "Energy Totals Data/Simulation Non-zero Matches", 5, 0, 5, 11, 0, 11);
        setLabelsEnTot(m_h_cmx_2d_energy_SumsSimEqData);
        m_h_cmx_2d_energy_SumsSimNeData =
            m_histTool->book2F("cmx_2d_energy_SumsSimNeData",
                               "Energy Totals Data/Simulation Non-zero Mismatches",
                               5, 0, 5, 11, 0, 11);
        setLabelsEnTot(m_h_cmx_2d_energy_SumsSimNeData);
        m_h_cmx_2d_energy_SumsSimNoData = m_histTool->book2F(
            "cmx_2d_energy_SumsSimNoData", "Energy Totals Simulation but no Data",
            5, 0, 5, 11, 0, 11);
        setLabelsEnTot(m_h_cmx_2d_energy_SumsSimNoData);
        m_h_cmx_2d_energy_SumsDataNoSim = m_histTool->book2F(
            "cmx_2d_energy_SumsDataNoSim", "Energy Totals Data but no Simulation",
            5, 0, 5, 11, 0, 11);
        setLabelsEnTot(m_h_cmx_2d_energy_SumsDataNoSim);
        m_h_cmx_2d_energy_EtMapsThreshSimEqData = m_histTool->book2F(
            "cmx_2d_energy_EtMapsThreshSimEqData",
            "Et Maps Data/Simulation Threshold Matches", 10, 0, 10, 24, 0, 24);
        setLabelsEnTotThr(m_h_cmx_2d_energy_EtMapsThreshSimEqData);
        m_h_cmx_2d_energy_EtMapsThreshSimNeData = m_histTool->book2F(
            "cmx_2d_energy_EtMapsThreshSimNeData",
            "Et Maps Data/Simulation Threshold Mismatches", 10, 0, 10, 24, 0, 24);
        setLabelsEnTotThr(m_h_cmx_2d_energy_EtMapsThreshSimNeData);

        // Summary

        m_histTool->setMonGroup(&monExpert);

        m_h_jem_2d_SimEqDataOverview = m_histTool->book2F(
            "jem_2d_SimEqDataOverview",
            "JEP Transmission/Comparison with Simulation Overview - Events with "
            "Matches;Crate/Module",
            36, 0, 36, NumberOfSummaryBins, 0, NumberOfSummaryBins);
        m_histTool->jemCMXCrateModule(m_h_jem_2d_SimEqDataOverview);
        setLabels(m_h_jem_2d_SimEqDataOverview, false);

        m_h_jem_2d_SimNeDataOverview = m_histTool->book2F(
            "jem_2d_SimNeDataOverview",
            "JEP Transmission/Comparison with Simulation Overview - Events with "
            "Mismatches;Crate/Module",
            36, 0, 36, NumberOfSummaryBins, 0, NumberOfSummaryBins);
        m_histTool->jemCMXCrateModule(m_h_jem_2d_SimNeDataOverview);
        setLabels(m_h_jem_2d_SimNeDataOverview, false);

        m_histTool->setMonGroup(&monShift);

        m_h_jem_1d_SimNeDataSummary = m_histTool->book1F(
            "jem_1d_SimNeDataSummary",
            "JEP Transmission/Comparison with Simulation Mismatch Summary;;Events",
            NumberOfSummaryBins, 0, NumberOfSummaryBins);
        setLabels(m_h_jem_1d_SimNeDataSummary);

        // Mismatch Event Number Samples

        m_histTool->setMonGroup(&monEvent1);

        TH2I_LW *hist = 0;
        m_v_2d_MismatchEvents.clear();
        m_v_2d_MismatchEvents.resize(8, hist);
        hist = m_histTool->bookJEMEventVsCrateModule(
            "jem_em_2d_jetEl_MismatchEvents",
            "Jet Elements EM Mismatch Event Numbers");
        m_v_2d_MismatchEvents[0] = hist;
        hist = m_histTool->bookJEMEventVsCrateModule(
            "jem_had_2d_jetEl_MismatchEvents",
            "Jet Elements Had Mismatch Event Numbers");
        m_v_2d_MismatchEvents[1] = hist;
        hist = m_histTool->bookJEMEventVsCrateModule(
            "jem_2d_roi_MismatchEvents", "JEM RoIs Mismatch Event Numbers");
        m_v_2d_MismatchEvents[2] = hist;
        hist = m_histTool->bookJEMEventVsCrateModule(
            "jem_2d_energy_MismatchEvents", "JEM Energy Mismatch Event Numbers");
        m_v_2d_MismatchEvents[4] = hist;

        m_histTool->setMonGroup(&monEvent2);

        hist = m_histTool->bookJEMEventVsCrateModule(
            "cmx_2d_tob_MismatchEvents", "CMX TOBs Mismatch Event Numbers");
        m_v_2d_MismatchEvents[3] = hist;
        hist = m_histTool->bookJEMEventVsCrateModule(
            "cmx_2d_energy_MismatchEvents", "CMX Energy Mismatch Event Numbers");
        m_v_2d_MismatchEvents[5] = hist;
        hist = m_histTool->bookEventNumbers("cmx_2d_thresh_SumsMismatchEvents",
                                            "CMX Hit Sums Mismatch Event Numbers",
                                            6, 0., 6.);
        axis = hist->GetYaxis();
        axis->SetBinLabel(1, "Local 0");
        axis->SetBinLabel(2, "Local 1");
        axis->SetBinLabel(3, "Remote");
        axis->SetBinLabel(4, "Total");
        axis->SetBinLabel(5, "Topo 0");
        axis->SetBinLabel(6, "Topo 1");
        m_v_2d_MismatchEvents[6] = hist;
        hist = m_histTool->bookEventNumbers(
            "cmx_2d_energy_SumsMismatchEvents",
            "CMX Energy Sums Mismatch Event Numbers", 10, 0., 10.);
        axis = hist->GetYaxis();
        axis->SetBinLabel(1, "Modules 0");
        axis->SetBinLabel(2, "Modules 1");
        axis->SetBinLabel(3, "Local 0");
        axis->SetBinLabel(4, "Local 1");
        axis->SetBinLabel(5, "Remote");
        axis->SetBinLabel(6, "Total");
        axis->SetBinLabel(7, "SumEt");
        axis->SetBinLabel(8, "MissingEt");
        axis->SetBinLabel(9, "MissEtSig");
        axis->SetBinLabel(10, "Engy RoIs");
        m_v_2d_MismatchEvents[7] = hist;

        m_histTool->unsetMonGroup();
        m_histBooked = true;

    } // end if (newRun ...

    msg(MSG::DEBUG) << "Leaving bookHistograms" << endmsg;

    return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode JEPSimMon::fillHistograms()
/*---------------------------------------------------------*/
{
    ATH_MSG_DEBUG("fillHistograms entered");

    if (!m_histBooked)
    {
        ATH_MSG_DEBUG("Histogram(s) not booked");
        return StatusCode::SUCCESS;
    }

    // Skip events believed to be corrupt or with ROB errors

    if (m_errorTool->corrupt() || m_errorTool->robOrUnpackingError())
    {
        ATH_MSG_DEBUG("Skipping corrupt event");
        return StatusCode::SUCCESS;
    }

    StatusCode sc;
    // Retrieve Trigger Towers from SG
    const xAOD::TriggerTowerContainer *triggerTowerTES = 0;
    sc = evtStore()->retrieve(triggerTowerTES, m_triggerTowerLocation);
    if (sc.isFailure() || !triggerTowerTES)
    {
        msg(MSG::DEBUG) << "No Trigger Tower container found" << endmsg;
    }

    // Retrieve Core and Overlap Jet Elements from SG
    const xAOD::JetElementContainer *jetElementTES = 0;
    const xAOD::JetElementContainer *jetElementOvTES = 0;
    sc = evtStore()->retrieve(jetElementTES, m_jetElementLocation);
    if (sc.isFailure() || !jetElementTES)
    {
        msg(MSG::DEBUG) << "No Core Jet Element container found" << endmsg;
    }
    sc = evtStore()->retrieve(jetElementOvTES, m_jetElementLocationOverlap);
    if (sc.isFailure() || !jetElementOvTES)
    {
        msg(MSG::DEBUG) << "No Overlap Jet Element container found" << endmsg;
    }
    // Retrieve JEM RoIs from SG
    const xAOD::JEMTobRoIContainer *jemRoiTES = 0;
    sc = evtStore()->retrieve(jemRoiTES, m_jemRoiLocation);
    if (sc.isFailure() || !jemRoiTES || jemRoiTES->empty())
    {
        msg(MSG::DEBUG) << "No DAQ JEM RoIs container found" << endmsg;
    }

    // Retrieve ROD Headers from SG
    m_rodTES = 0;
    m_limitedRoi = 0;
    sc = evtStore()->retrieve(m_rodTES, m_rodHeaderLocation);
    if (sc.isFailure() || !m_rodTES)
    {
        msg(MSG::DEBUG) << "No ROD Header container found" << endmsg;
    }
    // Retrieve CMX-Jet TOBs from SG
    const xAOD::CMXJetTobContainer *cmxJetTobTES = 0;
    sc = evtStore()->retrieve(cmxJetTobTES, m_cmxJetTobLocation);
    if (sc.isFailure() || !cmxJetTobTES)
    {
        msg(MSG::DEBUG) << "No CMX-Jet TOB container found" << endmsg;
    }
    // Retrieve CMX-Jet Hits from SG
    const xAOD::CMXJetHitsContainer *cmxJetHitsTES = 0;
    sc = evtStore()->retrieve(cmxJetHitsTES, m_cmxJetHitsLocation);
    if (sc.isFailure() || !cmxJetHitsTES)
    {
        msg(MSG::DEBUG) << "No CMX-Jet Hits container found" << endmsg;
    }

    // Retrieve CMX RoIs from SG
    const xAOD::CMXRoIContainer *cmxRoiTEScontainer = 0;
    const xAOD::CMXRoI *cmxRoiTES = 0;
    sc = evtStore()->retrieve(cmxRoiTEScontainer, m_cmxRoiLocation);
    if (sc.isFailure() || !cmxRoiTEScontainer || !cmxRoiTEScontainer->size())
    {
        msg(MSG::DEBUG) << "No DAQ CMX RoIs found" << endmsg;
    }
    cmxRoiTES = *cmxRoiTEScontainer->begin();

    // Retrieve JEM Et Sums from SG
    const xAOD::JEMEtSumsContainer *jemEtSumsTES = 0;
    sc = evtStore()->retrieve(jemEtSumsTES, m_jemEtSumsLocation);
    if (sc.isFailure() || !jemEtSumsTES)
    {
        msg(MSG::DEBUG) << "No JEM Et Sums container found" << endmsg;
    }

    // Retrieve CMX Et Sums from SG
    const xAOD::CMXEtSumsContainer *cmxEtSumsTES = 0;
    sc = evtStore()->retrieve(cmxEtSumsTES, m_cmxEtSumsLocation);
    if (sc.isFailure() || !cmxEtSumsTES)
    {
        msg(MSG::DEBUG) << "No CMX-Energy Et Sums container found" << endmsg;
    }

    // Maps to simplify comparisons

    JetElementMap jeMap;
    JetElementMap ovMap;
    JEMTobRoIMap jrMap;
    CmxJetTobMap ctMap;
    CmxJetHitsMap chMap;
    JemEtSumsMap jsMap;
    CmxEtSumsMap csMap;
    setupMap(jetElementTES, jeMap);
    setupMap(jetElementOvTES, ovMap);
    setupMap(jemRoiTES, jrMap);
    setupMap(cmxJetTobTES, ctMap);
    setupMap(cmxJetHitsTES, chMap);
    setupMap(jemEtSumsTES, jsMap);
    setupMap(cmxEtSumsTES, csMap);

    // Vectors for error overview bits;
    const int vecsizeJem = 2 * s_crates * s_modules;
    const int vecsizeCmx = 2 * s_crates * s_cmxs;
    ErrorVector errorsJEM(vecsizeJem);
    ErrorVector errorsCMX(vecsizeCmx);

    // Compare Jet Elements simulated from Trigger Towers with Jet Elements
    // from data

    xAOD::JetElementContainer *jetElementSIM = 0;
    xAOD::JetElementAuxContainer *jetElementSIMAux = 0;
    if (triggerTowerTES)
    {
        jetElementSIM = new xAOD::JetElementContainer;
        jetElementSIMAux = new xAOD::JetElementAuxContainer;
        jetElementSIM->setStore(jetElementSIMAux);

        simulate(triggerTowerTES, jetElementSIM);
    }

    JetElementMap jeSimMap;
    setupMap(jetElementSIM, jeSimMap);
    bool overlap = false;
    bool mismatchCore = false;
    bool mismatchOverlap = false;
    mismatchCore = compare(jeSimMap, jeMap, errorsJEM, overlap);
    if (jetElementOvTES)
    {
        overlap = true;
        mismatchOverlap = compare(jeSimMap, ovMap, errorsJEM, overlap);
    }
    jeSimMap.clear();
    delete jetElementSIM;
    delete jetElementSIMAux;

    // Compare RoIs simulated from Jet Elements with JEM RoIs from data

    xAOD::JEMTobRoIContainer *jemRoiSIM = 0;
    xAOD::JEMTobRoIAuxContainer *jemRoiSIMAux = 0;
    if (jetElementTES || jetElementOvTES)
    {
        jemRoiSIM = new xAOD::JEMTobRoIContainer;
        jemRoiSIMAux = new xAOD::JEMTobRoIAuxContainer;
        jemRoiSIM->setStore(jemRoiSIMAux);
        if (mismatchCore || mismatchOverlap)
        {
            simulate(jetElementTES, jetElementOvTES, jemRoiSIM);
        }
        else
        {
            simulate(jetElementTES, jemRoiSIM);
        }
    }
    JEMTobRoIMap jrSimMap;
    setupMap(jemRoiSIM, jrSimMap);
    compare(jrSimMap, jrMap, errorsJEM);
    jrSimMap.clear();
    delete jemRoiSIM;
    delete jemRoiSIMAux;

    // Compare CMX-Jet TOBS simulated from RoIs with TOBs from data

    xAOD::CMXJetTobContainer *cmxTobSIM = 0;
    xAOD::CMXJetTobAuxContainer *cmxTobSIMAux = 0;
    if (jemRoiTES)
    {
        cmxTobSIM = new xAOD::CMXJetTobContainer;
        cmxTobSIMAux = new xAOD::CMXJetTobAuxContainer;
        cmxTobSIM->setStore(cmxTobSIMAux);
        simulate(jemRoiTES, cmxTobSIM);
    }
    CmxJetTobMap cmxTobSimMap;
    setupMap(cmxTobSIM, cmxTobSimMap);
    compare(cmxTobSimMap, ctMap, errorsJEM, errorsCMX);
    cmxTobSimMap.clear();
    
    // Sasha: Delete later, will use simulated tobs later since data tobs does not
    // contains overflow bits
    // delete cmxTobSIM;
    // delete cmxTobSIMAux;

    // Compare Local sums simulated from CMX TOBs with Local sums from data

    xAOD::CMXJetHitsContainer *cmxLocalSIM = 0;
    xAOD::CMXJetHitsAuxContainer *cmxLocalSIMAux = 0;
    if (cmxJetTobTES)
    {
        cmxLocalSIM = new xAOD::CMXJetHitsContainer;
        cmxLocalSIMAux = new xAOD::CMXJetHitsAuxContainer;
        cmxLocalSIM->setStore(cmxLocalSIMAux);
        // simulate(cmxJetTobTES, cmxLocalSIM, xAOD::CMXJetHits::Sources::LOCAL_MAIN);
        simulate(cmxTobSIM, cmxLocalSIM, xAOD::CMXJetHits::Sources::LOCAL_MAIN);
        
    }
    CmxJetHitsMap cmxLocalSimMap;
    setupMap(cmxLocalSIM, cmxLocalSimMap);
    compare(cmxLocalSimMap, chMap, errorsCMX,
            xAOD::CMXJetHits::Sources::LOCAL_MAIN);
    
    dumpDataAndSim("Compare Local sums simulated from CMX TOBs with Local sums from data",
        chMap, cmxLocalSimMap);

    cmxLocalSimMap.clear();
    delete cmxLocalSIM;
    delete cmxLocalSIMAux;
    delete cmxTobSIM;
    delete cmxTobSIMAux;

    // Compare Local sums with Remote sums from data

    compare(chMap, chMap, errorsCMX, xAOD::CMXJetHits::Sources::REMOTE_MAIN);

    // Compare Total sums simulated from Remote sums with Total sums from data

    xAOD::CMXJetHitsContainer *cmxTotalSIM = 0;
    xAOD::CMXJetHitsAuxContainer *cmxTotalSIMAux = 0;
    if (cmxJetHitsTES)
    {
        cmxTotalSIM = new xAOD::CMXJetHitsContainer;
        cmxTotalSIMAux = new xAOD::CMXJetHitsAuxContainer;
        cmxTotalSIM->setStore(cmxTotalSIMAux);
        simulate(cmxJetHitsTES, cmxTotalSIM);
    }
    CmxJetHitsMap cmxTotalSimMap;
    setupMap(cmxTotalSIM, cmxTotalSimMap);
    compare(cmxTotalSimMap, chMap, errorsCMX,
            xAOD::CMXJetHits::Sources::TOTAL_MAIN);
    cmxTotalSimMap.clear();
    delete cmxTotalSIM;
    delete cmxTotalSIMAux;

    // Compare Topo Info simulated from CMX TOBs with Topo Info from data

    xAOD::CMXJetHitsContainer *cmxTopoSIM = 0;
    xAOD::CMXJetHitsAuxContainer *cmxTopoSIMAux = 0;
    if (cmxJetTobTES)
    {
        cmxTopoSIM = new xAOD::CMXJetHitsContainer;
        cmxTopoSIMAux = new xAOD::CMXJetHitsAuxContainer;
        cmxTopoSIM->setStore(cmxTopoSIMAux);
        simulate(cmxJetTobTES, cmxTopoSIM,
                 xAOD::CMXJetHits::Sources::TOPO_CHECKSUM);
    }
    CmxJetHitsMap cmxTopoSimMap;
    setupMap(cmxTopoSIM, cmxTopoSimMap);
    compare(cmxTopoSimMap, chMap, errorsCMX,
            xAOD::CMXJetHits::Sources::TOPO_CHECKSUM);
    cmxTopoSimMap.clear();
    delete cmxTopoSIM;
    delete cmxTopoSIMAux;

    // Compare JEMEtSums simulated from JetElements with JEMEtSums from data

    xAOD::JEMEtSumsContainer *jemEtSumsSIM = 0;
    xAOD::JEMEtSumsAuxContainer *jemEtSumsSIMAux = 0;
    if (jetElementTES)
    {
        jemEtSumsSIM = new xAOD::JEMEtSumsContainer;
        jemEtSumsSIMAux = new xAOD::JEMEtSumsAuxContainer;
        jemEtSumsSIM->setStore(jemEtSumsSIMAux);
        simulate(jetElementTES, jemEtSumsSIM);
    }
    JemEtSumsMap jemEtSumsSimMap;
    setupMap(jemEtSumsSIM, jemEtSumsSimMap);
    compare(jemEtSumsSimMap, jsMap, errorsJEM);
    jemEtSumsSimMap.clear();
    delete jemEtSumsSIM;
    delete jemEtSumsSIMAux;

    // Compare JEMEtSums with CMXEtSums from data

    compare(jsMap, csMap, errorsJEM, errorsCMX);

    // Compare Local sums simulated from CMXEtSums with Local sums from data

    xAOD::CMXEtSumsContainer *cmxEtLocalSIM = 0;
    xAOD::CMXEtSumsAuxContainer *cmxEtLocalSIMAux = 0;
    if (cmxEtSumsTES)
    {
        cmxEtLocalSIM = new xAOD::CMXEtSumsContainer;
        cmxEtLocalSIMAux = new xAOD::CMXEtSumsAuxContainer;
        cmxEtLocalSIM->setStore(cmxEtLocalSIMAux);
        simulate(cmxEtSumsTES, cmxEtLocalSIM,
                 xAOD::CMXEtSums::Sources::LOCAL_STANDARD);
    }
    CmxEtSumsMap cmxEtLocalSimMap;
    setupMap(cmxEtLocalSIM, cmxEtLocalSimMap);
    compare(cmxEtLocalSimMap, csMap, errorsCMX,
            xAOD::CMXEtSums::Sources::LOCAL_STANDARD);

    dumpDataAndSim(
        "Compare Local sums simulated from CMXEtSums with Local sums from data",
        csMap, cmxEtLocalSimMap);

    cmxEtLocalSimMap.clear();
    delete cmxEtLocalSIM;
    delete cmxEtLocalSIMAux;

    // Compare Local Energy sums with Remote sums from data

    compare(csMap, csMap, errorsCMX, xAOD::CMXEtSums::Sources::REMOTE_STANDARD);

    // Compare Total sums simulated from Remote sums with Total sums from data

    xAOD::CMXEtSumsContainer *cmxEtTotalSIM = 0;
    xAOD::CMXEtSumsAuxContainer *cmxEtTotalSIMAux = 0;
    if (cmxEtSumsTES)
    {
        cmxEtTotalSIM = new xAOD::CMXEtSumsContainer;
        cmxEtTotalSIMAux = new xAOD::CMXEtSumsAuxContainer;
        cmxEtTotalSIM->setStore(cmxEtTotalSIMAux);

        simulate(cmxEtSumsTES, cmxEtTotalSIM,
                 xAOD::CMXEtSums::Sources::TOTAL_STANDARD);
    }
    CmxEtSumsMap cmxEtTotalSimMap;
    setupMap(cmxEtTotalSIM, cmxEtTotalSimMap);
    compare(cmxEtTotalSimMap, csMap, errorsCMX,
            xAOD::CMXEtSums::Sources::TOTAL_STANDARD);
    
    dumpDataAndSim(
      "Compare Total sums simulated from Remote sums with Total sums from data",
       csMap, cmxEtTotalSimMap
    );

    cmxEtTotalSimMap.clear();
    delete cmxEtTotalSIM;
    delete cmxEtTotalSIMAux;

    // Compare Et Maps (sumEt/missingEt/missingEtSig) simulated from Total sums
    // with Et Maps from data

    xAOD::CMXEtSumsContainer *cmxSumEtSIM = 0;
    xAOD::CMXEtSumsAuxContainer *cmxSumEtSIMAux = 0;
    if (cmxEtSumsTES)
    {
        cmxSumEtSIM = new xAOD::CMXEtSumsContainer;
        cmxSumEtSIMAux = new xAOD::CMXEtSumsAuxContainer;
        cmxSumEtSIM->setStore(cmxSumEtSIMAux);

        simulate(cmxEtSumsTES, cmxSumEtSIM,
                 xAOD::CMXEtSums::Sources::SUM_ET_STANDARD);
    }
    CmxEtSumsMap cmxSumEtSimMap;
    setupMap(cmxSumEtSIM, cmxSumEtSimMap);
    compare(cmxSumEtSimMap, csMap, errorsCMX,
              xAOD::CMXEtSums::Sources::SUM_ET_STANDARD);
    
    dumpDataAndSim(
      "Compare Et Maps (sumEt/missingEt/missingEtSig) simulated from Total sums",
      csMap, cmxSumEtSimMap
    );
    

    cmxSumEtSimMap.clear();
    delete cmxSumEtSIM;
    delete cmxSumEtSIMAux;

    // Compare Total Energy sums and Et Maps with Energy RoIs from data

    compare(csMap, cmxRoiTES, errorsCMX);

    // Update error summary plots

    ErrorVector crateErr(s_crates);
    const int jemBins = s_crates * s_modules;
    const int cmxBins = s_crates * s_cmxs;
    for (int err = 0; err < NumberOfSummaryBins; ++err)
    {
        int error = 0;
        for (int loc = 0; loc < jemBins; ++loc)
        {
            if ((errorsJEM[loc] >> err) & 0x1)
            {
                m_h_jem_2d_SimEqDataOverview->Fill(loc, err, 1.);
            }
            if ((errorsJEM[loc + jemBins] >> err) & 0x1)
            {
                m_h_jem_2d_SimNeDataOverview->Fill(loc, err, 1.);
                error = 1;
                crateErr[loc / s_modules] |= (1 << err);
                fillEventSample(err, loc, true);
            }
            if (loc < cmxBins)
            {
                if ((errorsCMX[loc] >> err) & 0x1)
                {
                    m_h_jem_2d_SimEqDataOverview->Fill(loc + jemBins, err, 1.);
                }
                if ((errorsCMX[loc + cmxBins] >> err) & 0x1)
                {
                    m_h_jem_2d_SimNeDataOverview->Fill(loc + jemBins, err, 1.);
                    error = 1;
                    crateErr[loc / s_cmxs] |= (1 << err);
                    fillEventSample(err, loc, false);
                }
            }
        }
        if (error)
            m_h_jem_1d_SimNeDataSummary->Fill(err);
    }

    // Save error vector for global summary

    ErrorVector *save = new ErrorVector(crateErr);
    sc = evtStore()->record(save, m_errorLocation);
    if (sc != StatusCode::SUCCESS)
    {
        ATH_MSG_ERROR("Error recording JEM mismatch vector in TES ");
        return sc;
    }

    ATH_MSG_DEBUG("Leaving fillHistograms");

    return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode JEPSimMon::procHistograms()
/*---------------------------------------------------------*/
{
    msg(MSG::DEBUG) << "procHistograms entered" << endmsg;

    // if (endOfEventsBlock || endOfLumiBlock || endOfRun) {
    //}

    return StatusCode::SUCCESS;
}

//  Compare Simulated JetElements with data

bool JEPSimMon::compare(const JetElementMap &jeSimMap,
                        const JetElementMap &jeMap, ErrorVector &errors,
                        bool overlap)
{
    if (m_debug)
    {
        msg(MSG::DEBUG) << "Compare Simulated JetElements with data" << endmsg;
    }

    bool mismatch = false;

    JetElementMap::const_iterator simMapIter = jeSimMap.begin();
    JetElementMap::const_iterator simMapIterEnd = jeSimMap.end();
    JetElementMap::const_iterator datMapIter = jeMap.begin();
    JetElementMap::const_iterator datMapIterEnd = jeMap.end();

    while (simMapIter != simMapIterEnd || datMapIter != datMapIterEnd)
    {
        int simKey = 0;
        int datKey = 0;
        int simEm = 0;
        int simHad = 0;
        int datEm = 0;
        int datHad = 0;
        double eta = 0.;
        double phi = 0.;

        if (simMapIter != simMapIterEnd)
            simKey = simMapIter->first;
        if (datMapIter != datMapIterEnd)
            datKey = datMapIter->first;

        if ((datMapIter == datMapIterEnd) ||
            ((simMapIter != simMapIterEnd) && (datKey > simKey)))
        {
            // Simulated JetElement but no data JetElement

            const xAOD::JetElement *je = simMapIter->second;
            simEm = je->emJetElementET();
            simHad = je->hadJetElementET();
            eta = je->eta();
            phi = je->phi();
            ++simMapIter;
        }
        else if ((simMapIter == simMapIterEnd) ||
                 ((datMapIter != datMapIterEnd) && (simKey > datKey)))
        {
            // Data JetElement but no simulated JetElement

            const xAOD::JetElement *je = datMapIter->second;
            datEm = je->emJetElementET();
            datHad = je->hadJetElementET();
            eta = je->eta();
            phi = je->phi();
            ++datMapIter;
        }
        else
        {
            // Have both

            const xAOD::JetElement *jeS = simMapIter->second;
            const xAOD::JetElement *jeD = datMapIter->second;
            simEm = jeS->emJetElementET();
            simHad = jeS->hadJetElementET();
            datEm = jeD->emJetElementET();
            datHad = jeD->hadJetElementET();
            eta = jeD->eta();
            phi = jeD->phi();
            ++simMapIter;
            ++datMapIter;
        }

        if (!simEm && !simHad && !datEm && !datHad)
            continue;

        //  Fill in error vectors

        const LVL1::Coordinate coord(phi, eta);
        LVL1::CoordToHardware converter;
        const int crate = (overlap) ? converter.jepCrateOverlap(coord)
                                    : converter.jepCrate(coord);
        const int jem = (overlap) ? converter.jepModuleOverlap(coord)
                                  : converter.jepModule(coord);
        if (crate >= s_crates || jem >= s_modules)
            continue;
        const int loc = crate * s_modules + jem;
        const int jemBins = s_crates * s_modules;
        const int bitEm = (1 << EMElementMismatch);
        const int bitHad = (1 << HadElementMismatch);
        if (simEm && simEm == datEm)
            errors[loc] |= bitEm;
        if (simHad && simHad == datHad)
            errors[loc] |= bitHad;
        if (simEm != datEm)
            errors[loc + jemBins] |= bitEm;
        if (simHad != datHad)
            errors[loc + jemBins] |= bitHad;
        if (simEm != datEm || simHad != datHad)
        {
            mismatch = true;
            ATH_MSG_VERBOSE("JE mismatch, EM data/sim: "
                            << datEm << "/" << simEm << " Had data/sim: " << datHad
                            << "/" << simHad << " crate/jem: " << crate << "/" << jem
                            << " eta/phi: " << eta << "/" << phi);
        }
        TH2F_LW *hist1 = 0;
        TH2F_LW *hist2 = 0;
        if (overlap)
        {
            if (simEm && simEm == datEm)
                hist1 = m_h_jem_em_2d_etaPhi_jetEl_SimEqOverlap;
            if (simEm && datEm && simEm != datEm)
                hist1 = m_h_jem_em_2d_etaPhi_jetEl_SimNeOverlap;
            if (simEm && !datEm)
                hist1 = m_h_jem_em_2d_etaPhi_jetEl_SimNoOverlap;
            if (datEm && !simEm)
                hist1 = m_h_jem_em_2d_etaPhi_jetEl_OverlapNoSim;
            if (simHad && simHad == datHad)
                hist2 = m_h_jem_had_2d_etaPhi_jetEl_SimEqOverlap;
            if (simHad && datHad && simHad != datHad)
                hist2 = m_h_jem_had_2d_etaPhi_jetEl_SimNeOverlap;
            if (simHad && !datHad)
                hist2 = m_h_jem_had_2d_etaPhi_jetEl_SimNoOverlap;
            if (datHad && !simHad)
                hist2 = m_h_jem_had_2d_etaPhi_jetEl_OverlapNoSim;
        }
        else
        {
            if (simEm && simEm == datEm)
                hist1 = m_h_jem_em_2d_etaPhi_jetEl_SimEqCore;
            if (simEm && datEm && simEm != datEm)
                hist1 = m_h_jem_em_2d_etaPhi_jetEl_SimNeCore;
            if (simEm && !datEm)
                hist1 = m_h_jem_em_2d_etaPhi_jetEl_SimNoCore;
            if (datEm && !simEm)
                hist1 = m_h_jem_em_2d_etaPhi_jetEl_CoreNoSim;
            if (simHad && simHad == datHad)
                hist2 = m_h_jem_had_2d_etaPhi_jetEl_SimEqCore;
            if (simHad && datHad && simHad != datHad)
                hist2 = m_h_jem_had_2d_etaPhi_jetEl_SimNeCore;
            if (simHad && !datHad)
                hist2 = m_h_jem_had_2d_etaPhi_jetEl_SimNoCore;
            if (datHad && !simHad)
                hist2 = m_h_jem_had_2d_etaPhi_jetEl_CoreNoSim;
        }
        if (hist1)
            m_histTool->fillJEMEtaVsPhi(hist1, eta, phi);
        if (hist2)
            m_histTool->fillJEMEtaVsPhi(hist2, eta, phi);
    }

    return mismatch;
}

//  Compare Simulated RoIs with data

void JEPSimMon::compare(const JEMTobRoIMap &roiSimMap,
                        const JEMTobRoIMap &roiMap, ErrorVector &errors)
{
    if (m_debug)
        msg(MSG::DEBUG) << "Compare Simulated RoIs with data" << endmsg;

    LVL1::JEPRoIDecoder decoder;
    JEMTobRoIMap::const_iterator simMapIter = roiSimMap.begin();
    JEMTobRoIMap::const_iterator simMapIterEnd = roiSimMap.end();
    JEMTobRoIMap::const_iterator datMapIter = roiMap.begin();
    JEMTobRoIMap::const_iterator datMapIterEnd = roiMap.end();

    while (simMapIter != simMapIterEnd || datMapIter != datMapIterEnd)
    {
        int simKey = 0;
        int datKey = 0;
        unsigned int simEnLg = 0;
        unsigned int simEnSm = 0;
        unsigned int datEnLg = 0;
        unsigned int datEnSm = 0;
        const xAOD::JEMTobRoI *roi = 0;

        if (simMapIter != simMapIterEnd)
            simKey = simMapIter->first;
        if (datMapIter != datMapIterEnd)
            datKey = datMapIter->first;

        if ((datMapIter == datMapIterEnd) ||
            ((simMapIter != simMapIterEnd) && (datKey > simKey)))
        {
            // Simulated RoI but no data RoI

            roi = simMapIter->second;
            simEnLg = roi->energyLarge();
            simEnSm = roi->energySmall();
            ++simMapIter;
            ATH_MSG_VERBOSE("Sim  RoI crate/jem/frame/loc/energyLg/energySm: "
                            << roi->crate() << "/" << roi->jem() << "/"
                            << roi->frame() << "/" << roi->location() << "/"
                            << roi->energyLarge() << "/" << roi->energySmall());
        }
        else if ((simMapIter == simMapIterEnd) ||
                 ((datMapIter != datMapIterEnd) && (simKey > datKey)))
        {
            // Data RoI but no simulated RoI

            roi = datMapIter->second;
            datEnLg = roi->energyLarge();
            datEnSm = roi->energySmall();
            ++datMapIter;
            ATH_MSG_VERBOSE("Data RoI crate/jem/frame/loc/energyLg/energySm: "
                            << roi->crate() << "/" << roi->jem() << "/"
                            << roi->frame() << "/" << roi->location() << "/"
                            << roi->energyLarge() << "/" << roi->energySmall());
        }
        else
        {
            // Have both

            const xAOD::JEMTobRoI *roiS = simMapIter->second;
            roi = datMapIter->second;
            simEnLg = roiS->energyLarge();
            simEnSm = roiS->energySmall();
            datEnLg = roi->energyLarge();
            datEnSm = roi->energySmall();
            ++simMapIter;
            ++datMapIter;
            ATH_MSG_VERBOSE("Sim  RoI crate/jem/frame/loc/energyLg/energySm: "
                            << roiS->crate() << "/" << roiS->jem() << "/"
                            << roiS->frame() << "/" << roiS->location() << "/"
                            << roiS->energyLarge() << "/" << roiS->energySmall());
            ATH_MSG_VERBOSE("Data RoI crate/jem/frame/loc/energyLg/energySm: "
                            << roi->crate() << "/" << roi->jem() << "/"
                            << roi->frame() << "/" << roi->location() << "/"
                            << roi->energyLarge() << "/" << roi->energySmall());
        }

        if (!simEnLg && !simEnSm && !datEnLg && !datEnSm)
            continue;

        //  Check LimitedRoISet bit

        const int crate = roi->crate();
        if (!datEnLg && !datEnSm && limitedRoiSet(crate))
            continue;

        //  Fill in error plots

        const int jem = roi->jem();
        const int frame = roi->frame();
        const int local = roi->location();
        const int locX = crate * s_modules + jem;
        const int locY = frame * s_locCoords + local;
        const int jemBins = s_crates * s_modules;
        const int bit = (1 << RoIMismatch);
        const LVL1::CoordinateRange coord(
            decoder.coordinate(roi->roiWord())); // Should still work
        double eta = coord.eta();
        // Distinguish right forward columns 3 and 4 for checking purposes
        if ((jem == 7 || jem == 15) && frame > 3)
            eta = (local % 2) ? 4.05 : 3.2;
        const double phi = coord.phi();

        TH2F_LW *hist = 0;
        if (simEnLg && simEnLg == datEnLg)
        {
            errors[locX] |= bit;
            hist = m_h_jem_2d_roi_EnergyLgSimEqData;
        }
        else if (simEnLg != datEnLg)
        {
            errors[locX + jemBins] |= bit;
            if (simEnLg && datEnLg)
            {
                hist = m_h_jem_2d_roi_EnergyLgSimNeData;
            }
            else if (!datEnLg)
            {
                hist = m_h_jem_2d_roi_EnergyLgSimNoData;
            }
            else
            {
                hist = m_h_jem_2d_roi_EnergyLgDataNoSim;
            }
        }
        if (hist)
            hist->Fill(locX, locY);

        hist = 0;
        if (simEnSm && simEnSm == datEnSm)
        {
            errors[locX] |= bit;
            hist = m_h_jem_2d_roi_EnergySmSimEqData;
        }
        else if (simEnSm != datEnSm)
        {
            errors[locX + jemBins] |= bit;
            if (simEnSm && datEnSm)
            {
                hist = m_h_jem_2d_roi_EnergySmSimNeData;
            }
            else if (!datEnSm)
            {
                hist = m_h_jem_2d_roi_EnergySmSimNoData;
            }
            else
            {
                hist = m_h_jem_2d_roi_EnergySmDataNoSim;
            }
        }
        if (hist)
            hist->Fill(locX, locY);

        // do we need this?
        hist = 0;
        if (simEnLg == datEnLg && simEnSm == datEnSm)
        {
            hist = m_h_jem_2d_etaPhi_roi_SimEqData;
        }
        else
        {
            if ((simEnLg || simEnSm) && (datEnLg || datEnSm))
            {
                hist = m_h_jem_2d_etaPhi_roi_SimNeData;
            }
            else if (!datEnLg && !datEnSm)
            {
                hist = m_h_jem_2d_etaPhi_roi_SimNoData;
            }
            else
            {
                hist = m_h_jem_2d_etaPhi_roi_DataNoSim;
            }
        }
        if (hist)
            m_histTool->fillJEMRoIEtaVsPhi(hist, eta, phi);

        if (simEnLg != datEnLg || simEnSm != datEnSm)
        {
            ATH_MSG_VERBOSE(" RoI Mismatch Crate/JEM DataEnergy/SimEnergy: "
                            << crate << "/" << jem << " " << datEnLg << "/" << simEnLg
                            << " " << datEnSm << "/" << simEnSm);
        }
    }
}

//  Compare simulated CMX TOBs with data

void JEPSimMon::compare(const CmxJetTobMap &simMap, const CmxJetTobMap &datMap,
                        ErrorVector &errorsJEM, ErrorVector &errorsCMX)
{
    if (m_debug)
        msg(MSG::DEBUG) << "Compare simulated CMX TOBs with data" << endmsg;

    LVL1::JEPRoIDecoder decoder;
    CmxJetTobMap::const_iterator simMapIter = simMap.begin();
    CmxJetTobMap::const_iterator simMapIterEnd = simMap.end();
    CmxJetTobMap::const_iterator datMapIter = datMap.begin();
    CmxJetTobMap::const_iterator datMapIterEnd = datMap.end();

    while (simMapIter != simMapIterEnd || datMapIter != datMapIterEnd)
    {
        int simKey = 0;
        int datKey = 0;
        int simEnLg = 0;
        int simEnSm = 0;
        int datEnLg = 0;
        int datEnSm = 0;

        const xAOD::CMXJetTob *tob = 0;
        if (simMapIter != simMapIterEnd)
            simKey = simMapIter->first;
        if (datMapIter != datMapIterEnd)
            datKey = datMapIter->first;

        if ((datMapIter == datMapIterEnd) ||
            ((simMapIter != simMapIterEnd) && (datKey > simKey)))
        {
            // Simulated TOB but no data TOB

            tob = simMapIter->second;
            simEnLg = tob->energyLarge();
            simEnSm = tob->energySmall();
            ++simMapIter;
        }
        else if ((simMapIter == simMapIterEnd) ||
                 ((datMapIter != datMapIterEnd) && (simKey > datKey)))
        {
            // Data TOB but no simulated TOB

            tob = datMapIter->second;
            datEnLg = tob->energyLarge();
            datEnSm = tob->energySmall();
            ++datMapIter;
        }
        else
        {
            // Have both

            const xAOD::CMXJetTob *tobS = simMapIter->second;
            tob = datMapIter->second;
            simEnLg = tobS->energyLarge();
            simEnSm = tobS->energySmall();
            datEnLg = tob->energyLarge();
            datEnSm = tob->energySmall();
            ++simMapIter;
            ++datMapIter;
        }

        if (!simEnLg && !simEnSm && !datEnLg && !datEnSm)
            continue;

        //  Check LimitedRoISet bit

        const int crate = tob->crate();
        if (!simEnLg && !simEnSm && limitedRoiSet(crate))
            continue;

        //  Fill in error plots

        const int cmx = 1; //<<== CHECK
        const int jem = tob->jem();
        const int frame = tob->frame();
        const int loc = tob->location();
        const int locX = crate * s_modules + jem;
        const int locY = frame * 4 + loc;
        const int loc2 = crate * s_cmxs + cmx;
        const int jemBins = s_crates * s_modules;
        const int cmxBins = s_crates * s_cmxs;
        const int bit = (1 << CMXJetTobMismatch);
        const uint32_t roiWord =
            ((((((crate << 4) + jem) << 3) + frame) << 2) + loc) << 19;
        const LVL1::CoordinateRange coord(decoder.coordinate(roiWord));
        const double eta = coord.eta();
        const double phi = coord.phi();

        TH2F_LW *hist = 0;
        if (simEnLg || datEnLg)
        {
            if (simEnLg == datEnLg)
            {
                errorsJEM[locX] |= bit;
                errorsCMX[loc2] |= bit;
                hist = m_h_cmx_2d_tob_EnergyLgSimEqData;
            }
            else
            {
                errorsJEM[locX + jemBins] |= bit;
                errorsCMX[loc2 + cmxBins] |= bit;
                if (simEnLg && datEnLg)
                {
                    hist = m_h_cmx_2d_tob_EnergyLgSimNeData;
                }
                else if (simEnLg && !datEnLg)
                {
                    hist = m_h_cmx_2d_tob_EnergyLgSimNoData;
                }
                else
                {
                    hist = m_h_cmx_2d_tob_EnergyLgDataNoSim;
                }
            }
            if (hist)
                hist->Fill(locX, locY);
        }
        hist = 0;
        if (simEnSm || datEnSm)
        {
            if (simEnSm == datEnSm)
            {
                errorsJEM[locX] |= bit;
                errorsCMX[loc2] |= bit;
                hist = m_h_cmx_2d_tob_EnergySmSimEqData;
            }
            else
            {
                errorsJEM[locX + jemBins] |= bit;
                errorsCMX[loc2 + cmxBins] |= bit;
                if (simEnSm && datEnSm)
                {
                    hist = m_h_cmx_2d_tob_EnergySmSimNeData;
                }
                else if (simEnSm && !datEnSm)
                {
                    hist = m_h_cmx_2d_tob_EnergySmSimNoData;
                }
                else
                {
                    hist = m_h_cmx_2d_tob_EnergySmDataNoSim;
                }
            }
            if (hist)
                hist->Fill(locX, locY);
        }
        // do we need this?
        hist = 0;
        if (simEnLg == datEnLg && simEnSm == datEnSm)
        {
            hist = m_h_cmx_2d_etaPhi_tob_SimEqData;
        }
        else
        {
            if ((simEnLg || simEnSm) && (datEnLg || datEnSm))
            {
                hist = m_h_cmx_2d_etaPhi_tob_SimNeData;
            }
            else if (!datEnLg && !datEnSm)
            {
                hist = m_h_cmx_2d_etaPhi_tob_SimNoData;
            }
            else
            {
                hist = m_h_cmx_2d_etaPhi_tob_DataNoSim;
            }
        }
        if (hist)
            m_histTool->fillJEMRoIEtaVsPhi(hist, eta, phi);
    }
}

//  Compare Simulated CMX Hit Sums and Data CMX Hit Sums

void JEPSimMon::compare(const CmxJetHitsMap &cmxSimMap,
                        const CmxJetHitsMap &cmxMap, ErrorVector &errors,
                        int selection)
{
    if (m_debug)
    {
        msg(MSG::DEBUG) << "Compare Simulated CMX Hit Sums and Data CMX Hit Sums"
                        << endmsg;
    }

    const bool local = (selection == xAOD::CMXJetHits::Sources::LOCAL_MAIN);
    const bool remote = (selection == xAOD::CMXJetHits::Sources::REMOTE_MAIN);
    const bool total = (selection == xAOD::CMXJetHits::Sources::TOTAL_MAIN);
    const bool topo = (selection == xAOD::CMXJetHits::Sources::TOPO_CHECKSUM);
    if (!local && !remote && !total /* && !topo*/)
        return;
    
    

    unsigned int hitsSimMain0 = 0;
    unsigned int hitsSimMain1 = 0;
    unsigned int hitsSimFwd0 = 0;
    unsigned int hitsSimFwd1 = 0;
    unsigned int hitsDatMain0 = 0;
    unsigned int hitsDatMain1 = 0;
    unsigned int hitsDatFwd0 = 0;
    unsigned int hitsDatFwd1 = 0;
    int overflowSim = 0;
    int overflowDat = 0;
    CmxJetHitsMap::const_iterator cmxSimMapIter = cmxSimMap.begin();
    CmxJetHitsMap::const_iterator cmxSimMapIterEnd = cmxSimMap.end();
    CmxJetHitsMap::const_iterator cmxMapIter = cmxMap.begin();
    CmxJetHitsMap::const_iterator cmxMapIterEnd = cmxMap.end();

    while (cmxSimMapIter != cmxSimMapIterEnd || cmxMapIter != cmxMapIterEnd)
    {
        int cmxSimKey = 0;
        int cmxKey = 0;
        unsigned int cmxSimHit0 = 0;
        unsigned int cmxSimHit1 = 0;
        unsigned int cmxHit0 = 0;
        unsigned int cmxHit1 = 0;
        int cmxSimOvf = 0;
        int cmxOvf = 0;
        int crate = 0;
        int source = 0;

        if (cmxSimMapIter != cmxSimMapIterEnd)
            cmxSimKey = cmxSimMapIter->first;
        if (cmxMapIter != cmxMapIterEnd)
            cmxKey = cmxMapIter->first;

        if ((cmxMapIter == cmxMapIterEnd) ||
            ((cmxSimMapIter != cmxSimMapIterEnd) && (cmxKey > cmxSimKey)))
        {
            // Sim CMX Hits but no Data CMX Hits

            const xAOD::CMXJetHits *cmxS = cmxSimMapIter->second;
            ++cmxSimMapIter;
            source = cmxS->sourceComponent();
            if (local && source != xAOD::CMXJetHits::Sources::LOCAL_MAIN &&
                source != xAOD::CMXJetHits::Sources::LOCAL_FORWARD)
                continue;
            if (remote && source != xAOD::CMXJetHits::Sources::LOCAL_MAIN &&
                source != xAOD::CMXJetHits::Sources::LOCAL_FORWARD)
                continue;
            if (total && source != xAOD::CMXJetHits::Sources::TOTAL_MAIN &&
                source != xAOD::CMXJetHits::Sources::TOTAL_FORWARD)
                continue;
            if (topo && source != xAOD::CMXJetHits::Sources::TOPO_CHECKSUM &&
                source != xAOD::CMXJetHits::Sources::TOPO_OCCUPANCY_MAP &&
                source != xAOD::CMXJetHits::Sources::TOPO_OCCUPANCY_COUNTS)
                continue;
            cmxSimHit0 = cmxS->hits0();
            cmxSimHit1 = cmxS->hits1();
            crate = cmxS->crate();
            if (remote || total)
            {
                LVL1::DataError err(cmxS->error0());
                cmxSimOvf = err.get(LVL1::DataError::Overflow);
            }
        }
        else if ((cmxSimMapIter == cmxSimMapIterEnd) ||
                 ((cmxMapIter != cmxMapIterEnd) && (cmxSimKey > cmxKey)))
        {
            // Data CMX Hits but no Sim CMX Hits

            const xAOD::CMXJetHits *cmxD = cmxMapIter->second;
            ++cmxMapIter;
            source = cmxD->sourceComponent();
            if (local && source != xAOD::CMXJetHits::Sources::LOCAL_MAIN &&
                source != xAOD::CMXJetHits::Sources::LOCAL_FORWARD)
                continue;
            if (remote && source != xAOD::CMXJetHits::Sources::REMOTE_MAIN &&
                source != xAOD::CMXJetHits::Sources::REMOTE_FORWARD)
                continue;
            if (total && source != xAOD::CMXJetHits::Sources::TOTAL_MAIN &&
                source != xAOD::CMXJetHits::Sources::TOTAL_FORWARD)
                continue;
            if (topo && source != xAOD::CMXJetHits::Sources::TOPO_CHECKSUM &&
                source != xAOD::CMXJetHits::Sources::TOPO_OCCUPANCY_MAP &&
                source != xAOD::CMXJetHits::Sources::TOPO_OCCUPANCY_COUNTS)
                continue;
            cmxHit0 = cmxD->hits0();
            cmxHit1 = cmxD->hits1();
            crate = cmxD->crate();
            if (remote || total)
            {
                LVL1::DataError err(cmxD->error0());
                cmxOvf = err.get(LVL1::DataError::Overflow);
            }
        }
        else
        {
            // Have both

            const xAOD::CMXJetHits *cmxS = cmxSimMapIter->second;
            const xAOD::CMXJetHits *cmxD = cmxMapIter->second;
            ++cmxSimMapIter;
            ++cmxMapIter;
            source = cmxS->sourceComponent();
            if (local && source != xAOD::CMXJetHits::Sources::LOCAL_MAIN &&
                source != xAOD::CMXJetHits::Sources::LOCAL_FORWARD)
                continue;
            if (remote && source != xAOD::CMXJetHits::Sources::LOCAL_MAIN &&
                source != xAOD::CMXJetHits::Sources::LOCAL_FORWARD &&
                source != xAOD::CMXJetHits::Sources::REMOTE_MAIN &&
                source != xAOD::CMXJetHits::Sources::REMOTE_FORWARD)
                continue;
            if (total && source != xAOD::CMXJetHits::Sources::TOTAL_MAIN &&
                source != xAOD::CMXJetHits::Sources::TOTAL_FORWARD)
                continue;
            if (topo && source != xAOD::CMXJetHits::Sources::TOPO_CHECKSUM &&
                source != xAOD::CMXJetHits::Sources::TOPO_OCCUPANCY_MAP &&
                source != xAOD::CMXJetHits::Sources::TOPO_OCCUPANCY_COUNTS)
                continue;
            cmxSimHit0 = cmxS->hits0();
            cmxSimHit1 = cmxS->hits1();
            cmxHit0 = cmxD->hits0();
            cmxHit1 = cmxD->hits1();
            crate = cmxS->crate();
            if (remote || total)
            {
                LVL1::DataError err(cmxS->error0());
                cmxSimOvf = err.get(LVL1::DataError::Overflow);
                LVL1::DataError err2(cmxD->error0());
                cmxOvf = err2.get(LVL1::DataError::Overflow);
            }
        }

        if (!cmxSimHit0 && !cmxSimHit1 && !cmxHit0 && !cmxHit1 && !cmxSimOvf &&
            !cmxOvf)
            continue;

        //  Fill in error plots

        if (local || total)
        {
            const int cmx = 1; //<<== CHECK (and below)
            const int loc = crate * s_cmxs + cmx;
            const int cmxBins = s_crates * s_cmxs;
            const int bit =
                (local) ? (1 << LocalJetMismatch) : (1 << TotalJetMismatch);
            TH1F_LW *hist = 0;
            if (cmxSimHit0 == cmxHit0 && cmxSimHit1 == cmxHit1)
            {
                errors[loc] |= bit;
                hist = m_h_cmx_1d_thresh_SumsSimEqData;
            }
            else
            {
                errors[loc + cmxBins] |= bit;
                if ((cmxSimHit0 || cmxSimHit1) && (cmxHit0 || cmxHit1)) {
                    hist = m_h_cmx_1d_thresh_SumsSimNeData;
                }
                else if (!cmxHit0 && !cmxHit1)
                    hist = m_h_cmx_1d_thresh_SumsSimNoData;
                else
                    hist = m_h_cmx_1d_thresh_SumsDataNoSim;
            }
            const int loc1 = (total) ? 3 : crate;
            if (hist)
                hist->Fill(loc1);

            if (total && (cmxSimOvf || cmxOvf) &&
                source == xAOD::CMXJetHits::Sources::TOTAL_MAIN)
            {
                if (cmxSimOvf == cmxOvf)
                {
                    errors[loc] |= bit;
                    hist = m_h_cmx_1d_thresh_SumsOvfSimEqData;
                }
                else
                {
                    errors[loc + cmxBins] |= bit;
                    hist = m_h_cmx_1d_thresh_SumsOvfSimNeData;
                }
                if (hist)
                    hist->Fill(1);
            }

            int nThresh0 = 5;
            int nThresh1 = 5;
            int thrLen = 3;
            int offset0 = 0;
            int offset1 = 5;
            if (source == xAOD::CMXJetHits::Sources::LOCAL_FORWARD ||
                source == xAOD::CMXJetHits::Sources::TOTAL_FORWARD)
            {
                nThresh0 = 8;
                nThresh1 = 7;
                thrLen = 2;
                offset0 = 10;
                offset1 = 18;
            }
            int same =
                m_histTool->thresholdsSame(cmxHit0, cmxSimHit0, nThresh0, thrLen);
            int diff =
                m_histTool->thresholdsDiff(cmxHit0, cmxSimHit0, nThresh0, thrLen);
            m_histTool->fillXVsThresholds(m_h_cmx_2d_thresh_SumsThreshSimEqData, loc1,
                                          same, nThresh0, 1, offset0);
            m_histTool->fillXVsThresholds(m_h_cmx_2d_thresh_SumsThreshSimNeData, loc1,
                                          diff, nThresh0, 1, offset0);
            same = m_histTool->thresholdsSame(cmxHit1, cmxSimHit1, nThresh1, thrLen);
            diff = m_histTool->thresholdsDiff(cmxHit1, cmxSimHit1, nThresh1, thrLen);
            m_histTool->fillXVsThresholds(m_h_cmx_2d_thresh_SumsThreshSimEqData, loc1,
                                          same, nThresh1, 1, offset1);
            m_histTool->fillXVsThresholds(m_h_cmx_2d_thresh_SumsThreshSimNeData, loc1,
                                          diff, nThresh1, 1, offset1);
        }
        else if (topo)
        {
            const int cmx = 1;
            const int loc = crate * s_cmxs + cmx;
            const int cmxBins = s_crates * s_cmxs;
            const int bit = (1 << CMXJetTopoMismatch);
            TH2F_LW *hist = 0;
            if (cmxSimHit0 == cmxHit0 && cmxSimHit1 == cmxHit1)
            {
                errors[loc] |= bit;
                hist = m_h_cmx_2d_topo_SimEqData;
            }
            else
            {
                errors[loc + cmxBins] |= bit;
                if ((cmxSimHit0 || cmxSimHit1) && (cmxHit0 || cmxHit1))
                    hist = m_h_cmx_2d_topo_SimNeData;
                else if (!cmxHit0 && !cmxHit1)
                    hist = m_h_cmx_2d_topo_SimNoData;
                else
                    hist = m_h_cmx_2d_topo_DataNoSim;
            }
            const int loc1 =
                (source == xAOD::CMXJetHits::Sources::TOPO_CHECKSUM)
                    ? 0
                    : (source == xAOD::CMXJetHits::Sources::TOPO_OCCUPANCY_MAP) ? 1
                                                                                : 2;
            if (hist)
                hist->Fill(crate, loc1);
        }
        else
        {
            if (source == xAOD::CMXJetHits::Sources::LOCAL_MAIN)
            {
                if (crate == 0)
                {
                    hitsSimMain0 = cmxSimHit0;
                    hitsSimMain1 = cmxSimHit1;
                    overflowSim = cmxSimOvf;
                }
            }
            else if (source == xAOD::CMXJetHits::Sources::LOCAL_FORWARD)
            {
                if (crate == 0)
                {
                    hitsSimFwd0 = cmxSimHit0;
                    hitsSimFwd1 = cmxSimHit1;
                }
            }
            else if (source == xAOD::CMXJetHits::Sources::REMOTE_MAIN)
            {
                hitsDatMain0 = cmxHit0;
                hitsDatMain1 = cmxHit1;
                overflowDat = cmxOvf;
            }
            else
            {
                hitsDatFwd0 = cmxHit0;
                hitsDatFwd1 = cmxHit1;
            }
        }
    }
    if (remote)
    {
        const int crate = 1;
        const int cmx = 1;
        const int loc = crate * s_cmxs + cmx;
        const int cmxBins = s_crates * s_cmxs;
        const int bit = (1 << RemoteJetMismatch);
        const int loc1 = 2;

        TH1F_LW *hist = 0;
        if (hitsSimMain0 || hitsSimMain1 || hitsDatMain0 || hitsDatMain1)
        {
            if (hitsSimMain0 == hitsDatMain0 && hitsSimMain1 == hitsDatMain1)
            {
                errors[loc] |= bit;
                hist = m_h_cmx_1d_thresh_SumsSimEqData;
            }
            else
            {
                errors[loc + cmxBins] |= bit;
                if ((hitsSimMain0 || hitsSimMain1) && (hitsDatMain0 || hitsDatMain1))
                    hist = m_h_cmx_1d_thresh_SumsSimNeData;
                else if (!hitsDatMain0 && !hitsDatMain1)
                    hist = m_h_cmx_1d_thresh_SumsSimNoData;
                else
                    hist = m_h_cmx_1d_thresh_SumsDataNoSim;
            }
            if (hist)
                hist->Fill(loc1);
        }
        if (hitsSimFwd0 || hitsSimFwd1 || hitsDatFwd0 || hitsDatFwd1)
        {
            hist = 0;
            if (hitsSimFwd0 == hitsDatFwd0 && hitsSimFwd1 == hitsDatFwd1)
            {
                errors[loc] |= bit;
                hist = m_h_cmx_1d_thresh_SumsSimEqData;
            }
            else
            {
                errors[loc + cmxBins] |= bit;
                if ((hitsSimFwd0 || hitsSimFwd1) && (hitsDatFwd0 || hitsDatFwd1))
                    hist = m_h_cmx_1d_thresh_SumsSimNeData;
                else if (!hitsDatFwd0 && !hitsDatFwd1)
                    hist = m_h_cmx_1d_thresh_SumsSimNoData;
                else
                    hist = m_h_cmx_1d_thresh_SumsDataNoSim;
            }
            if (hist)
                hist->Fill(loc1);
        }
        if (overflowSim || overflowDat)
        {
            hist = 0;
            if (overflowSim == overflowDat)
            {
                errors[loc] |= bit;
                hist = m_h_cmx_1d_thresh_SumsOvfSimEqData;
            }
            else
            {
                errors[loc + cmxBins] |= bit;
                hist = m_h_cmx_1d_thresh_SumsOvfSimNeData;
            }
            if (hist)
                hist->Fill(0);
        }

        const int mainLen = 3;
        const int fwdLen = 2;
        int nThresh = 5;
        int offset = nThresh;
        int same = m_histTool->thresholdsSame(hitsDatMain0, hitsSimMain0, nThresh,
                                              mainLen);
        int diff = m_histTool->thresholdsDiff(hitsDatMain0, hitsSimMain0, nThresh,
                                              mainLen);
        m_histTool->fillXVsThresholds(m_h_cmx_2d_thresh_SumsThreshSimEqData, loc1,
                                      same, nThresh, 1);
        m_histTool->fillXVsThresholds(m_h_cmx_2d_thresh_SumsThreshSimNeData, loc1,
                                      diff, nThresh, 1);
        same = m_histTool->thresholdsSame(hitsDatMain1, hitsSimMain1, nThresh,
                                          mainLen);
        diff = m_histTool->thresholdsDiff(hitsDatMain1, hitsSimMain1, nThresh,
                                          mainLen);
        m_histTool->fillXVsThresholds(m_h_cmx_2d_thresh_SumsThreshSimEqData, loc1,
                                      same, nThresh, 1, offset);
        m_histTool->fillXVsThresholds(m_h_cmx_2d_thresh_SumsThreshSimNeData, loc1,
                                      diff, nThresh, 1, offset);
        offset += nThresh;
        nThresh = 8;
        same =
            m_histTool->thresholdsSame(hitsDatFwd0, hitsSimFwd0, nThresh, fwdLen);
        diff =
            m_histTool->thresholdsDiff(hitsDatFwd0, hitsSimFwd0, nThresh, fwdLen);
        m_histTool->fillXVsThresholds(m_h_cmx_2d_thresh_SumsThreshSimEqData, loc1,
                                      same, nThresh, 1, offset);
        m_histTool->fillXVsThresholds(m_h_cmx_2d_thresh_SumsThreshSimNeData, loc1,
                                      diff, nThresh, 1, offset);
        offset += nThresh;
        nThresh = 7;
        same =
            m_histTool->thresholdsSame(hitsDatFwd1, hitsSimFwd1, nThresh, fwdLen);
        diff =
            m_histTool->thresholdsDiff(hitsDatFwd1, hitsSimFwd1, nThresh, fwdLen);
        m_histTool->fillXVsThresholds(m_h_cmx_2d_thresh_SumsThreshSimEqData, loc1,
                                      same, nThresh, 1, offset);
        m_histTool->fillXVsThresholds(m_h_cmx_2d_thresh_SumsThreshSimNeData, loc1,
                                      diff, nThresh, 1, offset);
    }
}

//  Compare simulated JEM Et Sums with data

void JEPSimMon::compare(const JemEtSumsMap &jemSimMap,
                        const JemEtSumsMap &jemMap, ErrorVector &errors)
{
    if (m_debug)
        msg(MSG::DEBUG) << "Compare simulated JEM Et Sums with data" << endmsg;

    JemEtSumsMap::const_iterator simMapIter = jemSimMap.begin();
    JemEtSumsMap::const_iterator simMapIterEnd = jemSimMap.end();
    JemEtSumsMap::const_iterator datMapIter = jemMap.begin();
    JemEtSumsMap::const_iterator datMapIterEnd = jemMap.end();

    while (simMapIter != simMapIterEnd || datMapIter != datMapIterEnd)
    {
        int simKey = 0;
        int datKey = 0;
        unsigned int simEt = 0;
        unsigned int simEx = 0;
        unsigned int simEy = 0;
        unsigned int datEt = 0;
        unsigned int datEx = 0;
        unsigned int datEy = 0;
        int crate = 0;
        int jem = 0;

        if (simMapIter != simMapIterEnd)
            simKey = simMapIter->first;
        if (datMapIter != datMapIterEnd)
            datKey = datMapIter->first;

        if ((datMapIter == datMapIterEnd) ||
            ((simMapIter != simMapIterEnd) && (datKey > simKey)))
        {
            // Simulation EtSums but no data EtSums

            const xAOD::JEMEtSums *sime = simMapIter->second;
            simEt = sime->et();
            simEx = sime->ex();
            simEy = sime->ey();
            crate = sime->crate();
            jem = sime->module();
            ++simMapIter;
        }
        else if ((simMapIter == simMapIterEnd) ||
                 ((datMapIter != datMapIterEnd) && (simKey > datKey)))
        {
            // Data EtSums but no simulation EtSums

            const xAOD::JEMEtSums *date = datMapIter->second;
            datEt = date->et();
            datEx = date->ex();
            datEy = date->ey();
            crate = date->crate();
            jem = date->module();
            ++datMapIter;
        }
        else
        {
            // Have both

            const xAOD::JEMEtSums *sime = simMapIter->second;
            const xAOD::JEMEtSums *date = datMapIter->second;
            simEt = sime->et();
            simEx = sime->ex();
            simEy = sime->ey();
            datEt = date->et();
            datEx = date->ex();
            datEy = date->ey();
            crate = date->crate();
            jem = date->module();
            ++simMapIter;
            ++datMapIter;
        }

        if (!simEt && !simEx && !simEy && !datEt && !datEx && !datEy)
            continue;

        //  Fill in error vector

        const int loc = crate * s_modules + jem;
        const int jemBins = s_crates * s_modules;
        const int bit = (1 << JEMEtSumsMismatch);
        if (simEt == datEt && simEx == datEx && simEy == datEy)
            errors[loc] |= bit;
        if (simEt != datEt || simEx != datEx || simEy != datEy)
            errors[loc + jemBins] |= bit;
        if (simEt != datEt || simEx != datEx || simEy != datEy)
        {
            ATH_MSG_VERBOSE("EtSums Mismatch Crate/JEM, Data Et/Ex/Ey, Sim Et/Ex/Ey: "
                            << crate << "/" << jem << ", " << datEt << "/" << datEx
                            << "/" << datEy << ", " << simEt << "/" << simEx << "/"
                            << simEy);
        }
        TH2F_LW *hist = 0;
        if (simEx && simEx == datEx)
            hist = m_h_jem_2d_energy_SimEqData;
        else if (simEx != datEx)
        {
            if (simEx && datEx)
                hist = m_h_jem_2d_energy_SimNeData;
            else if (!datEx)
                hist = m_h_jem_2d_energy_SimNoData;
            else
                hist = m_h_jem_2d_energy_DataNoSim;
        }
        if (hist)
            hist->Fill(loc, 0);
        hist = 0;
        if (simEy && simEy == datEy)
            hist = m_h_jem_2d_energy_SimEqData;
        else if (simEy != datEy)
        {
            if (simEy && datEy)
                hist = m_h_jem_2d_energy_SimNeData;
            else if (!datEy)
                hist = m_h_jem_2d_energy_SimNoData;
            else
                hist = m_h_jem_2d_energy_DataNoSim;
        }
        if (hist)
            hist->Fill(loc, 1);
        hist = 0;
        if (simEt && simEt == datEt)
            hist = m_h_jem_2d_energy_SimEqData;
        else if (simEt != datEt)
        {
            if (simEt && datEt)
                hist = m_h_jem_2d_energy_SimNeData;
            else if (!datEt)
                hist = m_h_jem_2d_energy_SimNoData;
            else
                hist = m_h_jem_2d_energy_DataNoSim;
        }
        if (hist)
            hist->Fill(loc, 2);
    }
}

//  Compare JEM EtSums and CMX EtSums

void JEPSimMon::compare(const JemEtSumsMap &jemMap, const CmxEtSumsMap &cmxMap,
                        ErrorVector &errorsJEM, ErrorVector &errorsCMX)
{
    if (m_debug)
        msg(MSG::DEBUG) << "Compare JEM EtSums and CMX EtSums" << endmsg;

    JemEtSumsMap::const_iterator jemMapIter = jemMap.begin();
    JemEtSumsMap::const_iterator jemMapIterEnd = jemMap.end();
    CmxEtSumsMap::const_iterator cmxMapIter = cmxMap.begin();
    CmxEtSumsMap::const_iterator cmxMapIterEnd = cmxMap.end();

    while (jemMapIter != jemMapIterEnd || cmxMapIter != cmxMapIterEnd)
    {
        int jemKey = 0;
        int cmxKey = 0;
        unsigned int jemEt = 0;
        unsigned int jemEx = 0;
        unsigned int jemEy = 0;
        unsigned int cmxEt = 0;
        unsigned int cmxEx = 0;
        unsigned int cmxEy = 0;
        int crate = 0;
        int jem = 0;

        if (jemMapIter != jemMapIterEnd)
            jemKey = jemMapIter->first;
        if (cmxMapIter != cmxMapIterEnd)
            cmxKey = cmxMapIter->first;

        if ((cmxMapIter == cmxMapIterEnd) ||
            ((jemMapIter != jemMapIterEnd) && (cmxKey > jemKey)))
        {
            // JEM EtSums but no CMX EtSums

            const xAOD::JEMEtSums *jeme = jemMapIter->second;
            jemEt = jeme->et();
            jemEx = jeme->ex();
            jemEy = jeme->ey();
            crate = jeme->crate();
            jem = jeme->module();
            ++jemMapIter;
        }
        else if ((jemMapIter == jemMapIterEnd) ||
                 ((cmxMapIter != cmxMapIterEnd) && (jemKey > cmxKey)))
        {
            // CMX EtSums but no JEM EtSums

            const xAOD::CMXEtSums *cmxe = cmxMapIter->second;
            cmxEt = cmxe->et();
            cmxEx = cmxe->ex();
            cmxEy = cmxe->ey();
            crate = cmxe->crate();
            jem = cmxe->sourceComponent();
            ++cmxMapIter;
            if (jem > 15)
                continue;
        }
        else
        {
            // Have both

            const xAOD::JEMEtSums *jeme = jemMapIter->second;
            const xAOD::CMXEtSums *cmxe = cmxMapIter->second;
            jemEt = jeme->et();
            jemEx = jeme->ex();
            jemEy = jeme->ey();
            cmxEt = cmxe->et();
            cmxEx = cmxe->ex();
            cmxEy = cmxe->ey();
            crate = jeme->crate();
            jem = jeme->module();
            ++jemMapIter;
            ++cmxMapIter;
        }

        if (!jemEt && !jemEx && !jemEy && !cmxEt && !cmxEx && !cmxEy)
            continue;

        //  Fill in error vectors

        const int cmx = 0; // CMX-Energy is left ==> cmx 0 << CHECK
        const int loc = crate * s_modules + jem;
        const int loc2 = crate * s_cmxs + cmx;
        const int jemBins = s_crates * s_modules;
        const int cmxBins = s_crates * s_cmxs;
        const int bit = (1 << CMXEtSumsMismatch);
        if (jemEt == cmxEt && jemEx == cmxEx && jemEy == cmxEy)
        {
            errorsJEM[loc] |= bit;
            errorsCMX[loc2] |= bit;
        }
        else
        {
            errorsJEM[loc + jemBins] |= bit;
            errorsCMX[loc2 + cmxBins] |= bit;
        }
        TH2F_LW *hist = 0;
        if (jemEx && jemEx == cmxEx)
            hist = m_h_cmx_2d_energy_JemEqCmx;
        else if (jemEx != cmxEx)
        {
            if (jemEx && cmxEx)
                hist = m_h_cmx_2d_energy_JemNeCmx;
            else if (!cmxEx)
                hist = m_h_cmx_2d_energy_JemNoCmx;
            else
                hist = m_h_cmx_2d_energy_CmxNoJem;
        }
        if (hist)
            hist->Fill(loc, 0);
        hist = 0;
        if (jemEy && jemEy == cmxEy)
            hist = m_h_cmx_2d_energy_JemEqCmx;
        else if (jemEy != cmxEy)
        {
            if (jemEy && cmxEy)
                hist = m_h_cmx_2d_energy_JemNeCmx;
            else if (!cmxEy)
                hist = m_h_cmx_2d_energy_JemNoCmx;
            else
                hist = m_h_cmx_2d_energy_CmxNoJem;
        }
        if (hist)
            hist->Fill(loc, 1);
        hist = 0;
        if (jemEt && jemEt == cmxEt)
            hist = m_h_cmx_2d_energy_JemEqCmx;
        else if (jemEt != cmxEt)
        {
            if (jemEt && cmxEt)
                hist = m_h_cmx_2d_energy_JemNeCmx;
            else if (!cmxEt)
                hist = m_h_cmx_2d_energy_JemNoCmx;
            else
                hist = m_h_cmx_2d_energy_CmxNoJem;
        }
        if (hist)
            hist->Fill(loc, 2);
    }
}

//  Compare Simulated CMX EtSums and Data CMX EtSums

void JEPSimMon::compare(const CmxEtSumsMap &cmxSimMap,
                        const CmxEtSumsMap &cmxMap, ErrorVector &errors,
                        int selection)
{
    if (m_debug)
    {
        msg(MSG::DEBUG) << "Compare Simulated CMX EtSums and Data CMX EtSums"
                        << endmsg;
    }

    const bool local = (selection == xAOD::CMXEtSums::Sources::LOCAL_STANDARD);
    const bool remote = (selection == xAOD::CMXEtSums::Sources::REMOTE_STANDARD);
    const bool total = (selection == xAOD::CMXEtSums::Sources::TOTAL_STANDARD);
    const bool etmaps = (selection == xAOD::CMXEtSums::Sources::SUM_ET_STANDARD);
    if (!local && !remote && !total && !etmaps)
        return;

    unsigned int localEt = 0;
    unsigned int localEx = 0;
    unsigned int localEy = 0;
    unsigned int localEtM = 0;
    unsigned int localExM = 0;
    unsigned int localEyM = 0;
    unsigned int remoteEt = 0;
    unsigned int remoteEx = 0;
    unsigned int remoteEy = 0;
    unsigned int remoteEtM = 0;
    unsigned int remoteExM = 0;
    unsigned int remoteEyM = 0;
    CmxEtSumsMap::const_iterator cmxSimMapIter = cmxSimMap.begin();
    CmxEtSumsMap::const_iterator cmxSimMapIterEnd = cmxSimMap.end();
    CmxEtSumsMap::const_iterator cmxMapIter = cmxMap.begin();
    CmxEtSumsMap::const_iterator cmxMapIterEnd = cmxMap.end();

    while (cmxSimMapIter != cmxSimMapIterEnd || cmxMapIter != cmxMapIterEnd)
    {
        int cmxSimKey = 0;
        int cmxKey = 0;
        unsigned int cmxSimEt = 0;
        unsigned int cmxSimEx = 0;
        unsigned int cmxSimEy = 0;
        unsigned int cmxEt = 0;
        unsigned int cmxEx = 0;
        unsigned int cmxEy = 0;
        int crate = 0;
        int source = 0;

        if (cmxSimMapIter != cmxSimMapIterEnd)
            cmxSimKey = cmxSimMapIter->first;
        if (cmxMapIter != cmxMapIterEnd)
            cmxKey = cmxMapIter->first;

        if ((cmxMapIter == cmxMapIterEnd) ||
            ((cmxSimMapIter != cmxSimMapIterEnd) && (cmxKey > cmxSimKey)))
        {
            // Sim CMX EtSums but no Data CMX EtSums
            const xAOD::CMXEtSums *cmxS = cmxSimMapIter->second;
            ++cmxSimMapIter;
            source = cmxS->sourceComponent();
            if (local && source != xAOD::CMXEtSums::Sources::LOCAL_STANDARD &&
                source != xAOD::CMXEtSums::Sources::LOCAL_RESTRICTED)
                continue;
            if (remote && source != xAOD::CMXEtSums::Sources::LOCAL_STANDARD &&
                source != xAOD::CMXEtSums::Sources::LOCAL_RESTRICTED)
                continue;
            if (total && source != xAOD::CMXEtSums::Sources::TOTAL_STANDARD &&
                source != xAOD::CMXEtSums::Sources::TOTAL_RESTRICTED)
                continue;
            if (etmaps && source != xAOD::CMXEtSums::Sources::SUM_ET_STANDARD &&
                source != xAOD::CMXEtSums::Sources::SUM_ET_RESTRICTED && 
                source != xAOD::CMXEtSums::Sources::MISSING_ET_STANDARD &&
                source != xAOD::CMXEtSums::Sources::MISSING_ET_RESTRICTED 
                /*&& source != xAOD::CMXEtSums::Sources::MISSING_ET_SIG_STANDARD*/)
                continue;
            cmxSimEt = cmxS->et();
            cmxSimEx = cmxS->ex();
            cmxSimEy = cmxS->ey();
            if (!etmaps)
            { // include overflow bit in test
                cmxSimEt |= ((cmxS->etError() & 0x1) << 15);
                cmxSimEx |= ((cmxS->exError() & 0x1) << 15);
                cmxSimEy |= ((cmxS->eyError() & 0x1) << 15);
            }
            crate = cmxS->crate();
        }
        else if ((cmxSimMapIter == cmxSimMapIterEnd) ||
                 ((cmxMapIter != cmxMapIterEnd) && (cmxSimKey > cmxKey)))
        {
            // Data CMX EtSums but no Sim CMX EtSums

            const xAOD::CMXEtSums *cmxD = cmxMapIter->second;
            ++cmxMapIter;
            source = cmxD->sourceComponent();
            if (local && source != xAOD::CMXEtSums::Sources::LOCAL_STANDARD &&
                source != xAOD::CMXEtSums::Sources::LOCAL_RESTRICTED)
                continue;
            if (remote && source != xAOD::CMXEtSums::Sources::REMOTE_STANDARD &&
                source != xAOD::CMXEtSums::Sources::REMOTE_RESTRICTED)
                continue;
            if (total && source != xAOD::CMXEtSums::Sources::TOTAL_STANDARD &&
                source != xAOD::CMXEtSums::Sources::TOTAL_RESTRICTED)
                continue;
            if (etmaps && source != xAOD::CMXEtSums::Sources::SUM_ET_STANDARD &&
                source != xAOD::CMXEtSums::Sources::SUM_ET_RESTRICTED &&
                source != xAOD::CMXEtSums::Sources::MISSING_ET_STANDARD &&
                source != xAOD::CMXEtSums::Sources::MISSING_ET_RESTRICTED
                /* && source != xAOD::CMXEtSums::Sources::MISSING_ET_SIG_STANDARD*/)
                continue;
            cmxEt = cmxD->et();
            cmxEx = cmxD->ex();
            cmxEy = cmxD->ey();
            if (!etmaps)
            {
                cmxEt |= ((cmxD->etError() & 0x1) << 15);
                cmxEx |= ((cmxD->exError() & 0x1) << 15);
                cmxEy |= ((cmxD->eyError() & 0x1) << 15);
            }
            crate = cmxD->crate();
        }
        else
        {
            // Have both

            const xAOD::CMXEtSums *cmxS = cmxSimMapIter->second;
            const xAOD::CMXEtSums *cmxD = cmxMapIter->second;
            ++cmxSimMapIter;
            ++cmxMapIter;
            source = cmxS->sourceComponent();
            if (local && source != xAOD::CMXEtSums::Sources::LOCAL_STANDARD &&
                source != xAOD::CMXEtSums::Sources::LOCAL_RESTRICTED)
                continue;
            if (remote && source != xAOD::CMXEtSums::Sources::LOCAL_STANDARD &&
                source != xAOD::CMXEtSums::Sources::LOCAL_RESTRICTED &&
                source != xAOD::CMXEtSums::Sources::REMOTE_STANDARD &&
                source != xAOD::CMXEtSums::Sources::REMOTE_RESTRICTED)
                continue;
            if (total && source != xAOD::CMXEtSums::Sources::TOTAL_STANDARD &&
                source != xAOD::CMXEtSums::Sources::TOTAL_RESTRICTED)
                continue;
            if (etmaps && source != xAOD::CMXEtSums::Sources::SUM_ET_STANDARD &&
                source != xAOD::CMXEtSums::Sources::SUM_ET_RESTRICTED &&
                source != xAOD::CMXEtSums::Sources::MISSING_ET_STANDARD &&
                source != xAOD::CMXEtSums::Sources::MISSING_ET_RESTRICTED 
                /*&& source != xAOD::CMXEtSums::Sources::MISSING_ET_SIG_STANDARD*/)
                continue;
            cmxSimEt = cmxS->et();
            cmxSimEx = cmxS->ex();
            cmxSimEy = cmxS->ey();
            cmxEt = cmxD->et();
            cmxEx = cmxD->ex();
            cmxEy = cmxD->ey();
            if (!etmaps)
            {
                if (source == xAOD::CMXEtSums::Sources::LOCAL_RESTRICTED || source == xAOD::CMXEtSums::Sources::TOTAL_RESTRICTED)
                {
                  // Take error bits from data!
                  cmxSimEt |= ((cmxD->etError() & 0x1) << 15);
                  cmxSimEx |= ((cmxD->exError() & 0x1) << 15);
                  cmxSimEy |= ((cmxD->eyError() & 0x1) << 15);
                } else {
                  cmxSimEt |= ((cmxS->etError() & 0x1) << 15);
                  cmxSimEx |= ((cmxS->exError() & 0x1) << 15);
                  cmxSimEy |= ((cmxS->eyError() & 0x1) << 15);
                }

                cmxEt |= ((cmxD->etError() & 0x1) << 15);
                cmxEx |= ((cmxD->exError() & 0x1) << 15);
                cmxEy |= ((cmxD->eyError() & 0x1) << 15);
            }
            crate = cmxS->crate();
        }

        if (!cmxSimEt && !cmxSimEx && !cmxSimEy && !cmxEt && !cmxEx && !cmxEy)
            continue;

        //  Fill in error vectors

        if (local || total || etmaps)
        {
            const int cmx = 0; //<< CHECK
            const int loc = crate * s_cmxs + cmx;
            const int cmxBins = s_crates * s_cmxs;
            const int bit =
                (local)
                    ? (1 << LocalEnergyMismatch)
                    : (total)
                          ? (1 << TotalEnergyMismatch)
                          : (source == xAOD::CMXEtSums::Sources::SUM_ET_STANDARD ||
                             source == xAOD::CMXEtSums::Sources::SUM_ET_RESTRICTED)
                                ? (1 << SumEtMismatch)
                                : (source == xAOD::CMXEtSums::Sources::
                                                 MISSING_ET_STANDARD ||
                                   source == xAOD::CMXEtSums::Sources::
                                                 MISSING_ET_RESTRICTED)
                                      ? (1 << MissingEtMismatch)
                                      : (1 << MissingEtSigMismatch);
            const int offset =
                (source == xAOD::CMXEtSums::Sources::LOCAL_RESTRICTED ||
                 source == xAOD::CMXEtSums::Sources::REMOTE_RESTRICTED ||
                 source == xAOD::CMXEtSums::Sources::TOTAL_RESTRICTED ||
                 source == xAOD::CMXEtSums::Sources::SUM_ET_RESTRICTED ||
                 source == xAOD::CMXEtSums::Sources::MISSING_ET_RESTRICTED)
                    ? 6
                    : 0;
            if (cmxSimEt == cmxEt && cmxSimEx == cmxEx && cmxSimEy == cmxEy)
            {
                errors[loc] |= bit;
            }
            else
                errors[loc + cmxBins] |= bit;
            const int loc1 = (local) ? crate : 3;
            if (local || total)
            {
                TH2F_LW *hist = 0;
                if (cmxSimEx && cmxSimEx == cmxEx)
                    hist = m_h_cmx_2d_energy_SumsSimEqData;
                else if (cmxSimEx != cmxEx)
                {
                    if (cmxSimEx && cmxEx)
                        hist = m_h_cmx_2d_energy_SumsSimNeData;
                    else if (!cmxEx)
                        hist = m_h_cmx_2d_energy_SumsSimNoData;
                    else
                        hist = m_h_cmx_2d_energy_SumsDataNoSim;
                }
                if (hist)
                    hist->Fill(loc1, 0 + offset);
                hist = 0;
                if (cmxSimEy && cmxSimEy == cmxEy)
                    hist = m_h_cmx_2d_energy_SumsSimEqData;
                else if (cmxSimEy != cmxEy)
                {
                    if (cmxSimEy && cmxEy)
                        hist = m_h_cmx_2d_energy_SumsSimNeData;
                    else if (!cmxEy)
                        hist = m_h_cmx_2d_energy_SumsSimNoData;
                    else
                        hist = m_h_cmx_2d_energy_SumsDataNoSim;
                }
                if (hist)
                    hist->Fill(loc1, 1 + offset);
                hist = 0;
                if (cmxSimEt && cmxSimEt == cmxEt)
                    hist = m_h_cmx_2d_energy_SumsSimEqData;
                else if (cmxSimEt != cmxEt)
                {
                    if (cmxSimEt && cmxEt)
                        hist = m_h_cmx_2d_energy_SumsSimNeData;
                    else if (!cmxEt)
                        hist = m_h_cmx_2d_energy_SumsSimNoData;
                    else
                        hist = m_h_cmx_2d_energy_SumsDataNoSim;
                }
                if (hist)
                    hist->Fill(loc1, 2 + offset);
            }
            else
            {
                const int loc2 =
                    (source == xAOD::CMXEtSums::Sources::SUM_ET_STANDARD ||
                     source == xAOD::CMXEtSums::Sources::SUM_ET_RESTRICTED)
                        ? 3
                        : (source == xAOD::CMXEtSums::Sources::MISSING_ET_STANDARD ||
                           source == xAOD::CMXEtSums::Sources::MISSING_ET_RESTRICTED)
                              ? 4
                              : 5;
                TH2F_LW *hist = 0;
                if (cmxSimEt && cmxSimEt == cmxEt)
                    hist = m_h_cmx_2d_energy_SumsSimEqData;
                else if (cmxSimEt != cmxEt)
                {
                    if (cmxSimEt && cmxEt)
                        hist = m_h_cmx_2d_energy_SumsSimNeData;
                    else if (!cmxEt)
                        hist = m_h_cmx_2d_energy_SumsSimNoData;
                    else
                        hist = m_h_cmx_2d_energy_SumsDataNoSim;
                }
                if (hist)
                    hist->Fill(loc1, loc2 + offset);
                if (cmxSimEt || cmxEt)
                {
                    int loc3 = 0;
                    int nThresh = 8;
                    int offset2 = 0;
                    if (source == xAOD::CMXEtSums::Sources::MISSING_ET_STANDARD ||
                        source == xAOD::CMXEtSums::Sources::MISSING_ET_RESTRICTED)
                    {
                        loc3 = 2;
                        offset2 = 8;
                    }
                    else if (source ==
                             xAOD::CMXEtSums::Sources::MISSING_ET_SIG_STANDARD)
                    {
                        loc3 = 4;
                        offset2 = 16;
                    }
                    m_histTool->fillXVsThresholds(m_h_cmx_2d_energy_EtMapsThreshSimEqData,
                                                  loc3 + offset, cmxEt & cmxSimEt,
                                                  nThresh, 1, offset2);
                    m_histTool->fillXVsThresholds(m_h_cmx_2d_energy_EtMapsThreshSimNeData,
                                                  loc3 + offset, cmxEt ^ cmxSimEt,
                                                  nThresh, 1, offset2);
                }
            }
        }
        else
        {
            if (source == xAOD::CMXEtSums::Sources::LOCAL_STANDARD)
            {
                if (crate == 0)
                {
                    localEt = cmxSimEt;
                    localEx = cmxSimEx;
                    localEy = cmxSimEy;
                }
            }
            else if (source == xAOD::CMXEtSums::Sources::LOCAL_RESTRICTED)
            {
                if (crate == 0)
                {
                    localEtM = cmxSimEt;
                    localExM = cmxSimEx;
                    localEyM = cmxSimEy;
                }
            }
            else if (source == xAOD::CMXEtSums::Sources::REMOTE_STANDARD)
            {
                remoteEt = cmxEt;
                remoteEx = cmxEx;
                remoteEy = cmxEy;
            }
            else
            {
                remoteEtM = cmxEt;
                remoteExM = cmxEx;
                remoteEyM = cmxEy;
            }
        }
    }
    if (remote && (localEt || localEx || localEy || localEtM || localExM ||
                   localEyM || remoteEt || remoteEx || remoteEy || remoteEtM ||
                   remoteExM || remoteEyM))
    {
        const int crate = 1;
        const int cmx = 0;
        const int loc = crate * s_cmxs + cmx;
        const int cmxBins = s_crates * s_cmxs;
        const int bit = (1 << RemoteEnergyMismatch);
        if (localEt == remoteEt && localEx == remoteEx && localEy == remoteEy &&
            localEtM == remoteEtM && localExM == remoteExM &&
            localEyM == remoteEyM)
        {
            errors[loc] |= bit;
        }
        else
            errors[loc + cmxBins] |= bit;
        TH2F_LW *hist = 0;
        if (localEx && localEx == remoteEx)
            hist = m_h_cmx_2d_energy_SumsSimEqData;
        else if (localEx != remoteEx)
        {
            if (localEx && remoteEx)
                hist = m_h_cmx_2d_energy_SumsSimNeData;
            else if (!remoteEx)
                hist = m_h_cmx_2d_energy_SumsSimNoData;
            else
                hist = m_h_cmx_2d_energy_SumsDataNoSim;
        }
        if (hist)
            hist->Fill(2, 0);
        hist = 0;
        if (localEy && localEy == remoteEy)
            hist = m_h_cmx_2d_energy_SumsSimEqData;
        else if (localEy != remoteEy)
        {
            if (localEy && remoteEy)
                hist = m_h_cmx_2d_energy_SumsSimNeData;
            else if (!remoteEy)
                hist = m_h_cmx_2d_energy_SumsSimNoData;
            else
                hist = m_h_cmx_2d_energy_SumsDataNoSim;
        }
        if (hist)
            hist->Fill(2, 1);
        hist = 0;
        if (localEt && localEt == remoteEt)
            hist = m_h_cmx_2d_energy_SumsSimEqData;
        else if (localEt != remoteEt)
        {
            if (localEt && remoteEt)
                hist = m_h_cmx_2d_energy_SumsSimNeData;
            else if (!remoteEt)
                hist = m_h_cmx_2d_energy_SumsSimNoData;
            else
                hist = m_h_cmx_2d_energy_SumsDataNoSim;
        }
        if (hist)
            hist->Fill(2, 2);
        hist = 0;
        if (localExM && localExM == remoteExM)
            hist = m_h_cmx_2d_energy_SumsSimEqData;
        else if (localExM != remoteExM)
        {
            if (localExM && remoteExM)
                hist = m_h_cmx_2d_energy_SumsSimNeData;
            else if (!remoteExM)
                hist = m_h_cmx_2d_energy_SumsSimNoData;
            else
                hist = m_h_cmx_2d_energy_SumsDataNoSim;
        }
        if (hist)
            hist->Fill(2, 6);
        hist = 0;
        if (localEyM && localEyM == remoteEyM)
            hist = m_h_cmx_2d_energy_SumsSimEqData;
        else if (localEyM != remoteEyM)
        {
            if (localEyM && remoteEyM)
                hist = m_h_cmx_2d_energy_SumsSimNeData;
            else if (!remoteEyM)
                hist = m_h_cmx_2d_energy_SumsSimNoData;
            else
                hist = m_h_cmx_2d_energy_SumsDataNoSim;
        }
        if (hist)
            hist->Fill(2, 7);
        hist = 0;
        if (localEtM && localEtM == remoteEtM)
            hist = m_h_cmx_2d_energy_SumsSimEqData;
        else if (localEtM != remoteEtM)
        {
            if (localEtM && remoteEtM)
                hist = m_h_cmx_2d_energy_SumsSimNeData;
            else if (!remoteEtM)
                hist = m_h_cmx_2d_energy_SumsSimNoData;
            else
                hist = m_h_cmx_2d_energy_SumsDataNoSim;
        }
        if (hist)
            hist->Fill(2, 8);
    }
}

// Compare Et Maps and Energy Totals with Energy RoIs from data

void JEPSimMon::compare(const CmxEtSumsMap &cmxMap, const xAOD::CMXRoI *cmxRoi,
                        ErrorVector &errors)
{
  if (m_debug) {
    msg(MSG::DEBUG) << "Compare Et Maps and Energy Totals with RoIs from data"
                    << endmsg;
  }

  int sumEtMap = 0;
  int missEtMap = 0;
  int missEtSigMap = 0;
  int et = 0;
  int ex = 0;
  int ey = 0;
  int sumEtRoi = 0;
  int missEtRoi = 0;
  int missEtSigRoi = 0;
  int etRoi = 0;
  int exRoi = 0;
  int eyRoi = 0;
  int sumEtMapM = 0;
  int missEtMapM = 0;
  int etM = 0;
  int exM = 0;
  int eyM = 0;
  int sumEtRoiM = 0;
  int missEtRoiM = 0;
  int etRoiM = 0;
  int exRoiM = 0;
  int eyRoiM = 0;
  int key = 100 + xAOD::CMXEtSums::Sources::SUM_ET_STANDARD;
  CmxEtSumsMap::const_iterator iter = cmxMap.find(key);
  if (iter != cmxMap.end()) {
    const xAOD::CMXEtSums *sums = iter->second;
    sumEtMap = sums->et();
  }
  key = 100 + xAOD::CMXEtSums::Sources::MISSING_ET_STANDARD;
  iter = cmxMap.find(key);
  if (iter != cmxMap.end()) {
    const xAOD::CMXEtSums *sums = iter->second;
    missEtMap = sums->et();
  }
  key = 100 + xAOD::CMXEtSums::Sources::MISSING_ET_SIG_STANDARD;
  iter = cmxMap.find(key);
  if (iter != cmxMap.end()) {
    const xAOD::CMXEtSums *sums = iter->second;
    missEtSigMap = sums->et();
  }
  key = 100 + xAOD::CMXEtSums::Sources::TOTAL_STANDARD;
  iter = cmxMap.find(key);
  if (iter != cmxMap.end()) {
    const xAOD::CMXEtSums *sums = iter->second;
    et = (sums->et() | ((sums->etError() & 0x1) << 15));
    ex = (sums->ex() | ((sums->exError() & 0x1) << 15));
    ey = (sums->ey() | ((sums->eyError() & 0x1) << 15));
  }
  key = 100 + xAOD::CMXEtSums::Sources::SUM_ET_RESTRICTED;
  iter = cmxMap.find(key);
  if (iter != cmxMap.end()) {
    const xAOD::CMXEtSums *sums = iter->second;
    sumEtMapM = sums->et();
  }
  key = 100 + xAOD::CMXEtSums::Sources::MISSING_ET_RESTRICTED;
  iter = cmxMap.find(key);
  if (iter != cmxMap.end()) {
    const xAOD::CMXEtSums *sums = iter->second;
    missEtMapM = sums->et();
  }
  key = 100 + xAOD::CMXEtSums::Sources::TOTAL_RESTRICTED;
  iter = cmxMap.find(key);
  if (iter != cmxMap.end()) {
    const xAOD::CMXEtSums *sums = iter->second;
    etM = (sums->et() | ((sums->etError() & 0x1) << 15));
    exM = (sums->ex() | ((sums->exError() & 0x1) << 15));
    eyM = (sums->ey() | ((sums->eyError() & 0x1) << 15));

  }
  
  
  if (cmxRoi) {
    sumEtRoi = cmxRoi->sumEtHits();
    missEtRoi = cmxRoi->missingEtHits();
    missEtSigRoi = cmxRoi->missingEtSigHits();
    etRoi = (cmxRoi->et() | ((cmxRoi->etError() & 0x1) << 15));
    exRoi = (cmxRoi->ex() | ((cmxRoi->exError() & 0x1) << 15));
    eyRoi = (cmxRoi->ey() | ((cmxRoi->eyError() & 0x1) << 15));
    sumEtRoiM = cmxRoi->sumEtHits(xAOD::CMXRoI::MASKED);
    missEtRoiM = cmxRoi->missingEtHits(xAOD::CMXRoI::MASKED);
    etRoiM =
        (cmxRoi->et(xAOD::CMXRoI::MASKED) | ((cmxRoi->etError(xAOD::CMXRoI::MASKED) & 0x1) << 15));

    exRoiM =
        (cmxRoi->ex(xAOD::CMXRoI::MASKED) | ((cmxRoi->exError(xAOD::CMXRoI::MASKED) & 0x1) << 15));
    eyRoiM =
        (cmxRoi->ey(xAOD::CMXRoI::MASKED) | ((cmxRoi->eyError(xAOD::CMXRoI::MASKED) & 0x1) << 15));
  
    // If CMX RoI restricted et is zero and overflow bit is set, but total restriced et is
    // zero then explicitly set overflow bit also in total restricted et since readout
    // currently does not set this bit in total restricted. 
    if (etM == 0 && (cmxRoi->et(xAOD::CMXRoI::MASKED) == 0)
        && (cmxRoi->etError(xAOD::CMXRoI::MASKED) & 0x1)) {
      etM = etRoiM; 
    }

    if (exM == 0 && (cmxRoi->ex(xAOD::CMXRoI::MASKED) == 0)
        && (cmxRoi->exError(xAOD::CMXRoI::MASKED) & 0x1)) {
      exM = exRoiM; 
    }

    if (eyM == 0 && (cmxRoi->ey(xAOD::CMXRoI::MASKED) == 0)
        && (cmxRoi->eyError(xAOD::CMXRoI::MASKED) & 0x1)) {
      eyM = eyRoiM; 
    }
  }

  if (sumEtMap || sumEtRoi || missEtMap || missEtRoi || missEtSigMap ||
      missEtSigRoi || et || etRoi || ex || exRoi || ey || eyRoi || sumEtMapM ||
      sumEtRoiM || missEtMapM || missEtRoiM || etM || etRoiM || exM || exRoiM ||
      eyM || eyRoiM) {
    const int crate = 1;
    const int cmx = 0;
    const int loc = crate * s_cmxs + cmx;
    const int cmxBins = s_crates * s_cmxs;
    const int bit = (1 << EnergyRoIMismatch);

    if (sumEtMap == sumEtRoi && missEtMap == missEtRoi &&
        missEtSigMap == missEtSigRoi && et == etRoi && ex == exRoi &&
        ey == eyRoi && sumEtMapM == sumEtRoiM && missEtMapM == missEtRoiM &&
        etM == etRoiM && exM == exRoiM && eyM == eyRoiM)
      errors[loc] |= bit;
    else
      errors[loc + cmxBins] |= bit;
    TH2F_LW *hist = 0;
    if (ex && ex == exRoi)
      hist = m_h_cmx_2d_energy_SumsSimEqData;
    else if (ex != exRoi) {
      if (ex && exRoi)
        hist = m_h_cmx_2d_energy_SumsSimNeData;
      else if (!exRoi) {
        if (!limitedRoiSet(crate)) hist = m_h_cmx_2d_energy_SumsSimNoData;
      } else
        hist = m_h_cmx_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(4, 0);
    hist = 0;
    if (ey && ey == eyRoi)
      hist = m_h_cmx_2d_energy_SumsSimEqData;
    else if (ey != eyRoi) {
      if (ey && eyRoi)
        hist = m_h_cmx_2d_energy_SumsSimNeData;
      else if (!eyRoi) {
        if (!limitedRoiSet(crate)) hist = m_h_cmx_2d_energy_SumsSimNoData;
      } else
        hist = m_h_cmx_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(4, 1);
    hist = 0;
    if (et && et == etRoi)
      hist = m_h_cmx_2d_energy_SumsSimEqData;
    else if (et != etRoi) {
      if (et && etRoi)
        hist = m_h_cmx_2d_energy_SumsSimNeData;
      else if (!etRoi) {
        if (!limitedRoiSet(crate)) hist = m_h_cmx_2d_energy_SumsSimNoData;
      } else
        hist = m_h_cmx_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(4, 2);
    hist = 0;
    if (sumEtMap && sumEtMap == sumEtRoi)
      hist = m_h_cmx_2d_energy_SumsSimEqData;
    else if (sumEtMap != sumEtRoi) {
      if (sumEtMap && sumEtRoi)
        hist = m_h_cmx_2d_energy_SumsSimNeData;
      else if (!sumEtRoi) {
        if (!limitedRoiSet(crate)) hist = m_h_cmx_2d_energy_SumsSimNoData;
      } else
        hist = m_h_cmx_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(4, 3);
    hist = 0;
    if (missEtMap && missEtMap == missEtRoi)
      hist = m_h_cmx_2d_energy_SumsSimEqData;
    else if (missEtMap != missEtRoi) {
      if (missEtMap && missEtRoi)
        hist = m_h_cmx_2d_energy_SumsSimNeData;
      else if (!missEtRoi) {
        if (!limitedRoiSet(crate)) hist = m_h_cmx_2d_energy_SumsSimNoData;
      } else
        hist = m_h_cmx_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(4, 4);
    hist = 0;
    if (missEtSigMap && missEtSigMap == missEtSigRoi)
      hist = m_h_cmx_2d_energy_SumsSimEqData;
    else if (missEtSigMap != missEtSigRoi) {
      if (missEtSigMap && missEtSigRoi)
        hist = m_h_cmx_2d_energy_SumsSimNeData;
      else if (!missEtSigRoi) {
        if (!limitedRoiSet(crate)) hist = m_h_cmx_2d_energy_SumsSimNoData;
      } else
        hist = m_h_cmx_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(4, 5);
    hist = 0;
    if (exM && exM == exRoiM)
      hist = m_h_cmx_2d_energy_SumsSimEqData;
    else if (exM != exRoiM) {
      if (exM && exRoiM)
        hist = m_h_cmx_2d_energy_SumsSimNeData;
      else if (!exRoiM) {
        if (!limitedRoiSet(crate)) hist = m_h_cmx_2d_energy_SumsSimNoData;
      } else
        hist = m_h_cmx_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(4, 6);
    hist = 0;
    if (eyM && eyM == eyRoiM)
      hist = m_h_cmx_2d_energy_SumsSimEqData;
    else if (eyM != eyRoiM) {
      if (eyM && eyRoiM)
        hist = m_h_cmx_2d_energy_SumsSimNeData;
      else if (!eyRoiM) {
        if (!limitedRoiSet(crate)) hist = m_h_cmx_2d_energy_SumsSimNoData;
      } else
        hist = m_h_cmx_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(4, 7);
    hist = 0;
    if (etM && etM == etRoiM)
      hist = m_h_cmx_2d_energy_SumsSimEqData;
    else if (etM != etRoiM) {
      if (etM && etRoiM)
        hist = m_h_cmx_2d_energy_SumsSimNeData;
      else if (!etRoiM) {
        if (!limitedRoiSet(crate)) hist = m_h_cmx_2d_energy_SumsSimNoData;
      } else
        hist = m_h_cmx_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(4, 8);
    hist = 0;
    if (sumEtMapM && sumEtMapM == sumEtRoiM)
      hist = m_h_cmx_2d_energy_SumsSimEqData;
    else if (sumEtMapM != sumEtRoiM) {
      if (sumEtMapM && sumEtRoiM)
        hist = m_h_cmx_2d_energy_SumsSimNeData;
      else if (!sumEtRoiM) {
        if (!limitedRoiSet(crate)) hist = m_h_cmx_2d_energy_SumsSimNoData;
      } else
        hist = m_h_cmx_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(4, 9);
    hist = 0;
    if (missEtMapM && missEtMapM == missEtRoiM)
      hist = m_h_cmx_2d_energy_SumsSimEqData;
    else if (missEtMapM != missEtRoiM) {
      if (missEtMapM && missEtRoiM)
        hist = m_h_cmx_2d_energy_SumsSimNeData;
      else if (!missEtRoiM) {
        if (!limitedRoiSet(crate)) hist = m_h_cmx_2d_energy_SumsSimNoData;
      } else
        hist = m_h_cmx_2d_energy_SumsDataNoSim;
    }
    if (hist) hist->Fill(4, 10);

    const int thrLen = 1;
    const int nThresh = 8;
    if ((sumEtMap || sumEtRoi) &&
        !(sumEtMap && !sumEtRoi && limitedRoiSet(crate))) {
      m_histTool->fillXVsThresholds(m_h_cmx_2d_energy_EtMapsThreshSimEqData, 1,
                                    sumEtRoi & sumEtMap, nThresh, thrLen);
      m_histTool->fillXVsThresholds(m_h_cmx_2d_energy_EtMapsThreshSimNeData, 1,
                                    sumEtRoi ^ sumEtMap, nThresh, thrLen);
    }
    if ((missEtMap || missEtRoi) &&
        !(missEtMap && !missEtRoi && limitedRoiSet(crate))) {
      const int offset = 8;
      m_histTool->fillXVsThresholds(m_h_cmx_2d_energy_EtMapsThreshSimEqData, 3,
                                    missEtRoi & missEtMap, nThresh, thrLen,
                                    offset);
      m_histTool->fillXVsThresholds(m_h_cmx_2d_energy_EtMapsThreshSimNeData, 3,
                                    missEtRoi ^ missEtMap, nThresh, thrLen,
                                    offset);
    }
    if ((missEtSigMap || missEtSigRoi) &&
        !(missEtSigMap && !missEtSigRoi && limitedRoiSet(crate))) {
      const int offset = 16;
      m_histTool->fillXVsThresholds(m_h_cmx_2d_energy_EtMapsThreshSimEqData, 5,
                                    missEtSigRoi & missEtSigMap, nThresh,
                                    thrLen, offset);
      m_histTool->fillXVsThresholds(m_h_cmx_2d_energy_EtMapsThreshSimNeData, 5,
                                    missEtSigRoi ^ missEtSigMap, nThresh,
                                    thrLen, offset);
    }
    if ((sumEtMapM || sumEtRoiM) &&
        !(sumEtMapM && !sumEtRoiM && limitedRoiSet(crate))) {
      m_histTool->fillXVsThresholds(m_h_cmx_2d_energy_EtMapsThreshSimEqData, 7,
                                    sumEtRoiM & sumEtMapM, nThresh, thrLen);
      m_histTool->fillXVsThresholds(m_h_cmx_2d_energy_EtMapsThreshSimNeData, 7,
                                    sumEtRoiM ^ sumEtMapM, nThresh, thrLen);
    }
    if ((missEtMapM || missEtRoiM) &&
        !(missEtMapM && !missEtRoiM && limitedRoiSet(crate))) {
      const int offset = 8;
      m_histTool->fillXVsThresholds(m_h_cmx_2d_energy_EtMapsThreshSimEqData, 9,
                                    missEtRoiM & missEtMapM, nThresh, thrLen,
                                    offset);
      m_histTool->fillXVsThresholds(m_h_cmx_2d_energy_EtMapsThreshSimNeData, 9,
                                    missEtRoiM ^ missEtMapM, nThresh, thrLen,
                                    offset);
    }
  }
}

void JEPSimMon::fillEventSample(int err, int loc, bool isJem)
{
    int hist = 0;
    int y = 0;
    if (isJem)
    {
        hist = (err < 4) ? err : err - 4;
        y = loc;
    }
    else
    {
        hist = (err < 8) ? 6 : 7;
        y = loc / s_cmxs; // crate number
        if (err == RemoteJetMismatch)
            y = 2; // first 2 Local 0/1
        else if (err == TotalJetMismatch)
            y = 3;
        else if (err == CMXJetTopoMismatch)
            y += 4;
        else if (err == LocalEnergyMismatch)
            y += 2; // first 2 Module 0/1
        else if (err == RemoteEnergyMismatch)
            y = 4;
        else if (err == TotalEnergyMismatch)
            y = 5;
        else if (err == SumEtMismatch)
            y = 6;
        else if (err == MissingEtMismatch)
            y = 7;
        else if (err == MissingEtSigMismatch)
            y = 8;
        else if (err == EnergyRoIMismatch)
            y = 9;
    }
    if (m_v_2d_MismatchEvents[hist])
        m_histTool->fillEventNumber(m_v_2d_MismatchEvents[hist], y);
}

void JEPSimMon::setLabels(LWHist *hist, bool xAxis)
{
    // Simulation steps in red (#color[2]) depend on Trigger Menu
    LWHist::LWHistAxis *axis = (xAxis) ? hist->GetXaxis() : hist->GetYaxis();
    axis->SetBinLabel(1 + EMElementMismatch, "EM je");
    axis->SetBinLabel(1 + HadElementMismatch, "Had je");
    axis->SetBinLabel(1 + RoIMismatch, "#color[2]{RoIs}");
    axis->SetBinLabel(1 + CMXJetTobMismatch, "CMXTob");
    axis->SetBinLabel(1 + LocalJetMismatch, "#color[2]{Local}");
    axis->SetBinLabel(1 + RemoteJetMismatch, "Remote");
    axis->SetBinLabel(1 + TotalJetMismatch, "Total");
    axis->SetBinLabel(1 + CMXJetTopoMismatch, "Topo");
    axis->SetBinLabel(1 + JEMEtSumsMismatch, "JEMSums");
    axis->SetBinLabel(1 + CMXEtSumsMismatch, "CMXSums");
    axis->SetBinLabel(1 + LocalEnergyMismatch, "#color[2]{Local}");
    axis->SetBinLabel(1 + RemoteEnergyMismatch, "Remote");
    axis->SetBinLabel(1 + TotalEnergyMismatch, "Total");
    axis->SetBinLabel(1 + SumEtMismatch, "#color[2]{SumEt}");
    axis->SetBinLabel(1 + MissingEtMismatch, "#color[2]{MissingEt}");
    axis->SetBinLabel(1 + MissingEtSigMismatch, "#color[2]{MissEtSig}");
    axis->SetBinLabel(1 + EnergyRoIMismatch, "Engy RoIs");
}

void JEPSimMon::setLabelsSH(LWHist *hist)
{
    LWHist::LWHistAxis *axis = hist->GetXaxis();
    axis->SetBinLabel(1, "Local0");
    axis->SetBinLabel(2, "Local1");
    axis->SetBinLabel(3, "Remote");
    axis->SetBinLabel(4, "Total");
}

void JEPSimMon::setLabelsSHF(LWHist *hist)
{
    setLabelsSH(hist);
    m_histTool->jemThresholds(hist, 0, false);
}

void JEPSimMon::setLabelsTopo(TH2F_LW *hist)
{
    LWHist::LWHistAxis *axis = hist->GetXaxis();
    axis->SetBinLabel(1, "Crate 0");
    axis->SetBinLabel(2, "Crate 1");
    axis = hist->GetYaxis();
    axis->SetBinLabel(1, "Checksum");
    axis->SetBinLabel(2, "Map");
    axis->SetBinLabel(3, "Counts");
    axis->SetTitle("Topo Info");
}

void JEPSimMon::setLabelsEnTot(LWHist *hist)
{
    LWHist::LWHistAxis *axis = hist->GetXaxis();
    axis->SetBinLabel(1, "Local0");
    axis->SetBinLabel(2, "Local1");
    axis->SetBinLabel(3, "Remote");
    axis->SetBinLabel(4, "Total");
    axis->SetBinLabel(5, "RoI");
    axis = hist->GetYaxis();
    axis->SetBinLabel(1, "Ex");
    axis->SetBinLabel(2, "Ey");
    axis->SetBinLabel(3, "Et");
    axis->SetBinLabel(4, "SumEt");
    axis->SetBinLabel(5, "MissingEt");
    axis->SetBinLabel(6, "MissEtSig");
    axis->SetBinLabel(7, "ExM");
    axis->SetBinLabel(8, "EyM");
    axis->SetBinLabel(9, "EtM");
    axis->SetBinLabel(10, "SumEtM");
    axis->SetBinLabel(11, "MissingEtM");
}

void JEPSimMon::setLabelsEnTotThr(LWHist *hist)
{
    LWHist::LWHistAxis *axis = hist->GetXaxis();
    axis->SetBinLabel(1, "SumEt");
    axis->SetBinLabel(2, "SumEt RoI");
    axis->SetBinLabel(3, "MissingEt");
    axis->SetBinLabel(4, "MissingEt RoI");
    axis->SetBinLabel(5, "MissingEtSig");
    axis->SetBinLabel(6, "MissingEtSig RoI");
    axis->SetBinLabel(7, "SumEtM");
    axis->SetBinLabel(8, "SumEtM RoI");
    axis->SetBinLabel(9, "MissingEtM");
    axis->SetBinLabel(10, "MissingEtM RoI");
    m_histTool->sumEtThresholds(hist, 0, false);
    m_histTool->missingEtThresholds(hist, 8, false);
    m_histTool->missingEtSigThresholds(hist, 16, false);
}

void JEPSimMon::setupMap(const xAOD::JetElementContainer *coll,
                         JetElementMap &map)
{
    if (coll)
        m_jetElementTool->mapJetElements(coll, &map);
}

void JEPSimMon::setupMap(const xAOD::JEMTobRoIContainer *coll,
                         JEMTobRoIMap &map)
{
    if (coll)
    {
        xAOD::JEMTobRoIContainer::const_iterator pos = coll->begin();
        xAOD::JEMTobRoIContainer::const_iterator posE = coll->end();
        for (; pos != posE; ++pos)
        {
            const int crate = (*pos)->crate();
            const int jem = (*pos)->jem();
            const int frame = (*pos)->frame();
            const int loc = (*pos)->location();
            const int key = ((((((crate << 4) | jem) << 3) | frame) << 3) | loc);
            map.insert(std::make_pair(key, *pos));
        }
    }
}

void JEPSimMon::setupMap(const xAOD::CMXJetTobContainer *coll,
                         CmxJetTobMap &map)
{
    if (coll)
    {
        xAOD::CMXJetTobContainer::const_iterator pos = coll->begin();
        xAOD::CMXJetTobContainer::const_iterator posE = coll->end();
        for (; pos != posE; ++pos)
        {
            const int crate = (*pos)->crate();
            const int jem = (*pos)->jem();
            const int frame = (*pos)->frame();
            const int loc = (*pos)->location();
            const int key = ((((((crate << 4) | jem) << 3) | frame) << 3) | loc);
            map.insert(std::make_pair(key, *pos));
        }
    }
}

void JEPSimMon::setupMap(const xAOD::CMXJetHitsContainer *coll,
                         CmxJetHitsMap &map)
{
    if (coll)
    {
        xAOD::CMXJetHitsContainer::const_iterator pos = coll->begin();
        xAOD::CMXJetHitsContainer::const_iterator posE = coll->end();
        for (; pos != posE; ++pos)
        {
            const int crate = (*pos)->crate();
            const int source = (*pos)->sourceComponent();
            const int key = crate * 100 + source;
            map.insert(std::make_pair(key, *pos));
        }
    }
}

void JEPSimMon::setupMap(const xAOD::JEMEtSumsContainer *coll,
                         JemEtSumsMap &map)
{
    if (coll)
    {
        xAOD::JEMEtSumsContainer::const_iterator pos = coll->begin();
        xAOD::JEMEtSumsContainer::const_iterator posE = coll->end();
        for (; pos != posE; ++pos)
        {
            const int crate = (*pos)->crate();
            const int jem = (*pos)->module();
            const int key = crate * 100 + jem;
            map.insert(std::make_pair(key, *pos));
        }
    }
}

void JEPSimMon::setupMap(const xAOD::CMXEtSumsContainer *coll,
                         CmxEtSumsMap &map)
{
    if (coll)
    {
        xAOD::CMXEtSumsContainer::const_iterator pos = coll->begin();
        xAOD::CMXEtSumsContainer::const_iterator posE = coll->end();
        for (; pos != posE; ++pos)
        {
            const int crate = (*pos)->crate();
            const int source = (*pos)->sourceComponent();
            const int key = crate * 100 + source;
            map.insert(std::make_pair(key, *pos));
        }
    }
}

void JEPSimMon::simulate(const xAOD::TriggerTowerContainer *towers,
                         xAOD::JetElementContainer *elements)
{
    if (m_debug)
    {
        msg(MSG::DEBUG) << "Simulate Jet Elements from Trigger Towers" << endmsg;
    }

    // Make zero-suppressed collection to speed up simulation

    xAOD::TriggerTowerContainer *towersZ = new xAOD::TriggerTowerContainer;
    xAOD::TriggerTowerAuxContainer *towersZAux =
        new xAOD::TriggerTowerAuxContainer;
    towersZ->setStore(towersZAux);

    xAOD::TriggerTowerContainer::const_iterator pos = towers->begin();
    xAOD::TriggerTowerContainer::const_iterator posE = towers->end();
    for (; pos != posE; ++pos)
    {
        if ((*pos)->cpET() > 0 || (*pos)->jepET() > 0)
        {
            xAOD::TriggerTower *tt = new xAOD::TriggerTower;
            *tt = **pos;
            towersZ->push_back(tt);
        }
    }
    m_jetElementTool->makeJetElements(towersZ, elements);
    delete towersZ;
    delete towersZAux;
}

void JEPSimMon::simulate(const xAOD::JetElementContainer *elements,
                         const xAOD::JetElementContainer *elementsOv,
                         xAOD::JEMTobRoIContainer *rois)
{
    ATH_MSG_DEBUG("Simulate JEM RoIs from Jet Elements");

    // Process a crate at a time to use overlap data
    const int ncrates = 2;
    std::vector<xAOD::JetElementContainer *> crateColl;

    for (int crate = 0; crate < ncrates; ++crate)
    {
        crateColl.push_back(new xAOD::JetElementContainer(SG::VIEW_ELEMENTS));
    }

    LVL1::CoordToHardware converter;
    xAOD::JetElementContainer::const_iterator iter;
    xAOD::JetElementContainer::const_iterator iterE;
    if (elements)
    { // core data
        iter = elements->begin();
        iterE = elements->end();
        for (; iter != iterE; ++iter)
        {
            xAOD::JetElementContainer::value_type je = *iter;
            const LVL1::Coordinate coord(je->phi(), je->eta());
            const int crate = converter.jepCrate(coord);
            if (crate < ncrates)
                crateColl[crate]->push_back(je);
        }
    }
    if (elementsOv)
    { // overlap data
        iter = elementsOv->begin();
        iterE = elementsOv->end();
        for (; iter != iterE; ++iter)
        {
            xAOD::JetElementContainer::value_type je = *iter;
            const LVL1::Coordinate coord(je->phi(), je->eta());
            const int crate = converter.jepCrateOverlap(coord);
            if (crate < ncrates)
                crateColl[crate]->push_back(je);
        }
    }
    else if (elements)
    { // take overlap from core
        iter = elements->begin();
        iterE = elements->end();
        for (; iter != iterE; ++iter)
        {
            xAOD::JetElementContainer::value_type je = *iter;
            const LVL1::Coordinate coord(je->phi(), je->eta());
            const int crate = converter.jepCrateOverlap(coord);
            if (crate < ncrates)
                crateColl[crate]->push_back(je);
        }
    }
    for (int crate = 0; crate < ncrates; ++crate)
    {
        InternalRoiCollection *intRois = new InternalRoiCollection;
        m_jetTool->findRoIs(crateColl[crate], intRois);

        for (auto it = intRois->begin(); it != intRois->end(); ++it)
        {
            xAOD::JEMTobRoI *roi = (*it)->jemTobRoI();
            if (roi->crate() == crate)
            {
                rois->push_back(roi);
            }
            else
            {
                delete roi;
            }
        }

        // xAOD::JEMTobRoIContainer* roiTemp = new xAOD::JEMTobRoIContainer;

        // m_jetCmxTool->formJEMTobRoI(intRois, roiTemp);
        // xAOD::JEMTobRoIContainer::iterator roiIter  = roiTemp->begin();
        // xAOD::JEMTobRoIContainer::iterator roiIterE = roiTemp->end();
        // for (; roiIter != roiIterE; ++roiIter) {
        //   if ((*roiIter)->crate() == crate) {
        //     rois->push_back(*roiIter);
        //   }
        // }
        delete intRois;
        // delete roiTemp;
        delete crateColl[crate];
    }
}

// Quicker version when core and overlap the same

void JEPSimMon::simulate(const xAOD::JetElementContainer *elements,
                         xAOD::JEMTobRoIContainer *rois)
{
    if (m_debug)
    {
        msg(MSG::DEBUG) << "Simulate JEM RoIs from Jet Elements" << endmsg;
    }

    InternalRoiCollection *intRois = new InternalRoiCollection;
    if (elements)
    {
        m_jetTool->findRoIs(elements, intRois);
        for (auto it = intRois->begin(); it != intRois->end(); ++it)
        {
            rois->push_back((*it)->jemTobRoI());
        }
    }
    delete intRois;
}

void JEPSimMon::simulate(const xAOD::JEMTobRoIContainer *rois,
                         xAOD::CMXJetTobContainer *tobs)
{
    if (m_debug)
        msg(MSG::DEBUG) << "Simulate CMX-Jet TOBs from JEM RoIs" << endmsg;
    m_jetCmxTool->formCMXJetTob(rois, tobs);
}

void JEPSimMon::simulate(const xAOD::CMXJetTobContainer *tobs,
                         xAOD::CMXJetHitsContainer *hits, int selection)
{
    if (m_debug)
        msg(MSG::DEBUG) << "Simulate CMX-Jet Hit sums from CMX-Jet TOBs" << endmsg;

    if (selection == xAOD::CMXJetHits::Sources::LOCAL_MAIN)
    {
        m_jetCmxTool->formCMXJetHitsCrate(tobs, hits);
    }
    else if (selection == xAOD::CMXJetHits::Sources::TOPO_CHECKSUM)
    {
        m_jetCmxTool->formCMXJetHitsTopo(tobs, hits);
    }
}

void JEPSimMon::simulate(const xAOD::CMXJetHitsContainer *hitsIn,
                         xAOD::CMXJetHitsContainer *hitsOut)
{
    if (m_debug)
        msg(MSG::DEBUG) << "Simulate CMX Total Hit sums from Remote/Local"
                        << endmsg;

    m_jetCmxTool->formCMXJetHitsSystem(hitsIn, hitsOut);
}

void JEPSimMon::simulate(const xAOD::JetElementContainer *elements,
                         xAOD::JEMEtSumsContainer *sums)
{
    if (m_debug)
        msg(MSG::DEBUG) << "Simulate JEM EtSums from JetElements" << endmsg;

    m_energyCmxTool->formJEMEtSums(elements, sums);
}

void JEPSimMon::simulate(const xAOD::CMXEtSumsContainer *sumsIn,
                         xAOD::CMXEtSumsContainer *sumsOut, int selection)
{
    if (m_debug)
    {
        msg(MSG::DEBUG) << "Simulate CMX-Energy Total sums from CMX-Energy Sums"
                        << endmsg;
    }

    if (selection == xAOD::CMXEtSums::Sources::LOCAL_STANDARD)
    {
        m_energyCmxTool->formCMXEtSumsCrate(sumsIn, sumsOut);
    }
    else if (selection == xAOD::CMXEtSums::Sources::TOTAL_STANDARD)
    {
        m_energyCmxTool->formCMXEtSumsSystem(sumsIn, sumsOut);
    }
    else if (selection == xAOD::CMXEtSums::Sources::SUM_ET_STANDARD)
    {
        m_energyCmxTool->formCMXEtSumsEtMaps(sumsIn, sumsOut);
    }
}

// Check if LimitedRoISet bit set

bool JEPSimMon::limitedRoiSet(int crate)
{
    if (m_rodTES)
        loadRodHeaders();
    return (((m_limitedRoi >> crate) & 0x1) == 1);
}

// Load ROD Headers

void JEPSimMon::loadRodHeaders()
{
    if (m_rodTES)
    {
        m_limitedRoi = 0;
        xAOD::RODHeaderContainer::const_iterator rodIter = m_rodTES->begin();
        xAOD::RODHeaderContainer::const_iterator rodIterE = m_rodTES->end();
        for (; rodIter != rodIterE; ++rodIter)
        {
            const xAOD::RODHeader *rod = *rodIter;
            const int rodCrate = rod->crate() - 12;
            if (rodCrate >= 0 && rodCrate < s_crates && rod->dataType() == 1)
            {
                if (rod->limitedRoISet())
                {
                    m_limitedRoi |= (1 << rodCrate);
                }
            }
        }
        m_rodTES = 0;
    }
}
// ============================================================================
} // end namespace
// ============================================================================
