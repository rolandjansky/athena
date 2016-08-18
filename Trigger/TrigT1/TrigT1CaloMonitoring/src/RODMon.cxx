/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     RODMon.cxx
// PACKAGE:  TrigT1CaloMonitoring
//
// AUTHOR:   Peter Faulkner
//
//
// ********************************************************************

#include "LWHists/LWHist.h"
#include "LWHists/TH1F_LW.h"
#include "LWHists/TH2F_LW.h"
#include "LWHists/TH2I_LW.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "SGTools/StlVectorClids.h"

#include "EventInfo/EventInfo.h"
#include "EventInfo/TriggerInfo.h"

#include "AthenaMonitoring/AthenaMonManager.h"

#include "xAODTrigL1Calo/RODHeaderContainer.h"
#include "TrigT1Interfaces/TrigT1CaloDefs.h"
#include "TrigT1CaloMonitoringTools/ITrigT1CaloMonErrorTool.h"
#include "TrigT1CaloMonitoringTools/TrigT1CaloLWHistogramTool.h"

#include "RODMon.h"
// ============================================================================
namespace LVL1 {
// ============================================================================

RODMon::RODMon(const std::string & type, const std::string & name,
               const IInterface* parent)
  : ManagedMonitorToolBase(type, name, parent),
    m_errorTool("LVL1::TrigT1CaloMonErrorTool/TrigT1CaloMonErrorTool"),
    m_histTool("LVL1::TrigT1CaloLWHistogramTool/TrigT1CaloLWHistogramTool"),
    m_events(0),
    m_histBooked(false),
    m_h_rod_1d_PpPayload(0),
    m_h_rod_1d_CpPayload(0),
    m_h_rod_1d_JepPayload(0),
    m_h_rod_1d_CpJepRoiPayload(0),
    m_h_rod_2d_PpStatus(0),
    m_h_rod_2d_CpJepStatus(0),
    m_h_rod_2d_CpJepRoiStatus(0),
    m_h_rod_2d_PpRobStatusGeneric(0),
    m_h_rod_2d_CpJepRobStatusGeneric(0),
    m_h_rod_2d_CpJepRoiRobStatusGeneric(0),
    m_h_rod_2d_PpRobStatusSpecific(0),
    m_h_rod_2d_CpJepRobStatusSpecific(0),
    m_h_rod_2d_CpJepRoiRobStatusSpecific(0),
    m_h_rod_1d_EvtStatusGeneric(0),
    m_h_rod_1d_EvtStatusSpecific(0),
    m_h_rod_2d_PpUnpack(0),
    m_h_rod_2d_CpJepUnpack(0),
    m_h_rod_2d_CpJepRoiUnpack(0),
    m_h_rod_1d_ErrorSummary(0),
    m_h_rod_1d_RobErrorSummary(0),
    m_h_rod_1d_UnpackErrorSummary(0),
    m_h_rod_2d_ErrorEventNumbers(0),
    m_h_rod_2d_RobErrorEventNumbers(0),
    m_h_rod_2d_EvtErrorEventNumbers(0),
    m_h_rod_2d_UnpackErrorEventNumbers(0)
    /*---------------------------------------------------------*/
{

  declareProperty("ErrorTool", m_errorTool);
  declareProperty("HistogramTool", m_histTool);

  declareProperty("RodHeaderLocation",
                  m_rodHeaderLocation = LVL1::TrigT1CaloDefs::RODHeaderLocation);
  declareProperty("RodHeaderLocationCPRoIB",
                  m_cpRoibRodHeaderLocation = LVL1::TrigT1CaloDefs::RODHeaderLocation + "CPRoIB");
  declareProperty("RodHeaderLocationJEPRoIB",
                  m_jepRoibRodHeaderLocation = LVL1::TrigT1CaloDefs::RODHeaderLocation + "JEPRoIB");
  declareProperty("ErrorLocation",
                  m_errorLocation = "L1CaloRODErrorVector");

  declareProperty("RootDirectory", m_rootDir = "L1Calo/ROD");
  declareProperty("OnlineTest", m_onlineTest = false,
                  "Test online code when running offline");

}

/*---------------------------------------------------------*/
RODMon::~RODMon()
/*---------------------------------------------------------*/
{
}

#ifndef PACKAGE_VERSION
#define PACKAGE_VERSION "unknown"
#endif

/*---------------------------------------------------------*/
StatusCode RODMon:: initialize()
/*---------------------------------------------------------*/
{
  msg(MSG::INFO) << "Initializing " << name() << " - package version "
                 << PACKAGE_VERSION << endmsg;

  StatusCode sc;

  sc = ManagedMonitorToolBase::initialize();
  if (sc.isFailure()) return sc;

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
StatusCode RODMon::bookHistogramsRecurrent()
/*---------------------------------------------------------*/
{
  msg(MSG::DEBUG) << "bookHistograms entered" << endmsg;

  bool online = m_onlineTest;
  if ( m_environment == AthenaMonManager::online ) {
    // book histograms that are only made in the online environment...
    online = true;
  }

  if ( m_dataType == AthenaMonManager::cosmics ) {
    // book histograms that are only relevant for cosmics data...
  }

  //if ( newEventsBlock || newLumiBlock ) { }

  if ( newRun ) {

    MgmtAttr_t attr = ATTRIB_UNMANAGED;

    MonGroup monShift ( this, m_rootDir, run, attr );
    MonGroup monExpert( this, m_rootDir, run, attr );
    MonGroup monEvents( this, m_rootDir, run, attr, "", "eventSample" );
    MonGroup monAverage( this, m_rootDir, run, attr, "", "weightedAverage" );
    MonGroup monROB( this, m_rootDir + "/ROBStatus", run, attr );
    MonGroup monROBEvents( this, m_rootDir + "/ROBStatus", run, attr, "",
                           "eventSample" );
    MonGroup monUnpack( this, m_rootDir + "/Unpacking", run, attr );
    MonGroup monUnpackEvents( this, m_rootDir + "/Unpacking", run, attr, "",
                              "eventSample" );

    //  Payload Averages

    m_histTool->setMonGroup(&monAverage);

    m_sumPayloads1.clear();
    m_sumPayloads1.resize(80, 0.);
    m_sumPayloads2.clear();
    m_sumPayloads2.resize(160, 0.);
    m_events = 0;
    std::string axisTitles = (online)
                             ? ";Complete Run | Recent Events        Crate/S-Link;Words per Event"
                             : ";Crate/S-Link;Words per Event";
    int nbins = (online) ? 65 : 32;
    m_h_rod_1d_PpPayload = m_histTool->book1F("rod_1d_PpPayload",
                           "ROD PP Average Payload Size" + axisTitles,
                           nbins, 0, nbins);
    m_histTool->numberPairs(m_h_rod_1d_PpPayload, 0, 7, 0, 3, 2);
    if (online) {
      m_h_rod_1d_PpPayload->GetXaxis()->SetBinLabel(33, "---");
      m_histTool->numberPairs(m_h_rod_1d_PpPayload, 0, 7, 0, 3, 2, 33);
    }
    nbins = (online) ? 17 : 8;
    m_h_rod_1d_CpPayload = m_histTool->book1F("rod_1d_CpPayload",
                           "ROD CP Average Payload Size" + axisTitles,
                           nbins, 0, nbins);
    m_histTool->numberPairs2(m_h_rod_1d_CpPayload, 0, 3, 0, 3, 2);
    if (online) {
      m_h_rod_1d_CpPayload->GetXaxis()->SetBinLabel(9, "---");
      m_histTool->numberPairs2(m_h_rod_1d_CpPayload, 0, 3, 0, 3, 2, 9);
    }
    m_h_rod_1d_JepPayload = m_histTool->book1F("rod_1d_JepPayload",
                            "ROD JEP Average Payload Size" + axisTitles,
                            nbins, 0, nbins);
    m_histTool->numberPairs(m_h_rod_1d_JepPayload, 0, 1, 0, 3);
    if (online) {
      m_h_rod_1d_JepPayload->GetXaxis()->SetBinLabel(9, "---");
      m_histTool->numberPairs(m_h_rod_1d_JepPayload, 0, 1, 0, 3, 1, 9);
    }
    nbins = (online) ? 25 : 12;
    m_h_rod_1d_CpJepRoiPayload = m_histTool->book1F("rod_1d_CpJepRoiPayload",
                                 "ROD CP and JEP RoI Average Payload Size" + axisTitles,
                                 nbins, 0, nbins);
    m_histTool->numberPairs2(m_h_rod_1d_CpJepRoiPayload, 0, 3, 0, 3, 2);
    m_h_rod_1d_CpJepRoiPayload->GetXaxis()->SetBinLabel(1, "CP");
    m_histTool->numberPairs2(m_h_rod_1d_CpJepRoiPayload, 0, 1, 0, 3, 2, 8);
    m_h_rod_1d_CpJepRoiPayload->GetXaxis()->SetBinLabel(9, "JEP");
    if (online) {
      m_h_rod_1d_CpJepRoiPayload->GetXaxis()->SetBinLabel(13, "---");
      m_histTool->numberPairs2(m_h_rod_1d_CpJepRoiPayload, 0, 3, 0, 3, 2, 13);
      m_h_rod_1d_CpJepRoiPayload->GetXaxis()->SetBinLabel(14, "CP");
      m_histTool->numberPairs2(m_h_rod_1d_CpJepRoiPayload, 0, 1, 0, 3, 2, 21);
      m_h_rod_1d_CpJepRoiPayload->GetXaxis()->SetBinLabel(22, "JEP");
    }

    //  Status bits

    m_histTool->setMonGroup(&monExpert);

    m_h_rod_2d_PpStatus = m_histTool->book2F("rod_2d_PpStatus",
                          "PP ROD Status Bits and Payload Check;;Crate/S-Link",
                          NumberOfStatusBins, 0, NumberOfStatusBins,
                          32, 0, 32);
    setLabelsStatus(m_h_rod_2d_PpStatus);
    m_histTool->numberPairs(m_h_rod_2d_PpStatus, 0, 7, 0, 3, 2, 0, false);
    m_h_rod_2d_PpStatus->GetXaxis()->SetBinLabel(1 + NoPayload, "No Payload");
    m_h_rod_2d_CpJepStatus = m_histTool->book2F("rod_2d_CpJepStatus",
                             "CP and JEP ROD Status Bits and Payload Check;;Crate/S-Link",
                             NumberOfStatusBins, 0, NumberOfStatusBins,
                             16, 0, 16);
    setLabelsStatus(m_h_rod_2d_CpJepStatus);
    m_histTool->numberPairs2(m_h_rod_2d_CpJepStatus, 0, 3, 0, 3, 2, 0, false);
    m_h_rod_2d_CpJepStatus->GetYaxis()->SetBinLabel(1, "CP 0/0");
    m_histTool->numberPairs(m_h_rod_2d_CpJepStatus, 0, 1, 0, 3, 1, 8, false);
    m_h_rod_2d_CpJepStatus->GetYaxis()->SetBinLabel(9, "JEP 0/0");
    m_h_rod_2d_CpJepStatus->GetXaxis()->SetBinLabel(1 + NoPayload, "No Payload");
    m_h_rod_2d_CpJepRoiStatus = m_histTool->book2F("rod_2d_CpJepRoiStatus",
                                "CP and JEP RoI ROD Status Bits;;Crate/S-Link",
                                NumberOfStatusBins, 0, NumberOfStatusBins,
                                12, 0, 12);
    setLabelsStatus(m_h_rod_2d_CpJepRoiStatus);
    m_histTool->numberPairs2(m_h_rod_2d_CpJepRoiStatus, 0, 3, 0, 3, 2, 0, false);
    m_h_rod_2d_CpJepRoiStatus->GetYaxis()->SetBinLabel(1, "CP 0/0");
    m_histTool->numberPairs2(m_h_rod_2d_CpJepRoiStatus, 0, 1, 0, 3, 2, 8, false);
    m_h_rod_2d_CpJepRoiStatus->GetYaxis()->SetBinLabel(9, "JEP 0/0");

    //  ROB Status bits

    m_histTool->setMonGroup(&monROB);

    m_h_rod_2d_PpRobStatusGeneric = m_histTool->book2F("rod_2d_PpRobStatusGeneric",
                                    "PP ROB Status Bits Generic Field;;Crate/S-Link",
                                    16, 0, 16, 32, 0, 32);
    setLabelsROBStatusGen(m_h_rod_2d_PpRobStatusGeneric);
    m_histTool->numberPairs(m_h_rod_2d_PpRobStatusGeneric, 0, 7, 0, 3, 2, 0, false);
    m_h_rod_2d_CpJepRobStatusGeneric = m_histTool->book2F("rod_2d_CpJepRobStatusGeneric",
                                       "CP and JEP ROB Status Bits Generic Field;;Crate/S-Link",
                                       16, 0, 16, 16, 0, 16);
    setLabelsROBStatusGen(m_h_rod_2d_CpJepRobStatusGeneric);
    m_histTool->numberPairs2(m_h_rod_2d_CpJepRobStatusGeneric, 0, 3, 0, 3, 2, 0, false);
    m_h_rod_2d_CpJepRobStatusGeneric->GetYaxis()->SetBinLabel(1, "CP 0/0");
    m_histTool->numberPairs(m_h_rod_2d_CpJepRobStatusGeneric, 0, 1, 0, 3, 1, 8, false);
    m_h_rod_2d_CpJepRobStatusGeneric->GetYaxis()->SetBinLabel(9, "JEP 0/0");
    m_h_rod_2d_CpJepRoiRobStatusGeneric = m_histTool->book2F("rod_2d_CpJepRoiRobStatusGeneric",
                                          "CP and JEP RoI ROB Status Bits Generic Field;;Crate/S-Link",
                                          16, 0, 16, 12, 0, 12);
    setLabelsROBStatusGen(m_h_rod_2d_CpJepRoiRobStatusGeneric);
    m_histTool->numberPairs2(m_h_rod_2d_CpJepRoiRobStatusGeneric, 0, 3, 0, 3, 2, 0, false);
    m_h_rod_2d_CpJepRoiRobStatusGeneric->GetYaxis()->SetBinLabel(1, "CP 0/0");
    m_histTool->numberPairs2(m_h_rod_2d_CpJepRoiRobStatusGeneric, 0, 1, 0, 3, 2, 8, false);
    m_h_rod_2d_CpJepRoiRobStatusGeneric->GetYaxis()->SetBinLabel(9, "JEP 0/0");

    m_h_rod_2d_PpRobStatusSpecific = m_histTool->book2F("rod_2d_PpRobStatusSpecific",
                                     "PP ROB Status Bits Specific Field;;Crate/S-Link",
                                     16, 0, 16, 32, 0, 32);
    setLabelsROBStatusSpec(m_h_rod_2d_PpRobStatusSpecific);
    m_histTool->numberPairs(m_h_rod_2d_PpRobStatusSpecific, 0, 7, 0, 3, 2, 0, false);
    m_h_rod_2d_CpJepRobStatusSpecific = m_histTool->book2F("rod_2d_CpJepRobStatusSpecific",
                                        "CP and JEP ROB Status Bits Specific Field;;Crate/S-Link",
                                        16, 0, 16, 16, 0, 16);
    setLabelsROBStatusSpec(m_h_rod_2d_CpJepRobStatusSpecific);
    m_histTool->numberPairs2(m_h_rod_2d_CpJepRobStatusSpecific, 0, 3, 0, 3, 2, 0, false);
    m_h_rod_2d_CpJepRobStatusSpecific->GetYaxis()->SetBinLabel(1, "CP 0/0");
    m_histTool->numberPairs(m_h_rod_2d_CpJepRobStatusSpecific, 0, 1, 0, 3, 1, 8, false);
    m_h_rod_2d_CpJepRobStatusSpecific->GetYaxis()->SetBinLabel(9, "JEP 0/0");
    m_h_rod_2d_CpJepRoiRobStatusSpecific = m_histTool->book2F("rod_2d_CpJepRoiRobStatusSpecific",
                                           "CP and JEP RoI ROB Status Bits Specific Field;;Crate/S-Link",
                                           16, 0, 16, 12, 0, 12);
    setLabelsROBStatusSpec(m_h_rod_2d_CpJepRoiRobStatusSpecific);
    m_histTool->numberPairs2(m_h_rod_2d_CpJepRoiRobStatusSpecific, 0, 3, 0, 3, 2, 0, false);
    m_h_rod_2d_CpJepRoiRobStatusSpecific->GetYaxis()->SetBinLabel(1, "CP 0/0");
    m_histTool->numberPairs2(m_h_rod_2d_CpJepRoiRobStatusSpecific, 0, 1, 0, 3, 2, 8, false);
    m_h_rod_2d_CpJepRoiRobStatusSpecific->GetYaxis()->SetBinLabel(9, "JEP 0/0");

    // Event Status bits

    m_h_rod_1d_EvtStatusGeneric = m_histTool->book1F("rod_1d_EvtStatusGeneric",
                                  "Full Event Status Bits Generic Field", 16, 0, 16);
    setLabelsROBStatusGen(m_h_rod_1d_EvtStatusGeneric);
    m_h_rod_1d_EvtStatusSpecific = m_histTool->book1F("rod_1d_EvtStatusSpecific",
                                   "Full Event Status Bits Specific Field", 16, 16, 32);
    setLabelsEvtStatusSpec(m_h_rod_1d_EvtStatusSpecific);

    //  Unpacking Errors

    m_histTool->setMonGroup(&monUnpack);

    const unsigned int numUnpErr = 21;
    m_h_rod_2d_PpUnpack = m_histTool->book2F("rod_2d_PpUnpack",
                          "PP Bytestream Unpacking Errors;;Crate/S-Link",
                          numUnpErr, 1, numUnpErr + 1, 32, 0, 32);
    setLabelsUnpacking(m_h_rod_2d_PpUnpack);
    m_histTool->numberPairs(m_h_rod_2d_PpUnpack, 0, 7, 0, 3, 2, 0, false);
    m_h_rod_2d_CpJepUnpack = m_histTool->book2F("rod_2d_CpJepUnpack",
                             "CP and JEP Bytestream Unpacking Errors;;Crate/S-Link",
                             numUnpErr, 1, numUnpErr + 1, 16, 0, 16);
    setLabelsUnpacking(m_h_rod_2d_CpJepUnpack);
    m_histTool->numberPairs2(m_h_rod_2d_CpJepUnpack, 0, 3, 0, 3, 2, 0, false);
    m_h_rod_2d_CpJepUnpack->GetYaxis()->SetBinLabel(1, "CP 0/0");
    m_histTool->numberPairs(m_h_rod_2d_CpJepUnpack, 0, 1, 0, 3, 1, 8, false);
    m_h_rod_2d_CpJepUnpack->GetYaxis()->SetBinLabel(9, "JEP 0/0");
    m_h_rod_2d_CpJepRoiUnpack = m_histTool->book2F("rod_2d_CpJepRoiUnpack",
                                "CP and JEP RoI Bytestream Unpacking Errors;;Crate/S-Link",
                                numUnpErr, 1, numUnpErr + 1, 12, 0, 12);
    setLabelsUnpacking(m_h_rod_2d_CpJepRoiUnpack);
    m_histTool->numberPairs2(m_h_rod_2d_CpJepRoiUnpack, 0, 3, 0, 3, 2, 0, false);
    m_h_rod_2d_CpJepRoiUnpack->GetYaxis()->SetBinLabel(1, "CP 0/0");
    m_histTool->numberPairs2(m_h_rod_2d_CpJepRoiUnpack, 0, 1, 0, 3, 2, 8, false);
    m_h_rod_2d_CpJepRoiUnpack->GetYaxis()->SetBinLabel(9, "JEP 0/0");

    //  Error summary

    m_histTool->setMonGroup(&monShift);

    m_h_rod_1d_ErrorSummary = m_histTool->book1F("rod_1d_ErrorSummary",
                              "ROD Error Summary;;Events",
                              NumberOfStatusBins, 0, NumberOfStatusBins);
    setLabelsStatus(m_h_rod_1d_ErrorSummary);
    m_h_rod_1d_ErrorSummary->GetXaxis()->SetBinLabel(1 + TriggerType, "");
    m_h_rod_1d_ErrorSummary->GetXaxis()->SetBinLabel(1 + LimitedRoI,  "");
    m_h_rod_1d_ErrorSummary->GetXaxis()->SetBinLabel(1 + NoPayload, "No Payload");
    m_h_rod_1d_RobErrorSummary = m_histTool->book1F("rod_1d_RobErrorSummary",
                                 "ROB Status Error Summary;;Events", 7, 0, 7);
    setLabelsROBStatusGen(m_h_rod_1d_RobErrorSummary);
    m_h_rod_1d_RobErrorSummary->GetXaxis()->SetBinLabel(6, "OtherGeneric");
    m_h_rod_1d_RobErrorSummary->GetXaxis()->SetBinLabel(7, "Specific");
    m_h_rod_1d_UnpackErrorSummary = m_histTool->book1F("rod_1d_UnpackErrorSummary",
                                    "Bytestream Unpacking Error Summary;;Events",
                                    numUnpErr, 1, numUnpErr + 1);
    setLabelsUnpacking(m_h_rod_1d_UnpackErrorSummary);

    //  Error event numbers

    m_histTool->setMonGroup(&monEvents);

    m_h_rod_2d_ErrorEventNumbers = m_histTool->bookEventNumbers("rod_2d_ErrorEventNumbers",
                                   "ROD Error Event Numbers",
                                   NumberOfStatusBins, 0, NumberOfStatusBins);
    setLabelsStatus(m_h_rod_2d_ErrorEventNumbers, false);
    m_h_rod_2d_ErrorEventNumbers->GetYaxis()->SetBinLabel(1 + TriggerType, "");
    m_h_rod_2d_ErrorEventNumbers->GetYaxis()->SetBinLabel(1 + LimitedRoI,  "");
    m_h_rod_2d_ErrorEventNumbers->GetYaxis()->SetBinLabel(1 + NoPayload,
        "#splitline{No}{Payload}");

    m_histTool->setMonGroup(&monROBEvents);

    const std::string corruptType(m_errorTool->flagCorruptEvents());
    nbins = (corruptType == "AnyROBOrUnpackingError") ? 8 : 7;
    m_h_rod_2d_RobErrorEventNumbers = m_histTool->bookEventNumbers("rod_2d_RobErrorEventNumbers",
                                      "ROB Status Error Event Numbers", nbins, 0, nbins);
    setLabelsROBStatusGen(m_h_rod_2d_RobErrorEventNumbers, false);
    m_h_rod_2d_RobErrorEventNumbers->GetYaxis()->SetBinLabel(6, "#splitline{Other}{Generic}");
    m_h_rod_2d_RobErrorEventNumbers->GetYaxis()->SetBinLabel(7, "Specific");
    if (nbins == 8) m_h_rod_2d_RobErrorEventNumbers->GetYaxis()->SetBinLabel(8,
          "#splitline{Rejected}{Events}");

    nbins = (corruptType == "FullEventTimeout") ? 8 : 7;
    m_h_rod_2d_EvtErrorEventNumbers = m_histTool->bookEventNumbers("rod_2d_EvtErrorEventNumbers",
                                      "Full Event Status Error Event Numbers", nbins, 0, nbins);
    setLabelsROBStatusGen(m_h_rod_2d_EvtErrorEventNumbers, false);
    m_h_rod_2d_EvtErrorEventNumbers->GetYaxis()->SetBinLabel(6, "#splitline{Other}{Generic}");
    m_h_rod_2d_EvtErrorEventNumbers->GetYaxis()->SetBinLabel(7, "Specific");
    if (nbins == 8) m_h_rod_2d_EvtErrorEventNumbers->GetYaxis()->SetBinLabel(8,
          "#splitline{Rejected}{Events}");

    m_histTool->setMonGroup(&monUnpackEvents);

    m_h_rod_2d_UnpackErrorEventNumbers = m_histTool->bookEventNumbers(
                                           "rod_2d_UnpackErrorEventNumbers",
                                           "Bytestream Unpacking Error Event Numbers",
                                           numUnpErr, 1, numUnpErr + 1);
    setLabelsUnpacking(m_h_rod_2d_UnpackErrorEventNumbers, false);

    m_histTool->unsetMonGroup();
    m_histBooked = true;

  } // end if (newRun ...

  msg(MSG::DEBUG) << "Leaving bookHistograms" << endmsg;

  return StatusCode::SUCCESS;
}

/*---------------------------------------------------------*/
StatusCode RODMon::fillHistograms()
/*---------------------------------------------------------*/
{
  const bool debug = msgLvl(MSG::DEBUG);
  if (debug) msg(MSG::DEBUG) << "fillHistograms entered" << endmsg;

  if (!m_histBooked) {
    if (debug) msg(MSG::DEBUG) << "Histogram(s) not booked" << endmsg;
    return StatusCode::SUCCESS;
  }

  const bool online = m_onlineTest ||
                      (m_environment == AthenaMonManager::online);

  StatusCode sc;

  // Error summary vectors
  const unsigned int numUnpErr = 19;
  std::vector<int> errors(NumberOfStatusBins);
  std::vector<int> errorsROB(7);
  std::vector<int> errorsEvt(7);
  std::vector<int> errorsUnpack(numUnpErr + 1);
  std::vector<int> crateErr(14);
  std::vector<int> robErrorFlags(80, 0);

  const bool corrupt(m_errorTool->corrupt());
  const std::string corruptType(m_errorTool->flagCorruptEvents());

  // Update ROB Status and Unpacking Errors

  if ( !corrupt || corruptType == "AnyROBOrUnpackingError" ) {

    //Retrieve ROB and Unpacking Error vector from error tool
    const ROBErrorCollection* errVecTES = 0;
    sc = m_errorTool->retrieve(errVecTES);
    if ( sc.isFailure()  ||  !errVecTES ) {
      if (debug) {
        msg(MSG::DEBUG) << "No ROB Status and Unpacking Error vector found"
                        << endmsg;
      }
    }

    if (errVecTES && !errVecTES->empty()) {
      ROBErrorCollection::const_iterator robIter  = errVecTES->begin();
      ROBErrorCollection::const_iterator robIterE = errVecTES->end();
      unsigned int numRobErr = *robIter;
      ++robIter;
      while (robIter != robIterE) {
        const unsigned int sourceId = *robIter;
        ++robIter;
        if (robIter != robIterE) {
          const int crate = sourceId & 0xf;
          const int slink = (sourceId >> 4) & 0x3;
          const int dataType = (sourceId >> 7) & 0x1;
          const int rob = crate + dataType * 6;
          const int pos = rob * 4 + slink;
          unsigned int err = *robIter;
          ++robIter;
          if (err == 0) continue;
          TH2F_LW* hist1 = m_h_rod_2d_PpRobStatusGeneric;
          TH2F_LW* hist2 = m_h_rod_2d_PpRobStatusSpecific;
          TH2F_LW* hist3 = m_h_rod_2d_PpUnpack;
          int val = pos;
          if (pos >= 56) {
            hist1 = m_h_rod_2d_CpJepRoiRobStatusGeneric;
            hist2 = m_h_rod_2d_CpJepRoiRobStatusSpecific;
            hist3 = m_h_rod_2d_CpJepRoiUnpack;
            val = (pos >= 72) ? (pos - 72) / 2 + 8 : (pos - 56) / 2;
          } else if (pos >= 32) {
            hist1 = m_h_rod_2d_CpJepRobStatusGeneric;
            hist2 = m_h_rod_2d_CpJepRobStatusSpecific;
            hist3 = m_h_rod_2d_CpJepUnpack;
            val = (pos >= 48) ? pos - 48 + 8 : (pos - 32) / 2;
          }
          if (numRobErr) {
            for (int bit = 0; bit < 32; ++bit) {
              const int robErr = (err >> bit) & 0x1;
              if (robErr) {
                if (bit < 16) hist1->Fill(bit, val);
                else          hist2->Fill(bit - 16, val);
                if      (bit < 5)  errorsROB[bit] = 1;
                else if (bit < 16) errorsROB[5]   = 1;
                else               errorsROB[6]   = 1;
              }
            }
            crateErr[crate] |= (1 << ROBStatusError);
            robErrorFlags[pos] = 1;
            numRobErr--;
          } else {
            if (err > numUnpErr) err = numUnpErr;
            hist3->Fill(err, val);
            errorsUnpack[err] = 1;
            crateErr[crate] |= (1 << UnpackingError);
            if (err == 3) robErrorFlags[pos] = 1;
          }
        }
      }
    }
  }

  // Event status errors

  unsigned int evtStatus = 0;
  const EventInfo* evtInfo = 0;
  sc = evtStore()->retrieve(evtInfo);
  if ( sc.isSuccess() ) {
    const TriggerInfo* trigInfo = evtInfo->trigger_info();
    if (trigInfo) evtStatus = trigInfo->statusElement();
  }
  if (evtStatus) {
    for (int bit = 0; bit < 32; ++bit) {
      const int evtErr = (evtStatus >> bit) & 0x1;
      if (evtErr) {
        if (bit < 16) m_h_rod_1d_EvtStatusGeneric->Fill(bit);
        else          m_h_rod_1d_EvtStatusSpecific->Fill(bit);
        if      (bit < 5)  errorsEvt[bit] = 1;
        else if (bit < 16) errorsEvt[5]   = 1;
        else               errorsEvt[6]   = 1;
      }
    }
  }

  // Skip corrupt events in main plots

  if ( !corrupt ) {

    //Retrieve DAQ ROD Headers from SG
    const xAOD::RODHeaderContainer* rodTES = 0;
    if (evtStore()->contains<xAOD::RODHeaderContainer>(m_rodHeaderLocation)) {
      sc = evtStore()->retrieve(rodTES, m_rodHeaderLocation);
    } else sc = StatusCode::FAILURE;
    if ( sc.isFailure()  ||  !rodTES ) {
      if (debug) msg(MSG::DEBUG) << "No DAQ ROD Header container found"
                                   << endmsg;
    }

    //Retrieve CP RoIB ROD Headers from SG
    const xAOD::RODHeaderContainer* cpRoibTES = 0;
    if (evtStore()->contains<xAOD::RODHeaderContainer>(m_cpRoibRodHeaderLocation)) {
      sc = evtStore()->retrieve(cpRoibTES, m_cpRoibRodHeaderLocation);
    } else sc = StatusCode::FAILURE;
    if ( sc.isFailure()  ||  !cpRoibTES ) {
      if (debug) msg(MSG::DEBUG) << "No CP RoIB ROD Header container found"
                                   << endmsg;
    }

    //Retrieve JEP RoIB ROD Headers from SG
    const xAOD::RODHeaderContainer* jepRoibTES = 0;
    if (evtStore()->contains<xAOD::RODHeaderContainer>(m_jepRoibRodHeaderLocation)) {
      sc = evtStore()->retrieve(jepRoibTES, m_jepRoibRodHeaderLocation);
    } else sc = StatusCode::FAILURE;
    if ( sc.isFailure()  ||  !jepRoibTES ) {
      if (debug) msg(MSG::DEBUG) << "No JEP RoIB ROD Header container found"
                                   << endmsg;
    }

    //=============================================
    //   ROD Payload plots
    //=============================================

    std::vector<int> noFragmentFlags(80, 1);
    std::vector<int> noPayloadFlags(56, 1);
    std::vector<const xAOD::RODHeaderContainer*> cols;
    if (rodTES)     cols.push_back(rodTES);
    if (cpRoibTES)  cols.push_back(cpRoibTES);
    if (jepRoibTES) cols.push_back(jepRoibTES);
    std::vector<const xAOD::RODHeaderContainer*>::const_iterator colIter =
      cols.begin();
    std::vector<const xAOD::RODHeaderContainer*>::const_iterator colIterEnd =
      cols.end();
    for (; colIter != colIterEnd; ++colIter) {
      xAOD::RODHeaderContainer::const_iterator iter    = (*colIter)->begin();
      xAOD::RODHeaderContainer::const_iterator iterEnd = (*colIter)->end();
      for (; iter != iterEnd; ++iter) {
        const xAOD::RODHeader* header = *iter;
        const int crate = header->crate();
        const int slink = header->sLink();
        const int dataType = header->dataType();
        const int rod = crate + dataType * 6;
        const int nData = header->payloadSize();
        const int pos = rod * 4 + slink;
        // Skip obviously corrupt data
        if (crate > 13 || slink > 3 || nData < 0 ||
            nData > 10000 || pos >= 80) continue;
        noFragmentFlags[pos] = 0;
        if (pos < 56 && nData > 0) noPayloadFlags[pos] = 0;
        m_sumPayloads1[pos] += nData;
        m_sumPayloads2[pos] += nData;
        // Status bits
        TH2F_LW* hist = m_h_rod_2d_PpStatus;
        int val = pos;
        if (pos >= 72) {
          hist = m_h_rod_2d_CpJepRoiStatus;
          val = (pos - 72) / 2 + 8;
        } else if (pos >= 56) {
          hist = m_h_rod_2d_CpJepRoiStatus;
          val = (pos - 56) / 2;
        } else if (pos >= 48) {
          hist = m_h_rod_2d_CpJepStatus;
          val = pos - 48 + 8;
        } else if (pos >= 32) {
          hist = m_h_rod_2d_CpJepStatus;
          val = (pos - 32) / 2;
        }
        // ToDo: Fix properly in RODHeader.
        // gLinkError is actually OR'ed with cmmParityError
        // (email from Weiming 26/06/09)
        //if (header->gLinkError()) {
        // gLinkError and LinkError are interchanged
        // (email from Bruce 10/03/10
        //if (header->cmmParityError()) {
        if (header->lvdsLinkError()) {
          hist->Fill(GLink, val);
          errors[GLink] = 1;
          crateErr[crate] |= (1 << GLink);
        }
        //if (header->cmmParityError()) {
        //  hist->Fill(CMMParity, val);
        //  errors[CMMParity] = 1;
        //  crateErr[crate] |= (1 << CMMParity);
        //}
        //if (header->lvdsLinkError()) {
        if (header->cmmParityError()) {                                 //<<== CHECK
          hist->Fill(LVDSLink, val);
          errors[LVDSLink] = 1;
          crateErr[crate] |= (1 << LVDSLink);
        }
        if (header->rodFifoOverflow()) {
          hist->Fill(FIFOOverflow, val);
          errors[FIFOOverflow] = 1;
          crateErr[crate] |= (1 << FIFOOverflow);
        }
        if (header->dataTransportError()) {
          hist->Fill(DataTransport, val);
          errors[DataTransport] = 1;
          crateErr[crate] |= (1 << DataTransport);
        }
        if (header->gLinkTimeout()) {
          hist->Fill(Timeout, val);
          errors[Timeout] = 1;
          crateErr[crate] |= (1 << Timeout);
        }
        if (header->bcnMismatch()) {
          hist->Fill(BCNMismatch, val);
          errors[BCNMismatch] = 1;
          crateErr[crate] |= (1 << BCNMismatch);
        }
        if (header->triggerTypeTimeout()) hist->Fill(TriggerType, val);
        if (pos >= 56 && header->limitedRoISet()) hist->Fill(LimitedRoI, val);
      }
    }

    // Update average payload plots

    ++m_events;
    const int events1 = 20;
    int events2 = m_events % events1;
    if (events2 == 0) events2 = events1;
    int events3 = events1;
    if (m_events <= events1) events3 = 0;
    const int events4 = events2 + events3;
    const double error1 = 1. / sqrt(m_events);
    const double error2 = 1. / sqrt(events4);
    for (int i = 0; i < 80; ++i) {
      const double average1 = m_sumPayloads1[i] / m_events;
      const double average2 = (m_sumPayloads2[i] + m_sumPayloads2[i + 80]) / events4;
      if (events2 == events1) {
        m_sumPayloads2[i + 80] = m_sumPayloads2[i];
        m_sumPayloads2[i] = 0;
      }
      if (i >= 72) {
        if (i % 2 == 0) {
          const int bin = (i - 72) / 2 + 9;
          m_h_rod_1d_CpJepRoiPayload->SetBinContentAndError(bin, average1, error1);
          if (online) {
            m_h_rod_1d_CpJepRoiPayload->SetBinContentAndError(13 + bin, average2, error2);
          }
        }
      } else if (i >= 56) {
        if (i % 2 == 0) {
          const int bin = (i - 56) / 2 + 1;
          m_h_rod_1d_CpJepRoiPayload->SetBinContentAndError(bin, average1, error1);
          if (online) {
            m_h_rod_1d_CpJepRoiPayload->SetBinContentAndError(13 + bin, average2, error2);
          }
        }
      } else if (i >= 48) {
        const int bin = i - 48 + 1;
        m_h_rod_1d_JepPayload->SetBinContentAndError(bin, average1, error1);
        if (online) {
          m_h_rod_1d_JepPayload->SetBinContentAndError(9 + bin, average2, error2);
        }
      } else if (i >= 32) {
        if (i % 2 == 0) {
          const int bin = (i - 32) / 2 + 1;
          m_h_rod_1d_CpPayload->SetBinContentAndError(bin, average1, error1);
          if (online) {
            m_h_rod_1d_CpPayload->SetBinContentAndError(9 + bin, average2, error2);
          }
        }
      } else {
        const int bin = i + 1;
        m_h_rod_1d_PpPayload->SetBinContentAndError(bin, average1, error1);
        if (online) {
          m_h_rod_1d_PpPayload->SetBinContentAndError(33 + bin, average2, error2);
        }
      }
    }

    // Update missing ROD fragments and payloads

    for (int pos = 0; pos < 80; ++pos) {
      if (robErrorFlags[pos]) {
        noFragmentFlags[pos] = 0;
        if (pos < 56) noPayloadFlags[pos] = 0;
      }
      if (noFragmentFlags[pos] || (pos < 56 && noPayloadFlags[pos])) {
        TH2F_LW* hist = m_h_rod_2d_PpStatus;
        int val = pos;
        int crate = pos / 4;
        if (crate > 13) crate -= 6;
        if (pos >= 72) {
          if (pos % 2) continue;
          hist = m_h_rod_2d_CpJepRoiStatus;
          val = (pos - 72) / 2 + 8;
        } else if (pos >= 56) {
          if (pos % 2) continue;
          hist = m_h_rod_2d_CpJepRoiStatus;
          val = (pos - 56) / 2;
        } else if (pos >= 48) {
          hist = m_h_rod_2d_CpJepStatus;
          val = pos - 48 + 8;
        } else if (pos >= 32) {
          if (pos % 2) continue;
          hist = m_h_rod_2d_CpJepStatus;
          val = (pos - 32) / 2;
        }
        if (noFragmentFlags[pos]) {
          hist->Fill(NoFragment, val);
          errors[NoFragment] = 1;
          crateErr[crate] |= (1 << NoFragment);
        } else {
          hist->Fill(NoPayload, val);
          errors[NoPayload] = 1;
          crateErr[crate] |= (1 << NoPayload);
        }
      }
    }
  }

  // Update summary plots

  for (int i = 0; i < NumberOfStatusBins; ++i) {
    if (errors[i]) {
      m_h_rod_1d_ErrorSummary->Fill(i);
      m_histTool->fillEventNumber(m_h_rod_2d_ErrorEventNumbers, i);
    }
  }
  for (int i = 0; i < 7; ++i) {
    if (errorsROB[i]) {
      m_h_rod_1d_RobErrorSummary->Fill(i);
      m_histTool->fillEventNumber(m_h_rod_2d_RobErrorEventNumbers, i);
    }
    if (errorsEvt[i]) {
      m_histTool->fillEventNumber(m_h_rod_2d_EvtErrorEventNumbers, i);
    }
  }
  if (corrupt && corruptType == "AnyROBOrUnpackingError") {
    m_histTool->fillEventNumber(m_h_rod_2d_RobErrorEventNumbers, 7);
  }
  if (corrupt && corruptType == "FullEventTimeout") {
    m_histTool->fillEventNumber(m_h_rod_2d_EvtErrorEventNumbers, 7);
  }

  for (unsigned int i = 1; i <= numUnpErr; ++i) {
    if (errorsUnpack[i]) {
      m_h_rod_1d_UnpackErrorSummary->Fill(i);
      m_histTool->fillEventNumber(m_h_rod_2d_UnpackErrorEventNumbers, i);
    }
  }

  // Save error vector for global summary

  if ( !corrupt ) {
    std::vector<int>* save = new std::vector<int>(crateErr);
    sc = evtStore()->record(save, m_errorLocation);
    if (sc != StatusCode::SUCCESS) {
      msg(MSG::ERROR) << "Error recording ROD error vector in TES " << endmsg;
      return sc;
    }
  }

  if (debug) msg(MSG::DEBUG) << "Leaving fillHistograms" << endmsg;

  return StatusCode::SUCCESS;

}

/*---------------------------------------------------------*/
StatusCode RODMon::procHistograms()
/*---------------------------------------------------------*/
{
  msg(MSG::DEBUG) << "procHistograms entered" << endmsg;

  // if (isEndOfEventsBlock || isEndOfLumiBlock || isEndOfRun) {
  // }

  return StatusCode::SUCCESS;
}

void RODMon::setLabelsStatus(LWHist* hist, bool xAxis)
{
  LWHist::LWHistAxis* axis = (xAxis) ? hist->GetXaxis() : hist->GetYaxis();
  if (xAxis) {
    axis->SetBinLabel(1 + GLink,         "GLinkError");
    //axis->SetBinLabel(1+CMMParity,     "CMMParityError");
    axis->SetBinLabel(1 + LVDSLink,      "LVDSLinkError");
    axis->SetBinLabel(1 + FIFOOverflow,  "RODFIFOOverflow");
    axis->SetBinLabel(1 + DataTransport, "DataTransportError");
    axis->SetBinLabel(1 + Timeout,       "GLinkTimeout");
    axis->SetBinLabel(1 + BCNMismatch,   "BCNMismatch");
    axis->SetBinLabel(1 + TriggerType,   "TriggerTypeTimeout");
    axis->SetBinLabel(1 + LimitedRoI,    "LimitedRoISet");
    axis->SetBinLabel(1 + NoFragment,    "No ROD Fragment");
  } else {
    axis->SetBinLabel(1 + GLink,         "#splitline{GLink}{Error}");
    axis->SetBinLabel(1 + LVDSLink,      "#splitline{LVDSLink}{Error}");
    axis->SetBinLabel(1 + FIFOOverflow,  "#splitline{RODFIFO}{Overflow}");
    axis->SetBinLabel(1 + DataTransport, "#splitline{Data}{Transport}");
    axis->SetBinLabel(1 + Timeout,       "#splitline{GLink}{Timeout}");
    axis->SetBinLabel(1 + BCNMismatch,   "#splitline{BCN}{Mismatch}");
    axis->SetBinLabel(1 + TriggerType,   "#splitline{Trigger}{Timeout}");
    axis->SetBinLabel(1 + LimitedRoI,    "#splitline{Limited}{RoISet}");
    axis->SetBinLabel(1 + NoFragment,    "#splitline{No ROD}{Fragment}");
  }
}

void RODMon::setLabelsROBStatusGen(LWHist* hist, bool xAxis)
{
  LWHist::LWHistAxis* axis = (xAxis) ? hist->GetXaxis() : hist->GetYaxis();
  axis->SetBinLabel(1,  "BCIDCheck");
  axis->SetBinLabel(2,  "EL1IDCheck");
  axis->SetBinLabel(3,  "Timeout");
  axis->SetBinLabel(4,  "DataError");
  axis->SetBinLabel(5,  "Overflow");
  if (!xAxis) {
    axis->SetBinLabel(1,  "#splitline{BCID}{Check}");
    axis->SetBinLabel(2,  "#splitline{EL1ID}{Check}");
  }
}

void RODMon::setLabelsROBStatusSpec(LWHist* hist, bool xAxis)
{
  LWHist::LWHistAxis* axis = (xAxis) ? hist->GetXaxis() : hist->GetYaxis();
  axis->SetBinLabel(1,  "TrigTypeSync");
  axis->SetBinLabel(2,  "FragmentSize");
  axis->SetBinLabel(3,  "DataBlock");
  axis->SetBinLabel(4,  "ControlWord");
  axis->SetBinLabel(5,  "MissingBOF");
  axis->SetBinLabel(6,  "MissingEOF");
  axis->SetBinLabel(7,  "HeaderMarker");
  axis->SetBinLabel(8,  "Format");
  axis->SetBinLabel(9,  "Duplicate");
  axis->SetBinLabel(10, "Sequence");
  axis->SetBinLabel(11, "Transmission");
  axis->SetBinLabel(12, "Truncated");
  axis->SetBinLabel(13, "Short");
  axis->SetBinLabel(14, "Lost");
  axis->SetBinLabel(15, "Pending");
  axis->SetBinLabel(16, "Discard");
}

void RODMon::setLabelsEvtStatusSpec(LWHist* hist, bool xAxis)
{
  LWHist::LWHistAxis* axis = (xAxis) ? hist->GetXaxis() : hist->GetYaxis();
  axis->SetBinLabel(1,  "L2Timeout");
  axis->SetBinLabel(2,  "L2PUTimeout");
  axis->SetBinLabel(3,  "SFIDupl");
  axis->SetBinLabel(4,  "DFMDupl");
  axis->SetBinLabel(5,  "L2PSC");
  axis->SetBinLabel(6,  "");
  axis->SetBinLabel(7,  "");
  axis->SetBinLabel(8,  "");
  axis->SetBinLabel(9,  "EFTimeout");
  axis->SetBinLabel(10, "PTTimeout");
  axis->SetBinLabel(11, "SFODupl");
  axis->SetBinLabel(12, "EFDRecov");
  axis->SetBinLabel(13, "EFPSC");
  axis->SetBinLabel(14, "EFDForcedAcpt");
  axis->SetBinLabel(15, "");
  axis->SetBinLabel(16, "PartialEvent");
}

void RODMon::setLabelsUnpacking(LWHist* hist, bool xAxis)
{
  LWHist::LWHistAxis* axis = (xAxis) ? hist->GetXaxis() : hist->GetYaxis();
  axis->SetBinLabel(1,  "DuplicateROB");
  axis->SetBinLabel(2,  "RODSourceID");
  axis->SetBinLabel(3,  "RODnstatus");
  axis->SetBinLabel(4,  "UserHeader");
  axis->SetBinLabel(5,  "MissingHeader");
  axis->SetBinLabel(6,  "MissingSubBlock");
  axis->SetBinLabel(7,  "CrateNumber");
  axis->SetBinLabel(8,  "ModuleNumber");
  axis->SetBinLabel(9,  "Slices");
  axis->SetBinLabel(10, "DuplicateData");
  axis->SetBinLabel(11, "RoIType");
  axis->SetBinLabel(12, "Version");
  axis->SetBinLabel(13, "Format");
  axis->SetBinLabel(14, "ComprVersion");
  axis->SetBinLabel(15, "ComprSlices");
  axis->SetBinLabel(16, "DataTruncated");
  axis->SetBinLabel(17, "ExcessData");
  axis->SetBinLabel(18, "DataSourceID");
  axis->SetBinLabel(19, "ExcessTOBs");
  axis->SetBinLabel(20, "DataID");
  axis->SetBinLabel(21, "Unknown");
}
// ============================================================================
} // end namespace
// ============================================================================
